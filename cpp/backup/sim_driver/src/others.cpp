#include "others.h"

void FPGA_Run_Elementwise(int ElementWise_Mode, struct Mapped_Feature* feature_a, struct Mapped_Feature* feature_b, struct Mapped_Feature* feature_out, int Out_and_In_Mode, HANDLE device)
{
    int CHin_div_Tout = (feature_a->channel+Tout-1) / Tout; // in->width=out->chout
    int dat_in_surface_stride = Pixel_Data_Bytes*feature_a->width*feature_a->height;
    int dat_in_line_stride = Pixel_Data_Bytes*feature_a->width;
    int dat_out_surface_stride = Pixel_Data_Bytes*feature_out->width*feature_out->height;
    int dat_out_line_stride = Pixel_Data_Bytes*feature_out->width;
	
    CSB_Write(device, Elementwise_reg_bias+2,ElementWise_Mode);//ElementWise_Mode
    CSB_Write(device, Elementwise_reg_bias+3,(uint64_t)feature_a->payload);//feature_in_base_addr
    CSB_Write(device, Elementwise_reg_bias+4,dat_in_surface_stride);//feature_in_surface_stride_in
    CSB_Write(device, Elementwise_reg_bias+5,dat_in_line_stride);//feature_in_line_stride_in
    CSB_Write(device, Elementwise_reg_bias+6,(uint64_t)feature_out->payload);//feature_out_base_addr
    CSB_Write(device, Elementwise_reg_bias+7,dat_out_surface_stride);//feature_out_surface_stride_in
    CSB_Write(device, Elementwise_reg_bias+8,dat_out_line_stride);//feature_out_line_stride_in
    CSB_Write(device, Elementwise_reg_bias+9,CHin_div_Tout);//CHin_div_Tout
    CSB_Write(device, Elementwise_reg_bias+10,feature_a->height);
    CSB_Write(device, Elementwise_reg_bias+11,feature_a->width);
    CSB_Write(device, Elementwise_reg_bias+12,(uint64_t)feature_b->payload);//feature_in_base_addr
    CSB_Write(device, Elementwise_reg_bias+13,Out_and_In_Mode);//Elementwise_Out_to_BRAM_mode_r<=wdata[1], Elementwise_In_from_BRAM_mode_r<=wdata[0]

    //Kick of the run
    CSB_Write(device, Elementwise_reg_bias+0,0b01);//Elementwise_start
    while (CSB_Read(device, Elementwise_reg_bias + 1) != 1) {};
}
