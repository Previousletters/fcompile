from .transform import Functor
from .ne import Expr, Numb


class Static(Functor):

    @classmethod
    def JIT(cls, expr, inputs={}):
        return cls().export_main(expr, inputs)

    def export_main(self, expr, inputs):
        self.id = 0
        self.inputs = inputs
        self.inputs_payload = {}
        self.reg_ops = {}
        mapped_ret = self.visit(expr)
        self.output_payload = {"output": [mapped_ret.payload, mapped_ret.payload_size]}
        return {
            "inputs": self.inputs_payload,
            "regops": self.reg_ops,
            "output": self.output_payload
        }

    def visit_input(self, expr):
        if expr.name in list(self.inputs.keys()):
            shape, dtype = self.inputs[expr.name]
        else:
            if hasattr(expr, "shape"):
                shape, dtype = expr.shape, expr.dtype
            else:
                raise RuntimeError(f"Input shape dict has no {expr.name}, please check!")
        mapped = expr.build(shape, dtype)
        self.inputs_payload[expr.name] = [mapped.payload, mapped.payload_size]
        return mapped

    def visit_func(self, expr):
        mapped_args = [self.visit(arg) for arg in expr.args]
        mapped_ret = expr.build(*mapped_args)
        mapped_name = f"{expr.op_name}_{self.id}"
        self.id += 1
        self.reg_ops[mapped_name] = expr.reg_ops
        return mapped_ret


class Dynamic(Functor):

    @classmethod
    def JIT(cls, expr, inputs={}):
        return cls().export_main(expr, inputs)

    def export_main(self, expr, inputs):
        self.id = 0
        self.inputs = inputs
        self.inputs_payload = {}
        self.reg_ops = {}
        self.dynamic_attrs = []
        mapped_ret = self.visit(expr)
        self.output_payload = {"output": [mapped_ret.payload, mapped_ret.payload_size]}
        return {
            "attrs": self.dynamic_attrs,
            "inputs": self.inputs_payload,
            "regops": self.reg_ops,
            "output": self.output_payload
        }

    def visit_input(self, expr):
        if expr.name in list(self.inputs.keys()):
            shape, dtype = self.inputs[expr.name]
        else:
            if hasattr(expr, "shape"):
                shape, dtype = expr.shape, expr.dtype
            else:
                raise RuntimeError(f"Input shape dict has no {expr.name}, please check!")
        for n in shape:
            if isinstance(n, Expr):
                for var in n.get_vars():
                    if var not in self.dynamic_attrs:
                        self.dynamic_attrs.append(var)
        mapped = expr.build(shape, dtype)
        self.inputs_payload[expr.name] = [mapped.payload, mapped.payload_size]
        return mapped

    def visit_func(self, expr):
        mapped_args = [self.visit(arg) for arg in expr.args]
        mapped_ret = expr.build(*mapped_args)
        mapped_name = f"{expr.op_name}_{self.id}"
        self.id += 1
        simple_reg_ops = []
        dynamic_vars = []
        for ops in expr.reg_ops:
            simple_expr = ops[2]
            if isinstance(simple_expr, Expr):
                simple_expr = simple_expr.simplify()
                for var in simple_expr.get_vars():
                    if var not in dynamic_vars:
                        dynamic_vars.append(var)
                simple_expr = simple_expr.export("cpp")
            simple_reg_ops.append([ops[0], ops[1], simple_expr])
        self.reg_ops[mapped_name] = {"attrs": dynamic_vars,
                                     "body": simple_reg_ops}
        return mapped_ret
