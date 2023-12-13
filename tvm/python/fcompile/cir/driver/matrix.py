from .basic import *
# from ..ffi import FP32_to_FP20
from ...ne import Var, For, Numb, expr_for_hook
from .register import register_driver

Matrix_reg_bias = 192


@register_driver("hbm", "softmax", "1126")
def FPGA_Run_Softmax(feature_in, feature_out, Softmax_Out_and_In_Mode, tag):
    width = ((feature_in.width + AXI_BURST_LEN_SOFTMAX - 1) // AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX
    dat_in_surface_stride = feature_in.height * width * Pixel_Data_Bytes
    dat_in_line_stride = width * Pixel_Data_Bytes
    dat_out_surface_stride = feature_out.height * width * Pixel_Data_Bytes
    dat_out_line_stride = width * Pixel_Data_Bytes

    CSB_Write(tag, Matrix_reg_bias+3, feature_in.payload)
    CSB_Write(tag, Matrix_reg_bias+4, dat_in_surface_stride)
    CSB_Write(tag, Matrix_reg_bias+5, dat_in_line_stride)
    CSB_Write(tag, Matrix_reg_bias+6, feature_out.payload)
    CSB_Write(tag, Matrix_reg_bias+7, dat_out_surface_stride)
    CSB_Write(tag, Matrix_reg_bias+8, dat_out_line_stride)

    CSB_Write(tag, Matrix_reg_bias+9 ,(feature_in.channel+Tout-1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+10,feature_in.height)
    CSB_Write(tag, Matrix_reg_bias+11,width)

    CSB_Write(tag, Matrix_reg_bias+19,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+23,feature_in.channel)
    CSB_Write(tag, Matrix_reg_bias+0,0b00001000)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)


@register_driver("hbm", "activation", "1126")
def FPGA_Run_Activation(fp16_wt, fp16_bias, fp16_x_region, feature_in, feature_out, Activation_Out_and_In_Mode, tag):
    width = ((feature_in.width + AXI_BURST_LEN_SOFTMAX - 1) // AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX
    dat_in_surface_stride = Pixel_Data_Bytes * feature_in.height * width
    dat_in_line_stride = Pixel_Data_Bytes * width
    dat_out_surface_stride = Pixel_Data_Bytes * feature_out.height * width
    dat_out_line_stride = Pixel_Data_Bytes * width

    CSB_Write(tag, Matrix_reg_bias+3 ,feature_in.payload);
    CSB_Write(tag, Matrix_reg_bias+4 ,dat_in_surface_stride);
    CSB_Write(tag, Matrix_reg_bias+5 ,dat_in_line_stride);
    CSB_Write(tag, Matrix_reg_bias+6 ,feature_out.payload);
    CSB_Write(tag, Matrix_reg_bias+7 ,dat_out_surface_stride);
    CSB_Write(tag, Matrix_reg_bias+8 ,dat_out_line_stride);
    CSB_Write(tag, Matrix_reg_bias+9 ,(feature_in.channel+Tout-1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+11,width)
    CSB_Write(tag, Matrix_reg_bias+19,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+26,Activation_Out_and_In_Mode)
    CSB_Write(tag, Matrix_reg_bias+29,0b110000000000000000)
    for i in range(47):
        parameter_addr = i+1
        if i < 16:
            CSB_Write(tag, Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_wt[i])
        else:
            if i<32:
                CSB_Write(tag, Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_bias[i-16])
            else:
                CSB_Write(tag, Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_x_region[i-32])
    CSB_Write(tag, Matrix_reg_bias+29,0b100000000000000000)
    CSB_Write(tag, Matrix_reg_bias+0,0b000010000)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)


@register_driver("hbm", "layer_norm", "1126")
def FPGA_Run_LN(feature_in, wt_and_bias, feature_out, RMS_Norm, LN_Out_and_In_Mode, tag):
    chin_padding_with_tout = ((feature_in.channel + Tout - 1) // Tout) * Tout
    LN_num_per_AXI_DW = AXI_DAT_WIDTH // (2*MAX_BN_DW)
    Layer_Norm = 1-RMS_Norm
    LN_CH_burst_times_minus1 = int(chin_padding_with_tout//LN_num_per_AXI_DW)>>int(log2_AXI_BURST_LEN)
    dat_in_surface_stride = Pixel_Data_Bytes * feature_in.height * feature_in.width
    dat_in_line_stride = Pixel_Data_Bytes * feature_in.width
    dat_out_surface_stride = Pixel_Data_Bytes * feature_out.height * feature_out.width
    dat_out_line_stride = Pixel_Data_Bytes * feature_out.width
    temp = 1.0/(chin_padding_with_tout*1.0)
    fp20_data = FP32_to_FP20(temp)

    CSB_Write(tag, Matrix_reg_bias+2 ,wt_and_bias.payload)
    CSB_Write(tag, Matrix_reg_bias+3 ,feature_in.payload)
    CSB_Write(tag, Matrix_reg_bias+4 ,dat_in_surface_stride)
    CSB_Write(tag, Matrix_reg_bias+5 ,dat_in_line_stride)
    CSB_Write(tag, Matrix_reg_bias+6 ,feature_out.payload)
    CSB_Write(tag, Matrix_reg_bias+7 ,dat_out_surface_stride)
    CSB_Write(tag, Matrix_reg_bias+8 ,dat_out_line_stride)
    CSB_Write(tag, Matrix_reg_bias+9 ,(chin_padding_with_tout+Tout-1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+10,feature_in.height)
    CSB_Write(tag, Matrix_reg_bias+11,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+18,fp20_data)

    CSB_Write(tag, Matrix_reg_bias+19,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+28,LN_CH_burst_times_minus1)
    CSB_Write(tag, Matrix_reg_bias+30,Layer_Norm)
    
    CSB_Write(tag, Matrix_reg_bias+0,0b00100000)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)
    
    CSB_Write(tag, Matrix_reg_bias+26,LN_Out_and_In_Mode)
    CSB_Write(tag, Matrix_reg_bias+0,0b01000000)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)


@register_driver("hbm", "transpose", "1126")
def FPGA_Run_Transpose(feature_in, feature2weight_out, Transpose_Out_and_In_Mode, tag, log2_WT_base_addr_Bank_Step=8, Left_WT_Base_Addr=0):
    width = ((feature_in.width + Tout - 1) // Tout) * Tout
    in_ch = ((feature_in.channel + T_quant_block - 1) // T_quant_block) * T_quant_block
    WT_CHin_div_Tblock = in_ch//T_quant_block
    wt_burst_times = ((feature_in.width + Tout - 1) // Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)//MAX_DAT_DW
    CHout_div_Tout = (width+Tout-1) // Tout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block

    WT_CHin_div_Tin = (in_ch+base_Tin-1)//base_Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin
    DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*feature_in.height

    CSB_Write(tag, Matrix_reg_bias+4 ,DAT_IN_SURFACE_STRIDE)
    CSB_Write(tag, Matrix_reg_bias+9 ,(in_ch+Tout-1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+11,width)
    CSB_Write(tag, Matrix_reg_bias+19,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+23,feature_in.channel)
    CSB_Write(tag, Matrix_reg_bias+25,wt_burst_times)
    CSB_Write(tag, Matrix_reg_bias+26,Transpose_Out_and_In_Mode)

    CSB_Write(tag, Matrix_reg_bias+3,feature_in.payload)
    CSB_Write(tag, Matrix_reg_bias+6,feature2weight_out.payload)

    CSB_Write(tag, Matrix_reg_bias+7,log2_WT_base_addr_Bank_Step)
    CSB_Write(tag, Matrix_reg_bias+8,Left_WT_Base_Addr)

    CSB_Write(tag, Matrix_reg_bias+0, 0b010)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)


@register_driver("hbm", "transpose", "1110")
def FPGA_Run_Transpose_1110(feature_in, feature2weight_out, Transpose_Out_and_In_Mode, tag):
    width = ((feature_in.width + Tout - 1) // Tout) * Tout
    in_ch = ((feature_in.channel + T_quant_block - 1) // T_quant_block) * T_quant_block
    pixel_in = width
    wt_burst_times = (MAX_DAT_DW+T_quant_block*4)//MAX_DAT_DW
    CHout_div_Tout = (width+Tout-1) // Tout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block

    WT_CHin_div_Tin = (in_ch+base_Tin-1)//base_Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin
    WT_CHin_div_Tblock = (WT_CHin_Padding_with_Tin+T_quant_block-1)//T_quant_block
    DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*feature_in.height
    
    CSB_Write(tag, Matrix_reg_bias+4 ,DAT_IN_SURFACE_STRIDE)
    CSB_Write(tag, Matrix_reg_bias+9 ,(in_ch+Tout-1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+11,Tout)
    CSB_Write(tag, Matrix_reg_bias+19,pixel_in)
    CSB_Write(tag, Matrix_reg_bias+23,in_ch)
    CSB_Write(tag, Matrix_reg_bias+25,wt_burst_times)
    CSB_Write(tag, Matrix_reg_bias+26,Transpose_Out_and_In_Mode)

    dat_out_base_addr = feature2weight_out.payload

    @expr_for_hook(CSB_For, CSB_End)
    def for_body_1(chout):

        @expr_for_hook(CSB_For, CSB_End)
        def for_body_2(chin):
            in_base_addr = feature_in.payload+chout*(Pixel_Data_Bytes*Tout)+chin*(DAT_IN_SURFACE_STRIDE*T_quant_block//Tout)
            CSB_Write(tag, Matrix_reg_bias+3 ,in_base_addr)
            CSB_Write(tag, Matrix_reg_bias+6 ,dat_out_base_addr + chin * Block_wt_bits//8)
            CSB_Write(tag, Matrix_reg_bias+0,0b00000010)
            CSB_Read(tag, Matrix_reg_bias + 1, 1)

        chin = Var("b")
        for_block_2 = For(chin, Numb(0), WT_CHin_div_Tblock, for_body_2)
        for_block_2.run(tag)

    chout = Var("n")
    for_block_1 = For(chout, Numb(0), CHout_div_Tout, for_body_1)
    for_block_1.run(tag)


@register_driver("hbm", "feature2weight", "1126")
def FPGA_Run_Feature2Weight(feature_in, weight_out, Out_and_In_Mode, tag, log2_WT_base_addr_Bank_Step=8, Left_WT_Base_Addr=0):
    width = ((feature_in.width + Tout - 1) // Tout) * Tout
    in_ch = ((feature_in.channel + T_quant_block - 1) // T_quant_block) * T_quant_block
    WT_CHin_div_Tblock = in_ch//T_quant_block
    wt_burst_times = ((feature_in.width + Tout - 1) // Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)//MAX_DAT_DW
    CHout_div_Tout = (width+Tout-1) // Tout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block

    WT_CHin_div_Tin = (in_ch+base_Tin-1)//base_Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin
    DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*feature_in.width*feature_in.height

    CSB_Write(tag, Matrix_reg_bias+3,feature_in.payload)
    CSB_Write(tag, Matrix_reg_bias+4,DAT_IN_SURFACE_STRIDE)
    CSB_Write(tag, Matrix_reg_bias+6,weight_out.payload)
    CSB_Write(tag, Matrix_reg_bias+7,log2_WT_base_addr_Bank_Step)
    CSB_Write(tag, Matrix_reg_bias+8,Left_WT_Base_Addr)
    CSB_Write(tag, Matrix_reg_bias+19,feature_in.width)
    CSB_Write(tag, Matrix_reg_bias+20,(feature_in.width + Tout - 1) // Tout)
    CSB_Write(tag, Matrix_reg_bias+26,Out_and_In_Mode)

    CSB_Write(tag, Matrix_reg_bias+0, 0b10000000)
    CSB_Read(tag, Matrix_reg_bias + 1, 1)
