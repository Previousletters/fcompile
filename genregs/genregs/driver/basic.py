from functools import reduce
from .. import ne
from ..adr import DataEnum, DataType

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
WT_BRAM_NUM = HBM_Port
AXI_BURST_LEN = Tout
log2_AXI_BURST_LEN = log2_Tout
WT_CH_Tgroup = T_quant_block*HBM_AXI_DATA_WIDTH//WT_quant_scale_DW
DAT_BRAM_DEPTH = (1<<23)//base_Tin//MAX_DAT_DW//DAT_BRAM_NUM
WT_BRAM_DEPTH = (1<<24)//HBM_AXI_DATA_WIDTH//WT_BRAM_NUM
AXI_DAT_WIDTH = MAX_DAT_DW*Tout*Tb
AXI_BN_WIDTH = MAX_BN_DW*Tout*Tb
Pixel_Data_Width = AXI_DAT_WIDTH
Pixel_Data_Bytes = int((AXI_DAT_WIDTH)>>3)
AXI_BURST_LEN_SOFTMAX = 4
BN_FIFO_DEP = (AXI_BURST_LEN*MAX_DAT_DW*Tb)//(MAX_BN_DW*2)
BN_FIFO_NUM = (MAX_BN_DW*2)//(MAX_DAT_DW*Tb)


def CSB_For(expr, tag):
    tag.reg_ops.append(expr)


def CSB_End(expr, tag):
    tag.reg_ops.append([-1, 0, 0])


def CSB_Write(regs, addr, data):
    if data is None:
        regs.append([1, addr, 0, 0])
    elif isinstance(data, ne.Expr):
        regs.append([1, addr, data.simplify().export("cpp"), len(data.get_vars())])
    else:
        regs.append([1, addr, data & 0xffffffff, 0])


def CSB_Read(regs, addr, data):
    if data is None:
        regs.append([0, addr, 0, 0])
    elif isinstance(data, ne.Expr):
        regs.append([0, addr, data.simplify().export("cpp"), len(data.get_vars())])
    else:
        regs.append([0, addr, data & 0xffffffff, 0])


def HWC2NHWT(shape):
    if len(shape) < 3:
        shape = [1 for i in range(3-len(shape))] + shape
    new_shape = []
    new_shape.append((shape[-1] + Tout - 1) // Tout)
    new_shape += [shape[0], shape[1], Tout]
    return new_shape


def get_tensor_size(tensor):
    new_shape = [t.simplify(1).data if isinstance(t, ne.Expr) else t for t in tensor.shape]
    if tensor.dtype.mapped == DataEnum.ddr:
        new_shape = HWC2NHWT(new_shape)
        data_numb = reduce(lambda x, y: x*y, new_shape)
        return data_numb * tensor.dtype.get_bytes()
    elif tensor.dtype.mapped == DataEnum.hbm:
        in_ch, out_ch = new_shape
        CHout_div_Tout = (out_ch + Tout - 1) // Tout
        WT_CHin_Padding_with_Tin = int((in_ch + base_Tin - 1) // base_Tin) * base_Tin
        WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1) // WT_CH_Tgroup
        require_bytes, wt_bit = 0, 4
        for _ in range(CHout_div_Tout):
            for _ in range(WT_scale_group_nums):
                for _ in range(Tout//HBM_Port):
                    for _ in range(HBM_AXI_DATA_WIDTH//32):
                        require_bytes += 1

        for _ in range(CHout_div_Tout):
            for j in range(WT_scale_group_nums):
                for _ in range(Tout // HBM_Port):
                    wt_start_ch_in = j*WT_CH_Tgroup
                    wt_end_ch_in = WT_CHin_Padding_with_Tin if (j==WT_scale_group_nums-1) else (j+1)*WT_CH_Tgroup
                    for _ in range(wt_bit*wt_start_ch_in//32, wt_bit*wt_end_ch_in//32):
                        require_bytes += 1
        
        return require_bytes*4
