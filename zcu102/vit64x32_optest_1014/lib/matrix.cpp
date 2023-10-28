#include "matrix.h"
#include "conv.h"

///// function for hardware//////////
void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int LN_Out_and_In_Mode)
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

	CSB_Write(Matrix_reg_bias+2 ,(uint64_t)(wt_and_bias->payload));
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,feature_in->surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,feature_in->line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,feature_out->surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,feature_out->line_stride);
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
	CSB_Write(Matrix_reg_bias+10,1);//height
    CSB_Write(Matrix_reg_bias+11,feature_in->width);
	
	CSB_Write(Matrix_reg_bias+13,wt_and_bias->scale);//LN_k_factor_scale
	CSB_Write(Matrix_reg_bias+15,wt_and_bias->conv_out_scale);//LN_bias_scale
    CSB_Write(Matrix_reg_bias+16,feature_out->scale);

    CSB_Write(Matrix_reg_bias+17,1);//recip_CH_int8
    CSB_Write(Matrix_reg_bias+18,341);//recip_CH_int16
    CSB_Write(Matrix_reg_bias+19,feature_in->width);//pixel_in

    
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00100000);//Ln_stage0_start
	while (CSB_Read(Matrix_reg_bias+1) != 1){	}
	
	CSB_Write(Matrix_reg_bias+26,LN_Out_and_In_Mode);//LN_Out_and_In_Mode
	CSB_Write(Matrix_reg_bias+0,0b01000000);//Ln_stage1_start
	while (CSB_Read(Matrix_reg_bias+1) != 1){	}
	CSB_Write(Matrix_reg_bias+26,0);
}


void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out, int Transpose_Out_and_In_Mode)
{
	
	int pixel_in,pixel_out,wt_num_div_Tout;
    pixel_in=feature_in->width;//197;
    pixel_out=feature_in->channel;//192;
	wt_num_div_Tout=pixel_in*((feature_in->channel+Tout-1) / Tout);
	
//	printf(" \n reg 3 =%d",(uint64_t)(feature_in->payload));
//	printf(" \n reg 4 =%d",feature_in->surface_stride);
//	printf(" \n reg 6 =%d",(uint64_t)(feature2weight_out->payload));
//	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout);
//    printf(" \n reg 11=%d",pixel_in);//feature_in->width;
//    printf(" \n reg 19=%d",pixel_in);//pixel_in
//    printf(" \n reg 23=%d",pixel_out);//pixel_out
//    printf(" \n reg 25=%d",wt_num_div_Tout);//wt_num_div_Tout
	
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,feature_in->surface_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature2weight_out->payload));
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
    CSB_Write(Matrix_reg_bias+11,pixel_in);//feature_in->width;
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
    CSB_Write(Matrix_reg_bias+23,pixel_out);//pixel_out
    CSB_Write(Matrix_reg_bias+25,wt_num_div_Tout);//wt_num_div_Tout
	
	CSB_Write(Matrix_reg_bias+26,Transpose_Out_and_In_Mode);//LN_Out_and_In_Mode
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00000010);//Transpose_start
    
	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for concat_wdma done\r\n");
	}
	CSB_Write(Matrix_reg_bias+26,0);
}
	
	
void FPGA_Run_Concat(struct Mapped_Feature* cls_token, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int Concat_Out_and_In_Mode)
{	
//	printf(" \n reg 3 =%d",(uint64_t)(feature_in->payload));
//	printf(" \n reg 4 =%d",feature_in->surface_stride);
//	printf(" \n reg 5 =%d",feature_in->line_stride);
//	printf(" \n reg 6 =%d",(uint64_t)(feature_out->payload));
//	printf(" \n reg 7 =%d",feature_out->surface_stride);
//	printf(" \n reg 8 =%d",feature_out->line_stride);
//	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout);
//    printf(" \n reg 11=%d",feature_in->width);//Win
//    printf(" \n reg 24=%d",(uint64_t)(cls_token->payload));
	
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,feature_in->surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,feature_in->line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,feature_out->surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,feature_out->line_stride);
	
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
    CSB_Write(Matrix_reg_bias+11,feature_in->width);//Win
    CSB_Write(Matrix_reg_bias+24,(uint64_t)(cls_token->payload));
    CSB_Write(Matrix_reg_bias+26,Concat_Out_and_In_Mode);
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00100);//Concat_start

	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for concat_wdma done\r\n");
	}
	CSB_Write(Matrix_reg_bias+26,0);
}

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int pixel_in,int pixel_out, int Softmax_Out_and_In_Mode)
{
	

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
	CSB_Write(Matrix_reg_bias+4 ,feature_in->surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,feature_in->line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,feature_out->surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,feature_out->line_stride);
	
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout );
	CSB_Write(Matrix_reg_bias+10,(pixel_in+Tout-1) / Tout);//height
    CSB_Write(Matrix_reg_bias+11,Tout);//Win=Tout
	
	CSB_Write(Matrix_reg_bias+13,feature_in->scale);//height
    CSB_Write(Matrix_reg_bias+16,feature_out->scale);//Win
	
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
	CSB_Write(Matrix_reg_bias+23,pixel_out);//pixel_out
	CSB_Write(Matrix_reg_bias+26,Softmax_Out_and_In_Mode);
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b00001000);//Ln_start
    
	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for concat_wdma done\r\n");
	}
	CSB_Write(Matrix_reg_bias+26,0);
}


void FPGA_Run_Activation(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out)
{
	int pixel_in,pixel_out;

    pixel_in=197;//feature_in->width*height;
    pixel_out=192;//feature_in->channel;
	printf(" \n ****************");
	printf(" \n reg 3 =%ld",(uint64_t)(feature_in->payload));
	printf(" \n reg 4 =%d",feature_in->surface_stride);
	printf(" \n reg 5 =%d",feature_in->line_stride);
	printf(" \n reg 6 =%ld",(uint64_t)(feature_out->payload));
	printf(" \n reg 7 =%d",feature_out->surface_stride);
	printf(" \n reg 8 =%d",feature_out->line_stride);
	printf(" \n reg 9 =%d",(feature_in->channel+Tout-1) / Tout);
    printf(" \n reg 11=%d",pixel_in);//Win	
	printf(" \n reg 13=%d",feature_in->scale);//height
    printf(" \n reg 16=%d",feature_out->scale);//Win
    printf(" \n reg 19=%d",pixel_in);//pixel_in
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));	
	
	CSB_Write(Matrix_reg_bias+3 ,(uint64_t)(feature_in->payload));
	CSB_Write(Matrix_reg_bias+4 ,feature_in->surface_stride);
	CSB_Write(Matrix_reg_bias+5 ,feature_in->line_stride);
	CSB_Write(Matrix_reg_bias+6 ,(uint64_t)(feature_out->payload));
	CSB_Write(Matrix_reg_bias+7 ,feature_out->surface_stride);
	CSB_Write(Matrix_reg_bias+8 ,feature_out->line_stride);
	CSB_Write(Matrix_reg_bias+9 ,(feature_in->channel+Tout-1) / Tout);
    CSB_Write(Matrix_reg_bias+11,pixel_in);//Win
	CSB_Write(Matrix_reg_bias+13,feature_in->scale);//in scale
    CSB_Write(Matrix_reg_bias+16,feature_out->scale);//out scale
    CSB_Write(Matrix_reg_bias+19,pixel_in);//pixel_in
	
	printf(" \n reg 3 =%d",CSB_Read(Matrix_reg_bias+3 ));
	printf(" \n reg 4 =%d",CSB_Read(Matrix_reg_bias+4 ));
	printf(" \n reg 5 =%d",CSB_Read(Matrix_reg_bias+5 ));
	printf(" \n reg 6 =%d",CSB_Read(Matrix_reg_bias+6 ));
	printf(" \n reg 7 =%d",CSB_Read(Matrix_reg_bias+7 ));
	printf(" \n reg 8 =%d",CSB_Read(Matrix_reg_bias+8 ));
	printf(" \n reg 9 =%d",CSB_Read(Matrix_reg_bias+9 ));
    printf(" \n reg 11=%d",CSB_Read(Matrix_reg_bias+11));
	printf(" \n reg 13=%d",CSB_Read(Matrix_reg_bias+13));
    printf(" \n reg 16=%d",CSB_Read(Matrix_reg_bias+16));
    printf(" \n reg 19=%d",CSB_Read(Matrix_reg_bias+19));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));	
	//Kick of the run
	CSB_Write(Matrix_reg_bias+0,0b000010000);//Ln_start
 	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));	
	
	while (CSB_Read(Matrix_reg_bias+1) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for concat_wdma done\r\n");
	}
 	printf(" \n ---");
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));
	printf(" \n reg 1 =%d",CSB_Read(Matrix_reg_bias+1));	
}