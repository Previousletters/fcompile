from ...adr.base import Op
from .sparse_conv import RunConv


def Conv2d(args, output, attrs):
    return RunConv(args[0], args[1], output, attrs)


Op.Get("accel.sparse.conv2d").attrs["driver"] = Conv2d
