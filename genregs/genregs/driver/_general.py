from ..adr import Op, Tuple, Tensor


def SplitDriver(args, output, attrs):
    offset = 0
    new_tensors = []
    for t in output.tensors:
        t.offset = offset
        new_tensors.append(t)
        offset += t.get_bytesize()
    return Tuple(new_tensors)


Op.Get("accel.split").attrs["driver"] = SplitDriver


def ReshapeDriver(args, output, attrs):
    return output


Op.Get("accel.reshape").attrs["driver"] = ReshapeDriver
