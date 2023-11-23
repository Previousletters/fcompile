from ..fir import Functor


class ModelSIM(Functor):

    def run(self, fmod, inputs):
        self.inputs = inputs
        self.tin = fmod.tin
        self.tout = fmod.tout
        return self.visit(fmod.fexpr)

    def visit_var(self, var):
        if var.expr_name not in self.inputs.keys():
            raise RuntimeError("Not found the input named " + var.expr_name)
        if self.inputs[var.expr_name].dtype not in ["int8", "float16"]:
            raise RuntimeWarning("The input dtype of " + var.expr_name + "should be int8")
        return self.inputs[var.expr_name]

    def visit_cvar(self, var):
        if var.expr_name not in self.inputs.keys():
            return var.data
        if self.inputs[var.expr_name].dtype != "int8":
            raise RuntimeWarning("The input dtype of " + var.expr_name + "should be int8")
        return self.inputs[var.expr_name]

    def visit_call(self, call):
        args = [self.visit(arg) for arg in call.args]
        return call.op.modelsim(args, call.attrs, self.tin, self.tout)

    def visit_extern(self, call):
        args = [self.visit(arg) for arg in call.args]
        return call.op(args)

    
def modelsim(fmod, inputs):
    return ModelSIM().run(fmod, inputs)
