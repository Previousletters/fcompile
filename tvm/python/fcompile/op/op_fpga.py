FPGA_OP_MAP = {}


def register_fpga_op(op_name, level=10):
    def _register_obj(obj):
        global FPGA_OP_MAP
        FPGA_OP_MAP[op_name] = obj

        #def _call(*args, **kwargs):
        #    return fn(*args, **kwargs)
        return obj
    return _register_obj


def make_define(define: dict, simulator: str) -> str:
    define_list = []
    for key, value in define.items():
        if simulator == "modelsim":
            tp_str = f"+define+{key}={value}"
        elif simulator == "vivado":
            tp_str = f"--define\\ {key}={value}\\ "
        else:
            raise RuntimeError("Unsupport " + simulator + " for simulation!")
        define_list.append(tp_str)
    define_str = "".join(define_list)
    return "\"" + define_str + "\""
