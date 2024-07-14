#ifndef TDLIRON_OPS_TD
#define TDLIRON_OPS_TD

include "mlir/IR/CommonAttrConstraints.td"
include "mlir/IR/BuiltinAttributes.td"
include "mlir/IR/BuiltinTypes.td"
include "mlir/IR/OpBase.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/BuiltinTypeInterfaces.td"
include "mlir/IR/BuiltinDialect.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/IR/RegionKindInterface.td"

include "interface/ShapeInterface.td"

def TdlironDialect : Dialect {
  let name = "tdliron";
  let cppNamespace = "::tdliron";
  let summary = "Tdliron Dialect";
  // let useDefaultTypePrinterParser = true; // 新增
  // let useDefaultAttributePrinterParser = 1;
}

/*
class TdlironType<string name, list<Trait> traits=[]>: TypeDef<TdlironDialect, name, traits>;

def Feature : TdlironType<"Feature"> {
  let mnemonic = "feature";
  let summary = "Feature data type of accelerator";
  let description = [{
  }];
  let parameters = (ins
    ArrayRefParameter<"int64_t">:$shape,
    "Type":$elementType
  );
  let assemblyFormat = "`<` $shape `, ` $elementType `>`";
}

def Weight : TdlironType<"Weight"> {
  let mnemonic = "weight";
  let summary = "Weight data type of accelerator";
  let description = [{
  }];
  let parameters = (ins
    ArrayRefParameter<"int64_t">:$shape,
    "Type":$elementType
  );
  let assemblyFormat = "`<` $shape `, ` $elementType `>`";
}
*/

class Hbm_Attr<string attrName, string attrMnemonic, list<Trait> traits = []>
    : AttrDef<TdlironDialect, attrName, traits> {
  let mnemonic = attrMnemonic;
}

def Hbm_RtMemAttr : Hbm_Attr<"RtMem", "rtm"> {
  let summary = "Runtime Memory";
  let parameters = (ins
    "int64_t":$addr,
    "int64_t":$size,
    "int64_t":$id,
    "bool":$hbm
  );
  let assemblyFormat = "`<` struct(params) `>`";
}

class Hbm_Op<string mnemonic, list<Trait> traits = []> :
  Op<TdlironDialect, mnemonic, traits>;
  
def F16TensorOrNone: AnyTypeOf<[F16Tensor, NoneType]>;

def Feature: AnyTypeOf<[3DTensorOf<[F16]>, 4DTensorOf<[F16]>]>; // <H, C//Tout, W, Tout>
def Weight: 2DTensorOf<[I8]>; // <x, 32>

def Hbm_MVMSingleOp: Hbm_Op<"mvm_single", []> {
  let summary = "matrix multipiler operator";

  let description = [{
  }];

  let arguments = (ins
    Feature:$input,
    Weight:$weight,
    Feature:$output,
    AnyTypeOf<[UI32, NoneType]>:$bn,
    AnyTypeOf<[UI32, NoneType]>:$res,

    I32ArrayAttr:$chin,
    I32ArrayAttr:$chout
  );
}

def Hbm_MVMOp: Hbm_Op<"mvm", [
    DeclareOpInterfaceMethods<ShapeInferenceOpInterface>
  ]> {
  let summary = "matrix multipiler operator";

  let description = [{
  }];

  let arguments = (ins
    F16Tensor:$input,
    I32Tensor:$weight
  );

  let results = (outs F16Tensor:$output);
  let extraClassDeclaration = [{
    void assign_fw_param(void *param);
  }];
  let hasVerifier = true;
}

def Hbm_ConstantOp : Hbm_Op<"const", []> {
  let summary = "hbm constant operator";
  let arguments = (ins 
    DefaultValuedAttr<BoolAttr, "true">:$hbm
  );

  let results = (outs AnyTensor:$output);
  let extraClassDeclaration = [{
    int64_t get_Bytes();
  }];
  let hasVerifier = true;
  // let hasCanonicalizer = 1;
}

/*
def ReturnOp : Hbm_Op<"ret", [Terminator, ReturnLike]> {
  let summary = "return operation";
  let arguments = (ins AnyType:$data);
  let assemblyFormat = "$data attr-dict `:` type($data)";
}
*/


#endif
