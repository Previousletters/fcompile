import numpy as np
from ..op import Const
from ..fir import Mutator, FCVar

class SetParams(Mutator):

    def set(self, fexpr, params):
        self.params = params
        return self.visit(fexpr)
        
    def visit_var(self, var):
        if var.expr_name in self.params.keys():
            data = self.params[var.expr_name]
            if data.shape != var.op.shape:
                raise RuntimeError("The shape of " + var.expr_name + " is not same with inputs!")
            if data.dtype != var.op.dtype:
                raise RuntimeError("The dtype of " + var.expr_name + " is not same with inputs!")
            op = Const(var.op.shape, var.op.dtype)
            return FCVar(var.expr_name, op, data)
        else:
            return var


class ProcessParams(Mutator):

    def process(self, fexpr, tin, tout):
        self.tin, self.tout = tin, tout
        return self.visit(fexpr)

    def visit_call(self, call):
        call = super().visit_call(call)
        new_data = [arg.data if hasattr(arg, "data") else np.array([]) for arg in call.args]
        new_data = call.op.param_process(new_data, call.attrs, self.tin, self.tout)
        new_args = []
        for data, arg in zip(new_data, call.args):
            if data.size != 0:
                arg.data = data
            new_args.append(arg)
        call.args = new_args
        return call


def FPGAParameters(fmod, params):
    new_fexpr = SetParams().set(fmod.fexpr, params)
    fmod.fexpr = ProcessParams().process(new_fexpr, fmod.tin, fmod.tout)
    return fmod