from ..adr import Op, Tuple, Tensor
from .basic import get_tensor_size


def SplitDriver(args, output, attrs):
    offset = 0
    new_tensors = []
    for t in output.tensors:
        t.offset = offset
        new_tensors.append(t)
        offset += get_tensor_size(t)
    return Tuple(new_tensors)


Op.Get("accel.split").attrs["driver"] = SplitDriver


def ReshapeDriver(args, output, attrs):
    return output


Op.Get("accel.reshape").attrs["driver"] = ReshapeDriver
