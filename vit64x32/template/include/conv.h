#pragma once

#ifndef SRC_CONV_H_
#define SRC_CONV_H_

#include "basic.h"

struct Conv_Cfg
{
	int CHin;
	int Tin_L;
	int Win;
	int CHout;
	int overlap;
	int Kx;
	int Ky;
	int Sx;
	int Sy;
	int pad_x;
	int pad_y;
	int dat_banks;
	int method;
	int out_width;
	int out_height;
	int dat_num_per_row;
	int wt_size_in_bytes;
	int Hout_Split_Times;
	int in_height_first;
	int in_height_middle;
	int in_height_last;
	int out_height_first;
	int out_height_middle;
	int out_height_last;
	int CHout_Split_Times;
	int out_ch_slice;
	int out_ch_slice_last;
};

///// function for software//////////
struct Conv_Cfg Get_Conv_Cfg(int Hin, int Win, int CHin, int CHout,
	int Kx, int Ky, int Sx, int Sy, int Px, int Py, int data_width, int mp_mode);

void Save_Conv_Cfg(const char* cfg_name, struct Conv_Cfg conv_cfg, FILE* fp);

void Mul_Conv_Bias_Soft(struct Conv_Cfg conv_cfg, int relu_en,
	struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* bias, struct Mapped_Feature* feature_out);

void Mul_Conv_Soft(struct Conv_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out);


///// function for hardware//////////
#ifdef Run_on_FPGA
void FPGA_Conv(struct Conv_Cfg conv_cfg, int gelu_en, int relu_en, int Conv_Out_and_In_mode,
	struct Mapped_Feature* feature_in,
	struct Mapped_Weight* wt,
	struct Mapped_Feature* feature_out);


void FPGA_Conv_single_time(int Tin_factor, int Tin_L, int CH_in_res_Tin_div_Tout_minus1, int shift_sacle,
	int CHin, int Hin, int Win, int CHout,
	int Kx, int Ky, int Sx, int Sy,
	int pad_x, int pad_y, int gelu_en, int relu_en, int Conv_Out_and_In_mode, int out_dw,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride, int feature_in_scale,
	int wt_base_addr, int wt_size_in_bytes, int wt_num_div_Tin, int wt_scale,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, int feature_out_scale,
	int out_width, int out_height, int dat_buf_num, int cdma_dat_reuse, int cdma_wt_reuse);


void FPGA_Vit_Conv0(struct Conv_Cfg conv_cfg, int relu_en, int Vit_Conv0_Out_and_In_Mode, 
	struct Mapped_Feature* feature_in,
	struct Mapped_Weight* wt,
	struct Mapped_Feature* feature_out);

void FPGA_Vit_Conv0_single_time(int Tin_factor, int Tin_L, int CH_in_res_Tin_div_Tout_minus1, int shift_sacle,
	int CHin, int Hin, int Win, int CHout,
	int Kx, int Ky, int Sx, int Sy,
	int pad_x, int pad_y, int relu_en, int Vit_Conv0_Out_and_In_Mode, int out_dw,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride, int feature_in_scale,
	int wt_base_addr, int wt_size_in_bytes, int wt_num_div_Tin, int wt_scale,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, int feature_out_scale,
	int out_width, int out_height, int dat_buf_num, int cdma_dat_reuse, int cdma_wt_reuse);

#endif

#endif /* SRC_VPU_H_ */
