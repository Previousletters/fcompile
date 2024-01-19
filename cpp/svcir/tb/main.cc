#include <cstdint>
#include <iostream>
#include <vector>

#include <op/base.h>
#include <op/attrs.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>
#include <svir/expr_print.h>
#include <transform/infer_type.h>
#include <transform/const_process.h>

#include <export/csb_head.h>

svir::Expr* transpose(svir::Expr* input, int out_and_in_mode, int log2_bank_step, int left_wt_base_addr) {
    svir::TransposeAttrs* transpose_attr = new svir::TransposeAttrs();
    transpose_attr->out_and_in_mode = out_and_in_mode;
    transpose_attr->log2_bank_step = log2_bank_step;
    transpose_attr->left_wt_base_addr = left_wt_base_addr;
    svir::Expr* output = new svir::Call(svir::Op::Get("transpose"), {input}, static_cast<svir::Attrs*>(transpose_attr));
    return output;
}

svir::Expr* layer_norm(svir::Expr* data, svir::Expr* weight, int out_and_in_mode) {
    svir::LayerNormAttrs* attrs = new svir::LayerNormAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    svir::Expr* output = new svir::Call(svir::Op::Get("layer_norm"), {data, weight}, static_cast<svir::Attrs*>(attrs));
    return output;
}

svir::Expr* mvm(svir::Expr* data, svir::Expr* weight, int relu, int skip, int log2_bank_step, int left_wt_base_addr) {
    svir::MVMAttrs* mvm_attr = new svir::MVMAttrs();
    mvm_attr->relu = relu;
    mvm_attr->skip = skip;
    mvm_attr->out_to_BRAM = 0;
    mvm_attr->in_from_BRAM = 0;
    mvm_attr->out_base_addr = 0;
    mvm_attr->in_base_addr = 0;
    mvm_attr->log2_bank_step = log2_bank_step;
    mvm_attr->left_wt_base_addr = left_wt_base_addr;
    svir::Expr* output = new svir::Call(svir::Op::Get("mvm"), {data, weight}, static_cast<svir::Attrs*>(mvm_attr));
    return output;
}

svir::Expr* mvm_bn(svir::Expr* data, svir::Expr* weight, svir::Expr* bn, int relu, int skip, int log2_bank_step, int left_wt_base_addr) {
    svir::MVMAttrs* mvm_attr = new svir::MVMAttrs();
    mvm_attr->relu = relu;
    mvm_attr->skip = skip;
    mvm_attr->out_to_BRAM = 0;
    mvm_attr->in_from_BRAM = 0;
    mvm_attr->out_base_addr = 0;
    mvm_attr->in_base_addr = 0;
    mvm_attr->log2_bank_step = log2_bank_step;
    mvm_attr->left_wt_base_addr = left_wt_base_addr;
    svir::Expr* output = new svir::Call(svir::Op::Get("mvm_bn"), {data, weight, bn}, static_cast<svir::Attrs*>(mvm_attr));
    return output;
}

int main() {
    svir::Expr* input = new svir::Var("data", {1, 19, 4096});
    svir::Expr* ln_weight = new svir::Constant("ln_weight", {4096*2}, "feature", {"../test/chatglm_page1/LN_k.bin", ""});
    svir::Expr* q_weight = new svir::Constant("q_weight", {4096, 128*32}, "weight", {"../test/chatglm_page1/MVM_BN_Wq.bin", "../test/chatglm_page1/MVM_BN_Scaleq.bin"});
    svir::Expr* q_bias = new svir::Constant("q_bias", {256*32}, "feature", {"", "../test/chatglm_page1/MVM_BN_Biasq.bin"});
    
    svir::Expr* ln_out = layer_norm(input, ln_weight, 0);
    svir::Expr* output = mvm_bn(ln_out, q_weight, q_bias, 0, 1, 28, 0);
    
    std::string data;
    std::cout << "----------Origin----------" << std::endl;
    data = Print().print(output);
    std::cout << data << std::endl;

    transform::InferType().Visit(output);
    std::cout << "----------Infered---------" << std::endl;
    data = Print().print(output);
    std::cout << data << std::endl;

    transform::ConstProcess().Visit(output);
    std::cout << "----------Infered---------" << std::endl;
    data = Print().print(output);
    std::cout << data << std::endl;

    auto codegen = backend::CSBHead();
    codegen.Build(output, "atten");
    std::string call_name = "LN_Q_4096x4096";
    codegen.GetSource("../test/" + call_name + ".h", call_name);
    codegen.SaveParams("../test/processed/" + call_name);
    return 0;
}
