from .functor import Functor, Mutator


class PrintFir(Functor):

    def fir_body(self, fexpr):
        self.id = 0
        self.body = []
        self.extern = []
        expr_name = self.visit(fexpr)
        self.body.append(f"return {expr_name}")
        return self.body, self.extern

    def visit_var(self, var):
        self.body.append(f"{var.expr_name} = {var.op.name}() {str(var)}")
        return var.expr_name

    def visit_cvar(self, cvar):
        self.body.append(f"{cvar.expr_name} = {cvar.op.name}() {str(cvar)}")
        return cvar.expr_name

    def visit_call(self, call):
        str_args = ", ".join([self.visit(arg) for arg in call.args])
        ret_name = f"%{self.id}"
        self.body.append(f"{ret_name} = {call.op.name}({str_args}) {str(call)}")
        self.id += 1
        return ret_name

    def visit_extern(self, call):
        str_args = ", ".join([self.visit(arg) for arg in call.args])
        ret_name = f"%{self.id}"
        self.body.append(f"{ret_name} = {call.op.name}({str_args}) {str(call)}")
        self.extern.append(str(call.op))
        self.id += 1
        return ret_name

    def visit_function(self, function):
        call = function.body
        str_args = ", ".join([self.visit(arg) for arg in function.args])
        ret_name = f"%{self.id}"
        self.body.append(f"{ret_name} = {function.name}({str_args}) {str(call)}")
        self.id += 1
        return ret_name


class GetFirVar(Functor):

    def get(self, fexpr):
        self.vars = []
        self.visit(fexpr)
        return self.vars

    def visit_var(self, var):
        self.vars.append(var)
