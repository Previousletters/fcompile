from ..adr import Op, Tensor
from .booth.conv import *
import subprocess


def Conv2DDriver(args, output, attrs):
    dshape, wshape = args[0][0].shape, args[1][0].shape
    daddrs, waddrs = args[0][1], args[1][1]
    oaddrs = output[1]
    scales = attrs["scales"]
    if len(scales) == 3:
        scales += [scales[-1]]
    Sy, Sx = attrs["stride"]
    Py, Px = attrs["padding"]
    return RunConv(dshape[2], dshape[0], dshape[1], wshape[0], wshape[3], wshape[2], 
                   Sx, Sy, Px, Py, attrs["relu"], attrs["l0_dw"], attrs["l1_dw"], 
                   daddrs, waddrs, oaddrs, scales[0], scales[1], scales[2], scales[3])


Op.Get("accel.booth.conv2d").attrs["driver"] = Conv2DDriver

