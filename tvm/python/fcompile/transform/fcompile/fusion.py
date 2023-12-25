from ...utils import shape2byte
from ...cir import InferType
from ...op import *
from ...fir import FCall, FExtern, FFunction, Mutator, Visitor
from .register import register_transform


class OpNumbVisitor(Visitor):

    def build(self, fexpr):
        self.visit(fexpr)
        memo_numb = {}
        for key, value in self.memo_map.items():
            memo_numb[key] = value[1]
        return memo_numb


class Fusion(Mutator):

    class GetFusedOps(Mutator):

        def get(self, fexpr, memo_numb):
            self.memo_numb = memo_numb
            self.fused_ops = [fexpr.op.fused[0]["fused"]]
            self.fused_args = []
            self.arg_types = []
            self.attrs = {"widths": [], "scales": []}
            new_expr = self.visit(fexpr)
            if len(self.fused_ops) > 1:
                self.attrs["widths"].append(fexpr.attrs["widths"][-1])
                self.attrs["scales"].append(fexpr.attrs["scales"][-1])
                return FFunction(self.fused_args, new_expr, self.fused_ops, self.arg_types, self.attrs)
            else:
                return fexpr

        def visit_call(self, call):
            if self.memo_numb[call] == 1:
                fused_config = call.op.fused[0]
                new_args = []
                for index in range(len(call.args)):
                    new_arg = call.args[index]
                    if type(call.args[index].op) in fused_config["args"][index]:
                        self.fused_ops.append(call.args[index].op.fused[0]["fused"])
                        new_arg = self.visit(call.args[index])
                    else:
                        self.attrs["widths"].append(call.attrs["widths"][index])
                        self.attrs["scales"].append(call.attrs["scales"][index])
                        self.arg_types.append(call.op.arg_types[index])
                        self.fused_args.append(call.args[index])
                    new_args.append(new_arg)
            return call

    def build(self, fmod):
        self.memo_numb = OpNumbVisitor().build(fmod.fexpr)
        fmod.fexpr = self.visit(fmod.fexpr)
        return fmod

    def get_id(self):
        if hasattr(self, "id"):
            self.id += 1
            return self.id
        else:
            self.id = 0
            return self.id

    def visit_call(self, call):
        if hasattr(call.op, "fused"):
            call = self.GetFusedOps().get(call, self.memo_numb)
        new_args = [self.visit(arg) for arg in call.args]
        call.args = new_args
        return call


@register_transform("fusion_ops")
def wrapper(fmod):
    return Fusion().build(fmod)
