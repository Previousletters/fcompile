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

int main() {
    svir::Expr* input = new svir::Var("data", {1, 19, 4096});
    svir::Expr* ln_weight = new svir::Constant("ln_weight", {4096*2}, "feature", {"../test/chatglm_page1/LN_k.bin", ""});
    svir::Expr* q_weight = new svir::Constant("q_weight", {4096, 128*32}, "weight", {"../test/chatglm_page1/MVM_BN_Wq.bin", "../test/chatglm_page1/MVM_BN_Scaleq.bin"});
    svir::Expr* q_bias = new svir::Constant("q_bias", {256*32}, "feature", {"", "../test/chatglm_page1/MVM_BN_Biasq.bin"});
    
    svir::Expr* ln_out = svir::layer_norm(input, ln_weight);
    svir::Expr* output = svir::mvm_bn(ln_out, q_weight, q_bias, 0, 1);
    
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
