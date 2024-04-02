from ..adr import Functor, Call, VM, Tensor


class InferType(Functor):

    def __init__(self, device=None):
        super(InferType, self).__init__()
        self.device = device
        self.glb_const = {}

    def visit_var(self, expr):
        device = expr.device
        if self.device is not None:
            device = self.device
        expr.checked_type = Tensor(expr.shape, expr.dtype, device)
        return expr

    def visit_constant(self, expr):
        if expr.dtype is None:
            return expr
        device = expr.device
        if self.device is not None:
            device = self.device
        expr.checked_type = Tensor(expr.shape, expr.dtype, device)
        if "global::" in expr.name:
            if expr.name in self.glb_const.keys():
                return self.glb_const[expr.name]
            else:
                self.glb_const[expr.name] = expr
        return expr

    def visit_tupleitem(self, expr):
        arg = self.visit(expr.expr)
        expr.expr = arg
        expr.checked_type = arg.checked_type.tensors[expr.index]
        return expr

    def visit_call(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        new_type = [arg.checked_type for arg in new_args]
        func = expr.op.attrs["rel"]
        state, checked_type = func(new_type, expr.attrs)
        if state:
            # new_expr = Call(expr.op, new_args, expr.attrs)
            new_expr = expr
            new_expr.args = new_args
            new_expr.checked_type = checked_type
            return new_expr
        else:
            raise RuntimeError("Check Error! " + expr.op.name + ", " + checked_type)

    def visit_vm(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        new_type = [arg.checked_type for arg in new_args]
        func = expr.op.attrs["rel"]
        state, checked_type = func(new_type, expr.attrs)
        if state:
            # new_expr = VM(expr.op, new_expr, expr.attrs)
            new_expr = expr
            new_expr.args = new_args
            new_expr.checked_type = checked_type
            return new_expr
        else:
            raise RuntimeError("Check Error! " + expr.op.name + ", " + checked_type)



def infer_type(expr, device=None):
    return InferType(device).visit(expr)
