from functools import reduce
from ..adr.base import DataType, DataEnum
from .. import ne
from .base_accel import Accel


class BoothSparse(Accel):

    name = "booth_sparse"

    MAX_DW2 = 16
    Max_Sparsity = 8
    Tout = 32
    log2_Max_Sparsity = 3
    PE_NUM_Per_Block = 4
    MAX_DAT_DW = 8
    base_Tin = 32
    base_log2Tin = 5
    BRAM_NUM = 2
    BRAM_DEPTH = 1 << 24
    Pixel_Data_Bytes = ((Tout*MAX_DAT_DW) >> 3)

    @classmethod
    def malloc_bytes(cls, shape, dtype):
        shape = [i.simplify(1).data if isinstance(i, ne.Expr) else i for i in shape]
        if dtype.mapped == DataEnum.ddr:
            new_shape = [i for i in shape]
            new_shape[-1] = (new_shape[-1] + cls.Tout - 1) // cls.Tout
            new_shape = [cls.Tout] + new_shape
            data_numb = reduce(lambda x, y: x*y, new_shape)
            return data_numb * dtype.get_bytes()
