from tvm.relay import expr

from . import _make
from ..dyn.nn import _make as _dyn_make
from ...expr import const, Expr, Constant

from tvm.ir import container

def get_pad_tuple1d(padding):
    """Common code to get the 1 dimensional pad option
    Parameters
    ----------
    padding : Union[int, Tuple[int, ...]]
        Padding size
    Returns
    -------
    pad_left : int
        Padding size on left
    pad_right : int
        Padding size on right.
    """
    # compute the padding size
    if isinstance(padding, container.Array):
        padding = list(padding)
    if isinstance(padding, (tuple, list)):
        if len(padding) == 1:
            pad_w = padding[0] * 2
        elif len(padding) == 2:
            return padding[0], padding[1]
        else:
            raise ValueError("Size of padding can only be 1 or 2")
    elif isinstance(padding, int):
        pad_w = padding * 2
    else:
        raise ValueError("Unknown padding option %s" % padding)
    pad_left = (pad_w + 1) // 2
    return pad_left, pad_w - pad_left


def conv2d(
    data,
    weight,
    strides=(1, 1),
    padding=(0, 0, 0, 0),
    kernel_size=None,
    widths=None,
    scales=None,
    activate=0
):
    if isinstance(kernel_size, int):
        kernel_size = (kernel_size, kernel_size)
    if isinstance(strides, int):
        strides = (strides, strides)
    #TODO check the length of widths and scales
    if isinstance(padding, int):
        padding = (padding, padding, padding, padding)
    return _make.vit_conv2d(
        data,
        weight,
        strides,
        padding,
        kernel_size,
        widths,
        scales,
        activate
    )