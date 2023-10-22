FPGA_OP_MAP = {}

def register_fpga_op(op_name, level=10):
    def _register_fn(fn):
        global FPGA_OP_MAP
        FPGA_OP_MAP[op_name] = fn
        def _call(*args, **kwargs):
            return fn(*args, **kwargs)
        return _call
    return _register_fn


def make_define(define:dict) -> str:
    define_list = []
    for key, value in define.items():
        tp_str = f"+define+{key}={value}"
        define_list.append(tp_str)
    define_str = "".join(define_list)  
    return "\"" + define_str + "\""
