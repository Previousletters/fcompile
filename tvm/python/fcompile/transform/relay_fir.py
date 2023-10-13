import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple

from ..op import Input, FPGA_OP_MAP, TVMOp
from ..fir import FVar, FCall, FExtern


class RelayFIR(ExprFunctor):

    def convert(self, mod):
        return self.visit(mod["main"])
    
    def get_id(self):
        if hasattr(self, "id"):
            self.id += 1
            return self.id
        else:
            self.id = 0
            return self.id

    def visit_function(self, func):
        return self.visit(func.body)

    def visit_call(self, call):
        call_op = str(call.op)
        dshape = get_const_tuple(call.checked_type.shape)
        dtype = call.checked_type.dtype
        if call_op in FPGA_OP_MAP.keys():
            funct = FPGA_OP_MAP[call_op](dshape, dtype)
            attrs = dict(call.attrs)
            args = [self.visit(arg) for arg in call.args]
            id_ = self.get_id()
            return FCall(args, funct, attrs, id_)
        else:
            attrs = dict(call.attrs)
            args = [self.visit(arg) for arg in call.args]
            var_ = []
            for n, arg in enumerate(args):
                var = relay.var(f"var{n}", shape=arg.op.shape, dtype=arg.op.dtype)
                var_.append(var)
            new_call = relay.Call(call.op, var_, call.attrs, call.type_args)
            id_ = self.get_id()
            funct = TVMOp(var_, new_call, dshape, dtype, id_)
            return FExtern(args, funct, attrs, id_)

    def visit_var(self, var):
        dshape = get_const_tuple(var.checked_type.shape)
        dtype = var.checked_type.dtype
        funct = Input(dshape, dtype)
        return FVar(var.name_hint, funct)
