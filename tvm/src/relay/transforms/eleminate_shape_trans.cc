#include <tvm/ir/expr.h>
#include <tvm/relay/analysis.h>
#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/attrs/transform.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/op_attr_types.h>
#include <tvm/relay/transform.h>

#include <unordered_map>
#include <unordered_set>

#include "../op/make_op.h"

namespace tvm {
namespace relay {

bool CheckSameShape(const Array<PrimExpr>& shape, const Array<PrimExpr>& target) {
  if (shape.size() != target.size())
    return false;
  for (uint64_t n = 0; n < shape.size(); n++) {
    if (shape[n].as<IntImmNode>()->value != target[n].as<IntImmNode>()->value)
      return false;
  }
  return true;
}

// Search dense op weight name from Expr
class ShapeTransVisitor : private ExprVisitor {
 public:
  ShapeTransVisitor(const std::vector<Op>& shape_op) : shape_op_(shape_op) {}

  Expr Search(const Expr& expr, Array<PrimExpr> shape) {
    last_ = expr;
    shape_ = shape;
    VisitExpr(expr);
    return last_;
  }

 private:
  void VisitExpr_(const CallNode* n) final {
    if (std::find(shape_op_.begin(), shape_op_.end(), n->op) != shape_op_.end()) {
      if (n->args[0]->checked_type().defined()) {
        if (CheckSameShape(shape_, n->args[0]->checked_type().as<TensorTypeNode>()->shape))
          last_ = n->args[0];
        for (const auto& arg : n->args) {
          VisitExpr(arg);
        }
      }
    }
  }
  // Cache op

  std::vector<Op> shape_op_;
  Expr last_;
  Array<PrimExpr> shape_;
};  // SearchDenseOpWeight


// Mutate ```nn.dense``` to ```nn.conv2d```
class EleminateMutator : public ExprMutator {
 public:
  EleminateMutator() {
    shape_op_.push_back(Op::Get("reshape"));
    shape_op_.push_back(Op::Get("nn.batch_flatten"));
    shape_op_.push_back(Op::Get("expand_dims"));
  }

  Expr VisitExpr_(const CallNode* n) final {
    auto new_n = n;
    if (std::find(shape_op_.begin(), shape_op_.end(), n->op) == shape_op_.end()) {
      Array<Expr> new_args;
      for (uint64_t i = 0; i < n->args.size(); i++) {
        Expr new_arg = n->args[i];
        if (new_arg.as<CallNode>()) {
          if (std::find(shape_op_.begin(), shape_op_.end(), new_arg.as<CallNode>()->op) != shape_op_.end()) {
            if (n->args[i]->checked_type().defined()) {
              Array<PrimExpr> shape = new_arg->checked_type().as<TensorTypeNode>()->shape;
              new_arg = ShapeTransVisitor(shape_op_).Search(n->args[i], shape);
            }
          }
        }
        new_args.push_back(ExprMutator::VisitExpr(new_arg));
      }
      return Call(n->op, new_args, n->attrs);
    }
    return ExprMutator::VisitExpr_(new_n);
  }

 private:
  // Cached op
  std::vector<Op> shape_op_;
};  // class DenseToSparseDenseAlter

Expr EleminateShapeTrans(const Expr& e) {
  return EleminateMutator().Mutate(e);
}

namespace transform {

Pass EleminateShapeTrans() {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(EleminateShapeTrans(f));
      };
  return CreateFunctionPass(pass_func, 0, "EleminateShapeTrans", {});
}

TVM_REGISTER_GLOBAL("relay._transform.EleminateShapeTrans").set_body_typed(EleminateShapeTrans);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
