from . import _make
from ..dyn.nn import _make as _dyn_make
from ...expr import const, Expr, Constant

from tvm.ir import container


def quantize(
    data,
    width=None,
    scale=None
):
    return _make.quantize(
        data,
        width,
        scale
    )


def dequantize(
    data,
    width=None,
    scale=None
):
    return _make.dequantize(
        data,
        width,
        scale
    )


def gelu(
    data
):
    return _make.gelu(
        data
    )
