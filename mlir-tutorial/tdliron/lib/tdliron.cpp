#include "mlir/IR/Types.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/Transforms/InliningUtils.h"

#include "interface/ShapeInterface.h"
#include "tdliron/Tdliron.h"

#include "tdliron/TdlironDialect.h.inc"
#include "tdliron/TdlironDialect.cpp.inc"
#define GET_OP_CLASSES
#include "tdliron/Tdliron.cpp.inc"

#include "mlir/IR/DialectImplementation.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace tdliron;

void TdlironDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "tdliron/Tdliron.cpp.inc"
  >();
}

#define GET_TYPEDEF_CLASSES
#include "tdliron/TdlironTypes.cpp.inc"

// void TdlironDialect::registerTypes() {
//   addTypes<
// #define GET_TYPEDEF_LIST
// #include "tdliron/TdlironTypes.cpp.inc"
//   >();
// }

/*
void MVMOp::shape_inference() {
  std::vector<int64_t> inp_shape = module::getShape(getInput());
  std::vector<int64_t> inw_shape = module::getShape(getWeight());
  int inp_dims = inp_shape.size();
  int inw_dims = inw_shape.size();
  int64_t chout;
  int64_t tout = inp_shape[inp_dims-1];
  std::vector<int64_t> out_shape;
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
  module::setShapeOrVerify(getOutput(), out_shape);
}
*/