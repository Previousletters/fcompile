from .. import cir
from .op_base import AccelFMap, AccelWMap, Output


AccelFMap.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.Input(args[0], obj.shape[1:], "feature"),
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]


def weight_lower(args, obj, attrs):
    if isinstance(args[0], cir.Op):
        return cir.Feature2Weight(args[0])
    else:
        return cir.Input(args[0], obj.shape, "weight")


AccelWMap.lowers = [
    {
        "lower": weight_lower,
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]

Output.lowers = [
    {
        "lower": None,
        "fused": [None],
        "process": lambda expr, params: [expr, params]
    }
]
