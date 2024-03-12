from ...adr import Functor, Op, Call


class Fused_Two(Functor):

    def __init__(self, leaf: str, root: str):
        super().__init__()
        self.leaf_op = Op.Get(leaf)
        self.root_op = Op.Get(root)

    def visit_call(self, expr):
        if expr.op != self.root_op:
            return expr
        if len(expr.args) == 1:
            return expr
        if isinstance(expr.args[0], Call):
            return expr
        if expr.args[0].op != self.leaf_op:
            return expr
        return super().visit_call(expr)