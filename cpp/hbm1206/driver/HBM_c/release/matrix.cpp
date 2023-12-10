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

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int pixel_in, int Softmax_Out_and_In_Mode)
{
    int width = ((feature_in->width + AXI_BURST_LEN_SOFTMAX - 1) / AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX;
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * width;
    int dat_in_line_stride = Pixel_Data_Bytes * width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * width;
    int dat_out_line_stride = Pixel_Data_Bytes * width;

//  pixel_in=197; feature_in->width*height;
//  pixel_out=197;//feature_in->channel;
	
//	printf(" \n reg 3 =%d",(uint64_t)(feature_in->payload));
//	printf(" \n reg 4 =%d",feature_in->surface_stride);
//	printf(" \n reg 5 =%d",feature_in->line_stride);
//	printf(" \n reg 6 =%d",(uint64_t)(feature_out->payload));
//	printf(" \n reg 7 =%d",feature_out->surface_stride);
//	printf(" \n reg 8 =%d",feature_out->line_stride);
//	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout );
//	printf(" \n reg 10=%d",(pixel_in+Tout-1) / Tout);//height
//    printf(" \n reg 11=%d",Tout);//Win=Tout
//	printf(" \n reg 13=%d",feature_in->scale);//height
//    printf(" \n reg 16=%d",feature_out->scale);//Win
//    printf(" \n reg 19=%d",pixel_in);//pixel_in
//	printf(" \n reg 23=%d",pixel_out);//pixel_out
	
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,dat_out_line_stride);
	
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout );
	CSB_Write(Matrix_reg_bias+10,feature_in->height);//height
    CSB_Write(Matrix_reg_bias+11,width);//Win=Tout
	
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
	CSB_Write(Matrix_reg_bias+23,feature_in->channel);//pixel_out
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00001000);//Ln_start
    
	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for concat_wdma done\r\n");
	}
}


void FPGA_Run_Activation(unsigned short fp16_wt[], unsigned short fp16_bias[], unsigned short fp16_x_region[], struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int pixel_in, int Activation_Out_and_In_Mode)
{
	int parameter_addr;
    int width = ((feature_in->width + AXI_BURST_LEN_SOFTMAX - 1) / AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX;
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * width;
    int dat_in_line_stride = Pixel_Data_Bytes * width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * width;
    int dat_out_line_stride = Pixel_Data_Bytes * width;

//	printf(" \n ****************");
//	printf(" \n reg 3 =%ld",(uint64_t)(feature_in->payload));
//	printf(" \n reg 4 =%d",feature_in->surface_stride);
//	printf(" \n reg 5 =%d",feature_in->line_stride);
//	printf(" \n reg 6 =%ld",(uint64_t)(feature_out->payload));
//	printf(" \n reg 7 =%d",feature_out->surface_stride);
//	printf(" \n reg 8 =%d",feature_out->line_stride);
//	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout);
//    printf(" \n reg 11=%d",pixel_in);//Win	
//	printf(" \n reg 13=%d",feature_in->scale);//height
//    printf(" \n reg 16=%d",feature_out->scale);//Win
//    printf(" \n reg 19=%d\n",pixel_in);//pixel_in

	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,dat_out_line_stride);
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
    CSB_Write(Matrix_reg_bias+11,width);//Win
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
    CSB_Write(Matrix_reg_bias+26,Activation_Out_and_In_Mode);
    CSB_Write(Matrix_reg_bias+29,0b110000000000000000);//parameters start
    for(int i=0;i<47;i++)
    {
        parameter_addr=i+1; 
        if(i<16)
            CSB_Write(Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_wt[i]); //FP16_wt
        else 
            if(i<32)
                CSB_Write(Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_bias[i-16]); //FP16_wt
            else
                CSB_Write(Matrix_reg_bias+29,(parameter_addr<<18)+(1<<16)+fp16_x_region[i-32]); //FP16_wt
    }
    CSB_Write(Matrix_reg_bias+29,0b100000000000000000);//parameters done
 
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b000010000);//Ln_start
	
	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
    }
}

void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int RMS_Norm, int LN_Out_and_In_Mode)
{
//	printf(" \n reg2 = %d",(uint64_t)(wt_and_bias->payload));
//	printf(" \n reg3 = %d",(uint64_t)(feature_in->payload));
//	printf(" \n reg4 = %d",feature_in->surface_stride);
//	printf(" \n reg5 = %d",feature_in->line_stride);
//	printf(" \n reg6 = %d",(uint64_t)(feature_out->payload));
//	printf(" \n reg7 = %d",feature_out->surface_stride);
//	printf(" \n reg8 = %d",feature_out->line_stride);
//	printf(" \n reg9 = %d",(feature_in->channel+Tout-1) / Tout);
//	printf(" \n reg10= %d",1);//height
//    printf(" \n reg11= %d",feature_in->width);
//	printf(" \n reg13= %d",wt_and_bias->scale);//LN_k_factor_scale
//	printf(" \n reg15= %d",wt_and_bias->conv_out_scale);//LN_bias_scale
//    printf(" \n reg16= %d",feature_out->scale);
//    printf(" \n reg17= %d",1);//recip_CH_int8
//    printf(" \n reg18= %d",341);//recip_CH_int16
//    printf(" \n reg19= %d",feature_in->width);//pixel_in
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

	CSB_Write(Matrix_reg_bias+2 ,(uint64_t)(wt_and_bias->payload));
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,dat_in_surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,dat_in_line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,dat_out_surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,dat_out_line_stride);
	CSB_Write(Matrix_reg_bias+9 ,(chin_padding_with_tout+Tout-1) / Tout);
	CSB_Write(Matrix_reg_bias+10,feature_in->height);//height
    CSB_Write(Matrix_reg_bias+11,feature_in->width);
    CSB_Write(Matrix_reg_bias+18,fp20_data);

    CSB_Write(Matrix_reg_bias+19,feature_in->width);//pixel_in
    CSB_Write(Matrix_reg_bias+28,LN_CH_burst_times_minus1);
    CSB_Write(Matrix_reg_bias+30,Layer_Norm);
    
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00100000);//Ln_stage0_start
	while (CSB_Read(Matrix_reg_bias+1) != 1){	}
	
	CSB_Write(Matrix_reg_bias+26,LN_Out_and_In_Mode);//LN_Out_and_In_Mode
	CSB_Write(Matrix_reg_bias+0,0b01000000);//Ln_stage1_start
	while (CSB_Read(Matrix_reg_bias+1) != 1){	}
}

void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out, int Transpose_Out_and_In_Mode)
{
    int width = ((feature_in->width + Tout - 1) / Tout) * Tout;
    int in_ch = ((feature_in->channel + T_quant_block - 1) / T_quant_block) * T_quant_block;
	int pixel_in,pixel_out,wt_burst_times,CHout_div_Tout, Block_wt_bits;
    pixel_in = width;//197;
    wt_burst_times = (MAX_DAT_DW+T_quant_block*4)/MAX_DAT_DW;
    CHout_div_Tout = (width+Tout-1) / Tout; // in->width=out->chout
    Block_wt_bits = Tout*MAX_DAT_DW+Tout*4*T_quant_block;

    int WT_CHin_div_Tin = (in_ch+base_Tin-1)/base_Tin;
    int WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*base_Tin;
    int WT_CHin_div_Tblock = (WT_CHin_Padding_with_Tin+T_quant_block-1)/T_quant_block;
    int DAT_IN_SURFACE_STRIDE = Pixel_Data_Bytes*width*feature_in->height;
	
//	printf(" \n reg 3 =%d",(uint64_t)(feature_in->payload));
//	printf(" \n reg 4 =%d",feature_in->surface_stride);
//	printf(" \n reg 6 =%d",(uint64_t)(feature2weight_out->payload));
//	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout);
//    printf(" \n reg 11=%d",pixel_in);//feature_in->width;
//    printf(" \n reg 19=%d",pixel_in);//pixel_in
//    printf(" \n reg 23=%d",pixel_out);//pixel_out
//    printf(" \n reg 25=%d",wt_num_div_Tout);//wt_num_div_Tout
	
	CSB_Write(Matrix_reg_bias+4 ,DAT_IN_SURFACE_STRIDE);
	CSB_Write(Matrix_reg_bias+9 ,(in_ch+Tout-1) / Tout);
    CSB_Write(Matrix_reg_bias+11,Tout);//feature_in->width;
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
    CSB_Write(Matrix_reg_bias+23,in_ch);//pixel_out
    CSB_Write(Matrix_reg_bias+25,wt_burst_times);//wt_num_div_Tout
	CSB_Write(Matrix_reg_bias+26,Transpose_Out_and_In_Mode);//LN_Out_and_In_Mode

    uint64_t in_base_addr, dat_out_base_addr;
    dat_out_base_addr = (uint64_t)(feature2weight_out->payload);

    for (int chout=0; chout<CHout_div_Tout;chout=chout+1)
        for (int chin=0; chin<WT_CHin_div_Tblock;chin=chin+1)
        {
            in_base_addr = (uint64_t)(feature_in->payload)+chout*(Pixel_Data_Bytes*Tout)+chin*(DAT_IN_SURFACE_STRIDE*T_quant_block/Tout);
            CSB_Write(Matrix_reg_bias+3 ,in_base_addr);//feature_in_base_addr
            CSB_Write(Matrix_reg_bias+6 ,dat_out_base_addr);//feature_out_base_addr
            //Kick of the run
	        CSB_Write(Matrix_reg_bias+0,0b00000010);//Transpose_start
                
            //Waiting for Ln_wdma done
            while (CSB_Read(Matrix_reg_bias+1) != 1)
            {
                //usleep(10);
                //xil_printf("Wait for concat_wdma done\r\n");
            }
            dat_out_base_addr=dat_out_base_addr+Block_wt_bits/8;
        }
	//Kick of the run
}


