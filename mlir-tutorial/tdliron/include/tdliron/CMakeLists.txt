add_mlir_dialect(Tdliron tdliron)

set(LLVM_TARGET_DEFINITIONS Passes.td)
mlir_tablegen(Passes.h.inc -gen-pass-decls -name Hbm)
add_public_tablegen_target(MLIRHbmPassIncGen)