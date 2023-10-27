import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple
import numpy as np

from .register import register_transform

class MatchReshape(ExprVisitor):

    def find(self, expr, shape, params, start):
        self.next = 0
        self.start = start
        self.trans = []
        self.params = params
        self.shape = [int(i) for i in shape]
        self.result = expr
        self.visit(expr)
        return self.next, self.result, self.params

    # TODO: this part has bug, which could not recognize runtime and offline
    def visit_call(self, call):
        if str(call.op) == "reshape":
            dshape = call.args[0].checked_type.shape
            dshape = [int(i) for i in dshape]
            if dshape == self.shape:
                self.result = call.args[0]
            else:
                newshape = [int(i) for i in call.attrs["newshape"]]
                self.trans = [[np.reshape, newshape]] + self.trans
                self.visit(call.args[0])
        elif str(call.op) == "transpose":
            axes = [int(i) for i in call.attrs["axes"]]
            self.trans = [[np.transpose, axes]] + self.trans
            self.visit(call.args[0])
        else:
            if self.start:
                self.next = 1
                self.result = call

    def visit_var(self, var):
        self.next = 1
        self.result = relay.var(var.name_hint, shape=self.shape, dtype="int8")
        if var.name_hint in self.params.keys():
            data = self.params[var.name_hint].asnumpy()
            for [func, attrs] in self.trans:
                data = func(data, attrs)
            self.params[var.name_hint] = tvm.nd.array(data)


class Eliminate(ExprMutator):

    def convert(self, mod, params):
        self.memo = {}
        self.new_vars = []
        self.params = params
        expr = mod["main"].body
        self.start = 1
        new_expr = self.visit(expr)
        func = relay.Function(self.new_vars, new_expr)
        new_mod = tvm.IRModule.from_expr(func)
        return new_mod, self.params

    def visit_var(self, var):
        new_var = var
        if hasattr(self, "last_shape"):
            del self.last_shape
        self.new_vars.append(new_var)
        return new_var


    def visit_call(self, call):
        if str(call.op) == "reshape":
            self.last_shape = call.attrs["newshape"]
            is_next, new_call, self.params = MatchReshape().find(call, self.last_shape, self.params, self.start)
            if is_next:
                return self.visit(new_call)
            return super().visit_call(new_call)
        else:
            self.start = 0
            if hasattr(self, "last_shape"):
                del self.last_shape
            return super().visit_call(call)


@register_transform("eliminate")
def transform(mod, params):
    mod, params = Eliminate().convert(mod, params)
    return mod, params
