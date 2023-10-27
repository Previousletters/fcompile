#include <tvm/relay/attrs/accel.h>
#include <tvm/relay/op.h>
#include <tvm/tir/analysis.h>
#include <tvm/tir/data_layout.h>

#include <vector>

namespace tvm {
namespace relay {

TVM_REGISTER_NODE_TYPE(QuantAttrs);

namespace base {

bool AccelQuantizeRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
               const TypeReporter& reporter) {
  // types: [data, result]
  ICHECK_EQ(types.size(), 2) << "Unique: expect 2 types but " << types.size() << " provided";
  ICHECK_EQ(num_inputs, 1) << "Unique: expect 1 inputs but " << num_inputs << " provided";
  auto data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

Expr MakeQuantize(Expr data, int bwidth, int dscale) {
  auto attrs = make_object<QuantAttrs>();
  attrs->bwidth = bwidth;
  attrs->dscale = dscale;
  static const Op& op = Op::Get("accel.quantize");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.quantize").set_body_typed(MakeQuantize);

RELAY_REGISTER_OP("accel.quantize")
    .describe(
        R"code(This operation returns the unique elements and the new index of each item in a given 1-D array.
    )code" TVM_ADD_FILELINE)
    .set_num_inputs(1)
    .add_argument("data", "Tensor", "The input tensor")
    .add_type_rel("AccelQuantize", AccelQuantizeRel)
    .set_support_level(3);

bool AccelDequantizeRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
               const TypeReporter& reporter) {
  // types: [data, result]
  ICHECK_EQ(types.size(), 2) << "Unique: expect 2 types but " << types.size() << " provided";
  ICHECK_EQ(num_inputs, 1) << "Unique: expect 1 inputs but " << num_inputs << " provided";
  auto data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

Expr MakeDequantize(Expr data, int bwidth, int dscale) {
  auto attrs = make_object<QuantAttrs>();
  attrs->bwidth = bwidth;
  attrs->dscale = dscale;
  static const Op& op = Op::Get("accel.dequantize");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.dequantize").set_body_typed(MakeDequantize);

RELAY_REGISTER_OP("accel.dequantize")
    .describe(
        R"code(This operation returns the unique elements and the new index of each item in a given 1-D array.
    )code" TVM_ADD_FILELINE)
    .set_num_inputs(1)
    .add_argument("data", "Tensor", "The input tensor")
    .add_type_rel("AccelDequantize", AccelDequantizeRel)
    .set_support_level(3);

bool AccelGELURel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
               const TypeReporter& reporter) {
  // types: [data, result]
  ICHECK_EQ(types.size(), 2) << "Unique: expect 2 types but " << types.size() << " provided";
  ICHECK_EQ(num_inputs, 1) << "Unique: expect 1 inputs but " << num_inputs << " provided";
  auto data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

Expr MakeGELU(Expr data) {
  static const Op& op = Op::Get("accel.gelu");
  return Call(op, {data}, Attrs(), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.gelu").set_body_typed(MakeGELU);

RELAY_REGISTER_OP("accel.gelu")
    .describe(
        R"code(This operation returns the unique elements and the new index of each item in a given 1-D array.
    )code" TVM_ADD_FILELINE)
    .set_num_inputs(1)
    .add_argument("data", "Tensor", "The input tensor")
    .add_type_rel("AccelGELU", AccelGELURel)
    .set_support_level(3);

}

}
}
