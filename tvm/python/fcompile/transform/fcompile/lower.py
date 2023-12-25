from ...utils import shape2byte
from ...cir import InferType
from ...op import *
from ...fir import FCall, FExtern, Functor
from .register import register_transform


class Lower(Functor):

    cir_prefix = "subgraph_"

    def build(self, fmod):
        self.lower_mod = []
        self.visit(fmod.fexpr)
        return self.lower_mod

    def get_id(self):
        if hasattr(self, "id"):
            self.id += 1
            return self.id
        else:
            self.id = 0
            return self.id

    def visit_var(self, var):
        byte = shape2byte(var.op.shape, var.op.dtype)
        ret_node = {
            "node": "var",
            "name": var.expr_name,
            "args": [byte],
            "attrs": ["input"],
        }
        ret_index = len(self.lower_mod)
        self.lower_mod.append(ret_node)
        return [var.expr_name, ret_index]

    def visit_cvar(self, cvar):
        ret_node = {
            "node": "cvar",
            "data": cvar.data,
            "name": cvar.expr_name,
            "args": None,
        }
        ret_index = len(self.lower_mod)
        self.lower_mod.append(ret_node)
        return [cvar.expr_name, ret_index]

    def visit_call(self, call):
        func = call.op.lowers[0]["lower"]
        new_args = [self.visit(arg) for arg in call.args]
        if func is None:
            new_cir = new_args[0][0]
            InferType.infer(new_cir)
            func_name = self.cir_prefix + str(self.get_id())
            ret_node = {
                "node": "cir",
                "source": new_cir,
                "name": func_name,
            }
            self.lower_mod.append(ret_node)
            self.subgraph_output(call)
            ret_index = len(self.lower_mod)-1
            return [None, ret_index]
        else:
            ir = func([arg[0] for arg in new_args], call.op, call.attrs)
            self.subgraph_input(call, ir, new_args)
            return [ir, None]

    def subgraph_input(self, call, ir, args):
        if isinstance(call.op, (AccelFMap, AccelWMap)):
            ir.index = args[0][1]

    def subgraph_output(self, call):
        if isinstance(call.op, Output):
            byte = shape2byte(call.op.shape, call.op.dtype)
            ret_nodes = [
                {
                    "node": "var",
                    "name": call.op.name + str(self.get_id()),
                    "args": [byte],
                    "attrs": ["output"],
                },
                {
                    "node": "demap",
                    # "name": "demap_" + str(self.get_id()),
                    "args": [len(self.lower_mod)-1, len(self.lower_mod)],
                    "attrs": ["feature", call.op.shape[1:]]
                },
            ]
            self.lower_mod += ret_nodes


@register_transform("cir_lower")
def wrapper(fmod):
    return Lower().build(fmod)
