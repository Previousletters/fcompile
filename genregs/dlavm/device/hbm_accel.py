from functools import reduce
from ..adr.base import DataType, DataEnum
from .. import ne
from .base_accel import Accel


class HBM(Accel):
    name = "hbm"
    Tb = 1
    HBM_Port = 32
    base_Tin = 128
    Tout = 32
    log2_Tout = 5
    MAX_DAT_DW = 16
    MAX_WT_DW = 4
    MAX_BN_DW = 16
    T_quant_block = 128
    HBM_AXI_DATA_WIDTH = 256
    WT_quant_scale_DW = 16
    DAT_BRAM_NUM = 1
    HBM_Port = 32
    log2_Bank_Step = 28
    WT_BRAM_NUM = HBM_Port
    AXI_BURST_LEN = Tout
    log2_AXI_BURST_LEN = log2_Tout
    WT_CH_Tgroup = T_quant_block*HBM_AXI_DATA_WIDTH//WT_quant_scale_DW
    DAT_BRAM_DEPTH = (1<<23)//base_Tin//MAX_DAT_DW//DAT_BRAM_NUM
    WT_BRAM_DEPTH = (1<<22)//HBM_AXI_DATA_WIDTH//WT_BRAM_NUM
    AXI_DAT_WIDTH = MAX_DAT_DW*Tout*Tb
    AXI_BN_WIDTH = MAX_BN_DW*Tout*Tb
    Pixel_Data_Width = AXI_DAT_WIDTH
    Pixel_Data_Bytes = int((AXI_DAT_WIDTH)>>3)
    AXI_BURST_LEN_SOFTMAX = 4
    SINGLE_BN_FIFO_DEP = (AXI_BURST_LEN*MAX_DAT_DW*Tb)//(MAX_BN_DW*2)
    BN_FIFO_DEP = SINGLE_BN_FIFO_DEP * 4
    BN_FIFO_NUM = (MAX_BN_DW*2)//(MAX_DAT_DW*Tb)

    @classmethod
    def malloc_bytes(cls, shape, dtype):
        shape = [i.simplify(1).data if isinstance(i, ne.Expr) else i for i in shape]
        if dtype.mapped == DataEnum.ddr:
            new_shape = [i for i in shape]
            new_shape[-1] = (new_shape[-1] + cls.Tout - 1) // cls.Tout
            new_shape = [cls.Tout] + new_shape
            data_numb = reduce(lambda x, y: x*y, new_shape)
            return data_numb * dtype.get_bytes()
        elif dtype.mapped == DataEnum.hbm:
            in_ch, out_ch = shape 
            CHout_div_Tout = (out_ch + cls.Tout - 1) // cls.Tout
            WT_CHin_Padding_with_Tin = int((in_ch + cls.base_Tin - 1) // cls.base_Tin) * cls.base_Tin
            WT_scale_group_nums = (WT_CHin_Padding_with_Tin+cls.WT_CH_Tgroup-1) // cls.WT_CH_Tgroup
            require_bytes, wt_bit = 0, 4
            for _ in range(CHout_div_Tout):
                for _ in range(WT_scale_group_nums):
                    for _ in range(cls.Tout//cls.HBM_Port):
                        for _ in range(cls.HBM_AXI_DATA_WIDTH//32):
                            require_bytes += 1
            for _ in range(CHout_div_Tout):
                for j in range(WT_scale_group_nums):
                    for _ in range(cls.Tout // cls.HBM_Port):
                        wt_start_ch_in = j*cls.WT_CH_Tgroup
                        wt_end_ch_in = WT_CHin_Padding_with_Tin if (j==WT_scale_group_nums-1) else (j+1)*cls.WT_CH_Tgroup
                        for _ in range(wt_bit*wt_start_ch_in//32, wt_bit*wt_end_ch_in//32):
                            require_bytes += 1
            return require_bytes*4
        else:
            raise RuntimeError(f"HBM accelerator has no this storage: {dtype.mapped}")


class HBM0321(HBM):
    name = "hbm_0321"

    DAT_BRAM_DEPTH = (1<<22)//HBM.base_Tin//HBM.MAX_DAT_DW//HBM.DAT_BRAM_NUM
    WT_BRAM_DEPTH = (1<<22)//HBM.HBM_AXI_DATA_WIDTH//HBM.WT_BRAM_NUM
    AXI_BURST_LEN_SOFTMAX = 1
    BN_FIFO_DEP = HBM.SINGLE_BN_FIFO_DEP * 4
    MAX_TOKEN = 128
    MAX_CH_per_HEAD = 128
    MIN_WT_HEAD = 2


class ASYNHBM0402(HBM0321):
    name = "asyn_hbm_0402"
    
    ASYN_FACTOR = 2
    WT_BRAM_DEPTH = HBM0321.WT_BRAM_DEPTH // ASYN_FACTOR


class HBM0424(HBM0321):
    name = "hbm_0424"

    MAX_TOKEN = 1024
    MAX_CFG_NUM = 12


class HBM0507(HBM0424):
    name = "hbm_0507"

    AUX_WT_BUF_DEPTH = 1024