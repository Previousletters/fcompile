from .functor import Functor, Mutator


class PrintFir(Functor):

    def fir_body(self, fexpr):
        self.body = []
        self.visit(fexpr)
        return self.body

    def visit_var(self, var):
        self.body.append(str(var))

    def visit_cvar(self, cvar):
        self.body.append(str(cvar))

    def visit_call(self, call):
        _ = [self.visit(arg) for arg in call.args]
        self.body.append(str(call))

    def visit_extern(self, call):
        _ = [self.visit(arg) for arg in call.args]
        self.body.append(str(call))


class GetFirVar(Functor):

    def get(self, fexpr):
        self.vars = []
        self.visit(fexpr)
        return self.vars

    def visit_var(self, var):
        self.vars.append(var)
