from .expr import Expr, Numb, Var


class For:

    def __init__(self, var_expr, min_expr, max_expr, body):
        self.var_expr = var_expr
        self.min_expr = min_expr
        self.max_expr = max_expr
        if isinstance(min_expr, str):
            self.min_expr = Var(min_expr)
        elif isinstance(min_expr, (int, float)):
            self.min_expr = Numb(min_expr)
        if isinstance(max_expr, str):
            self.max_expr = Var(max_expr)
        elif isinstance(max_expr, (int, float)):
            self.max_expr = Numb(max_expr)
        self.body = body

    def run(self, *args):
        if isinstance(self.min_expr, Expr):
            self.min_expr = self.min_expr.simplify(0)
        if isinstance(self.max_expr, Expr):
            self.max_expr = self.max_expr.simplify(0)
        if not isinstance(self.min_expr, Numb) or not isinstance(self.max_expr, Numb):
            self.body([self.var_expr, self.min_expr, self.max_expr], *args)
            return f"for {self.var_expr.name} in [{str(self.min_expr), str(self.max_expr)}] -> f{self.body.__name__}({self.var_expr.name})"
        min_num, max_num = self.min_expr, self.max_expr
        if isinstance(self.min_expr, (int, float)):
            pass
        elif isinstance(self.min_expr, Numb):
            min_num = self.min_expr.data
        if isinstance(self.max_expr, (int, float)):
            pass
        elif isinstance(self.max_expr, Numb):
            max_num = self.max_expr.data
        for i in range(min_num, max_num):
            self.body(i)
        return f"for {self.var_expr.name} in [{str(min_num), str(max_num)}] -> f{self.body.__name__}({self.var_expr.name})"


def expr_for_hook(hook_st, hook_end):
    def _register_fn(fn):
        def _call(var, *args):
            if isinstance(var, list):
                hook_st(var, *args)
                fn(var[0])
                hook_end(var, *args)
            else:
                fn(var)
        return _call
    return _register_fn
