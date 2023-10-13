import numpy as np
import json

from .op_base import Op
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..utils import list2str

import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform
from tvm.contrib import graph_executor

class TVMOp(Op):

    name = "tvm_extern"
    arg_types = [[OpType.t_val]]
    ret_type = OpType.t_val

    def __init__(self, var_, expr, shape, dtype, id):
        super().__init__(shape, dtype)
        self.var_ = var_
        self.expr = expr
        self.func_name = self.name + str(id)


    def __str__(self):
        func = relay.Function(self.var_, self.expr)
        mod = IRModule.from_expr(func)
        mod = transform.InferType()(mod)
        mod_str = str(mod).replace("main", self.func_name)
        return mod_str


    def __call__(self, args):
        dev = tvm.cpu(0)
        inputs = {}
        for n, arg in enumerate(args):
            inputs[f"var{n}"] = tvm.nd.array(arg, dev)
        func = relay.Function(self.var_, self.expr)
        mod = IRModule.from_expr(func)
        mod = transform.InferType()(mod)
        lib = relay.build(mod, "llvm")
        m = graph_executor.GraphModule(lib["default"](dev))
        m.run(params=inputs)
        tvm_output = m.get_output(0)
        return tvm_output.numpy()


    def fpga_jit(self, name, args, attrs):
        ret = {}
        ret_name = "tvm_" + name
        ptr_name = "op_" + name
        tvm_name = "tvalue_" + name
        arg_type_name = "arg_type_ids_" + name
        byte = shape2byte(self.shape, self.dtype)
        dlt_str = CreateDLTensor(ret_name, ptr_name, self.shape)
        args_str = [arg["name"] for arg in args] + [ret_name]
        tvl_str = f"TVMValue {tvm_name}[{len(args_str)}];"
        arg_type_ids = f"int32_t {arg_type_name}[{len(args_str)}] = " + \
                        "{" + list2str([3 for i in args_str]) + "};"
        tvl_ptr_str = [f"{tvm_name}[{i}].v_handle = &{arg};" for i, arg in enumerate(args_str)]
        func_str = f"{self.func_name}({tvm_name}, {arg_type_name});"
        ret["static"] = {"name" : ptr_name, "type" : OpType.c_ptr, "attrs" : [byte]}
        ret["callop"] = dlt_str + [tvl_str, arg_type_ids] + tvl_ptr_str + [func_str]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape, "pointer" : ptr_name}
        ret["extern"] = [f"{self.func_name}(void*, int32_t*);", self.get_source()]
        return ret

    
    def get_source(self):
        func = relay.Function(self.var_, self.expr)
        mod = IRModule.from_expr(func)
        mod = transform.InferType()(mod)

        with tvm.transform.PassContext(
            opt_level=9, config={"tir.disable_vectorize": True}
        ):
            graph, c_mod, _ = relay.build(mod, "c")
        
        graph = json.loads(graph)
        funcs = [node["name"] for node in graph["nodes"] if node["op"] == "tvm_op"]

        source = c_mod.get_source().split("\n")
        dis_codes = ["#define TVM_EXPORTS", "extern \"C\"", "void* __tvm_module_ctx = NULL;"]
        new_source = []
        for s in source:
            if s not in dis_codes:
                if funcs[0] in s:
                    s = s.replace("TVM_DLL", "static")
                new_source.append(s)

        api_source = """
void %(func_name)s(void* args, int32_t* arg_type_ids) {
  %(func)s(args, arg_type_ids, 3, 0, 0, 0);
}""" %{"func_name" : self.func_name, "func" : funcs[0]}
        return "\n".join(new_source) + api_source



