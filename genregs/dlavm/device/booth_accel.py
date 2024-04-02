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
    BRAM_DEPTH = 1<<24


