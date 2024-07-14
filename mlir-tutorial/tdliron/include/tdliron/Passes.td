#ifndef TDLIRON_PASS
#define TDLIRON_PASS

include "mlir/IR/OpBase.td"
include "mlir/Pass/PassBase.td"

def InferShape : Pass<"infer-shape-type"> {
  let summary = "inference shape and type";
  let constructor = "tdliron::createInferShapePass()";
}

def AddressAssign : Pass<"address-assign", "ModuleOp"> {
  let summary = "assign address in ddr/hbm";
  let constructor = "tdliron::createAddressAssignPass()";
  let options = [
    Option<"reuse_addr", "reuse_addr", "bool", /*default=*/"true",
           "reuse tensor memory.">,
    Option<"merge_weight", "merge_weight", "bool", /*default=*/"false",
           "merge weight memory.">,
    Option<"compress_weight", "compress_weight", "bool", /*default=*/"true",
           "compress weight memory.">,
    Option<"weight_map_file", "weight_map_file", "std::string", /*default=*/"\"_weight_map.csv\"",
           "record weight offset with its name into a csv map file.">,
  ];
}

#endif // TDLIRON_PASS