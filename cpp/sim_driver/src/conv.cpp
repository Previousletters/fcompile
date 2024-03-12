#include "conv.h"
#include <iostream>


///// function for hardware//////////
void FPGA_RunHBM_MVM(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,Skip_Factor-1);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,0);
    CSB_Write(device, 27,0);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 1) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
}

void FPGA_RunHBM_MVM_afterF2W(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->width;
    // int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width * (feature_in->channel / Tout) * 2;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width * Skip_Factor;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->width;
    // int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width * (feature_out->channel / Tout) * 2;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width * Skip_Factor;

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    Hin                        = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,0);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,0);
    CSB_Write(device, 27,0);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 1) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
}

void FPGA_RunHBM_MVM_BN(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    ObjCheck(feature_bn);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    int BN_FIFO_bits=AXI_BN_WIDTH*BN_FIFO_DEP*BN_FIFO_NUM;
    int BN_FIFO_chout_num=BN_FIFO_bits/(MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,Skip_Factor-1);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,(uint64_t)feature_bn->payload);
    CSB_Write(device, 27,0);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 1) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
    AutoFree(feature_bn);
}


void FPGA_RunHBM_MVM_BN_Res(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    ObjCheck(feature_bn);
    ObjCheck(feature_add);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    int BN_FIFO_bits=AXI_BN_WIDTH*BN_FIFO_DEP*BN_FIFO_NUM;
    int BN_FIFO_chout_num=BN_FIFO_bits/(MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,Skip_Factor-1);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,(uint64_t)feature_bn->payload);
    CSB_Write(device, 27,(uint64_t)feature_add->payload);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 1) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
    AutoFree(feature_bn);
    AutoFree(feature_add);
}

void FPGA_RunHBM_MVM_BN_Res_ArgMax(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, struct Mapped_Feature* feature_aug, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    ObjCheck(feature_bn);
    ObjCheck(feature_add);
    ObjCheck(feature_aug);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * (feature_out->width);
    int dat_out_line_stride = Pixel_Data_Bytes * (feature_out->width);

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    int BN_FIFO_bits=AXI_BN_WIDTH*BN_FIFO_DEP*BN_FIFO_NUM;
    int BN_FIFO_chout_num=BN_FIFO_bits/(MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,Skip_Factor-1);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,(uint64_t)feature_bn->payload);
    CSB_Write(device, 27,(uint64_t)feature_add->payload);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);
    CSB_Write(device, 60,(uint64_t)feature_aug->payload);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 61) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
    AutoFree(feature_bn);
    AutoFree(feature_add);
}

void FPGA_RunHBM_MVM_BN_Res_AugMax(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, struct Mapped_Feature* feature_aug, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    ObjCheck(feature_bn);
    ObjCheck(feature_add);
    ObjCheck(feature_aug);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * (feature_out->width + 1);
    int dat_out_line_stride = Pixel_Data_Bytes * (feature_out->width + 1);

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    int BN_FIFO_bits=AXI_BN_WIDTH*BN_FIFO_DEP*BN_FIFO_NUM;
    int BN_FIFO_chout_num=BN_FIFO_bits/(MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,Skip_Factor-1);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,(uint64_t)feature_bn->payload);
    CSB_Write(device, 27,(uint64_t)feature_add->payload);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);
    CSB_Write(device, 60,(uint64_t)feature_aug->payload);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 61) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
    AutoFree(feature_bn);
    AutoFree(feature_add);
}

void FPGA_RunHBM_MVM_BN_Res_afterTRP(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    ObjCheck(feature_in);
    ObjCheck(wt);
    ObjCheck(feature_bn);
    ObjCheck(feature_add);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width * (feature_in->channel / Tout) * 2;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width * (feature_in->channel / Tout) * 2; // WARNING!!!!!!!!!!!!!!!!!!

    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);

    int overlap;
    int dat_num_per_row;
    int min_dat_depth;//mininum required dat banks
    int min_wt_depth;//mininum required wt banks

    int single_dat_buf_num;
    int best_single_dat_banks = 0;
    int best_wt_banks = 0;
    int mininum_bw = 0;
    int best_method = 0;
    int wt_factor;

    int total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    int Hout_Split_Times;
    int in_height_first,in_height_middle,in_height_last;    
    int Hout_first,Hout_middle,Hout_last;
    int CHout_Split_Times;
    int out_ch_slice,out_ch_slice_last;
    int wt_dw = 4;
    
    int Win = feature_in->width;
    int Hin = feature_in->height;
    int CHout = feature_out->channel;
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    Hin                        = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((feature_in->channel+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin                   = (CHin_div_Tout*Tout) ;

    int WT_CHin_div_Tin        = ((CHin+Tin-1)/Tin)   ;
    int WT_CHin_Padding_with_Tin=(WT_CHin_div_Tin*Tin);
    
    int WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin/T_quant_block;
    //int WT_scale_bits          =(CHout_Padding*WT_CHin_div_Tblock*wt_scale_dw);
    int WT_scale_group_nums    =((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    int WT_scale_bits          =(CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums);
    int WT_SIZE_IN_BYTES       = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3));
    int WT_NUM_DIV_TIN         = (CHout_Padding*WT_CHin_div_Tin);

    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((Tin*MAX_WT_DW)/HBM_AXI_DATA_WIDTH)*(Tout/HBM_Port);

    if (min_dat_depth>DAT_BRAM_DEPTH)
    {
        printf("Error: FPGA DAT BRAM DEPTH not enough!!!!\n");
        exit(-1);
    }
    if (min_wt_depth>WT_BRAM_DEPTH)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    int BN_FIFO_bits=AXI_BN_WIDTH*BN_FIFO_DEP*BN_FIFO_NUM;
    int BN_FIFO_chout_num=BN_FIFO_bits/(MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice_last=CHout_Padding%out_ch_slice;
    }

    int CHout_Split_Times_minus1=CHout_Split_Times-1;
    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*8;
    int Height = Win * Hin;
    int CHout_last = out_ch_slice_last;
    CHout = out_ch_slice;

	//cfg Conv_path
    CSB_Write(device, 2, CHin);
    CSB_Write(device, 3, Win);
    CSB_Write(device, 4, Hin);
    CSB_Write(device, 5, Wout);
    CSB_Write(device, 6, Hout);
	CSB_Write(device, 7, CHout);
	CSB_Write(device, 8, CHout_last);
	CSB_Write(device, 9, Height);
	CSB_Write(device, 10,current_in_base_addr);
	CSB_Write(device, 11,(uint64_t)wt->payload);
	CSB_Write(device, 12,wt_size_in_bits);
    CSB_Write(device, 13,current_out_base_addr);   
    CSB_Write(device, 14,CHout_Split_Times_minus1);
    CSB_Write(device, 15,(Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 16,relu_en);
    CSB_Write(device, 17,0);
    CSB_Write(device, 18, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 19,0);
    CSB_Write(device, 20,0);
    CSB_Write(device, 21,1);
    CSB_Write(device, 22,1);
    CSB_Write(device, 23,1);
    CSB_Write(device, 24,1);
    CSB_Write(device, 25,0);
    CSB_Write(device, 26,(uint64_t)feature_bn->payload);
    CSB_Write(device, 27,(uint64_t)feature_add->payload);
    CSB_Write(device, 28,0);
    CSB_Write(device, 29,dat_in_surface_stride);
    CSB_Write(device, 30,dat_in_line_stride);
    CSB_Write(device, 31,dat_out_surface_stride);
    CSB_Write(device, 32,dat_out_line_stride);

    CSB_Write(device, 33,mode);//Conv Path: MVM_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

    while (CSB_Read(device, 1) != 1)
    {
    }
    if (Skip_Factor==1)
        AutoFree(feature_in);
    AutoFree(wt);
    AutoFree(feature_bn);
    AutoFree(feature_add);
}


