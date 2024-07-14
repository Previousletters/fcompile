//===- ShapeInferencePass.cpp - Shape Inference ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a Function level pass performing interprocedural
// propagation of array shapes through function specialization.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include "mlir/Pass/Pass.h"
#include "tdliron/Passes.h"
#include "tdliron/Tdliron.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
namespace {

void setAddress(Value v, int64_t addr) {
  auto type = v.getType().cast<RankedTensorType>();
  Builder builder(v.getContext());
  auto addrAttr = builder.getI64IntegerAttr(addr);
  auto new_type =
      RankedTensorType::get(type.getShape(), type.getElementType(), addrAttr);
  v.setType(new_type);
}

struct AddressAssignPass
    : public mlir::PassWrapper<AddressAssignPass, OperationPass<func::FuncOp>> {
  StringRef getArgument() const final { return "addrassign"; }
  void runOnOperation() override {
    auto f = getOperation();
    int64_t hbm_addr = 0;
    f.walk([&](tdliron::ConstantOp op) {
      setAddress(op.getOutput(), hbm_addr);
      hbm_addr += op.get_Bytes();
    });
  }
};
};

std::unique_ptr<mlir::Pass> tdliron::createAddressAssignPass() {
  return std::make_unique<AddressAssignPass>();
}