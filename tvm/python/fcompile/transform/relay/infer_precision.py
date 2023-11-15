import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple

from .register import register_transform

class GetDequantize(ExprVisitor):

    def get(self, expr):
        self.visit(expr)
        if hasattr(self, "bwidth"):
            return [self.bwidth, self.dscale]
        else:
            return None

    def visit_call(self, call):
        if str(call.op) == "accel.dequantize":
            self.bwidth = int(call.attrs["bwidth"])
            self.dscale = int(call.attrs["dscale"])
        elif str(call.op) != "accel.quantize":
            super().visit_call(call)


class InferPrecision(ExprMutator):

    def convert(self, mod):
        self.memo = {}
        self.new_vars = []
        expr = mod["main"].body
        new_expr = self.visit(expr)
        func = relay.Function(self.new_vars, new_expr)
        new_mod = tvm.IRModule.from_expr(func)
        return new_mod

    def visit_var(self, var):
        var = super().visit_var(var)
        self.new_vars.append(var)
        return var

    def visit_call(self, call):
        if str(call.op) == "multiply" and hasattr(self, "bwidth"):
            qprecision = GetDequantize().get(call)
            if qprecision == None:
                return super().visit_call(call)
            dquant = relay.accel.base.quantize(call.args[0], qprecision[0], qprecision[1])
            wquant = relay.accel.base.quantize(call.args[1], 8, 8)
            output = relay.multiply(dquant, wquant)
            new_call = relay.accel.base.dequantize(output, self.bwidth, self.dscale)
            del self.bwidth, self.dscale
        elif str(call.op) == "transpose" and hasattr(self, "bwidth"):
            axes, offset = [int(i) for i in call.attrs["axes"]], 1
            for n in range(1, len(axes)):
                if axes[n] == len(axes) - offset:
                    offset += 1
            if offset != 3:
                return super().visit_call(call)
            qprecision = GetDequantize().get(call)
            if qprecision == None:
                return super().visit_call(call)
            dquant = relay.accel.base.quantize(call.args[0], qprecision[0], qprecision[1])
            output = relay.transpose(dquant, call.attrs["axes"])
            new_call = relay.accel.base.dequantize(output, self.bwidth, self.dscale)
            del self.bwidth, self.dscale
        elif str(call.op) == "accel.gelu" and hasattr(self, "bwidth"):
            qprecision = GetDequantize().get(call)
            if qprecision == None:
                return super().visit_call(call)
            dquant = relay.accel.base.quantize(call.args[0], qprecision[0], qprecision[1])
            output = relay.accel.base.gelu(dquant)
            new_call = relay.accel.base.dequantize(output, self.bwidth, self.dscale)
            del self.bwidth, self.dscale
        elif str(call.op) == "accel.quantize":
            self.bwidth = int(call.attrs["bwidth"])
            self.dscale = int(call.attrs["dscale"])
            new_call = call
        else:
            new_call = call
        return super().visit_call(new_call)

@register_transform("infer_precision")
def transform(mod, params):
    mod = InferPrecision().convert(mod)
    return mod, params
