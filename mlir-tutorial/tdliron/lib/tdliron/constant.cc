#include "tdliron/Tdliron.h"

using namespace mlir;
using namespace tdliron;


mlir::LogicalResult ConstantOp::verify() {
  return mlir::success();
}

int64_t ConstantOp::get_Bytes() {
  int64_t bytes = 0;
  if (getHbmAttr()) {
    auto out_tensor = getOutput().getType().cast<RankedTensorType>();
    auto out_shape = out_tensor.getShape();
    bytes = 100;
  } else {
    bytes = 10;
  }
  return bytes;
}