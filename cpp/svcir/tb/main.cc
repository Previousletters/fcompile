#include <cstdint>
#include <iostream>
#include <vector>

#include <op/base.h>
#include <op/attrs.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>
#include <svir/expr_print.h>
#include <transform/infer_type.h>

#include <export/csb_head.h>

int main() {
    svir::SVExpr* input = new svir::SVVar("data", {1, 1, 10, 4096});
    svir::SVExpr* weight = new svir::SVVar("weight", {1, 1, 1024, 4096});
    svir::TransposeAttrs* transpose_attr = new svir::TransposeAttrs();
    transpose_attr->out_and_in_mode = 0;
    transpose_attr->log2_bank_step = 28;
    transpose_attr->left_wt_base_addr = 0;
    svir::SVExpr* transpose = new svir::SVCall(svir::Op::Get("transpose"), {weight}, static_cast<svir::Attrs*>(transpose_attr));
    svir::MVMAttrs* mvm_attr = new svir::MVMAttrs();
    mvm_attr->relu = 0;
    mvm_attr->skip = 1;
    mvm_attr->out_to_BRAM = 0;
    mvm_attr->in_from_BRAM = 0;
    mvm_attr->out_base_addr = 0;
    mvm_attr->in_base_addr = 0;
    mvm_attr->log2_bank_step = 28;
    mvm_attr->left_wt_base_addr = 0;
    svir::SVExpr* output = new svir::SVCall(svir::Op::Get("mvm"), {input, transpose}, static_cast<svir::Attrs*>(mvm_attr));
    std::string data;
    std::cout << "----------Origin----------" << std::endl;
    data = Print().print(output);
    std::cout << data << std::endl;

    transform::InferType().Visit(output);
    std::cout << "----------Infered---------" << std::endl;
    data = Print().print(output);
    std::cout << data << std::endl;
    backend::CSBHead().Test(output);
    return 0;
}
