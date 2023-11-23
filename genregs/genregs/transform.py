from .cir import Input


# Deep First Search with memory
class Functor:

    def __init__(self):
        self.memo = {}

    def visit(self, expr):
        if expr in list(self.memo.keys()):
            return self.memo[expr]
        if isinstance(expr, Input):
            ret = self.visit_input(expr)
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


# Print Expr Pass
class PrintExpr(Functor):

    @classmethod
    def print(cls, expr):
        cls().show_main(expr)

    def show_main(self, expr):
        self.id = 0
        self.args = []
        self.body = []
        ret_name = self.visit(expr)
        arg_str = ", ".join(self.args)
        print(f"def main({arg_str}) " + "{")
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
            func += " /* ret=(" + ", ".join([str(i) for i in expr.shape]) + "), "
            func += "dtype=" + expr.dtype + " */"
        self.body.append(func)
        return ret_name


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
