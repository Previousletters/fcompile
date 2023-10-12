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
  IndexExpr pad_h = param->padding[0] + param->padding[1];
  IndexExpr pad_w = param->padding[2] + param->padding[3];
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

}

}
}
