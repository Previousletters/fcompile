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

bool AddRel(std::vector<Expr*> args, Attrs* attrs, Report* report) {
    if (args.size() != 2)
        return false;
    if (report->args[0]->as<Tensor>()->dtype != "feature" ||
        report->args[1]->as<Tensor>()->dtype != "feature")
        return false;
    std::vector<int> ashape = report->args[0]->as<Tensor>()->shape;
    std::vector<int> bshape = report->args[1]->as<Tensor>()->shape;
    std::vector<int> oshape = ashape;
    report->out = new Tensor(oshape, "feature");
    return true;
}

OP_REG("add", 2, 1, AddRel);

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

};
