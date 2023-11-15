#include <tvm/relay/attrs/accel.h>
#include <tvm/relay/op.h>
#include <tvm/tir/analysis.h>
#include <tvm/tir/data_layout.h>

#include <vector>

namespace tvm {
namespace relay {

TVM_REGISTER_NODE_TYPE(AccelOpAttrs);

namespace vit {

Expr MakeAccelVitConv2D(Expr data, Expr weight, Array<IndexExpr> strides, Array<IndexExpr> padding,
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
  const Op& op = Op::Get("accel.vit.conv2d");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_conv2d")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> strides, Array<IndexExpr> padding,
                    Array<IndexExpr> kernel_size, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelVitConv2D(data, weight, strides, padding, kernel_size, widths, scales, activate);
    });

bool AccelVitConv2DRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.conv2d")
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
    .add_type_rel("AccelVitConv2D", AccelVitConv2DRel);

Expr MakeAccelVitMM(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
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
  const Op& op = Op::Get("accel.vit.mm");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_mm")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelVitMM(data, weight, widths, scales, activate);
    });

bool AccelVitMMRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.mm")
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
    .add_type_rel("AccelVitMM", AccelVitMMRel);

Expr MakeAccelVitSoftmax(Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
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
  const Op& op = Op::Get("accel.vit.softmax");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_softmax")
    .set_body_typed([](Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelVitSoftmax(data, widths, scales, activate);
    });

bool AccelVitSoftmaxRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 2);
  const auto* data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  //TODO: trans_layout 
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.vit.softmax")
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
    .add_type_rel("AccelVitSoftmax", AccelVitSoftmaxRel);

Expr MakeAccelVitTranspose(Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales) {
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
  const Op& op = Op::Get("accel.vit.transpose");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_transpose")
    .set_body_typed([](Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales) {
      return MakeAccelVitTranspose(data, widths, scales);
    });

bool AccelVitTransposeRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.transpose")
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
    .add_type_rel("AccelVitTranspose", AccelVitTransposeRel);

Expr MakeAccelVitLayerNorm(Expr data, Expr k_bias, Array<IndexExpr> widths, 
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
  const Op& op = Op::Get("accel.vit.layer_norm");
  return Call(op, {data, k_bias}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_layer_norm")
    .set_body_typed([](Expr data, Expr k_bias, Array<IndexExpr> widths, 
                       Array<IndexExpr> scales) {
      return MakeAccelVitLayerNorm(data, k_bias, widths, scales);
    });

bool AccelVitLayerNormRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.layer_norm")
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
    .add_type_rel("AccelVitLayerNorm", AccelVitLayerNormRel);

Expr MakeAccelVitConv2DAdd(Expr data, Expr weight, Expr res_add, Array<IndexExpr> strides, 
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
  const Op& op = Op::Get("accel.vit.conv2d_add");
  return Call(op, {data, weight, res_add}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_conv2d_add")
    .set_body_typed([](Expr data, Expr weight, Expr res_add, Array<IndexExpr> strides, 
                    Array<IndexExpr> padding, Array<IndexExpr> kernel_size, Array<IndexExpr> widths, 
                    Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelVitConv2DAdd(data, weight, res_add, strides, padding, kernel_size, 
                                   widths, scales, activate);
    });

bool AccelVitConv2DAddRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.conv2d_add")
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
    .add_type_rel("AccelVitConv2DAdd", AccelVitConv2DAddRel);

Expr MakeAccelVitActivate(Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
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
  const Op& op = Op::Get("accel.vit.activation");
  return Call(op, {data}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_activation")
    .set_body_typed([](Expr data, Array<IndexExpr> widths, Array<IndexExpr> scales, IndexExpr activate) {
      return MakeAccelVitActivate(data, widths, scales, activate);
    });

bool AccelVitActivateRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
                       const TypeReporter& reporter) {
  ICHECK_EQ(types.size(), 2);
  const auto* data = types[0].as<TensorTypeNode>();
  if (data == nullptr) return false;
  reporter->Assign(types[1], TensorType(data->shape, data->dtype));
  return true;
}

RELAY_REGISTER_OP("accel.vit.activation")
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
    .add_type_rel("AccelVitActivate", AccelVitActivateRel);

Expr MakeAccelVitAdd(Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
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
  const Op& op = Op::Get("accel.vit.add");
  return Call(op, {data, weight}, Attrs(attrs), {});
}

TVM_REGISTER_GLOBAL("relay.op.accel._make.vit_add")
    .set_body_typed([](Expr data, Expr weight, Array<IndexExpr> widths, Array<IndexExpr> scales, 
                    IndexExpr activate) {
      return MakeAccelVitAdd(data, weight, widths, scales, activate);
    });

bool AccelVitAddRel(const Array<Type>& types, int num_inputs, const Attrs& attrs,
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

RELAY_REGISTER_OP("accel.vit.add")
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
    .add_type_rel("AccelVitAdd", AccelVitAddRel);


}


}
}
