from .base import Op, Call, Var, Constant, DataEnum, DataType
from ..device import BoothSparse


def var_ddr(name, shape, dtype=DataEnum.int8, device=BoothSparse):
    dtype = DataType(dtype, DataEnum.ddr)
    return Var(name, shape, dtype, device)


def const_ddr(name, data, shape=None, dtype=DataEnum.int8, device=BoothSparse):
    dtype = DataType(dtype, DataEnum.ddr)
    return Constant(name, data, shape, dtype, device)


def conv2d(data, weight, stride, padding, l0_dw, l1_dw, sparsity, scales, relu=0):
    attrs = {
        "stride": stride,
        "padding": padding,
        "l0_dw": l0_dw,
        "l1_dw": l1_dw,
        "sparsity": sparsity,
        "scales": scales,
        "relu": relu
    }
    return Call(Op.Get("accel.sparse.conv2d"), [data, weight], attrs)
   