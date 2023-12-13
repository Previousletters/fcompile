from .. import cir
from .op_base import AccelFMap, AccelWMap, Output


AccelFMap.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.Input(args[0], obj.shape[1:], "feature"),
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]

AccelWMap.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.Input(args[0], obj.shape[1:], "weight"),
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]

Output.lowers = [
    {
        "lower": lambda args, obj, attrs: args[0],
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]
