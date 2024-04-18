from .. import driver
from ..adr import Functor, DataEnum, Tensor, Tuple
from .graph_plan_memory import GraphPlanMemory
from .codegen_csb_head import CodeGenCSBHead
from .codegen_cfg_head import CodeGenCFGHead
from .codegen_test_head import CodeGenTestHead
from .codegen_test_head_ops import CodeGenTestHeadOps


class GraphCSBHead(Functor):

    def build(self, expr, init_addr):
        expr, self.storage = GraphPlanMemory().main(expr, init_addr)
        self.nodes = []
        outputs = self.visit(expr)
        self._node_output(outputs)
        return expr, self.nodes, self.storage

    def _wrap_storage(self, checked_type):
        if isinstance(checked_type, Tensor):
            return [{"id": checked_type.storage_id, "offset": checked_type.offset}]
        elif isinstance(checked_type, Tuple):
            ret = []
            for tensor in checked_type.tensors:
                ret.append({"id": tensor.storage_id, "offset": tensor.offset})
            return ret
        else:
            print("unknown type of checked_type: " + type(checked_type))
            exit(-1)

    def _node_output(self, checked_type):
        if isinstance(checked_type, Tensor):
            self.nodes.append({
                "node": "output",
                "name": "output",
                "storage": self._wrap_storage(checked_type),
                "shape": checked_type.shape,
            })
        elif isinstance(checked_type, Tuple):
            for num, tensor in enumerate(checked_type.tensors):
                self.nodes.append({
                    "node": "output",
                    "name": "output" + str(num),
                    "storage": self._wrap_storage(tensor),
                    "shape": tensor.shape,
                })
        else:
            print("unknown type of checked_type: " + type(checked_type))
            exit(-1)

    def _make_output(self, checked_type):
        if isinstance(checked_type, Tensor):
            storage_id, offset = checked_type.storage_id, checked_type.offset
            return [checked_type, self.storage.get_address(storage_id, offset)]
        elif isinstance(checked_type, Tuple):
            outputs = []
            for tensor in checked_type.tensors:
                storage_id, offset = tensor.storage_id, tensor.offset
                outputs.append([tensor, self.storage.get_address(storage_id, offset)])
            return outputs
        else:
            print("unknown type of checked_type: " + type(checked_type))
            exit(-1)

    def visit_var(self, expr):
        self.nodes.append({
            "node": "var",
            "name": expr.name,
            "storage": self._wrap_storage(expr.checked_type),
            "shape": expr.checked_type.shape,
        })
        return expr.checked_type

    def visit_constant(self, expr):
        if expr.data is not None:
            self.nodes.append({
                "node": "const",
                "name": expr.name,
                "data": expr.data,
                "storage": self._wrap_storage(expr.checked_type),
                "shape": expr.checked_type.shape,
            })
        else:
            self.nodes.append({
                "node": "const",
                "name": expr.name,
                "storage": self._wrap_storage(expr.checked_type),
                "shape": expr.checked_type.shape,
            })
        return expr.checked_type

    def visit_tupleitem(self, expr):
        arg = self.visit(expr.expr)
        expr.checked_type = arg.tensors[expr.index]
        return arg.tensors[expr.index]

    def visit_call(self, expr):
        tensors = [self.visit(arg) for arg in expr.args]
        wrap_args = []
        for arg in tensors:
            wrap_args.append([arg, self.storage.get_address(arg.storage_id, arg.offset)])
        output = self._make_output(expr.checked_type)
        if "driver" in expr.op.attrs.keys():
            self.nodes.append({
                "node": "accel_op",
                "op_name": expr.op.name,
                "csb_regs": expr.op.attrs["driver"](wrap_args, output, expr.attrs),
                "storage": self._wrap_storage(expr.checked_type),
            })
        elif "source" in expr.op.attrs.keys():
            self.nodes.append({
                "node": "cpu_op",
                "op_name": expr.op.name,
                "source": expr.op.attrs["source"](wrap_args, output, expr.attrs),
                "storage": self._wrap_storage(expr.checked_type),
            })
        else:
            print(f"Please register the compute function of {expr.op.name}")
            print(expr.op.attrs.keys())
            exit(-1)
        return expr.checked_type

    def visit_vm(self, expr):
        tensors = [self.visit(arg) for arg in expr.args]
        new_checked_type = expr.op.attrs["driver"](tensors, expr.checked_type, expr.attrs)
        expr.checked_type = new_checked_type
        self.nodes.append({
            "node": "virtual_op",
            "op_name": expr.op.name,
            "storage": self._wrap_storage(expr.checked_type),
        })
        return new_checked_type


def csb_head(expr, mod_name, init_addr):
    expr, mod, storage = GraphCSBHead().build(expr, init_addr)
    source = CodeGenCSBHead().build(mod_name, mod, storage)
    return expr, source, storage, mod


def cfg_head(expr, mod_name, init_addr):
    expr, mod, storage = GraphCSBHead().build(expr, init_addr)
    source, params = CodeGenCFGHead().build(mod_name, mod, storage)
    return expr, source, storage, mod, params


def csb_test_head(expr, mod_name, init_addr):
    expr, mod, storage = GraphCSBHead().build(expr, init_addr)
    source = CodeGenTestHead().build(mod_name, mod, storage)
    return expr, source, storage, mod


def csb_test_head_ops(expr, mod_name, init_addr):
    expr, mod, storage = GraphCSBHead().build(expr, init_addr)
    source = CodeGenTestHeadOps().build(mod_name, mod, storage)
    return expr, source, storage, mod
