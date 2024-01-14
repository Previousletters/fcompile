#include <map>
#include <tvm/ir/expr.h>
#include <tvm/relay/analysis.h>
#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/attrs/accel.h>
#include <tvm/relay/attrs/transform.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/op_attr_types.h>
#include <tvm/relay/transform.h>

#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace tvm {
namespace relay {

// Mutate ```nn.dense``` to ```nn.conv2d```
class FusedVSCOpsMutator : public ExprRewriter {
 public:
  FusedVSCOpsMutator()
      : relu_op_(Op::Get("nn.relu")), 
        target_op_(Op::Get("accel.vsc.conv2d")) {}

  Expr Rewrite_(const CallNode* pre, const Expr& post) override {
    if (pre->op == relu_op_) {
      const CallNode* last = post.as<CallNode>()->args[0].as<CallNode>();
      if (last->op == target_op_) {
        const auto* attrs = last->attrs.as<AccelOpAttrs>();
        auto new_attrs = make_object<AccelOpAttrs>();
        new_attrs->strides = attrs->strides;
        new_attrs->padding = attrs->padding;
        new_attrs->kernel_size = attrs->kernel_size;
        new_attrs->kernel_layout = attrs->kernel_layout;
        new_attrs->data_layout = attrs->data_layout;
        new_attrs->out_layout = attrs->out_layout;
        new_attrs->widths = attrs->widths;
        new_attrs->scales = attrs->scales;
        new_attrs->activate = 1;
        new_attrs->channels = attrs->channels;       
        return Call(target_op_, last->args, Attrs(new_attrs));
      }
    }
    return post;
  }

 private:
  // Cached op
  const Op& relu_op_;
  const Op& target_op_;
};  // class DenseToSparseDenseAlter

Expr FusedVSCOps(const Expr& e) {
  auto rewriter = FusedVSCOpsMutator();
  return PostOrderRewrite(e, &rewriter);
}

namespace transform {

Pass FusedVSCOps() {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(FusedVSCOps(f));
      };
  return CreateFunctionPass(pass_func, 0, "FusedVSCOps", {});
}

TVM_REGISTER_GLOBAL("relay._transform.FusedVSCOps").set_body_typed(FusedVSCOps);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
