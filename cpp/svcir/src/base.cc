#include <iostream>
#include <op/base.h>
#include <op/attrs.h>

namespace svir {

std::unordered_map<std::string, Op> Op::memo_;

bool MVMRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature" ||
        report->args[1]->as<Tensor>()->dtype != "weight")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> wshape = report->args[1]->as<Tensor>()->shape;
    if (dshape[2] != wshape[0])
        return false;
    std::vector<int> oshape = { dshape[0], dshape[1], wshape[1] };
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("mvm", 2, 1, MVMRel);

Expr* mvm(Expr* data, Expr* weight, int relu, int skip, int log2_bank_step, int left_wt_base_addr) {
    MVMAttrs* mvm_attr = new MVMAttrs();
    mvm_attr->relu = relu;
    mvm_attr->skip = skip;
    mvm_attr->out_to_BRAM = 0;
    mvm_attr->in_from_BRAM = 0;
    mvm_attr->out_base_addr = 0;
    mvm_attr->in_base_addr = 0;
    mvm_attr->log2_bank_step = log2_bank_step;
    mvm_attr->left_wt_base_addr = left_wt_base_addr;
    Expr* output = new Call(Op::Get("mvm"), {data, weight}, static_cast<Attrs*>(mvm_attr));
    return output;
}

bool MVMBNRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 3)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature" ||
        report->args[1]->as<Tensor>()->dtype != "weight"  ||
        report->args[2]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> wshape = report->args[1]->as<Tensor>()->shape;
    std::vector<int> bshape = report->args[2]->as<Tensor>()->shape;
    if (dshape[2] != wshape[0])
        return false;
    if (wshape[1]*2 != bshape[0])
        return false;
    std::vector<int> oshape = { dshape[0], dshape[1], wshape[1] };
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("mvm_bn", 3, 1, MVMBNRel);

Expr* mvm_bn(Expr* data, Expr* weight, Expr* bn, int relu, int skip, int log2_bank_step, int left_wt_base_addr) {
    MVMAttrs* mvm_attr = new MVMAttrs();
    mvm_attr->relu = relu;
    mvm_attr->skip = skip;
    mvm_attr->out_to_BRAM = 0;
    mvm_attr->in_from_BRAM = 0;
    mvm_attr->out_base_addr = 0;
    mvm_attr->in_base_addr = 0;
    mvm_attr->log2_bank_step = log2_bank_step;
    mvm_attr->left_wt_base_addr = left_wt_base_addr;
    Expr* output = new Call(Op::Get("mvm_bn"), {data, weight, bn}, static_cast<Attrs*>(mvm_attr));
    return output;
}

bool LayerNormRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature" ||
        report->args[1]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> wshape = report->args[1]->as<Tensor>()->shape;
    if (dshape[2]*2 != wshape[0])
        return false;
    std::vector<int> oshape = { dshape[0], dshape[1], dshape[2] };
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("layer_norm", 2, 1, LayerNormRel);

Expr* layer_norm(Expr* data, Expr* weight, int out_and_in_mode) {
    LayerNormAttrs* attrs = new LayerNormAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("layer_norm"), {data, weight}, static_cast<Attrs*>(attrs));
    return output;
}

bool ElementWiseRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature" ||
        report->args[1]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> ashape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> bshape = report->args[1]->as<Tensor>()->shape;
    if (ashape[0] != bshape[0] || ashape[1] != bshape[1] || ashape[2] != bshape[2])
        return false;
    std::vector<int> oshape = ashape;
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("add", 2, 1, ElementWiseRel);
OP_REG("mul", 2, 1, ElementWiseRel);
OP_REG("minus", 2, 1, ElementWiseRel);

Expr* add(Expr* data_a, Expr* data_b, int out_and_in_mode) {
    ElementAttrs* attrs = new ElementAttrs();
    attrs->element_mode = 0;
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("add"), {data_a, data_b}, static_cast<Attrs*>(attrs));
    return output;
}

Expr* minus(Expr* data_a, Expr* data_b, int out_and_in_mode) {
    ElementAttrs* attrs = new ElementAttrs();
    attrs->element_mode = 1;
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("minus"), {data_a, data_b}, static_cast<Attrs*>(attrs));
    return output;
}

Expr* mul(Expr* data_a, Expr* data_b, int out_and_in_mode) {
    ElementAttrs* attrs = new ElementAttrs();
    attrs->element_mode = 2;
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("mul"), {data_a, data_b}, static_cast<Attrs*>(attrs));
    return output;
}

bool TransposeRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 1)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> oshape = { dshape[2], dshape[1] };
    report->out = new Tensor(oshape, "weight");
    return true;
}

OP_REG("transpose", 1, 1, TransposeRel);

Expr* transpose(Expr* input, int out_and_in_mode, int log2_bank_step, int left_wt_base_addr) {
    TransposeAttrs* transpose_attr = new TransposeAttrs();
    transpose_attr->out_and_in_mode = out_and_in_mode;
    transpose_attr->log2_bank_step = log2_bank_step;
    transpose_attr->left_wt_base_addr = left_wt_base_addr;
    Expr* output = new Call(Op::Get("transpose"), {input}, static_cast<Attrs*>(transpose_attr));
    return output;
}

bool SoftmaxRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 1)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> oshape = dshape;
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("softmax", 1, 1, SoftmaxRel);

Expr* softmax(Expr* input, int out_and_in_mode) {
    SoftmaxAttrs* attrs = new SoftmaxAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("softmax"), {input}, static_cast<Attrs*>(attrs));
    return output;
}

bool Feature2WeightRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 1)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> oshape = { dshape[1], dshape[2] };
    report->out = new Tensor(oshape, "weight");
    return true;
}

OP_REG("feature2weight", 1, 1, Feature2WeightRel);

Expr* feature2weight(Expr* input, int out_and_in_mode, int log2_bank_step, int left_wt_base_addr) {
    F2WeightAttrs* attrs = new F2WeightAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    attrs->log2_bank_step = log2_bank_step;
    attrs->left_wt_base_addr = left_wt_base_addr;
    Expr* output = new Call(Op::Get("feature2weight"), {input}, static_cast<Attrs*>(attrs));
    return output;
}

bool PosEmbRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> wshape = report->args[1]->as<Tensor>()->shape;
    if (dshape[2] != wshape[1]*2)
        return false;
    std::vector<int> oshape = dshape;
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("rotary_pos_emb", 2, 1, PosEmbRel);

Expr* rotary_pos_emb(Expr* input, Expr* pos_in, int pos_numb, int out_and_in_mode) {
    PosEmbAttrs* attrs = new PosEmbAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    attrs->pos_numb = pos_numb;
    Expr* output = new Call(Op::Get("rotary_pos_emb"), {input, pos_in}, static_cast<Attrs*>(attrs));
    return output;
}

bool ActivateRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> dshape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> oshape = dshape;
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("silu", 2, 1, ActivateRel);

Expr* silu(Expr* data, Expr* approx, int out_and_in_mode) {
    ActivateAttrs* attrs = new ActivateAttrs();
    attrs->out_and_in_mode = out_and_in_mode;
    Expr* output = new Call(Op::Get("silu"), {data, approx}, static_cast<Attrs*>(attrs));
    return output;
}

};
