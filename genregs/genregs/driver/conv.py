from .basic import *
from ..ne import Var, If, For, Numb, expr_for_hook


def FPGA_RunHBM_MVM(relu_en, mode, f_in, wt, f_bn, f_res, f_out, Skip_Factor, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
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

    dshape, wshape, oshape = f_in[0].shape, wt[0].shape, f_out[0].shape
    daddrs, waddrs, baddrs, raddrs, oaddrs = f_in[1], wt[1], f_bn[1], f_res[1], f_out[1]
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1]
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1]

    current_in_base_addr = in_base_addr if In_from_BRAM else daddrs
    current_out_base_addr = out_base_addr if Out_to_BRAM else oaddrs

    wt_dw = 4
    Tin, Tin_div_Tout = base_Tin, (base_Tin+Tout-1)//Tout
    if len(dshape) == 2:
        Hin, Win, CHin, CHout = 1, dshape[0], dshape[1], oshape[1]
    elif len(dshape) == 3:
        Hin, Win, CHin, CHout = dshape[0], dshape[1], dshape[2], oshape[2]
    Hout, Wout = Hin//Skip_Factor, Win
    CHout_div_Tout = (CHout+Tout-1)//Tout
    CHin_div_Tout = (CHin+Tout-1)//Tout
    CHout_Padding = CHout_div_Tout*Tout
    CHout_Padding_with_Tout = CHout_div_Tout*Tout
    CHin = CHin_div_Tout*Tout
    WT_CHin_div_Tin = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*Tin
    WT_CHin_div_Tblock = WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits = CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN = CHout_Padding*WT_CHin_div_Tin

    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!")
        exit(-1)
    if min_wt_depth > WT_BRAM_DEPTH:
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH // min_wt_depth) * Tout
    if baddrs is not None:
        BN_FIFO_bits = AXI_BN_WIDTH * BN_FIFO_DEP * BN_FIFO_NUM
        BN_FIFO_chout_num = BN_FIFO_bits // (MAX_BN_DW*2)
        out_ch_slice = If(out_ch_slice > BN_FIFO_chout_num, BN_FIFO_chout_num, out_ch_slice)
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, 1, (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice_last = If(CHout_Padding % out_ch_slice, CHout_Padding % out_ch_slice, out_ch_slice)

    CHout_Split_Times_minus1 = CHout_Split_Times-1
    wt_size_in_bits = WT_SIZE_IN_BYTES//CHout_Padding*8
    Height = Win * Hin
    CHout_last = out_ch_slice_last
    CHout = out_ch_slice

    regs = []
    CSB_Write(regs, 2, CHin)
    CSB_Write(regs, 3, Win)
    CSB_Write(regs, 4, Hin)
    CSB_Write(regs, 5, Wout)
    CSB_Write(regs, 6, Hout)
    CSB_Write(regs, 7, CHout)
    CSB_Write(regs, 8, CHout_last)
    CSB_Write(regs, 9, Height)
    CSB_Write(regs, 10, current_in_base_addr)
    CSB_Write(regs, 11, waddrs)
    CSB_Write(regs, 12, wt_size_in_bits)
    CSB_Write(regs, 13, current_out_base_addr)
    CSB_Write(regs, 14, CHout_Split_Times_minus1)
    CSB_Write(regs, 15, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, 16, relu_en)
    CSB_Write(regs, 17, Skip_Factor-1)
    CSB_Write(regs, 18, (Out_to_BRAM << 1) | In_from_BRAM)

    CSB_Write(regs, 19, 0)
    CSB_Write(regs, 20, 0)
    CSB_Write(regs, 21, 1)
    CSB_Write(regs, 22, 1)
    CSB_Write(regs, 23, 1)
    CSB_Write(regs, 24, 1)
    CSB_Write(regs, 25, 0)
    CSB_Write(regs, 26, baddrs)
    CSB_Write(regs, 27, raddrs)
    CSB_Write(regs, 28, 0)
    CSB_Write(regs, 29, dat_in_surface_stride)
    CSB_Write(regs, 30, dat_in_line_stride)
    CSB_Write(regs, 31, dat_out_surface_stride)
    CSB_Write(regs, 32, dat_out_line_stride)
    CSB_Write(regs, 33, mode)
    CSB_Read(regs, 1, 1)
    return regs


def FPGA_RunHBM_MVM_BN_Res_ArgMax(relu_en, mode, f_in, wt, f_bn, f_res, f_out, arg_out, Skip_Factor, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
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

    dshape, wshape, oshape, ashape = f_in[0].shape, wt[0].shape, f_out[0].shape, arg_out[0].shape
    daddrs, waddrs, baddrs, raddrs, oaddrs, aaddrs = f_in[1], wt[1], f_bn[1], f_res[1], f_out[1], arg_out[1]
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1]
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1]

    current_in_base_addr = in_base_addr if In_from_BRAM else daddrs
    current_out_base_addr = out_base_addr if Out_to_BRAM else oaddrs

    wt_dw = 4
    Tin, Tin_div_Tout = base_Tin, (base_Tin+Tout-1)//Tout
    Hin, Win, CHout = dshape[0], dshape[1], oshape[2]
    Hout, Wout = Hin//Skip_Factor, Win
    CHout_div_Tout = (CHout+Tout-1)//Tout
    CHin_div_Tout = (dshape[2]+Tout-1)//Tout
    CHout_Padding = CHout_div_Tout*Tout
    CHout_Padding_with_Tout = CHout_div_Tout*Tout
    CHin = CHin_div_Tout*Tout
    WT_CHin_div_Tin = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*Tin
    WT_CHin_div_Tblock = WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits = CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN = CHout_Padding*WT_CHin_div_Tin

    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!")
        exit(-1)
    if min_wt_depth > WT_BRAM_DEPTH:
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH // min_wt_depth) * Tout
    if baddrs is not None:
        BN_FIFO_bits = AXI_BN_WIDTH * BN_FIFO_DEP * BN_FIFO_NUM
        BN_FIFO_chout_num = BN_FIFO_bits // (MAX_BN_DW*2)
        out_ch_slice = If(out_ch_slice > BN_FIFO_chout_num, BN_FIFO_chout_num, out_ch_slice)
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, 1, (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice_last = If(CHout_Padding % out_ch_slice, CHout_Padding % out_ch_slice, out_ch_slice)

    CHout_Split_Times_minus1 = CHout_Split_Times-1
    wt_size_in_bits = WT_SIZE_IN_BYTES//CHout_Padding*8
    Height = Win * Hin
    CHout_last = out_ch_slice_last
    CHout = out_ch_slice

    regs = []
    CSB_Write(regs, 2, CHin)
    CSB_Write(regs, 3, Win)
    CSB_Write(regs, 4, Hin)
    CSB_Write(regs, 5, Wout)
    CSB_Write(regs, 6, Hout)
    CSB_Write(regs, 7, CHout)
    CSB_Write(regs, 8, CHout_last)
    CSB_Write(regs, 9, Height)
    CSB_Write(regs, 10, current_in_base_addr)
    CSB_Write(regs, 11, waddrs)
    CSB_Write(regs, 12, wt_size_in_bits)
    CSB_Write(regs, 13, current_out_base_addr)
    CSB_Write(regs, 14, CHout_Split_Times_minus1)
    CSB_Write(regs, 15, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, 16, relu_en)
    CSB_Write(regs, 17, Skip_Factor-1)
    CSB_Write(regs, 18, (Out_to_BRAM << 1) | In_from_BRAM)

    CSB_Write(regs, 19, 0)
    CSB_Write(regs, 20, 0)
    CSB_Write(regs, 21, 1)
    CSB_Write(regs, 22, 1)
    CSB_Write(regs, 23, 1)
    CSB_Write(regs, 24, 1)
    CSB_Write(regs, 25, 0)
    CSB_Write(regs, 26, baddrs)
    CSB_Write(regs, 27, raddrs)
    CSB_Write(regs, 28, 0)
    CSB_Write(regs, 29, dat_in_surface_stride)
    CSB_Write(regs, 30, dat_in_line_stride)
    CSB_Write(regs, 31, dat_out_surface_stride)
    CSB_Write(regs, 32, dat_out_line_stride)
    CSB_Write(regs, 33, mode)
    CSB_Write(regs, 60, aaddrs)
    CSB_Read(regs, 61, 1)
    return regs


def FPGA_RunHBM_MVM_F2W(relu_en, mode, f_in, wt, f_out, Skip_Factor, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
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

    dshape, wshape, oshape = f_in[0].shape, wt[0].shape, f_out[0].shape
    daddrs, waddrs, oaddrs = f_in[1], wt[1], f_out[1]
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1] * Skip_Factor
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1] * Skip_Factor

    current_in_base_addr = in_base_addr if In_from_BRAM else daddrs
    current_out_base_addr = out_base_addr if Out_to_BRAM else oaddrs

    wt_dw = 4
    Tin, Tin_div_Tout = base_Tin, (base_Tin+Tout-1)//Tout
    Hin, Win, CHout = dshape[0], dshape[1], oshape[2]
    Hout, Wout = Hin//Skip_Factor, Win
    Hin = Hin // Skip_Factor
    CHout_div_Tout = (CHout+Tout-1)//Tout
    CHin_div_Tout = (dshape[2]+Tout-1)//Tout
    CHout_Padding = CHout_div_Tout*Tout
    CHout_Padding_with_Tout = CHout_div_Tout*Tout
    CHin = CHin_div_Tout*Tout
    WT_CHin_div_Tin = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*Tin
    WT_CHin_div_Tblock = WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits = CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN = CHout_Padding*WT_CHin_div_Tin

    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!")
        exit(-1)
    if min_wt_depth > WT_BRAM_DEPTH:
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH // min_wt_depth) * Tout
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, 1, (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice_last = If(CHout_Padding % out_ch_slice, CHout_Padding % out_ch_slice, out_ch_slice)

    CHout_Split_Times_minus1 = CHout_Split_Times-1
    wt_size_in_bits = WT_SIZE_IN_BYTES//CHout_Padding*8
    Height = Win * Hin
    CHout_last = out_ch_slice_last
    CHout = out_ch_slice

    regs = []
    CSB_Write(regs, 2, CHin)
    CSB_Write(regs, 3, Win)
    CSB_Write(regs, 4, Hin)
    CSB_Write(regs, 5, Wout)
    CSB_Write(regs, 6, Hout)
    CSB_Write(regs, 7, CHout)
    CSB_Write(regs, 8, CHout_last)
    CSB_Write(regs, 9, Height)
    CSB_Write(regs, 10, current_in_base_addr)
    CSB_Write(regs, 11, waddrs)
    CSB_Write(regs, 12, wt_size_in_bits)
    CSB_Write(regs, 13, current_out_base_addr)
    CSB_Write(regs, 14, CHout_Split_Times_minus1)
    CSB_Write(regs, 15, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, 16, relu_en)
    CSB_Write(regs, 17, 0)
    CSB_Write(regs, 18, (Out_to_BRAM << 1) | In_from_BRAM)

    CSB_Write(regs, 19, 0)
    CSB_Write(regs, 20, 0)
    CSB_Write(regs, 21, 1)
    CSB_Write(regs, 22, 1)
    CSB_Write(regs, 23, 1)
    CSB_Write(regs, 24, 1)
    CSB_Write(regs, 25, 0)
    CSB_Write(regs, 26, 0)
    CSB_Write(regs, 27, 0)
    CSB_Write(regs, 28, 0)
    CSB_Write(regs, 29, dat_in_surface_stride)
    CSB_Write(regs, 30, dat_in_line_stride)
    CSB_Write(regs, 31, dat_out_surface_stride)
    CSB_Write(regs, 32, dat_out_line_stride)

    CSB_Write(regs, 33, mode)
    CSB_Read(regs, 1, 1)
    return regs


def FPGA_RunHBM_MVM_BN_Res_afterTRP(relu_en, mode, f_in, wt, f_bn, f_res, f_out, Skip_Factor, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
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

    dshape, wshape, bshape, rshape, oshape = f_in[0].shape, wt[0].shape, f_bn[0].shape, f_res[0].shape, f_out[0].shape
    daddrs, waddrs, baddrs, raddrs, oaddrs = f_in[1], wt[1], f_bn[1], f_res[1], f_out[1]
    dat_in_surface_stride = Pixel_Data_Bytes * dshape[0] * dshape[1]
    dat_in_line_stride = Pixel_Data_Bytes * dshape[1] * Skip_Factor
    dat_out_surface_stride = Pixel_Data_Bytes * oshape[0] * oshape[1]
    dat_out_line_stride = Pixel_Data_Bytes * oshape[1] * Skip_Factor

    current_in_base_addr = in_base_addr if In_from_BRAM else daddrs
    current_out_base_addr = out_base_addr if Out_to_BRAM else oaddrs

    wt_dw = 4
    Tin, Tin_div_Tout = base_Tin, (base_Tin+Tout-1)//Tout
    Hin, Win, CHout = dshape[0], dshape[1], oshape[2]
    Hout, Wout = Hin//Skip_Factor, Win
    Hin = Hin // Skip_Factor
    CHout_div_Tout = (CHout+Tout-1)//Tout
    CHin_div_Tout = (dshape[2]+Tout-1)//Tout
    CHout_Padding = CHout_div_Tout*Tout
    CHout_Padding_with_Tout = CHout_div_Tout*Tout
    CHin = CHin_div_Tout*Tout
    WT_CHin_div_Tin = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*Tin
    WT_CHin_div_Tblock = WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits = CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN = CHout_Padding*WT_CHin_div_Tin

    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!")
        exit(-1)
    if min_wt_depth > WT_BRAM_DEPTH:
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH // min_wt_depth) * Tout
    if baddrs is not None:
        BN_FIFO_bits = AXI_BN_WIDTH * BN_FIFO_DEP * BN_FIFO_NUM
        BN_FIFO_chout_num = BN_FIFO_bits // (MAX_BN_DW*2)
        out_ch_slice = If(out_ch_slice > BN_FIFO_chout_num, BN_FIFO_chout_num, out_ch_slice)
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, 1, (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice_last = If(CHout_Padding % out_ch_slice, CHout_Padding % out_ch_slice, out_ch_slice)

    CHout_Split_Times_minus1 = CHout_Split_Times-1
    wt_size_in_bits = WT_SIZE_IN_BYTES//CHout_Padding*8
    Height = Win * Hin
    CHout_last = out_ch_slice_last
    CHout = out_ch_slice

    regs = []
    CSB_Write(regs, 2, CHin)
    CSB_Write(regs, 3, Win)
    CSB_Write(regs, 4, Hin)
    CSB_Write(regs, 5, Wout)
    CSB_Write(regs, 6, Hout)
    CSB_Write(regs, 7, CHout)
    CSB_Write(regs, 8, CHout_last)
    CSB_Write(regs, 9, Height)
    CSB_Write(regs, 10, current_in_base_addr)
    CSB_Write(regs, 11, waddrs)
    CSB_Write(regs, 12, wt_size_in_bits)
    CSB_Write(regs, 13, current_out_base_addr)
    CSB_Write(regs, 14, CHout_Split_Times_minus1)
    CSB_Write(regs, 15, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step)
    CSB_Write(regs, 16, relu_en)
    CSB_Write(regs, 17, 0)
    CSB_Write(regs, 18, (Out_to_BRAM << 1) | In_from_BRAM)

    CSB_Write(regs, 19, 0)
    CSB_Write(regs, 20, 0)
    CSB_Write(regs, 21, 1)
    CSB_Write(regs, 22, 1)
    CSB_Write(regs, 23, 1)
    CSB_Write(regs, 24, 1)
    CSB_Write(regs, 25, 0)
    CSB_Write(regs, 26, baddrs)
    CSB_Write(regs, 27, raddrs)
    CSB_Write(regs, 28, 0)
    CSB_Write(regs, 29, dat_in_surface_stride)
    CSB_Write(regs, 30, dat_in_line_stride)
    CSB_Write(regs, 31, dat_out_surface_stride)
    CSB_Write(regs, 32, dat_out_line_stride)

    CSB_Write(regs, 33, mode)
    CSB_Read(regs, 1, 1)
    return regs
