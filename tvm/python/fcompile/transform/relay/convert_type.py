import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple

from .register import register_transform

class ConvertType(ExprMutator):

    def clean_name(self, name):
        if "." in name:
            return name.replace(".", "_")
        return name

    def convert(self, mod):
        self.memo = {}
        self.new_vars = []
        expr = mod["main"].body
        new_expr = self.visit(expr)
        func = relay.Function(self.new_vars, new_expr)
        new_mod = tvm.IRModule.from_expr(func)
        return new_mod

    def visit_var(self, var):
        shape = var.type_annotation.shape
        name = self.clean_name(var.name_hint)
        new_var = relay.var(name, shape=shape, dtype="int8")
        self.new_vars.append(new_var)
        if hasattr(self, "bwidth") and hasattr(self, "dscale"):
            self.memo[var.name_hint] = [self.bwidth, self.dscale, name]
            del self.bwidth, self.dscale
        return new_var

    def visit_call(self, call):
        if str(call.op) == "reshape":
            newshape = list(call.attrs["newshape"])
            newshape = tuple([1 if i == -1 else i for i in newshape])
            new_call = relay.reshape(call.args[0], newshape)
        elif str(call.op) == "accel.quantize":
            self.bwidth = int(call.attrs["bwidth"])
            self.dscale = int(call.attrs["dscale"])
            new_call = relay.Call(call.op, call.args, call.attrs)
        else:
            new_call = relay.Call(call.op, call.args, call.attrs)
        return super().visit_call(new_call)


    def visit_constant(self, const):
        const = super().visit_constant(const)
        new_data = tvm.nd.array(const.data.asnumpy().astype("int8"))
        new_const = relay.Constant(new_data)
        return new_const


@register_transform("convert_type")
def transform(mod, params):
    convert = ConvertType()
    mod = convert.convert(mod)
    new_params = {}
    for name, prec in convert.memo.items():
        #TODO: make process more suitable for overflow
        if name in params.keys():
            data = params[name].asnumpy() * (2**prec[1])
            new_params[prec[2]] = tvm.nd.array(data.astype("int8"))
    return mod, new_params
