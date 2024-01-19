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

    void Visit_(Constant* constant) final {
        constant->checked = new Tensor(constant->shape, constant->dtype);
    }

    void Visit_(Var* var) final {
        var->checked = new Tensor(var->shape, "feature");
    }

    void Visit_(Call* call) final {
        Report* report = new Report();
        std::vector<Expr*> new_args;
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
