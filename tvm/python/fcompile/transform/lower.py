from ..cir import InferType
from ..op import *
from ..fir import FCall, FExtern, Functor


class Lower(Functor):

    def build(self, fmod):
        cir = self.visit(fmod.fexpr)
        InferType.infer(cir)
        return cir

    def get_id(self):
        if hasattr(self, "id"):
            self.id += 1
            return self.id
        else:
            self.id = 0
            return self.id

    def visit_var(self, var):
        return var.expr_name

    def visit_cvar(self, cvar):
        return cvar.expr_name

    def visit_call(self, call):
        func = call.op.lowers[0]["lower"]
        new_args = [self.visit(arg) for arg in call.args]
        return func(new_args, call.op, call.attrs)
