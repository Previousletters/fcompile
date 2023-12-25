#include <tvm/relay/attrs/accel.h>
#include <tvm/relay/op.h>
#include <tvm/tir/analysis.h>
#include <tvm/tir/data_layout.h>

#include <vector>

namespace tvm {
namespace relay {

TVM_REGISTER_NODE_TYPE(AccelOpAttrs);

namespace hbm {

Expr MakeAccelHBMConv2D(Expr data, Expr weight, Array<IndexExpr> strides, Array<IndexExpr> padding,
                    Array<IndexExpr> kernel_size, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = strides;
  attrs->padding = padding;
  attrs->kernel_size = kernel_size;
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.conv2d");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_conv2d")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> strides, Array<IndexExpr> padding,
                    Array<IndexExpr> kernel_size, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelHBMConv2D(data, weight, strides, padding, kernel_size, widths, scales, activate);
    });

bool AccelHBMConv2DRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  auto param = attrs.as<AccelOpAttrs>();
  //TODO: trans_layout 
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  Array<IndexExpr> wshape_hwio = weight->shape; // HWIO
  Array<IndexExpr> oshape({dshape_nhwc[0], 0, 0, wshape_hwio[3]});
  IndexExpr pad_h = param->padding[0]*2;
  IndexExpr pad_w = param->padding[1]*2;
  oshape.Set(1, indexdiv(dshape_nhwc[1] + pad_h - wshape_hwio[0], param->strides[0]) + 1);
  oshape.Set(2, indexdiv(dshape_nhwc[2] + pad_w - wshape_hwio[1], param->strides[1]) + 1);
  reporter->Assign(types[2], TensorType(oshape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.conv2d")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The weight tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMConv2D", AccelHBMConv2DRel);

Expr MakeAccelHBMMatMul(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.matmul");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_matmul")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelHBMMatMul(data, weight, widths, scales, activate);
    });

bool AccelHBMMatMulRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  //TODO: trans_layout 
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  Array<IndexExpr> wshape_hwio = weight->shape; // HWIO
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[3], wshape_hwio[2])) return false;
  Array<IndexExpr> oshape({dshape_nhwc[0], dshape_nhwc[1], dshape_nhwc[2], wshape_hwio[3]});
  reporter->Assign(types[2], TensorType(oshape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.matmul")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The weight tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMMatMul", AccelHBMMatMulRel);

Expr MakeAccelHBMLinear(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.linear");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_linear")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelHBMLinear(data, weight, widths, scales, activate);
    });

bool AccelHBMLinearRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  //TODO: trans_layout 
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  Array<IndexExpr> wshape_hwio = weight->shape; // HWIO
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[3], wshape_hwio[2])) return false;
  Array<IndexExpr> oshape({dshape_nhwc[0], dshape_nhwc[1], dshape_nhwc[2], wshape_hwio[3]});
  reporter->Assign(types[2], TensorType(oshape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.linear")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The weight tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMLinear", AccelHBMLinearRel);


Expr MakeAccelHBMSoftmax(Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.softmax");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_softmax")
    .set_body_typed([](Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelHBMSoftmax(data, widths, scales, activate);
    });

bool AccelHBMSoftmaxRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 2);
  const auto* data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  //TODO: trans_layout 
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.softmax")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(1)
    .add_argument("data", "Tensor", "The input tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMSoftmax", AccelHBMSoftmaxRel);

Expr MakeAccelHBMTranspose(Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = 0;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.transpose");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_transpose")
    .set_body_typed([](Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales) {
      return MakeAccelHBMTranspose(data, widths, scales);
    });

bool AccelHBMTransposeRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 2);
  const auto* data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  //TODO: trans_layout 
  Array<IndexExpr> oshape({dshape_nhwc[0], dshape_nhwc[1], dshape_nhwc[3], dshape_nhwc[2]});
  reporter->Assign(types[1], TensorType(oshape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.transpose")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(1)
    .add_argument("data", "Tensor", "The input tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMTranspose", AccelHBMTransposeRel);

Expr MakeAccelHBMLayerNorm(Expr data, Expr k_bias, Array<IndexExpr> widths, 
                           Array<IndexExpr> scales) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = 0;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.layer_norm");
  return Call(op, {data, k_bias}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_layer_norm")
    .set_body_typed([](Expr data, Expr k_bias, Array<IndexExpr> widths, 
                       Array<IndexExpr> scales) {
      return MakeAccelHBMLayerNorm(data, k_bias, widths, scales);
    });

bool AccelHBMLayerNormRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* k_bias = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (k_bias == nullptr) return false;
  Array<IndexExpr> dshape = data->shape;
  Array<IndexExpr> fshape = k_bias->shape;
  //TODO: trans_layout 
  if (!tvm::tir::ExprDeepEqual()(dshape[3]*2, fshape[3])) return false;
  reporter->Assign(types[2], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.layer_norm")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("k_bias", "Tensor", "The input tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMLayerNorm", AccelHBMLayerNormRel);

Expr MakeAccelHBMRMSNorm(Expr data, Expr k_bias, Array<IndexExpr> widths, 
                           Array<IndexExpr> scales) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = 0;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.rms_norm");
  return Call(op, {data, k_bias}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_rms_norm")
    .set_body_typed([](Expr data, Expr k_bias, Array<IndexExpr> widths, 
                       Array<IndexExpr> scales) {
      return MakeAccelHBMRMSNorm(data, k_bias, widths, scales);
    });

bool AccelHBMRMSNormRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* k_bias = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (k_bias == nullptr) return false;
  Array<IndexExpr> dshape = data->shape;
  Array<IndexExpr> fshape = k_bias->shape;
  //TODO: trans_layout 
  if (!tvm::tir::ExprDeepEqual()(dshape[3]*2, fshape[3])) return false;
  reporter->Assign(types[2], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.rms_norm")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("k_bias", "Tensor", "The input tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMRMSNorm", AccelHBMRMSNormRel);

Expr MakeAccelHBMConv2DAdd(Expr data, Expr weight, Expr res_add, Array<IndexExpr> strides, 
                    Array<IndexExpr> padding, Array<IndexExpr> kernel_size, Array<IndexExpr> widths, 
                    Array<IndexExpr> scales, IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = strides;
  attrs->padding = padding;
  attrs->kernel_size = kernel_size;
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.conv2d_add");
  return Call(op, {data, weight, res_add}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_conv2d_add")
    .set_body_typed([](Expr data, Expr weight, Expr res_add, Array<IndexExpr> strides, 
                    Array<IndexExpr> padding, Array<IndexExpr> kernel_size, Array<IndexExpr> widths, 
                    Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelHBMConv2DAdd(data, weight, res_add, strides, padding, kernel_size, 
                                   widths, scales, activate);
    });

bool AccelHBMConv2DAddRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 4);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  const auto* res_add = types[2].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  if (res_add == nullptr) return false;
  auto param = attrs.as<AccelOpAttrs>();
  //TODO: trans_layout 
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  Array<IndexExpr> wshape_hwio = weight->shape; // HWIO
  Array<IndexExpr> rshape_nhwc = res_add->shape; // HWIO
  Array<IndexExpr> oshape({dshape_nhwc[0], 0, 0, wshape_hwio[3]});
  IndexExpr pad_h = param->padding[0]*2;
  IndexExpr pad_w = param->padding[1]*2;
  oshape.Set(1, indexdiv(dshape_nhwc[1] + pad_h - wshape_hwio[0], param->strides[0]) + 1);
  oshape.Set(2, indexdiv(dshape_nhwc[2] + pad_w - wshape_hwio[1], param->strides[1]) + 1);
  if (!tvm::tir::ExprDeepEqual()(oshape[0], rshape_nhwc[0])) return false;
  if (!tvm::tir::ExprDeepEqual()(oshape[1], rshape_nhwc[1])) return false;
  if (!tvm::tir::ExprDeepEqual()(oshape[2], rshape_nhwc[2])) return false;
  if (!tvm::tir::ExprDeepEqual()(oshape[3], rshape_nhwc[3])) return false;
  reporter->Assign(types[3], TensorType(oshape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.conv2d_add")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(3)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The weight tensor.")
    .add_argument("res_add", "Tensor", "The weight tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMConv2DAdd", AccelHBMConv2DAddRel);

Expr MakeAccelHBMActivate(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.activation");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_activation")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelHBMActivate(data, weight, widths, scales, activate);
    });

bool AccelHBMActivateRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  reporter->Assign(types[2], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.activation")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The input tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMActivate", AccelHBMActivateRel);

Expr MakeAccelHBMAdd(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
  auto attrs = make_object<AccelOpAttrs>();
  attrs->strides = {1, 1};
  attrs->padding = {0, 0};
  attrs->kernel_size = {1, 1};
  attrs->widths = widths;
  attrs->scales = scales;
  attrs->activate = activate;
  attrs->kernel_layout = "HWIO";
  attrs->data_layout = "NHWC";
  attrs->out_layout = "NHWC";
  const Op& op = Op::Get("accel.hbm.add");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.hbm_add")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelHBMAdd(data, weight, widths, scales, activate);
    });

bool AccelHBMAddRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 3);
  const auto* data = types[0].as<TensorTypeNode>();
  const auto* weight = types[1].as<TensorTypeNode>();
  if (data == nullptr) return false;
  if (weight == nullptr) return false;
  //TODO: trans_layout 
  Array<IndexExpr> dshape_nhwc = data->shape; // HWIO
  Array<IndexExpr> wshape_hwio = weight->shape; // HWIO
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[0], wshape_hwio[0])) return false;
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[1], wshape_hwio[1])) return false;
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[2], wshape_hwio[2])) return false;
  if (!tvm::tir::ExprDeepEqual()(dshape_nhwc[3], wshape_hwio[3])) return false;
  reporter->Assign(types[2], TensorType(dshape_nhwc, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.hbm.add")
    .describe(R"code(2D convolution layer (e.g. spatial convolution over sequences).

This layer creates a convolution kernel that is convolved
with the layer input to produce a tensor of outputs.

- **data**: This depends on the `layout` parameter. Input is 3D array of shape
            (batch_size, in_channels, width) if `layout` is `NCW`.
- **weight**: (channels, in_channels, kernel_size)
- **out**:  This depends on the `layout` parameter. Output is 3D array of shape
            (batch_size, channels, out_width) if `layout` is `NCW`.

)code" TVM_ADD_FILELINE)
    .set_attrs_type<AccelOpAttrs>()
    .set_num_inputs(2)
    .add_argument("data", "Tensor", "The input tensor.")
    .add_argument("weight", "Tensor", "The weight tensor.")
    .set_support_level(2)
    .add_type_rel("AccelHBMAdd", AccelHBMAddRel);
}


}
}
