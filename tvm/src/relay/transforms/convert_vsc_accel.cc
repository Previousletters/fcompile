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

typedef std::map<const CallNode*, std::pair<Op, Attrs>> call_map_t;

// Search dense op weight name from Expr
class VarNameVisitor : private ExprVisitor {
 public:
  VarNameVisitor() = default;

  String Search(const Expr& expr) {
    name_ = "";
    VisitExpr(expr);
    return name_;
  }

 private:
  void VisitExpr_(const VarNode* n) final {
    name_ = n->name_hint();
  }
  // Cache op
  String name_;
};

// Search dense op weight name from Expr
class VSCOpVisitor : private ExprVisitor {
 public:
  VSCOpVisitor(const Map<String, Array<Integer>>& precision, int sim_8bit)
      : conv2d_op_(Op::Get("nn.conv2d")), 
        target_op_(Op::Get("accel.vsc.conv2d")), 
        precision_(precision), sim_8bit_(sim_8bit) {}

  call_map_t Search(const Expr& expr) {
    VisitExpr(expr);
    return map_attr_;
  }

 private:
  void VisitExpr_(const CallNode* n) final {
    if (n->op == conv2d_op_) {
      String wname = VarNameVisitor().Search(n->args[1]);
      if (precision_.find(wname) != precision_.end()) {
        auto wshape = n->args[1]->checked_type().as<TensorTypeNode>()->shape;
        const auto* attrs = n->attrs.as<Conv2DAttrs>();
        auto new_attrs = make_object<AccelOpAttrs>();
        new_attrs->strides = attrs->strides;
        new_attrs->padding = attrs->padding;
        new_attrs->kernel_size = {(int)wshape[2].as<IntImmNode>()->value, (int)wshape[3].as<IntImmNode>()->value};
        new_attrs->kernel_layout = "HWIO";
        new_attrs->data_layout = "NHWC";
        new_attrs->out_layout = "NHWC";
        new_attrs->widths = {precision_[wname][0], precision_[wname][2], precision_[wname][4]};
        new_attrs->scales = {precision_[wname][1], precision_[wname][3], precision_[wname][5]};
        new_attrs->activate = 0;
        new_attrs->channels = (int)wshape[0].as<IntImmNode>()->value;
        map_attr_[n] = std::pair<const Op&, Attrs>(target_op_, Attrs(new_attrs));
      } else if (sim_8bit_) {
        auto wshape = n->args[1]->checked_type().as<TensorTypeNode>()->shape;
        const auto* attrs = n->attrs.as<Conv2DAttrs>();
        auto new_attrs = make_object<AccelOpAttrs>();
        new_attrs->strides = attrs->strides;
        new_attrs->padding = attrs->padding;
        new_attrs->kernel_size = {(int)wshape[2].as<IntImmNode>()->value, (int)wshape[3].as<IntImmNode>()->value};
        new_attrs->kernel_layout = "HWIO";
        new_attrs->data_layout = "NHWC";
        new_attrs->out_layout = "NHWC";
        new_attrs->widths = {8, 8, 8};
        new_attrs->scales = {6, 6, 6};
        new_attrs->activate = 0;
        new_attrs->channels = (int)wshape[0].as<IntImmNode>()->value;
        map_attr_[n] = std::pair<const Op&, Attrs>(target_op_, Attrs(new_attrs));
      }
    }
    ExprVisitor::VisitExpr_(n);
  }
  // Cache op
  const Op& conv2d_op_;
  const Op& target_op_;
  Map<String, Array<Integer>> precision_;
  call_map_t map_attr_;
  int sim_8bit_;
};

// Mutate ```nn.dense``` to ```nn.conv2d```
class ConvertVSCAccelMutator : public ExprRewriter {
 public:
  ConvertVSCAccelMutator(const call_map_t& node_map)
      : node_map_(std::move(node_map)) {}

  Expr Rewrite_(const CallNode* pre, const Expr& post) override {
    if (node_map_.find(pre) != node_map_.end()) {
      return Call(node_map_[pre].first, post.as<CallNode>()->args, node_map_[pre].second);
    }
    return post;
  }

 private:
  // Cached op
  Map<String, Array<Integer>> precision_;
  call_map_t node_map_;
};  // class DenseToSparseDenseAlter

Expr ConvertVSCAccel(const Expr& e, const Map<String, Array<Integer>>& precision, int sim_8bit) {
  auto node_map = VSCOpVisitor(precision, sim_8bit).Search(e);
  auto rewriter = ConvertVSCAccelMutator(node_map);
  return PostOrderRewrite(e, &rewriter);
}

namespace transform {

Pass ConvertVSCAccel(const Map<String, Array<Integer>>& precision, int sim_8bit) {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(ConvertVSCAccel(f, precision, sim_8bit));
      };
  return CreateFunctionPass(pass_func, 0, "ConvertVSCAccel", {});
}

TVM_REGISTER_GLOBAL("relay._transform.ConvertVSCAccel").set_body_typed(ConvertVSCAccel);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
