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

// Mutate ```nn.dense``` to ```nn.conv2d```
class DenseToConv2DMutator : public ExprRewriter {
 public:
  DenseToConv2DMutator()
      : dense_op_(Op::Get("nn.dense")) {}

  Expr Rewrite_(const CallNode* pre, const Expr& post) override {
    if (pre->op == dense_op_) {
      auto dshape = pre->args[0]->checked_type().as<TensorTypeNode>()->shape;
      auto wshape = pre->args[1]->checked_type().as<TensorTypeNode>()->shape;
      auto oshape = pre->checked_type().as<TensorTypeNode>()->shape;
      Array<Integer> new_dshape({ (int)dshape[0].as<IntImmNode>()->value, 1, 1, (int)dshape[1].as<IntImmNode>()->value });
      Array<Integer> new_wshape({ (int)wshape[0].as<IntImmNode>()->value, (int)wshape[1].as<IntImmNode>()->value, 1, 1 });
      Array<Integer> new_oshape({ (int)oshape[0].as<IntImmNode>()->value, (int)oshape[1].as<IntImmNode>()->value });
      Expr new_dexpr = MakeReshape(post.as<CallNode>()->args[0], new_dshape);
      Expr new_wexpr = MakeReshape(post.as<CallNode>()->args[1], new_wshape);
      auto attrs = make_object<Conv2DAttrs>();
      attrs->dilation = {1, 1};
      attrs->groups = 1;
      attrs->channels = (int)wshape[0].as<IntImmNode>()->value;
      attrs->strides = {1, 1};
      attrs->padding = {0, 0};
      attrs->kernel_size = {1, 1};
      attrs->data_layout = "NHWC";
      attrs->kernel_layout = "OIHW";
      attrs->out_layout = "NHWC";
      attrs->out_dtype = pre->checked_type().as<TensorTypeNode>()->dtype;
      Expr new_conv = Call(Op::Get("nn.conv2d"), {new_dexpr, new_wexpr}, Attrs(attrs));
      Expr new_output = MakeReshape(new_conv, new_oshape);
      return new_output;
    }
    return post;
  }

 private:
  // Cached op
  const Op& dense_op_;
};  // class DenseToSparseDenseAlter

Expr DenseToConv2D(const Expr& e) {
  auto rewriter = DenseToConv2DMutator();
  return PostOrderRewrite(e, &rewriter);
}

namespace transform {

Pass DenseToConv2D() {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(DenseToConv2D(f));
      };
  return CreateFunctionPass(pass_func, 0, "DenseToConv2D", {});
}

TVM_REGISTER_GLOBAL("relay._transform.DenseToConv2D").set_body_typed(DenseToConv2D);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
