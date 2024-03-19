from ..adr import Functor, Call, VM, Tensor


class OfflineProcess(Functor):

    def visit_call(self, expr):
        expr.args = [self.visit(arg) for arg in expr.args]
        if "process" in expr.op.attrs.keys():
            func = expr.op.attrs["process"]
            func(expr.args, expr.attrs)
        return expr


def offline_process(expr):
    return OfflineProcess().visit(expr)

