#include "matrix.h"

int FP32_to_FP20(float fp32_i) 
{
    int fp32_i_s, fp32_i_e, fp32_i_f;
    int fp20_o_s, fp20_o_e, fp20_o_m_tmp, fp20_o_m;
    int overflow_tmp, underflow_tmp;
    
    fp32_i_s = 1 & (*((int*)&fp32_i) >> 31); // {1{1'b1}}
    fp32_i_e = 255 & (*((int*)&fp32_i) >> 23); // {8{1'b1}}
    fp32_i_f = 8388607 & (*((int*)&fp32_i)); // {23{1'b1}}

    if (fp32_i_e < 84) {
        fp20_o_s = 0;
        fp20_o_e = 0;
        fp20_o_m = 0;
        overflow_tmp = 0;
        underflow_tmp = 1;
    } else if (fp32_i_e >= 159) {
        fp20_o_s = fp32_i_s;
        fp20_o_e = 0b111110;
        fp20_o_m = 0b1111111111111;
        overflow_tmp = 1;
        underflow_tmp = 0;
    } else {
        fp20_o_s = fp32_i_s;
        overflow_tmp = 0;
        underflow_tmp = 0;
        if (fp32_i_e >= 97) {
            fp20_o_e = fp32_i_e - 96;
            int c_tmp = 1 & (fp32_i_f >> 9);
            if (c_tmp)
                fp20_o_m_tmp = fp32_i_f + 0b10000000000;
            else 
                fp20_o_m_tmp = fp32_i_f;
            fp20_o_m = ((1<<13)-1) & (fp20_o_m_tmp >> 10);
        } else {
            fp20_o_e = 0;
            int r_cnt = 97 - fp32_i_e;
            int i_m_tmp0 = (1 << 23) + fp32_i_f;
            int c_tmp = 1 & (i_m_tmp0 >> (9 + r_cnt));
            if (c_tmp) {
                int i_m_tmp1 = (i_m_tmp0 >> r_cnt) + 0b10000000000;
                fp20_o_m = ((1<<13)-1) & (i_m_tmp1 >> 10);
            } else {
                int i_m_tmp1 = (i_m_tmp0 >> r_cnt);
                fp20_o_m = ((1<<13)-1) & (i_m_tmp1 >> 10);
            }
        }
    }

    return (fp20_o_s << 19) + (fp20_o_e << 13) + fp20_o_m;
}

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int Softmax_Out_and_In_Mode, HANDLE device)
{
    ObjCheck(feature_in);
    int width = ((feature_in->width + AXI_BURST_LEN_SOFTMAX - 1) / AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX;
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_in->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_in->width;

    CSB_Write(device, Matrix_reg_bias+2 ,0);
	CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(device, Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(device, Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(device, Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(device, Matrix_reg_bias+8 ,dat_out_line_stride);
	
	CSB_Write(device, Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout );
	CSB_Write(device, Matrix_reg_bias+10,feature_in->height);//height
    CSB_Write(device, Matrix_reg_bias+11,width);//Win=Tout
    CSB_Write(device, Matrix_reg_bias+12,feature_in->width);//pixel_in
    CSB_Write(device, Matrix_reg_bias+13,Softmax_Out_and_In_Mode);
	CSB_Write(device, Matrix_reg_bias+14,0);
	CSB_Write(device, Matrix_reg_bias+15,0);
	CSB_Write(device, Matrix_reg_bias+16,0);
	CSB_Write(device, Matrix_reg_bias+17,0);
	CSB_Write(device, Matrix_reg_bias+18,feature_in->channel);
	CSB_Write(device, Matrix_reg_bias+19,0);
	CSB_Write(device, Matrix_reg_bias+20,0);
	CSB_Write(device, Matrix_reg_bias+21,0);
	CSB_Write(device, Matrix_reg_bias+22,0b00001000);//Ln_start
    
	while (CSB_Read(device, Matrix_reg_bias+1) != 1) {}
    AutoFree(feature_in);
}


void FPGA_Run_Activation(int parameters_addr, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int Activation_Out_and_In_Mode, HANDLE device)
{
    ObjCheck(feature_in);
    // int width = ((feature_in->width + AXI_BURST_LEN_SOFTMAX - 1) / AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX;
    int width = feature_in->width;
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * width;
    int dat_in_line_stride = Pixel_Data_Bytes * width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * width;
    int dat_out_line_stride = Pixel_Data_Bytes * width;

    CSB_Write(device, Matrix_reg_bias+2, parameters_addr);
	CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(device, Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(device, Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(device, Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(device, Matrix_reg_bias+8 ,dat_out_line_stride);
	CSB_Write(device, Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
    CSB_Write(device, Matrix_reg_bias+10,feature_in->height);//pixel_in
    CSB_Write(device, Matrix_reg_bias+11,width);//Win
    CSB_Write(device, Matrix_reg_bias+12,feature_in->width);//pixel_in
    CSB_Write(device, Matrix_reg_bias+13,Activation_Out_and_In_Mode);
    CSB_Write(device, Matrix_reg_bias+14,0);
    CSB_Write(device, Matrix_reg_bias+15,0);
    CSB_Write(device, Matrix_reg_bias+16,0);
    CSB_Write(device, Matrix_reg_bias+17,0);
    CSB_Write(device, Matrix_reg_bias+18,0);
    CSB_Write(device, Matrix_reg_bias+19,0);
    CSB_Write(device, Matrix_reg_bias+20,0);
    CSB_Write(device, Matrix_reg_bias+21,0);
	//Kick of the run
	CSB_Write(device, Matrix_reg_bias+22,0b010000);//Ln_start
	
	while (CSB_Read(device, Matrix_reg_bias+1) != 1)
	{
    }
    AutoFree(feature_in);
}

void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int RMS_Norm, int LN_Out_and_In_Mode, HANDLE device)
{
    ObjCheck(feature_in);
    ObjCheck(wt_and_bias);
    int chin_padding_with_tout = ((feature_in->channel + Tout - 1) / Tout) * Tout;
    int LN_num_per_AXI_DW = AXI_DAT_WIDTH/(2*MAX_BN_DW);
    int Layer_Norm = 1-RMS_Norm;
    int LN_CH_burst_times_minus1=int(chin_padding_with_tout/LN_num_per_AXI_DW)>>int(log2_AXI_BURST_LEN);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;
    float temp = 1.0f/(chin_padding_with_tout*1.0f);
    int fp20_data = FP32_to_FP20(temp);
    int Height = feature_in->height * feature_in->width;

	CSB_Write(device, Matrix_reg_bias+2 ,(uint64_t)(wt_and_bias->payload));
	CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(device, Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(device, Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(device, Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(device, Matrix_reg_bias+8 ,dat_out_line_stride);
	CSB_Write(device, Matrix_reg_bias+9 ,(chin_padding_with_tout+Tout-1) / Tout);
	CSB_Write(device, Matrix_reg_bias+10,feature_in->height);//height
    CSB_Write(device, Matrix_reg_bias+11,feature_in->width);
    CSB_Write(device, Matrix_reg_bias+12,Height);//pixel_in
	CSB_Write(device, Matrix_reg_bias+13,LN_Out_and_In_Mode);//LN_Out_and_In_Mode
    CSB_Write(device, Matrix_reg_bias+14,0);
    CSB_Write(device, Matrix_reg_bias+15,fp20_data);
    CSB_Write(device, Matrix_reg_bias+16,LN_CH_burst_times_minus1);
    CSB_Write(device, Matrix_reg_bias+17,Layer_Norm);
    CSB_Write(device, Matrix_reg_bias+18,0);
    CSB_Write(device, Matrix_reg_bias+19,0);
    CSB_Write(device, Matrix_reg_bias+20,0);
    CSB_Write(device, Matrix_reg_bias+21,0);
	CSB_Write(device, Matrix_reg_bias+22,0b0100000);//Ln_stage1_start
	while (CSB_Read(device, Matrix_reg_bias+1) != 1){	}
    AutoFree(feature_in);
    AutoFree(wt_and_bias);
}

void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out, int Transpose_Out_and_In_Mode, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    int width = ((feature_in->width + Tout - 1) / Tout) * Tout;
    int in_ch = ((feature_in->channel + T_quant_block - 1) / T_quant_block) * T_quant_block;
	int pixel_in,pixel_out,wt_burst_times,CHout_div_Tout, Block_wt_bits;
    int WT_CHin_div_Tblock = in_ch/T_quant_block;
    wt_burst_times = ((feature_in->width + Tout - 1) / Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)/MAX_DAT_DW;
    CHout_div_Tout = (width+Tout-1) / Tout; // in->width=out->chout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block;

    int WT_CHin_div_Tin = (in_ch+base_Tin-1)/base_Tin;
    int WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin;
    int DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*feature_in->height;
    int DAT_IN_LINE_STRIDE = Pixel_Data_Bytes*width;
    int Height = feature_in->height * feature_in->width;

	CSB_Write(device, Matrix_reg_bias+2 ,0);
    CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)feature_in->payload);//feature_in_base_addr
	CSB_Write(device, Matrix_reg_bias+4 ,DAT_IN_SURFACE_STRIDE);
	CSB_Write(device, Matrix_reg_bias+5 ,DAT_IN_LINE_STRIDE);
    CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)feature2weight_out->payload);//feature_out_base_addr
    CSB_Write(device, Matrix_reg_bias+7 ,log2_WT_base_addr_Bank_Step);//feature_in_base_addr
    CSB_Write(device, Matrix_reg_bias+8 ,Left_WT_Base_Addr);//feature_out_base_addr
	CSB_Write(device, Matrix_reg_bias+9 ,(in_ch+Tout-1) / Tout);
    CSB_Write(device, Matrix_reg_bias+10,feature_in->height);//feature_in->width;
    CSB_Write(device, Matrix_reg_bias+11,width);//feature_in->width;
    CSB_Write(device, Matrix_reg_bias+12,Height);//feature_in->width;
	CSB_Write(device, Matrix_reg_bias+13,Transpose_Out_and_In_Mode);//LN_Out_and_In_Mode
	CSB_Write(device, Matrix_reg_bias+14,0);
	CSB_Write(device, Matrix_reg_bias+15,0);
	CSB_Write(device, Matrix_reg_bias+16,0);
	CSB_Write(device, Matrix_reg_bias+17,0);
	CSB_Write(device, Matrix_reg_bias+18,feature_in->channel);
    CSB_Write(device, Matrix_reg_bias+19,wt_burst_times);//wt_num_div_Tout
	CSB_Write(device, Matrix_reg_bias+20,0);
	CSB_Write(device, Matrix_reg_bias+21,0);
	CSB_Write(device, Matrix_reg_bias+22,0b010);
	while (CSB_Read(device, Matrix_reg_bias+1) != 1){	}
	//Kick of the run
    AutoFree(feature_in);
}


void FPGA_Run_Feature2Weight(struct Mapped_Feature* feature_in, struct Mapped_Weight* weight_out, int Out_and_In_Mode, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    int width = ((feature_in->width + T_quant_block - 1) / T_quant_block) * T_quant_block;
    int in_ch = ((feature_in->channel + Tout - 1) / Tout) * Tout;
	int pixel_in,pixel_out,wt_burst_times,CHout_div_Tout, Block_wt_bits;
    int WT_CHin_div_Tblock = in_ch/T_quant_block;
    wt_burst_times = ((feature_in->width + Tout - 1) / Tout)*WT_CHin_div_Tblock*(MAX_DAT_DW+T_quant_block*4)/MAX_DAT_DW;
    CHout_div_Tout = (width+Tout-1) / Tout; // in->width=out->chout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block;

    int WT_CHin_div_Tin = (in_ch+base_Tin-1)/base_Tin;
    int WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin;
    int DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*feature_in->height;
    int Height = feature_in->height * feature_in->width;
	
	CSB_Write(device, Matrix_reg_bias+2 ,0);
    CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)feature_in->payload);//feature_in_base_addr
	CSB_Write(device, Matrix_reg_bias+4 ,DAT_IN_SURFACE_STRIDE);
	CSB_Write(device, Matrix_reg_bias+5 ,0);
    CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)weight_out->payload);//feature_out_base_addr
    CSB_Write(device, Matrix_reg_bias+7 ,log2_WT_base_addr_Bank_Step);//feature_in_base_addr
    CSB_Write(device, Matrix_reg_bias+8 ,Left_WT_Base_Addr);//feature_out_base_addr
	CSB_Write(device, Matrix_reg_bias+9 ,0);
	CSB_Write(device, Matrix_reg_bias+10,0);
	CSB_Write(device, Matrix_reg_bias+11,0);
    CSB_Write(device, Matrix_reg_bias+12,Height);//pixel_in
	CSB_Write(device, Matrix_reg_bias+13,Out_and_In_Mode);//LN_Out_and_In_Mode
    CSB_Write(device, Matrix_reg_bias+14,int((feature_in->channel + Tout - 1) / Tout));//pixel_in
	CSB_Write(device, Matrix_reg_bias+15,0);
    CSB_Write(device, Matrix_reg_bias+16,0);
    CSB_Write(device, Matrix_reg_bias+17,0);
    CSB_Write(device, Matrix_reg_bias+18,0);
    CSB_Write(device, Matrix_reg_bias+19,0);
    CSB_Write(device, Matrix_reg_bias+20,0);
    CSB_Write(device, Matrix_reg_bias+21,0);
	CSB_Write(device, Matrix_reg_bias+22,0b000001);
	while (CSB_Read(device, Matrix_reg_bias+1) != 1){	}
    AutoFree(feature_in);
	//Kick of the run
}

void FPGA_Run_PosEmb(struct Mapped_Feature* feature_in, struct Mapped_Feature* pos_in, struct Mapped_Feature* feature_out, int Pos_Num, int Out_and_In_Mode, HANDLE device)
{
    ObjCheck(feature_in);
    ObjCheck(pos_in);
    int chin_padding_with_tout = ((feature_in->channel + Tout - 1) / Tout) * Tout;
    int PosEmb_in_line_stride = Pixel_Data_Bytes * Pos_Num;
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;

	CSB_Write(device, Matrix_reg_bias+2 ,0);
	CSB_Write(device, Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(device, Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(device, Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(device, Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(device, Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(device, Matrix_reg_bias+8 ,dat_out_line_stride);
	CSB_Write(device, Matrix_reg_bias+9 ,(chin_padding_with_tout+Tout-1) / Tout);
	CSB_Write(device, Matrix_reg_bias+10,feature_in->height);//height
    CSB_Write(device, Matrix_reg_bias+11,feature_in->width);
    CSB_Write(device, Matrix_reg_bias+12,0);
    CSB_Write(device, Matrix_reg_bias+13,Out_and_In_Mode);
    CSB_Write(device, Matrix_reg_bias+14,0);
    CSB_Write(device, Matrix_reg_bias+15,0);
    CSB_Write(device, Matrix_reg_bias+16,0);
    CSB_Write(device, Matrix_reg_bias+17,0);
    CSB_Write(device, Matrix_reg_bias+18,0);
    CSB_Write(device, Matrix_reg_bias+19,0);
    CSB_Write(device, Matrix_reg_bias+20,(uint64_t)(pos_in->payload));
    CSB_Write(device, Matrix_reg_bias+21,PosEmb_in_line_stride);
	CSB_Write(device, Matrix_reg_bias+22,0b0100);
	while (CSB_Read(device, Matrix_reg_bias+1) != 1){	}
    AutoFree(feature_in);
}


