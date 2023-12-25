from .transform import Functor
from ..ne import Expr, Numb


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
        self.static_cache = []
        self.dynamic_attrs = []
        mapped_ret = self.visit(expr)
        self.output_payload = {"output": [mapped_ret.payload, mapped_ret.payload_size]}
        return {
            "attrs": self.dynamic_attrs,
            "cache": self.static_cache,
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
        self.inputs_payload[expr.name] = [mapped.payload, mapped.payload_size, expr.index, expr.dtype, expr.shape]
        return mapped

    def visit_func(self, expr):
        mapped_args = [self.visit(arg) for arg in expr.args]
        mapped_ret = expr.build(*mapped_args)
        mapped_name = f"{expr.op_name}_{self.id}"
        self.id += 1
        simple_reg_ops = []
        dynamic_vars = []
        local_vars = []
        for ops in expr.reg_ops:
            simple_ops = []
            if ops[0] == 2:  # pass for loop
                local_vars.append(ops[1].name)
                simple_reg_ops.append(ops)
                continue
            for simple_expr in ops:
                if isinstance(simple_expr, Expr):
                    simple_expr = simple_expr.simplify()
                    for var in simple_expr.get_vars():
                        if var not in local_vars:
                            if var not in dynamic_vars:
                                dynamic_vars.append(var)
                            if var not in self.dynamic_attrs:
                                self.dynamic_attrs.append(var)
                    simple_expr = simple_expr.export("cpp")
                simple_ops.append(simple_expr)
            simple_reg_ops.append(simple_ops)
        self.reg_ops[mapped_name] = {"attrs": dynamic_vars,
                                     "body": simple_reg_ops}
        return mapped_ret

    def visit_cache(self, expr):
        self.static_cache.append(f"static uint8_t {expr.name} [{expr.size}];")
        return expr.name

    def visit_loadcache(self, expr):
        cache_name = self.visit(expr.cache)
        mapped_ret = expr.build()
        mapped_name = f"load_{self.id}"
        self.id += 1
        simple_reg_ops = []
        dynamic_vars = []
        simple_expr = 1
        for shape in expr.shape:
            if isinstance(shape, Expr):
                shape = shape.simplify()
                for var in shape.get_vars():
                    if var not in dynamic_vars:
                        dynamic_vars.append(var)
                    if var not in self.dynamic_attrs:
                        self.dynamic_attrs.append(var)
            simple_expr *= shape
        simple_reg_ops.append([10, mapped_ret.payload, cache_name, simple_expr*2]) # for 16bit
        self.reg_ops[mapped_name] = {"attrs": dynamic_vars,
                                     "body": simple_reg_ops}
        return mapped_ret

    def visit_storecache(self, expr):
        cache_name = self.visit(expr.cache)
        ret_f = self.visit(expr.expr)
        mapped_ret = expr.build(ret_f)
        mapped_name = f"store_{self.id}"
        self.id += 1
        simple_reg_ops = []
        dynamic_vars = []
        simple_expr = 2 # for 16bit
        for shape in expr.shape:
            if isinstance(shape, Expr):
                shape = shape.simplify()
                for var in shape.get_vars():
                    if var not in dynamic_vars:
                        dynamic_vars.append(var)
                    if var not in self.dynamic_attrs:
                        self.dynamic_attrs.append(var)
            simple_expr *= shape
        simple_expr = simple_expr.simplify()
        simple_reg_ops.append([10, cache_name, mapped_ret.payload, simple_expr])
        self.reg_ops[mapped_name] = {"attrs": dynamic_vars,
                                     "body": simple_reg_ops}
        return mapped_ret
