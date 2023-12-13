from .. import cir
from .op_hbm import LayerNorm, RMSNorm, Softmax


def make_layernorm(args, mode):
    return cir.LayerNorm(*args, mode=mode)


LayerNorm.lowers = [
    {
        "lower": lambda args, obj, attrs: make_layernorm(args, 1),
        "fused": [None, None],
        "process": lambda expr, params: [expr, params]
    }
]

RMSNorm.lowers = [
    {
        "lower": lambda args, obj, attrs: make_layernorm(args, 0),
        "fused": [None, None],
        "process": lambda expr, params: [expr, params]
    }
]

Softmax.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.Softmax(args[0]),
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]

print("-------------")
