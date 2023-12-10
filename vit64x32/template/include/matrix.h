#pragma once

#ifndef SRC_MATRIX_H_
#define SRC_MATRIX_H_

#include "basic.h"

///// function for software//////////


///// function for hardware//////////
void FPGA_Run_LN(struct Mapped_Feature* feature_in, struct Mapped_Feature* wt_and_bias, struct Mapped_Feature* feature_out, int LN_Out_and_In_Mode);

void FPGA_Run_Transpose(struct Mapped_Feature* feature_in, struct Mapped_Weight* feature2weight_out,int Transpose_Out_and_In_Mode);

void FPGA_Run_Concat(struct Mapped_Feature* cls_token, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int Concat_Out_and_In_Mode);

void FPGA_Run_Softmax(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out,int pixel_in,int pixel_out,int Softmax_Out_and_In_Mode);

void FPGA_Run_Activation(struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out);

#endif /* SRC_VPU_H_ */
