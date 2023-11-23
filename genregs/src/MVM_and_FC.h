#pragma once

#include "basic.h"

#ifdef VERSION1
#include "TOP_defines.h"

struct HBM_MVM_Cfg
{
	int CHin_Padding_with_Tin;
	int Hin;
	int Win;
	int CHout_Padding_with_Tout;
	int Hout;
	int Wout;
	int overlap;
	int best_single_dat_banks;
	int method;
	int dat_num_per_row;
	int wt_size_in_bytes;
    int wt_num_div_Tin;
	int Hout_Split_Times;
	int Hout_first;
	int Hout_middle;
	int Hout_last;
	int in_height_first;
	int in_height_middle;
	int in_height_last;
	int CHout_Split_Times;
	int out_ch_slice;
	int out_ch_slice_last;
};

///// function for software//////////
HBM_DLL struct HBM_MVM_Cfg Get_HBM_MVM_Cfg(int Hin, int Win, int CHin, int CHout, int in_feature_dw, int wt_dw, int wt_scale_dw);
void Save_HBM_MVM_Cfg(const char* cfg_name, struct HBM_MVM_Cfg conv_cfg, FILE* fp);

void Mul_Conv_Bias_Soft(struct Conv_Cfg conv_cfg, int relu_en,
	struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* bias, struct Mapped_Feature* feature_out);

void Mul_Conv_Soft(struct Conv_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out);

#define Run_on_FPGA

///// function for hardware//////////
#ifdef Run_on_FPGA
void run_FPGA_HBM_MVM(struct HBM_MVM_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE user_device);

void run_FPGA_HBM_MVM_single_time(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
	int Out_to_BRAM, int In_from_BRAM, int relu_en,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
	int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, 
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
	int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, HANDLE user_device
);

void run_FPGA_HBM_FC(struct HBM_MVM_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr,HANDLE user_device);

void run_FPGA_HBM_FC_single_time(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
	int Out_to_BRAM, int In_from_BRAM, int relu_en,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
	int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, 
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
	int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse,HANDLE user_device
);


#endif
#endif
