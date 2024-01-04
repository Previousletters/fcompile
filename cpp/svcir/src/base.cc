#include <op/base.h>

namespace svir {

std::unordered_map<std::string, Op> Op::memo_;

bool MVMRel(std::vector<SVExpr*> args, Attrs attrs, Report* report) {
    if (args.size() != 2)
        return false;
    std::vector<int> dshape = report->args[0]->as<SVTensor>()->shape;
    std::vector<int> wshape = report->args[1]->as<SVTensor>()->shape;
    if (dshape[3] != wshape[2])
        return false;
    std::vector<int> oshape = { dshape[0], dshape[1], dshape[2], wshape[3] };
    report->out = new SVTensor(oshape, "feature");
    return true;
}

OP_REG("mvm", 2, 1, MVMRel);

bool AddRel(std::vector<SVExpr*> args, Attrs attrs, Report* report) {
    if (args.size() != 2)
        return false;
    std::vector<int> ashape = report->args[0]->as<SVTensor>()->shape;
    std::vector<int> bshape = report->args[1]->as<SVTensor>()->shape;
    std::vector<int> oshape = ashape;
    report->out = new SVTensor(oshape, "feature");
    return true;
}

OP_REG("add", 2, 1, AddRel);

bool TransposeRel(std::vector<SVExpr*> args, Attrs attrs, Report* report) {
    if (args.size() != 1)
        return false;
    std::vector<int> dshape = report->args[0]->as<SVTensor>()->shape;
    std::vector<int> oshape = { dshape[0], dshape[1], dshape[3], dshape[2] };
    report->out = new SVTensor(oshape, "weight");
    return true;
}

OP_REG("transpose", 1, 1, TransposeRel);

};
