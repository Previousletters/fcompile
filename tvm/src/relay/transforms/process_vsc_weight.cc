#include <cstdint>
#include <map>
#include <tvm/ir/expr.h>
#include <tvm/relay/analysis.h>
#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/attrs/accel.h>
#include <tvm/relay/attrs/transform.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/op_attr_types.h>
#include <tvm/relay/transform.h>
#include <tvm/runtime/ndarray.h>

#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace tvm {
namespace relay {

// Mutate ```nn.dense``` to ```nn.conv2d```
class ProcessVSCWeightMutator : public ExprRewriter {
 public:
  ProcessVSCWeightMutator(){}
  // Cached op
};  // class DenseToSparseDenseAlter

Expr ProcessVSCWeight(const Expr& e, const Map<String, runtime::NDArray>& params) {
  for (auto& i : params) {
    std::cout << i.first << std::endl;
    std::cout << i.second->ndim << std::endl;
  }
  return e;
}

namespace transform {

Pass ProcessVSCWeight(const Map<String, runtime::NDArray>& params) {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(ProcessVSCWeight(f, params));
      };
  return CreateFunctionPass(pass_func, 0, "ProcessVSCWeight", {});
}

TVM_REGISTER_GLOBAL("relay._transform.ProcessVSCWeight").set_body_typed(ProcessVSCWeight);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
