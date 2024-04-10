from .graph_csb_head import GraphCSBHead
from .codegen_csb_head import CodeGenCSBHead
from .codegen_cfg_head import CodeGenCFGHead
from .codegen_test_head import CodeGenTestHead
from .codegen_test_head_ops import CodeGenTestHeadOps


class GraphTestbench(GraphCSBHead):

    def visit_call(self, expr):
        tensors = [self.visit(arg) for arg in expr.args]
        wrap_args = []
        for arg in tensors:
            wrap_args.append([arg, self.storage.get_address(arg.storage_id, arg.offset)])
        output = self._make_output(expr.checked_type)
        if "testbench" in expr.op.attrs.keys():
            self.nodes.append({
                "node": "accel_op",
                "op_name": expr.op.name,
                "csb_regs": expr.op.attrs["testbench"](wrap_args, output, expr.attrs),
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


def testbench(expr, mod_name, ddr_init_addr, hbm_init_addr):
    expr, mod, storage = GraphTestbench().build(expr, ddr_init_addr, hbm_init_addr)
    source = CodeGenCSBHead().build(mod_name, mod, storage)
    return expr, source, storage, mod


def testbench_cfg(expr, mod_name, ddr_init_addr, hbm_init_addr):
    expr, mod, storage = GraphTestbench().build(expr, ddr_init_addr, hbm_init_addr)
    source, params = CodeGenCFGHead().build(mod_name, mod, storage)
    return expr, source, storage, mod, params


def testbench_test_head(expr, mod_name, ddr_init_addr, hbm_init_addr):
    expr, mod, storage = GraphTestbench().build(expr, ddr_init_addr, hbm_init_addr)
    source = CodeGenTestHead().build(mod_name, mod, storage)
    return expr, source, storage, mod


def testbench_test_head_ops(expr, mod_name, ddr_init_addr, hbm_init_addr):
    expr, mod, storage = GraphTestbench().build(expr, ddr_init_addr, hbm_init_addr)
    source = CodeGenTestHeadOps().build(mod_name, mod, storage)
    return expr, source, storage, mod