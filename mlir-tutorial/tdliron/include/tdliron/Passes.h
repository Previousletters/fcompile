#ifndef MLIR_TDLIRON_PASSES_H
#define MLIR_TDLIRON_PASSES_H

#include <memory>

namespace tdliron {
// 先生成定义

#define GEN_PASS_DECL
#include "tdliron/Passes.h.inc"

// 在写 create 函数表
std::unique_ptr<mlir::Pass> createInferShapePass();

std::unique_ptr<mlir::Pass> createAddressAssignPass();

// 生成注册函数
#define GEN_PASS_REGISTRATION
#include "tdliron/Passes.h.inc"
};

#endif // MLIR_TUTORIAL_TOY_PASSES_H