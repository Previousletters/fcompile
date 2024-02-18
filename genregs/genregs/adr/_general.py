from .base import Op, Tensor, Tuple, DataEnum
from functools import reduce


def CHECK_ERROR(judge, error_str):
    if judge:
        print(error_str)
        exit(-1)


def ReshapeRel(args, attrs):
    if len(args) != 1:
        return False, []
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []

    def mul(x, y):
        return x * y
    if reduce(mul, attrs["new_shape"]) == reduce(mul, dshape):
        return True, Tensor(attrs["new_shape"], dtype)
    return False, []


Op.Register("accel.reshape", ReshapeRel)


def SplitRel(args, attrs):
    if len(args) != 1:
        return False, []
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if attrs["axis"] == 0:
        return True, Tuple([Tensor(dshape, dtype), Tensor(dshape, dtype)])
    if attrs["axis"] == 2:
        CHECK_ERROR(sum(attrs["new_chs"]) != dshape[2], "Check attrs of split Error")
        new_chs = attrs["new_chs"]
        otensors = []
        for new_ch in new_chs:
            otensors.append(Tensor([dshape[0], dshape[1], new_ch], dtype))
        return True, Tuple(otensors)
    return False, []


Op.Register("accel.split", SplitRel)
