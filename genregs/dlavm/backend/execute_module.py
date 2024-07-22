from time import strftime, localtime
from .. import ne
from ..runtime import hbm


class ExecuteModule:

    def __init__(self, module, storage, device):
        self.module = module
        self.storage = storage
        self.device = device

    def execute(self, **args):
        for node in self.module:
            if node["node"] == "accel_op":
                self._exe_accel(node, **args)
            elif node["node"] == "cpu_op":
                self._exe_cpu(node, **args)
            elif node["node"] == "virtual_op":
                self._exe_virtual(node, **args)

    def set_inputs(self, inputs):
        for node in self.module:
            if node["node"] == "var":
                self._set_var(node, inputs)

    def set_consts(self, consts=None):
        for node in self.module:
            if node["node"] == "var":
                self._set_const(node, consts)

    def get_outputs(self):
        outputs = []
        for node in self.module:
            if node["node"] == "output":
                outputs += self._get_output(node)
        return outputs

    def _set_var(node, inputs):
        pass

    def _set_const(node, consts):
        pass

    def _get_output(node):
        pass

    def _exe_accel(self, node, **args):
        pass

    def _exe_cpu(self, node, **args):
        pass

    def _exe_virtual(self, node, **args):
        pass