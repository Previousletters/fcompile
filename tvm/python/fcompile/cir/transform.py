from .expr import Input, TupleItem, Cache, LoadCache, StoreCache


# Deep First Search with memory
class Functor:

    def __init__(self):
        self.memo = {}

    def visit(self, expr):
        if expr in list(self.memo.keys()):
            return self.memo[expr]
        if isinstance(expr, Input):
            ret = self.visit_input(expr)
        elif isinstance(expr, TupleItem):
            ret = self.visit_tupleitem(expr)
        elif isinstance(expr, Cache):
            ret = self.visit_cache(expr)
        elif isinstance(expr, LoadCache):
            ret = self.visit_loadcache(expr)
        elif isinstance(expr, StoreCache):
            ret = self.visit_storecache(expr)
        else:
            ret = self.visit_func(expr)
        self.memo[expr] = ret
        return ret

    def visit_input(self, expr):
        return expr

    def visit_func(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        expr.args = new_args
        return expr

    def visit_tupleitem(self, expr):
        return expr

    def visit_cache(self, expr):
        return expr

    def visit_loadcache(self, expr):
        expr.cache = self.visit(expr.cache)
        return expr

    def visit_storecache(self, expr):
        expr.expr = self.visit(expr.expr)
        expr.cache = self.visit(expr.cache)
        return expr


# Print Expr Pass
class PrintExpr(Functor):

    @classmethod
    def print(cls, expr, func_name="main"):
        cls().show_main(expr, func_name)

    def show_main(self, expr, func_name):
        self.id = 0
        self.args = []
        self.body = []
        ret_name = self.visit(expr)
        arg_str = ", ".join(self.args)
        print(f"def {func_name}({arg_str}) " + "{")
        print("\n".join(self.body))
        print(f"  return {ret_name}\n" + "}")

    def visit_input(self, expr):
        self.args.append(expr.name)
        func = f"  {expr.name} = input()"
        if hasattr(expr, "shape"):
            func += " /* ret=(" + ", ".join([str(i) for i in expr.shape]) + "), "
            func += "dtype=" + expr.dtype + " */"
        self.body.append(func)
        return expr.name

    def visit_func(self, expr):
        arg_str = ", ".join([self.visit(arg) for arg in expr.args])
        ret_name = "%" + str(self.id)
        self.id += 1
        func = f"  {ret_name} = {expr.op_name}({arg_str})"
        if hasattr(expr, "shape"):
            if expr.dtype == "tuple":
                str_shape = []
                for shape in expr.shape:
                    str_shape.append("[" + ", ".join([str(i) for i in shape]) + "]")
                func += " /* ret=(" + ", ".join(str_shape) + "), "
            else:
                func += " /* ret=(" + ", ".join([str(i) for i in expr.shape]) + "), "
            func += "dtype=" + expr.dtype + " */"
        self.body.append(func)
        return ret_name

    def visit_tupleitem(self, expr):
        arg_name = self.visit(expr.expr)
        ret_name = "%" + str(self.id)
        self.id += 1
        func = f"  {ret_name} = {arg_name}.{expr.index}"
        if hasattr(expr, "shape"):
            func += " /* ret=(" + ", ".join([str(i) for i in expr.shape]) + "), "
            func += "dtype=" + expr.dtype + " */"
        self.body.append(func)
        return ret_name

    def visit_cache(self, expr):
        return expr.name

    def visit_loadcache(self, expr):
        cache_name = self.visit(expr.cache)
        ret_name = "%" + str(self.id)
        self.id += 1
        shape = "(" + ", ".join([str(i) for i in expr.shape]) + ")"
        func = f"  {ret_name} = {expr.op_name}({cache_name}, shape={shape}, dtype={expr.dtype})"
        self.body.append(func)
        return ret_name

    def visit_storecache(self, expr):
        ret_expr = self.visit(expr.expr)
        cache_name = self.visit(expr.cache)
        shape = "(" + ", ".join([str(i) for i in expr.shape]) + ")"
        func = f"  {expr.op_name}({ret_expr}, {cache_name}, shape={shape}, dtype={expr.dtype})"
        self.body.append(func)
        return ret_expr


# Infer Type Expr Pass
class InferType(Functor):

    @classmethod
    def infer(cls, expr, inputs={}):
        cls().infer_main(expr, inputs)

    def infer_main(self, expr, inputs):
        self.inputs = inputs
        self.visit(expr)

    def visit_input(self, expr):
        if expr.name in list(self.inputs.keys()):
            expr.set_shape(*self.inputs[expr.name])
        elif hasattr(expr, "shape") and hasattr(expr, "dtype"):
            pass
        else:
            raise RuntimeError(f"Input shape dict has no {expr.name}, please check!")
        return [expr.shape, expr.dtype]

    def visit_func(self, expr):
        checked_args = [self.visit(arg) for arg in expr.args]
        checked_ret = expr.infer(*checked_args)
        return checked_ret

    def visit_tupleitem(self, expr):
        checked_expr = self.visit(expr.expr)
        return expr.infer(checked_expr)

    def visit_cache(self, expr):
        return None

    def visit_loadcache(self, expr):
        return expr.infer()

    def visit_storecache(self, expr):
        return expr.infer(self.visit(expr.expr))
