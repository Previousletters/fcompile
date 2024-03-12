from .basic import *
from ..clib import FP32_to_FP20
from ..ne import Var, For, Numb, expr_for_hook

Matrix_reg_bias = 192


def FPGA_Run_Softmax(f_in, f_out, Out_and_In_Mode):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX

    dshape, oshape = f_in[0].shape, f_out[0].shape
    daddrs, oaddrs = f_in[1], f_out[1]
    width = ((dshape[1] + AXI_BURST_LEN_SOFTMAX - 1) // AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1]
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1]

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, 0)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, dat_in_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+5, dat_in_line_stride)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, dat_out_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+8, dat_out_line_stride)

    CSB_Write(regs, Matrix_reg_bias+9, (dshape[2]+Tout-1) // Tout)
    CSB_Write(regs, Matrix_reg_bias+10, dshape[0])
    CSB_Write(regs, Matrix_reg_bias+11, width)
    CSB_Write(regs, Matrix_reg_bias+12, dshape[1])
    CSB_Write(regs, Matrix_reg_bias+13, Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14, 0)
    CSB_Write(regs, Matrix_reg_bias+15, 0)
    CSB_Write(regs, Matrix_reg_bias+16, 0)
    CSB_Write(regs, Matrix_reg_bias+17, 0)
    CSB_Write(regs, Matrix_reg_bias+18, dshape[2])
    CSB_Write(regs, Matrix_reg_bias+19, 0)
    CSB_Write(regs, Matrix_reg_bias+20, 0)
    CSB_Write(regs, Matrix_reg_bias+21, 0)
    CSB_Write(regs, Matrix_reg_bias+22, 0b00001000)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs


def FPGA_Run_LN(f_in, wt_and_bias, f_out, RMS_Norm, Out_and_In_Mode):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX
    AXI_DAT_WIDTH = f_in[0].device.AXI_DAT_WIDTH
    log2_AXI_BURST_LEN = f_in[0].device.log2_AXI_BURST_LEN

    dshape, wshape, oshape = f_in[0].shape, wt_and_bias[0].shape, f_out[0].shape
    daddrs, waddrs, oaddrs = f_in[1], wt_and_bias[1], f_out[1]
    chin_padding_with_tout = ((dshape[2] + Tout - 1) // Tout) * Tout
    LN_num_per_AXI_DW = AXI_DAT_WIDTH // (2*MAX_BN_DW)
    Layer_Norm = 1 - RMS_Norm
    LN_CH_burst_times_minus1 = int(chin_padding_with_tout // LN_num_per_AXI_DW)>>int(log2_AXI_BURST_LEN)
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1]
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1]
    temp = 1 / chin_padding_with_tout
    fp20_data = FP32_to_FP20(temp)
    Height = dshape[0] * dshape[1]

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, waddrs)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, dat_in_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+5, dat_in_line_stride)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, dat_out_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+8, dat_out_line_stride)
    CSB_Write(regs, Matrix_reg_bias+9, (chin_padding_with_tout+Tout-1) // Tout)
    CSB_Write(regs, Matrix_reg_bias+10, dshape[0])
    CSB_Write(regs, Matrix_reg_bias+11, dshape[1])
    CSB_Write(regs, Matrix_reg_bias+12, Height)
    CSB_Write(regs, Matrix_reg_bias+13, Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14, 0)
    CSB_Write(regs, Matrix_reg_bias+15, fp20_data)
    CSB_Write(regs, Matrix_reg_bias+16, LN_CH_burst_times_minus1)
    CSB_Write(regs, Matrix_reg_bias+17, Layer_Norm)
    CSB_Write(regs, Matrix_reg_bias+18, 0)
    CSB_Write(regs, Matrix_reg_bias+19, 0)
    CSB_Write(regs, Matrix_reg_bias+20, 0)
    CSB_Write(regs, Matrix_reg_bias+21, 0)
    CSB_Write(regs, Matrix_reg_bias+22, 0b0100000)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs


def FPGA_Run_PosEmb(f_in, pos_in, f_out, Out_and_In_Mode):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX
    AXI_DAT_WIDTH = f_in[0].device.AXI_DAT_WIDTH
    log2_AXI_BURST_LEN = f_in[0].device.log2_AXI_BURST_LEN

    dshape, wshape, oshape = f_in[0].shape, pos_in[0].shape, f_out[0].shape
    daddrs, waddrs, oaddrs = f_in[1], pos_in[1], f_out[1]
    Pos_Num = dshape[1]
    chin_padding_with_tout = ((dshape[2] + Tout - 1) // Tout) * Tout
    PosEmb_in_line_stride = Pixel_Data_Bytes * Pos_Num
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1]
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1]

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, 0)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, dat_in_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+5, dat_in_line_stride)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, dat_out_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+8, dat_out_line_stride)
    CSB_Write(regs, Matrix_reg_bias+9, (chin_padding_with_tout+Tout-1) // Tout)
    CSB_Write(regs, Matrix_reg_bias+10, dshape[0])
    CSB_Write(regs, Matrix_reg_bias+11, dshape[1])
    CSB_Write(regs, Matrix_reg_bias+12, 0)
    CSB_Write(regs, Matrix_reg_bias+13, Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14, 0)
    CSB_Write(regs, Matrix_reg_bias+15, 0)
    CSB_Write(regs, Matrix_reg_bias+16, 0)
    CSB_Write(regs, Matrix_reg_bias+17, 0)
    CSB_Write(regs, Matrix_reg_bias+18, 0)
    CSB_Write(regs, Matrix_reg_bias+19, 0)
    CSB_Write(regs, Matrix_reg_bias+20, waddrs)
    CSB_Write(regs, Matrix_reg_bias+21, PosEmb_in_line_stride)
    CSB_Write(regs, Matrix_reg_bias+22, 0b0100)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs


def FPGA_Run_Feature2Weight(f_in, wt_out, Out_and_In_Mode, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr=0):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_DAT_DW = f_in[0].device.MAX_DAT_DW
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX
    AXI_DAT_WIDTH = f_in[0].device.AXI_DAT_WIDTH
    log2_AXI_BURST_LEN = f_in[0].device.log2_AXI_BURST_LEN

    dshape, oshape = f_in[0].shape, wt_out[0].shape
    daddrs, oaddrs = f_in[1], wt_out[1]
    width = ((dshape[1] + T_quant_block - 1) // T_quant_block) * T_quant_block
    in_ch = ((dshape[2] + Tout - 1) // Tout) * Tout
    WT_CHin_div_Tblock = in_ch//T_quant_block
    wt_burst_times = ((dshape[1] + Tout - 1) // Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)//MAX_DAT_DW
    CHout_div_Tout = (width+Tout-1) // Tout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block

    WT_CHin_div_Tin = (in_ch+base_Tin-1) // base_Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin
    DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width * dshape[0]
    Height = dshape[0] * dshape[1]

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, 0)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, DAT_IN_SURFACE_STRIDE)
    CSB_Write(regs, Matrix_reg_bias+5, 0)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, Matrix_reg_bias+8, Left_WT_Base_Addr)
    CSB_Write(regs, Matrix_reg_bias+9, 0)
    CSB_Write(regs, Matrix_reg_bias+10, 0)
    CSB_Write(regs, Matrix_reg_bias+11, 0)
    CSB_Write(regs, Matrix_reg_bias+12, Height)
    CSB_Write(regs, Matrix_reg_bias+13, Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14, int((dshape[2] + Tout - 1) // Tout))
    CSB_Write(regs, Matrix_reg_bias+15, 0)
    CSB_Write(regs, Matrix_reg_bias+16, 0)
    CSB_Write(regs, Matrix_reg_bias+17, 0)
    CSB_Write(regs, Matrix_reg_bias+18, 0)
    CSB_Write(regs, Matrix_reg_bias+19, 0)
    CSB_Write(regs, Matrix_reg_bias+20, 0)
    CSB_Write(regs, Matrix_reg_bias+21, 0)
    CSB_Write(regs, Matrix_reg_bias+22, 0b000001)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs


def FPGA_Run_Transpose(f_in, wt_out, Out_and_In_Mode, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_DAT_DW = f_in[0].device.MAX_DAT_DW
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX
    AXI_DAT_WIDTH = f_in[0].device.AXI_DAT_WIDTH
    log2_AXI_BURST_LEN = f_in[0].device.log2_AXI_BURST_LEN

    dshape, oshape = f_in[0].shape, wt_out[0].shape
    daddrs, oaddrs = f_in[1], wt_out[1]
    width = ((dshape[1] + Tout - 1) // Tout) * Tout
    in_ch = ((dshape[2] + T_quant_block - 1) // T_quant_block) * T_quant_block
    WT_CHin_div_Tblock = in_ch // T_quant_block
    wt_burst_times = ((dshape[1] + Tout - 1) // Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)//MAX_DAT_DW
    CHout_div_Tout = (width+Tout-1) // Tout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block

    WT_CHin_div_Tin = (in_ch+base_Tin-1)//base_Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin
    DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*dshape[0]
    DAT_IN_LINE_STRIDE = Pixel_Data_Bytes*width
    Height = dshape[0] * dshape[1]

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, 0)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, DAT_IN_SURFACE_STRIDE)
    CSB_Write(regs, Matrix_reg_bias+5, DAT_IN_LINE_STRIDE)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, Matrix_reg_bias+8, Left_WT_Base_Addr)
    CSB_Write(regs, Matrix_reg_bias+9, (in_ch+Tout-1) // Tout)
    CSB_Write(regs, Matrix_reg_bias+10, dshape[0])
    CSB_Write(regs, Matrix_reg_bias+11, width)
    CSB_Write(regs, Matrix_reg_bias+12, Height)
    CSB_Write(regs, Matrix_reg_bias+13, Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14, 0)
    CSB_Write(regs, Matrix_reg_bias+15, 0)
    CSB_Write(regs, Matrix_reg_bias+16, 0)
    CSB_Write(regs, Matrix_reg_bias+17, 0)
    CSB_Write(regs, Matrix_reg_bias+18, dshape[2])
    CSB_Write(regs, Matrix_reg_bias+19, wt_burst_times)
    CSB_Write(regs, Matrix_reg_bias+20, 0)
    CSB_Write(regs, Matrix_reg_bias+21, 0)
    CSB_Write(regs, Matrix_reg_bias+22, 0b010)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs


def FPGA_Run_Activation(act_sim, f_in, f_out, Out_and_In_Mode):
    Pixel_Data_Bytes = f_in[0].device.Pixel_Data_Bytes
    base_Tin, Tout = f_in[0].device.base_Tin, f_in[0].device.Tout
    T_quant_block = f_in[0].device.T_quant_block
    WT_CH_Tgroup = f_in[0].device.WT_CH_Tgroup
    HBM_AXI_DATA_WIDTH = f_in[0].device.HBM_AXI_DATA_WIDTH
    MAX_WT_DW = f_in[0].device.MAX_WT_DW
    HBM_Port = f_in[0].device.HBM_Port
    DAT_BRAM_DEPTH = f_in[0].device.DAT_BRAM_DEPTH
    WT_BRAM_DEPTH = f_in[0].device.WT_BRAM_DEPTH
    AXI_BN_WIDTH = f_in[0].device.AXI_BN_WIDTH
    BN_FIFO_DEP = f_in[0].device.BN_FIFO_DEP
    BN_FIFO_NUM = f_in[0].device.BN_FIFO_NUM
    MAX_DAT_DW = f_in[0].device.MAX_DAT_DW
    MAX_BN_DW = f_in[0].device.MAX_BN_DW
    AXI_BURST_LEN_SOFTMAX = f_in[0].device.AXI_BURST_LEN_SOFTMAX
    AXI_DAT_WIDTH = f_in[0].device.AXI_DAT_WIDTH
    log2_AXI_BURST_LEN = f_in[0].device.log2_AXI_BURST_LEN

    dshape, oshape = f_in[0].shape, f_out[0].shape
    daddrs, waddrs, oaddrs = f_in[1], act_sim[1], f_out[1]
    width = dshape[1]
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * width
    dat_in_line_stride = Pixel_Data_Bytes * width
    dat_out_surface_stride = Pixel_Data_Bytes * dshape[0] * width
    dat_out_line_stride = Pixel_Data_Bytes * width

    regs = []
    CSB_Write(regs, Matrix_reg_bias+2, waddrs)
    CSB_Write(regs, Matrix_reg_bias+3, daddrs)
    CSB_Write(regs, Matrix_reg_bias+4, dat_in_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+5, dat_in_line_stride)
    CSB_Write(regs, Matrix_reg_bias+6, oaddrs)
    CSB_Write(regs, Matrix_reg_bias+7, dat_out_surface_stride)
    CSB_Write(regs, Matrix_reg_bias+8, dat_out_line_stride)
    CSB_Write(regs, Matrix_reg_bias+9, (dshape[2]+Tout-1) // Tout)
    CSB_Write(regs, Matrix_reg_bias+10,dshape[0])
    CSB_Write(regs, Matrix_reg_bias+11,width)
    CSB_Write(regs, Matrix_reg_bias+12,dshape[1])
    CSB_Write(regs, Matrix_reg_bias+13,Out_and_In_Mode)
    CSB_Write(regs, Matrix_reg_bias+14,0)
    CSB_Write(regs, Matrix_reg_bias+15,0)
    CSB_Write(regs, Matrix_reg_bias+16,0)
    CSB_Write(regs, Matrix_reg_bias+17,0)
    CSB_Write(regs, Matrix_reg_bias+18,0)
    CSB_Write(regs, Matrix_reg_bias+19,0)
    CSB_Write(regs, Matrix_reg_bias+20,0)
    CSB_Write(regs, Matrix_reg_bias+21,0)
    CSB_Write(regs, Matrix_reg_bias+22,0b010000)
    CSB_Read(regs, Matrix_reg_bias+1, 1)
    return regs