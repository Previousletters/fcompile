class Functor:

    def __init__(self):
        self.memo_map = {} # {expr : [return, number]}

    def visit(self, fexpr):
        if fexpr in self.memo_map.keys():
            self.memo_map[fexpr][1] += 1
            return self.memo_map[fexpr][0]

        if fexpr.Type == "Var":
            ret = self.visit_var(fexpr)
        elif fexpr.Type == "CVar":
            ret = self.visit_cvar(fexpr)
        elif fexpr.Type == "FCall":
            ret = self.visit_call(fexpr)
        elif fexpr.Type == "FExtern":
            ret = self.visit_extern(fexpr)
        else:
            raise RuntimeError("Unknow type of " + fexpr.Type)

        self.memo_map[fexpr] = [ret, 1]
        return ret

    def visit_var(self, var):
        return var

    def visit_cvar(self, cvar):
        return cvar

    def visit_call(self, call):
        new_args = [self.visit(arg) for arg in call.args]
        return call

    def visit_extern(self, call):
        new_args = [self.visit(arg) for arg in call.args]
        return call


class Mutator(Functor):

    def visit_call(self, call):
        new_args = [self.visit(arg) for arg in call.args]
        call.args = new_args
        return call

    def visit_extern(self, call):
        new_args = [self.visit(arg) for arg in call.args]
        call.args = new_args
        return call

        