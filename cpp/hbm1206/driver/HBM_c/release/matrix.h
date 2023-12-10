#pragma once

#ifndef SRC_MATRIX_H_
#define SRC_MATRIX_H_

#include "basic.h"

#define Tb 1
#define base_Tin 128
#define Tout 64
#define MAX_BN_DW 16
#define MAX_DAT_DW 16
#define T_quant_block    128 //base_Tin //equal to the base_Tin
#define AXI_BURST_LEN_SOFTMAX 4
#define AXI_DAT_WIDTH (MAX_DAT_DW*Tout*Tb)
#define Pixel_Data_Bytes ((AXI_DAT_WIDTH)>>3)        
#define log2_AXI_BURST_LEN  6
#define Matrix_reg_bias 192
///// function for software//////////

// real_to_FP20，应用在FPGA_Run_LN之前以计算FP20_recip_CH_r
int FP32_to_FP20(float fp32_i);

///// function for hardware//////////
// RMS_Norm 与tasks对应，0时为Layer_Norm，1时为RMS_Norm
void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int RMS_Norm, int LN_Out_and_In_Mode);

void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out, int Transpose_Out_and_In_Mode);

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int pixel_in, int Softmax_Out_and_In_Mode);

// 激活函数的数据可以通过act_data.py生成，预先生成的数据已经放在 approx_pwlf_act.h 中，可直接调用
// pixel_in 参数为Height数据
void FPGA_Run_Activation(unsigned short fp16_wt[], unsigned short fp16_bias[], unsigned short fp16_x_region[], struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out, int pixel_in, int Activation_Out_and_In_Mode);

#endif /* SRC_VPU_H_ */
