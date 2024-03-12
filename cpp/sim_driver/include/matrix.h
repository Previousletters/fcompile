#pragma once

#ifndef SRC_MATRIX_H_
#define SRC_MATRIX_H_

#include "basic.h"

#define Matrix_reg_bias 192

///// function for software//////////

// real_to_FP20，应用在FPGA_Run_LN之前以计算FP20_recip_CH_r
int FP32_to_FP20(float fp32_i);

///// function for hardware//////////
// RMS_Norm 与tasks对应，0时为Layer_Norm，1时为RMS_Norm
void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int RMS_Norm, int LN_Out_and_In_Mode, HANDLE device=0);

void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out, int Transpose_Out_and_In_Mode, HANDLE device=0, int log2_WT_base_addr_Bank_Step=28, int Left_WT_Base_Addr=0);

void FPGA_Run_Feature2Weight(struct Mapped_Feature* feature_in, struct Mapped_Weight* weight_out, int Out_and_In_Mode, HANDLE device=0, int log2_WT_base_addr_Bank_Step=28, int Left_WT_Base_Addr=0);

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int Softmax_Out_and_In_Mode, HANDLE device=0);

void FPGA_Run_Activation(int parameters_addr, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int Activation_Out_and_In_Mode, HANDLE device=0);

void FPGA_Run_PosEmb(struct Mapped_Feature* feature_in, struct Mapped_Feature* pos_in, struct Mapped_Feature* feature_out, int Pos_Num, int Out_and_In_Mode, HANDLE device=0);

#endif
