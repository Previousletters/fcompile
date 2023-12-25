from .. import cir
from .op_hbm import LayerNorm, RMSNorm, Softmax, MatMul, Add, Linear, BatchNorm, Bias
from ._fusion_map import FusedHBM


def make_layernorm(args, mode):
    return cir.LayerNorm(*args, mode=mode)


LayerNorm.lowers = [
    {
        "lower": lambda args, obj, attrs: make_layernorm(args, 1),
        "process": lambda expr: expr
    }
]
LayerNorm.fused = [None]

RMSNorm.lowers = [
    {
        "lower": lambda args, obj, attrs: make_layernorm(args, 0),
        "process": lambda expr: expr
    }
]

Softmax.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.Softmax(args[0]),
        "process": lambda expr: expr
    }
]

MatMul.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.MVM(args[0], args[1]),
        "process": lambda expr: expr
    }
]
MatMul.fused = [
    {
        "args": [[], []],
        "fused": FusedHBM.MVM,
        "commutative": False,
    }
]

Linear.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.FC(args[0], args[1]),
        "process": lambda expr: expr
    }
]

Add.lowers = [
    {
        "lower": lambda args, obj, attrs: cir.ElementWise(args[0], args[1]),
        "process": lambda expr: expr
    }
]
Add.fused = [
    {
        "args": [[BatchNorm, Bias, MatMul, Linear], []],
        "fused": FusedHBM.Res,
        "commutative": True,
    }
]
