import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple

from ..op import Input, Const, FPGA_OP_MAP, TVMOp
from ..fir import FVar, FCVar, FCall, FExtern


class RelayFIR(ExprFunctor):

    def convert(self, mod):
        self.id = 0
        return self.visit(mod["main"])

    def visit_function(self, func):
        return self.visit(func.body)

    def visit_call(self, call):
        call_op = str(call.op)
        dshape = get_const_tuple(call.checked_type.shape)
        dtype = call.checked_type.dtype
        if call.attrs == None:
            attrs = None
        else:
            attrs = dict(call.attrs)
        if call_op in FPGA_OP_MAP.keys():
            funct = FPGA_OP_MAP[call_op](dshape, dtype)
            args = [self.visit(arg) for arg in call.args]
            return FCall(args, funct, attrs)
        else:
            args = [self.visit(arg) for arg in call.args]
            var_ = []
            for n, arg in enumerate(args):
                var = relay.var(f"var{n}", shape=arg.op.shape, dtype=arg.op.dtype)
                var_.append(var)
            new_call = relay.Call(call.op, var_, call.attrs, call.type_args)
            funct = TVMOp(var_, new_call, dshape, dtype, self.id)
            self.id += 1
            return FExtern(args, funct, attrs)

    def visit_var(self, var):
        dshape = get_const_tuple(var.checked_type.shape)
        dtype = var.checked_type.dtype
        funct = Input(dshape, dtype)
        return FVar(var.name_hint, funct)

    def visit_constant(self, const):
        dshape = get_const_tuple(const.checked_type.shape)
        dtype = const.checked_type.dtype
        funct = Const(dshape, dtype)
        const_name = f"const_{self.id}"
        self.id += 1
        return FCVar(const_name, funct, const.data.asnumpy())
