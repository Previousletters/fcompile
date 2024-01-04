#ifndef __TRANSFORM_INFER_TYPE__
#define __TRANSFORM_INFER_TYPE__

#include <cstdint>
#include <cstdlib>
#include <vector>

#include <svir/base.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>

using namespace svir;

namespace transform {

class InferType : public Mutator {

    void Visit_(SVVar* var) final {
        var->checked = new SVTensor(var->shape, "feature");
    }

    void Visit_(SVCall* call) final {
        Report* report = new Report();
        std::vector<SVExpr*> new_args;
        for (auto arg : call->args) {
            Mutator::Visit(arg);
            report->args.push_back(arg->checked);
        }
        if (call->op->infer(call->args, call->attrs, report)) {
            call->checked = report->out;
        } else {
            std::cout << "Error! " << call->op->op_type << std::endl;
            exit(-1);
        }
    }

};

};


#endif
