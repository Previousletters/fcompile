from .base import Op, Tensor, Tuple, DataEnum
from functools import reduce


def CHECK_ERROR(judge, error_str):
    if judge:
        print(error_str)
        exit(-1)


def ReshapeRel(args, attrs):
    if len(args) != 1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []

    def mul(x, y):
        return x * y
    new_shape = attrs["new_shape"]
    if reduce(mul, new_shape) < 0:
        for i in range(len(new_shape)):
            if new_shape[i] == -1:
                new_shape[i] = reduce(mul, dshape) // (-1*reduce(mul, new_shape))
    new_shape = [i for i in new_shape]
    if reduce(mul, new_shape) == reduce(mul, dshape):
        return True, Tensor(new_shape, dtype, device)
    return False, []


Op.Register("accel.reshape", ReshapeRel)


def SplitRel(args, attrs):
    if len(args) != 1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if attrs["axis"] == 0:
        return True, Tuple([Tensor(dshape, dtype, device), Tensor(dshape, dtype, device)])
    if attrs["axis"] == 2 or attrs["axis"] == -1:
        CHECK_ERROR(sum(attrs["new_chs"]) != dshape[2], "Check attrs of split Error")
        new_chs = attrs["new_chs"]
        otensors = []
        tmp_shape = [i for i in dshape[:-1]]
        for new_ch in new_chs:
            otensors.append(Tensor(tmp_shape + [new_ch], dtype, device))
        return True, Tuple(otensors)
    axis = attrs["axis"]
    return False, f"split attr axis={axis} is not support, 0 and -1 is available"


Op.Register("accel.split", SplitRel)
