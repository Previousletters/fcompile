#include "interface/ShapeInterface.h"
#include "tdliron/Tdliron.h"

using namespace mlir;
using namespace tdliron;

mlir::LogicalResult MVMOp::verify() {
  return mlir::success();
}


void MVMOp::shape_inference() {
  auto inp_tensor = getInput().getType().cast<RankedTensorType>();
  auto inw_tensor = getWeight().getType().cast<RankedTensorType>();
  auto inp_shape = inp_tensor.getShape();
  auto inw_shape = inw_tensor.getShape();
  int inp_dims = inp_shape.size();
  int inw_dims = inw_shape.size();
  int64_t chout;
  int64_t tout = inp_shape[inp_dims-1];
  SmallVector<int64_t> out_shape;
  if (inw_dims == 2) {
    chout = inw_shape[0];
    out_shape.push_back(((chout+tout-1)/tout));
    out_shape.push_back(inp_shape[inp_dims-2]);
    out_shape.push_back(inp_shape[inp_dims-1]);
  } else {
    for (int i = 0; i < inw_dims-1; i++) {
      out_shape.push_back(inw_shape[i]);
    }
    out_shape.push_back(inp_shape[inp_dims-2]);
    out_shape.push_back(inp_shape[inp_dims-1]);   
  }
  getOutput().setType(RankedTensorType::get(out_shape, inp_tensor.getElementType()));
}


/*
void MVMSingleOp::shape_inference() {
  auto inp_tensor = getInput().getType().cast<RankedTensorType>();
  auto inw_tensor = getWeight().getType().cast<RankedTensorType>();
  auto inp_shape = inp_tensor.getShape();
  auto inw_shape = inw_tensor.getShape();
  int inp_dims = inp_shape.size();
  int inw_dims = inw_shape.size();
  int64_t chout;
  int64_t tout = inp_shape[inp_dims-1];
  SmallVector<int64_t> out_shape;
  if (inw_dims == 2) {
    chout = inw_shape[0];
    out_shape.push_back(((chout+tout-1)/tout));
    out_shape.push_back(inp_shape[inp_dims-2]);
    out_shape.push_back(inp_shape[inp_dims-1]);
  } else {
    for (int i = 0; i < inw_dims-1; i++) {
      out_shape.push_back(inw_shape[i]);
    }
    out_shape.push_back(inp_shape[inp_dims-2]);
    out_shape.push_back(inp_shape[inp_dims-1]);   
  }
  getOutput().setType(RankedTensorType::get(out_shape, inp_tensor.getElementType()));
}
*/