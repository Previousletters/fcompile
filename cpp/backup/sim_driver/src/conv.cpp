#include "conv.h"


struct MVM_Cfg Get_MVM_Cfg(int Hin, int Win, int CHin, int CHout, int in_feature_dw, int wt_dw, int wt_scale_dw, int Skip_Factor)
{
    struct MVM_Cfg ret {};

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
    
    int Tin                    = base_Tin             ;
    int Wout                   = Win                  ; 
    int Hout                   = Hin/Skip_Factor      ;
    int CHout_div_Tout         = ((CHout+Tout-1)/Tout);
    int CHin_div_Tout          = ((CHin+Tout-1)/Tout) ;
    int Tin_div_Tout           = ((base_Tin+Tout-1)/Tout);
    int CHout_Padding_with_Tout   = (CHout_div_Tout*Tout);
    int CHout_Padding          = (CHout_div_Tout*Tout);
    int CHin_Padding_with_Tout = (CHin_div_Tout*Tout) ;

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
        return ret;
    }
    if (min_wt_depth>WT_BRAM_DEPTH*WT_BRAM_NUM)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        return ret;
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;

    if(CHout_Padding%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else {
        out_ch_slice=Tout;
        out_ch_slice_last=Tout;
    }
        
    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;
    
    Hout_first=DAT_BRAM_DEPTH/dat_num_per_row;
    in_height_first=Hout_first;
    Hout_middle=DAT_BRAM_DEPTH/dat_num_per_row;
    in_height_middle=Hout_middle;

    if(Hout_first>=Hout)
    {
        Hout_first=Hout;
        in_height_first=Hin;
    }

    if((Hout-Hout_first)%Hout_middle == 0)
    {
        Hout_Split_Times=(Hout-Hout_first)/Hout_middle+1;
        Hout_last=Hout_middle;
    }
    else
    {
        Hout_Split_Times=(Hout-Hout_first)/Hout_middle+2;
        Hout_last=(Hout-Hout_first)%Hout_middle;
    }
    in_height_last=Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap);

    
    total_bytes_if_reuse_wt=Tb*(dat_num_per_row*Hin)*CHout_Split_Times*(Tin*MAX_DAT_DW/8)
                          +CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW/8)
                          +(CHout_Split_Times * Hout_Split_Times * 100 * 32/8);
    total_bytes_if_reuse_dat=Hout_Split_Times*CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW/8)
                          +(dat_num_per_row*Hin)*(Tin*MAX_DAT_DW/8)*Tb
                          +(CHout_Split_Times*Hout_Split_Times * 100 * 32/8);

    // printf("total_bytes_if_reuse_wt=%d,total_bytes_if_reuse_dat=%d\n",total_bytes_if_reuse_wt,total_bytes_if_reuse_dat);

    if (total_bytes_if_reuse_wt < total_bytes_if_reuse_dat)
        best_method = 0;
    else
        best_method = 1;

    ret.CHin_Padding_with_Tin = CHin_Padding_with_Tout;
    ret.Hin = Hin;
    ret.Win = Win;
    ret.CHout_Padding_with_Tout = CHout_Padding_with_Tout;
    ret.Hout = Hout;
    ret.Wout = Wout;
    ret.overlap = overlap;
    ret.best_single_dat_banks = best_single_dat_banks;
    ret.method = best_method;
    ret.dat_num_per_row = dat_num_per_row;
    ret.wt_size_in_bytes = WT_SIZE_IN_BYTES;
    ret.wt_num_div_Tin = WT_NUM_DIV_TIN;
    ret.Hout_Split_Times = Hout_Split_Times;
    ret.Hout_first = Hout_first;
    ret.Hout_middle = Hout_middle;
    ret.Hout_last = Hout_last;
    ret.in_height_first = in_height_first;
    ret.in_height_middle = in_height_middle;
    ret.in_height_last = in_height_last;
    ret.CHout_Split_Times = CHout_Split_Times;
    ret.out_ch_slice = out_ch_slice;
    ret.out_ch_slice_last = out_ch_slice_last;
    ret.Skip_Factor = Skip_Factor;
    return ret;
}

void Save_MVM_Cfg(const char* cfg_name, struct MVM_Cfg mvm_cfg, FILE* fp)
{

    fprintf(fp, "struct MVM_Cfg %s={%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d};\n", cfg_name,
        mvm_cfg.CHin_Padding_with_Tin,
        mvm_cfg.Hin,
        mvm_cfg.Win,
        mvm_cfg.CHout_Padding_with_Tout,
        mvm_cfg.Hout,
        mvm_cfg.Wout,
        mvm_cfg.overlap,
        mvm_cfg.best_single_dat_banks,
        mvm_cfg.method,
        mvm_cfg.dat_num_per_row,
        mvm_cfg.wt_size_in_bytes,
        mvm_cfg.wt_num_div_Tin,
        mvm_cfg.Hout_Split_Times,
        mvm_cfg.Hout_first,
        mvm_cfg.Hout_middle,
        mvm_cfg.Hout_last,
        mvm_cfg.in_height_first,
        mvm_cfg.in_height_middle,
        mvm_cfg.in_height_last,
        mvm_cfg.CHout_Split_Times,
        mvm_cfg.out_ch_slice,
        mvm_cfg.out_ch_slice_last);
}

void FPGA_MVM(struct MVM_Cfg mvm_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;
    
    if (mvm_cfg.method == 0)//Hout_Split_Times->CHout_Split_Times
    {
        for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_wt_reuse_single;//dma_dat_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0; dma_wt_reuse_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                    dma_wt_reuse_single = 1;
                }

                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last; Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle; Hout_single = mvm_cfg.Hout_middle;
                    }

                FPGA_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,0,dma_wt_reuse_single, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
    else//CHout_Split_Times->Hout_Split_Times
    {
        for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_dat_reuse_single;//dma_wt_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (n == 0)
                    dma_dat_reuse_single = 0;
                else
                    dma_dat_reuse_single = 1;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                }


                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last;
                        Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle;
                        Hout_single = mvm_cfg.Hout_middle;
                    }
                
                FPGA_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,dma_dat_reuse_single,0, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
}

void FPGA_MVM_single_time
(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
    int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
    int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
    int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, 
    int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
    int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device)
{

    int rdata;
    int CH_in_res_Tin_div_Tout_minus1;
    int CH_in_div_Tin, CH_out_div_Tout, CH_in_div_Tout;
    int effect_in_pixel,effect_out_pixel,effect_out_pixel_div_Tout;
    
    if (CHin % base_Tin == 0)
        CH_in_res_Tin_div_Tout_minus1=base_Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%base_Tin)+Tout-1)/Tout-1);
    CH_in_div_Tin=(CHin + base_Tin - 1) / base_Tin;
    CH_in_div_Tout=(CHin + Tout - 1) / Tout;
    CH_out_div_Tout=(CHout + Tout - 1) / Tout;
    effect_in_pixel=Win * Hin;
    effect_out_pixel=Wout*Hout;
    effect_out_pixel_div_Tout=(effect_out_pixel+Tout-1)/Tout;
    
    //cfg conv_path
    CSB_Write(device, 3, single_dat_buf_num);
    CSB_Write(device, 4, Win);
    CSB_Write(device, 5, Hin);
    CSB_Write(device, 6, effect_in_pixel);
    CSB_Write(device, 7, CH_in_div_Tout);
    CSB_Write(device, 8, CH_in_div_Tin);
    CSB_Write(device, 31, CH_in_res_Tin_div_Tout_minus1);
    CSB_Write(device, 15, Wout);
    CSB_Write(device, 16, effect_out_pixel);
    CSB_Write(device, 17, Hout);
    CSB_Write(device, 18, CHout);
    CSB_Write(device, 19, CH_out_div_Tout);
    CSB_Write(device, 33, effect_out_pixel_div_Tout);

    CSB_Write(device, 20, feature_in_base);
    CSB_Write(device, 21, feature_in_surface_stride);
    CSB_Write(device, 22, feature_in_line_stride);
    CSB_Write(device, 23, wt_size_in_bits);
    CSB_Write(device, 32, wt_num_div_Tin);
    CSB_Write(device, 24, wt_base_addr);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 25, (dma_wt_reuse << 1) | dma_dat_reuse);
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    CSB_Write(device, 51, Skip_Factor-1);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 64+4,Hout);
    CSB_Write(device, 64+5,Wout);
    CSB_Write(device, 64+6,CH_out_div_Tout);
    CSB_Write(device, 64+12,effect_out_pixel);    //effect_out_pixel
    CSB_Write(device, 64+18,relu_en);
    CSB_Write(device, 64+20,feature_out_base);
    CSB_Write(device, 64+21,feature_out_surface_stride);
    CSB_Write(device, 64+22,feature_out_line_stride);    
    
    CSB_Write(device, 0, mode);//Kick off the run. MVM Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b0001);//{ResAdd, BN, wdma}
    
    while (CSB_Read(device, 64 + 19) != 1) {};
}


void FPGA_MVM_BN(struct MVM_Cfg mvm_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;
    
    if (mvm_cfg.method == 0)//Hout_Split_Times->CHout_Split_Times
    {
        for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_wt_reuse_single;//dma_dat_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0; dma_wt_reuse_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                    dma_wt_reuse_single = 1;
                }

                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last; Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle; Hout_single = mvm_cfg.Hout_middle;
                    }

                FPGA_MVM_BN_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,

                        ((uint64_t)feature_bn->payload + (BN_SURFACE_STRIDE*n*mvm_cfg.out_ch_slice/Tout)),
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,0,dma_wt_reuse_single, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
    else//CHout_Split_Times->Hout_Split_Times
    {
        for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_dat_reuse_single;//dma_wt_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (n == 0)
                    dma_dat_reuse_single = 0;
                else
                    dma_dat_reuse_single = 1;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                }


                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last;
                        Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle;
                        Hout_single = mvm_cfg.Hout_middle;
                    }
                
                FPGA_MVM_BN_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,

                        ((uint64_t)feature_bn->payload + (BN_SURFACE_STRIDE*n*mvm_cfg.out_ch_slice/Tout)),
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,dma_dat_reuse_single,0, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
}

void FPGA_MVM_BN_single_time
(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
    int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
    int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
    int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, int BN_base_addr,
    int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
    int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device)
{

    int rdata;
    int CH_in_res_Tin_div_Tout_minus1;
    int CH_in_div_Tin, CH_out_div_Tout, CH_in_div_Tout;
    int effect_in_pixel,effect_out_pixel,effect_out_pixel_div_Tout;
    
    if (CHin % base_Tin == 0)
        CH_in_res_Tin_div_Tout_minus1=base_Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%base_Tin)+Tout-1)/Tout-1);
    CH_in_div_Tin=(CHin + base_Tin - 1) / base_Tin;
    CH_in_div_Tout=(CHin + Tout - 1) / Tout;
    CH_out_div_Tout=(CHout + Tout - 1) / Tout;
    effect_in_pixel=Win * Hin;
    effect_out_pixel=Wout*Hout;
    effect_out_pixel_div_Tout=(effect_out_pixel+Tout-1)/Tout;
    int BN_num_per_AXI_DW = AXI_DAT_WIDTH / (2*MAX_BN_DW);
    int BN_CH_burst_times_minus1 = (CHout + (BN_num_per_AXI_DW*AXI_BURST_LEN)-1)/(BN_num_per_AXI_DW*AXI_BURST_LEN) - 1;
    
    //cfg conv_path
    CSB_Write(device, 3, single_dat_buf_num);
    CSB_Write(device, 4, Win);
    CSB_Write(device, 5, Hin);
    CSB_Write(device, 6, effect_in_pixel);
    CSB_Write(device, 7, CH_in_div_Tout);
    CSB_Write(device, 8, CH_in_div_Tin);
    CSB_Write(device, 31, CH_in_res_Tin_div_Tout_minus1);
    CSB_Write(device, 15, Wout);
    CSB_Write(device, 16, effect_out_pixel);
    CSB_Write(device, 17, Hout);
    CSB_Write(device, 18, CHout);
    CSB_Write(device, 19, CH_out_div_Tout);
    CSB_Write(device, 33, effect_out_pixel_div_Tout);

    CSB_Write(device, 20, feature_in_base);
    CSB_Write(device, 21, feature_in_surface_stride);
    CSB_Write(device, 22, feature_in_line_stride);
    CSB_Write(device, 23, wt_size_in_bits);
    CSB_Write(device, 32, wt_num_div_Tin);
    CSB_Write(device, 24, wt_base_addr);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 25, (dma_wt_reuse << 1) | dma_dat_reuse);
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    CSB_Write(device, 51, Skip_Factor-1);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 64+1,BN_CH_burst_times_minus1);
    CSB_Write(device, 64+2,BN_base_addr);
    CSB_Write(device, 64+4,Hout);
    CSB_Write(device, 64+5,Wout);
    CSB_Write(device, 64+6,CH_out_div_Tout);
    CSB_Write(device, 64+12,effect_out_pixel);    //effect_out_pixel
    CSB_Write(device, 64+18,relu_en);
    CSB_Write(device, 64+20,feature_out_base);
    CSB_Write(device, 64+21,feature_out_surface_stride);
    CSB_Write(device, 64+22,feature_out_line_stride);    
    
    CSB_Write(device, 0, mode);//Kick off the run. MVM Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b0011);//{ResAdd, BN, wdma}
    
    while (CSB_Read(device, 64 + 19) != 1) {};
}


void FPGA_MVM_BN_Res(struct MVM_Cfg mvm_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
    int current_in_base_addr, current_out_base_addr;
    current_in_base_addr =(In_from_BRAM==1)?in_base_addr:((uint64_t)feature_in->payload);
    current_out_base_addr=( Out_to_BRAM==1)?out_base_addr:((uint64_t)feature_out->payload);
    int dat_in_surface_stride = Pixel_Data_Bytes * feature_in->height * feature_in->width;
    int dat_in_line_stride = Pixel_Data_Bytes * feature_in->width;
    int dat_out_surface_stride = Pixel_Data_Bytes * feature_out->height * feature_out->width;
    int dat_out_line_stride = Pixel_Data_Bytes * feature_out->width;

    int ResAdd_base = (uint64_t)feature_add->payload;
    int ResAdd_surface_stride = Pixel_Data_Bytes * feature_add->height * feature_add->width;
    int ResAdd_line_stride = Pixel_Data_Bytes * feature_add->width;
    
    if (mvm_cfg.method == 0)//Hout_Split_Times->CHout_Split_Times
    {
        for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_wt_reuse_single;//dma_dat_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0; dma_wt_reuse_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                    dma_wt_reuse_single = 1;
                }

                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last; Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle; Hout_single = mvm_cfg.Hout_middle;
                    }

                FPGA_MVM_BN_Res_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,

                        ((uint64_t)feature_bn->payload + (BN_SURFACE_STRIDE*n*mvm_cfg.out_ch_slice/Tout)),
                        ResAdd_base+ResAdd_line_stride*line_offset_out+ResAdd_surface_stride*n*(mvm_cfg.out_ch_slice/Tout),ResAdd_surface_stride,ResAdd_line_stride,
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,0,dma_wt_reuse_single, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
    else//CHout_Split_Times->Hout_Split_Times
    {
        for (int k = 0; k < mvm_cfg.Hout_Split_Times; k = k + 1)
            for (int n = 0; n < mvm_cfg.CHout_Split_Times; n = n + 1)
            {
                int in_height_single, Hout_single;
                int line_offset_in, line_offset_out;
                int CH_in_single;
                int CH_out_single;
                int pad_y_single;
                int dma_dat_reuse_single;//dma_wt_reuse_single=0
                
                CH_in_single=mvm_cfg.CHin_Padding_with_Tin;//  
                
                if (n != mvm_cfg.CHout_Split_Times - 1)
                    CH_out_single = mvm_cfg.out_ch_slice;
                else
                    CH_out_single = mvm_cfg.out_ch_slice_last;

                if (n == 0)
                    dma_dat_reuse_single = 0;
                else
                    dma_dat_reuse_single = 1;

                if (k == 0)
                {
                    line_offset_in = 0; line_offset_out = 0; pad_y_single = 0;
                }
                else
                {
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap);
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle;
                    pad_y_single = 0;
                }


                if (k == 0)
                {
                    in_height_single = mvm_cfg.in_height_first; Hout_single = mvm_cfg.Hout_first;
                }
                else
                    if (k == mvm_cfg.Hout_Split_Times - 1)
                    {
                        in_height_single = mvm_cfg.in_height_last;
                        Hout_single = mvm_cfg.Hout_last;
                    }
                    else
                    {
                        in_height_single = mvm_cfg.in_height_middle;
                        Hout_single = mvm_cfg.Hout_middle;
                    }
                
                FPGA_MVM_BN_Res_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                                        Out_to_BRAM, In_from_BRAM, relu_en, mode,
                        (current_in_base_addr) + dat_in_line_stride * line_offset_in, 
                        dat_in_surface_stride, 
                        dat_in_line_stride,
                        
                        ((uint64_t)wt->payload) + mvm_cfg.wt_size_in_bytes / HBM_Port / mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes / mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin / mvm_cfg.CHout_Padding_with_Tout * CH_out_single,

                        ((uint64_t)feature_bn->payload + (BN_SURFACE_STRIDE*n*mvm_cfg.out_ch_slice/Tout)),
						ResAdd_base+ResAdd_line_stride*line_offset_out+ResAdd_surface_stride*n*(mvm_cfg.out_ch_slice/Tout),ResAdd_surface_stride,ResAdd_line_stride,
                        
                        (current_out_base_addr) + dat_out_line_stride * line_offset_out + dat_out_surface_stride * n * (mvm_cfg.out_ch_slice / Tout), 
                        dat_out_surface_stride, 
                        dat_out_line_stride,
                        
                        mvm_cfg.best_single_dat_banks,dma_dat_reuse_single,0, mvm_cfg.Skip_Factor, 8, 0, device);
            }
    }
}

void FPGA_MVM_BN_Res_single_time
(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
    int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
    int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
    int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, int BN_base_addr,
    int ResAdd_base, int ResAdd_surface_stride, int ResAdd_line_stride,
    int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
    int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device)
{

    int rdata;
    int CH_in_res_Tin_div_Tout_minus1;
    int CH_in_div_Tin, CH_out_div_Tout, CH_in_div_Tout;
    int effect_in_pixel,effect_out_pixel,effect_out_pixel_div_Tout;
    
    if (CHin % base_Tin == 0)
        CH_in_res_Tin_div_Tout_minus1=base_Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%base_Tin)+Tout-1)/Tout-1);
    CH_in_div_Tin=(CHin + base_Tin - 1) / base_Tin;
    CH_in_div_Tout=(CHin + Tout - 1) / Tout;
    CH_out_div_Tout=(CHout + Tout - 1) / Tout;
    effect_in_pixel=Win * Hin;
    effect_out_pixel=Wout*Hout;
    effect_out_pixel_div_Tout=(effect_out_pixel+Tout-1)/Tout;
    int BN_num_per_AXI_DW = AXI_DAT_WIDTH / (2*MAX_BN_DW);
    int BN_CH_burst_times_minus1 = (CHout + (BN_num_per_AXI_DW*AXI_BURST_LEN)-1)/(BN_num_per_AXI_DW*AXI_BURST_LEN) - 1;
    
    //cfg conv_path
    CSB_Write(device, 3, single_dat_buf_num);
    CSB_Write(device, 4, Win);
    CSB_Write(device, 5, Hin);
    CSB_Write(device, 6, effect_in_pixel);
    CSB_Write(device, 7, CH_in_div_Tout);
    CSB_Write(device, 8, CH_in_div_Tin);
    CSB_Write(device, 31, CH_in_res_Tin_div_Tout_minus1);
    CSB_Write(device, 15, Wout);
    CSB_Write(device, 16, effect_out_pixel);
    CSB_Write(device, 17, Hout);
    CSB_Write(device, 18, CHout);
    CSB_Write(device, 19, CH_out_div_Tout);
    CSB_Write(device, 33, effect_out_pixel_div_Tout);

    CSB_Write(device, 20, feature_in_base);
    CSB_Write(device, 21, feature_in_surface_stride);
    CSB_Write(device, 22, feature_in_line_stride);
    CSB_Write(device, 23, wt_size_in_bits);
    CSB_Write(device, 32, wt_num_div_Tin);
    CSB_Write(device, 24, wt_base_addr);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 25, (dma_wt_reuse << 1) | dma_dat_reuse);
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    // CSB_Write(device, 51, Skip_Factor-1);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]

    CSB_Write(device, 64+1,BN_CH_burst_times_minus1);
    CSB_Write(device, 64+2,BN_base_addr);
    CSB_Write(device, 64+4,Hout);
    CSB_Write(device, 64+5,Wout);
    CSB_Write(device, 64+6,CH_out_div_Tout);
    CSB_Write(device, 64+7,ResAdd_base);
    CSB_Write(device, 64+8,ResAdd_surface_stride);
    CSB_Write(device, 64+9,ResAdd_line_stride);
    CSB_Write(device, 64+12,effect_out_pixel);    //effect_out_pixel
    CSB_Write(device, 64+18,relu_en);
    CSB_Write(device, 64+20,feature_out_base);
    CSB_Write(device, 64+21,feature_out_surface_stride);
    CSB_Write(device, 64+22,feature_out_line_stride);    
    
    CSB_Write(device, 0, mode);//Kick off the run. MVM Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b0111);//{ResAdd, BN, wdma}
    
    while (CSB_Read(device, 64 + 19) != 1) {};
}


void FPGA_RunHBM_FC(int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
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
    int Hout                   = Hin                  ;
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
    if (min_wt_depth>WT_BRAM_DEPTH*WT_BRAM_NUM)
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

    int CH_in_extend=((CHin+Tin-1)/Tin)*Tin;
    int CH_in_res_Tin_div_Tout_minus1;
    if(CHin%Tin==0)
        CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%Tin)+Tout-1)/Tout-1);

    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*out_ch_slice*8;

	//cfg Conv_path
	CSB_Write(device, 7,(CHin+Tout-1)/Tout);
	CSB_Write(device, 8,(CHin+Tin-1)/Tin);
	CSB_Write(device, 31,CH_in_res_Tin_div_Tout_minus1);
	CSB_Write(device, 19,((out_ch_slice+Tout-1)/Tout));

	CSB_Write(device, 20,current_in_base_addr);
	CSB_Write(device, 23,wt_size_in_bits);
	CSB_Write(device, 24,(uint64_t)wt->payload);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    CSB_Write(device, 50,CHout_Split_Times_minus1);

    CSB_Write(device, 64+6,((out_ch_slice+Tout-1)/Tout));
    CSB_Write(device, 64+20,current_out_base_addr);   
    CSB_Write(device, 64+23,CHout_Split_Times_minus1);
    CSB_Write(device, 0,mode);//Conv Path: FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b1001);//{FC_mode,ResAdd, BN, wdma}

    while (CSB_Read(device, 64 + 19) != 1) {};
}


void FPGA_RunHBM_FC_BN(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
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
    int Hout                   = Hin                  ;
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
    if (min_wt_depth>WT_BRAM_DEPTH*WT_BRAM_NUM)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(CHout_Padding%out_ch_slice!=0)
        out_ch_slice=Tout;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    int CHout_Split_Times_minus1=CHout_Split_Times-1;

    int CH_in_extend=((CHin+Tin-1)/Tin)*Tin;
    int CH_in_res_Tin_div_Tout_minus1;
    if(CHin%Tin==0)
        CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%Tin)+Tout-1)/Tout-1);

    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*out_ch_slice*8;
    int BN_num_per_AXI_DW = AXI_DAT_WIDTH / (2*MAX_BN_DW);
    int BN_ch_group_times = CHout_Padding / BN_num_per_AXI_DW;
    int BN_CH_burst_times_minus1 = (out_ch_slice + (BN_num_per_AXI_DW*AXI_BURST_LEN)-1)/(BN_num_per_AXI_DW*AXI_BURST_LEN) - 1;

	//cfg Conv_path
	CSB_Write(device, 7,(CHin+Tout-1)/Tout);
	CSB_Write(device, 8,(CHin+Tin-1)/Tin);
	CSB_Write(device, 31,CH_in_res_Tin_div_Tout_minus1);
	CSB_Write(device, 19,((out_ch_slice+Tout-1)/Tout));

	CSB_Write(device, 20,current_in_base_addr);
	CSB_Write(device, 23,wt_size_in_bits);
	CSB_Write(device, 24,(uint64_t)wt->payload);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    CSB_Write(device, 50,CHout_Split_Times_minus1);

    CSB_Write(device, 64+1,BN_CH_burst_times_minus1);
    CSB_Write(device, 64+2,(uint64_t)feature_bn->payload);
    CSB_Write(device, 64+6,((out_ch_slice+Tout-1)/Tout));
    CSB_Write(device, 64+18,relu_en);
    CSB_Write(device, 64+20,current_out_base_addr);   
    CSB_Write(device, 64+23,CHout_Split_Times_minus1);
    CSB_Write(device, 0,mode);//Conv Path: FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b1011);//{FC_mode,ResAdd, BN, wdma}

    while (CSB_Read(device, 64 + 19) != 1) {};
}


void FPGA_RunHBM_FC_BN_Res(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr)
{
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
    int Hout                   = Hin                  ;
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
    if (min_wt_depth>WT_BRAM_DEPTH*WT_BRAM_NUM)
    {
        printf("Error: FPGA WT BRAM DEPTH not enough!!!!!\n");
        exit(-1);
    }

    out_ch_slice=(WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(CHout_Padding%out_ch_slice!=0)
        out_ch_slice=Tout;

    if(out_ch_slice>=CHout_Padding)
    {
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    }
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    int CHout_Split_Times_minus1=CHout_Split_Times-1;

    int CH_in_extend=((CHin+Tin-1)/Tin)*Tin;
    int CH_in_res_Tin_div_Tout_minus1;
    if(CHin%Tin==0)
        CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%Tin)+Tout-1)/Tout-1);

    int wt_size_in_bits = WT_SIZE_IN_BYTES/CHout_Padding*out_ch_slice*8;
    int BN_num_per_AXI_DW = AXI_DAT_WIDTH / (2*MAX_BN_DW);
    int BN_ch_group_times = CHout_Padding / BN_num_per_AXI_DW;
    int BN_CH_burst_times_minus1 = (out_ch_slice + (BN_num_per_AXI_DW*AXI_BURST_LEN)-1)/(BN_num_per_AXI_DW*AXI_BURST_LEN) - 1;

	//cfg Conv_path
	CSB_Write(device, 7,(CHin+Tout-1)/Tout);
	CSB_Write(device, 8,(CHin+Tin-1)/Tin);
	CSB_Write(device, 31,CH_in_res_Tin_div_Tout_minus1);
	CSB_Write(device, 19,((out_ch_slice+Tout-1)/Tout));

	CSB_Write(device, 20,current_in_base_addr);
	CSB_Write(device, 23,wt_size_in_bits);
	CSB_Write(device, 24,(uint64_t)wt->payload);
    CSB_Write(device, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step); // #TODO!!!
    CSB_Write(device, 47, (Out_to_BRAM << 1) | In_from_BRAM);//MVM_Out_to_BRAM_mode_r<=wdata[1], MVM_In_from_BRAM_mode_r<=wdata[0]
    CSB_Write(device, 50,CHout_Split_Times_minus1);

    CSB_Write(device, 64+1,BN_CH_burst_times_minus1);
    CSB_Write(device, 64+2,(uint64_t)feature_bn->payload);
    CSB_Write(device, 64+6,((out_ch_slice+Tout-1)/Tout));
    CSB_Write(device, 64+7,(uint64_t)feature_add->payload);
    CSB_Write(device, 64+18,relu_en);
    CSB_Write(device, 64+20,current_out_base_addr);   
    CSB_Write(device, 64+23,CHout_Split_Times_minus1);
    CSB_Write(device, 0,mode);//Conv Path: FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    CSB_Write(device, 64+0,0b1111);//{FC_mode,ResAdd, BN, wdma}

    while (CSB_Read(device, 64 + 19) != 1) {};
}

