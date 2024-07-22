from ...adr import Tensor, Constant
from ...clib import WT_TRANS, BN_TRANS


def MVMProcess(args, attrs):
    if attrs["skip"] == 1:
        data, weight = args
        if isinstance(weight, Constant):
            require_bytes = weight.checked_type.get_bytesize()
            weight.data = WT_TRANS(weight.data[0], weight.data[1], require_bytes)


def MVMBNProcess(args, attrs):
    if attrs["skip"] == 1:
        _, weight, bn = args
        if isinstance(weight, Constant):
            require_bytes = weight.checked_type.get_bytesize()
            weight.data = WT_TRANS(weight.data[0], weight.data[1], require_bytes)
        if isinstance(bn, Constant):
            require_bytes = bn.checked_type.get_bytesize()
            bn.data = BN_TRANS(bn.data[0], bn.data[1], require_bytes)


def LayerNormProcess(args, attrs):
    _, bn = args
    if isinstance(bn, Constant):
        require_bytes = bn.checked_type.get_bytesize()
        bn.data = BN_TRANS(bn.data[0], bn.data[1], require_bytes)