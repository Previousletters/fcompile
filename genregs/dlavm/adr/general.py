from .base import Op, VM, Call, Var, Constant, DataEnum, DataType


def split(data, axis, new_chs=[]):
    attrs = {
        "axis": axis,
        "new_chs": new_chs
    }
    return VM(Op.Get("accel.split"), [data], attrs)


def reshape(data, new_shape, force=0):
    attrs = {
        "new_shape": new_shape,
        "force": force
    }
    return VM(Op.Get("accel.reshape"), [data], attrs)


def realloc(data, new_shape):
    attrs = {
        "new_shape": new_shape
    }
    return VM(Op.Get("accel.realloc"), [data], attrs)

