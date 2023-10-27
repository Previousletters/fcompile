#ifndef __TVM_CUSTOM_ACCEL_H__
#define __TVM_CUSTOM_ACCEL_H__

#include <tvm/ir/attrs.h>
#include <tvm/relay/base.h>
#include <tvm/relay/expr.h>

#include <string>

namespace tvm {
namespace relay {

/*! \brief data type cast */
struct QuantAttrs : public tvm::AttrsNode<QuantAttrs> {
  int bwidth;
  int dscale;

  TVM_DECLARE_ATTRS(QuantAttrs, "relay.attrs.accel.QuantAttrs") {
    TVM_ATTR_FIELD(bwidth).describe("bit width");
    TVM_ATTR_FIELD(dscale).describe("data scale");
  }
};  // struct CastAttrs.

/*! \brief Attributes used in convolution operators */
struct AccelOpAttrs : public tvm::AttrsNode<AccelOpAttrs> {
  Array<IndexExpr> strides;
  Array<IndexExpr> padding;
  Array<IndexExpr> kernel_size;
  tvm::String data_layout;
  tvm::String kernel_layout;
  tvm::String out_layout;
  Array<IndexExpr> widths;
  Array<IndexExpr> scales;
  IndexExpr activate;

  TVM_DECLARE_ATTRS(AccelOpAttrs, "relay.attrs.accel.OpAttrs") {
    TVM_ATTR_FIELD(strides)
        .set_default(Array<IndexExpr>({1, 1}))
        .describe("Specifies the strides of the convolution.");
    TVM_ATTR_FIELD(padding)
        .set_default(Array<IndexExpr>({0, 0}))
        .describe(
            "If padding is non-zero, then the input is implicitly zero-padded"
            "Padding support both symmetric and asymmetric as"
            "one int : same padding used on all sides"
            "two int : bottom, right will use same padding as top, left"
            "four int : padding width in the order of (top, left, bottom, right)");
    TVM_ATTR_FIELD(kernel_size)
        .describe("Specifies the dimensions of the convolution window.")
        .set_default(NullValue<Array<IndexExpr>>());
    TVM_ATTR_FIELD(data_layout)
        .set_default("NHWC")
        .describe(
            "Dimension ordering of input data. Can be 'NCHW', 'NHWC', etc."
            "'N', 'C', 'H', 'W' stands for batch, channel, height, and width"
            "dimensions respectively. Convolution is applied on the 'H' and"
            "'W' dimensions.");
    TVM_ATTR_FIELD(kernel_layout)
        .set_default("HWIO")
        .describe(
            "Dimension ordering of weight. Can be 'OIHW', 'OIHW16o16i', etc."
            "'O', 'I', 'H', 'W' stands for num_filter, input_channel, height, and width"
            "dimensions respectively.");
    TVM_ATTR_FIELD(out_layout)
        .set_default("NHWC")
        .describe(
            "Dimension ordering of output. Can be 'NCHW', 'NHWC', etc."
            "'N', 'C', 'H', 'W' stands for batch, channel, height, and width"
            "dimensions respectively. Default to be same as input layout.");
    TVM_ATTR_FIELD(widths)
        .describe("The enable list for widths and scales.")
        .set_default(NullValue<Array<IndexExpr>>());
    TVM_ATTR_FIELD(scales)
        .describe("The enable list for widths and scales.")
        .set_default(NullValue<Array<IndexExpr>>());
    TVM_ATTR_FIELD(activate)
        .describe("The enable list for widths and scales.")
        .set_default(NullValue<IndexExpr>());
  }
};
}  // namespace relay
}  // namespace tvm

#endif
