''' fpga jit template for codegen
[
    {
        "node_type" : "call",
        "static" : {
            "name" : name,
            "type" : OpType.xxx,
            "attrs" : [xxx]
        }, # static global variable
        "callop" : [
            op1,
            op2,
        ], # string op function list in run module
        "params" : [
            op1,
            op2,
        ], # op function in param module
        "return" : {
            "name" : name,
            "type" : OpType.xxx,
            "shape" : shape,
            "free" : free
        } # used as arg for the next operator
          # most of the time not used in codegen
    }
]
'''
from ..fir import *

class FPGAJit(Functor):

    class Visitor(Functor):

        def get_num_memo(self, fexpr):
            self.visit(fexpr)
            self.num_memo = {}
            for expr, memo in self.memo_map.items():
                self.num_memo[expr] = memo[1]
            return self.num_memo

    def Jit(self, fmod):
        self.tin = fmod.tin
        self.tout = fmod.tout
        self.jit_memo = []
        self.num_memo = self.Visitor().get_num_memo(fmod.fexpr)
        self.visit(fmod.fexpr)
        return self.jit_memo


    def visit_var(self, var):
        name = var.expr_name
        ret = var.op.fpga_jit(name)
        ret["node_type"] = "var"
        self.jit_memo.append(ret)
        return ret["return"]


    def visit_cvar(self, cvar):
        name = cvar.expr_name
        byte = cvar.data.size*2 # *2 means saved as short
        ret = cvar.op.fpga_jit(name, byte)
        ret["data"] = [ret["return"]["name"], cvar.data]
        ret["node_type"] = "const"
        self.jit_memo.append(ret)
        return ret["return"]

    
    def visit_call(self, call):
        args = [self.visit(arg) for arg in call.args]
        name = call.expr_name.replace("%", "")
        call.attrs["tin"], call.attrs["tout"] = self.tin, self.tout
        ret = call.op.fpga_jit(name, args, call.attrs)
        if self.num_memo[call] == 1:
            ret["return"]["free"] = 1
        else:
            self.num_memo[call] -= 1
            ret["return"]["free"] = 0
        if call.op.name == "output":
            ret["node_type"] = "output"
        else:
            ret["node_type"] = "call"
        self.jit_memo.append(ret)
        return ret["return"]
    

    def visit_extern(self, call):
        args = [self.visit(arg) for arg in call.args]
        name = call.expr_name.replace("%", "")
        ret = call.op.fpga_jit(name, args, call.attrs)
        ret["node_type"] = "extern"
        self.jit_memo.append(ret)
        return ret["return"]