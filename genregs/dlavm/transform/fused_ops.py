from ..adr import Functor, Call
from .fused_lib import FusedStrategy


class FusedOps(Functor):

    def visit_call(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        expr.args = new_args
        funcs = FusedStrategy.Get(expr.op.name)
        if funcs is None:
            return expr
        for func in funcs:
            state, new_expr = func.CheckFused(expr)
            if state:
                print(new_expr)
                break
        return new_expr


def fused_ops(expr):
    return FusedOps().visit(expr)