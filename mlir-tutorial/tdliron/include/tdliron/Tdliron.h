#pragma once

#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Builders.h"

#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/IR/RegionKindInterface.h"

#include "interface/ShapeInterface.h"

#define GET_TYPEDEF_CLASSES
#include "tdliron/TdlironTypes.h.inc"

#define GET_OP_CLASSES
#include "tdliron/Tdliron.h.inc"
