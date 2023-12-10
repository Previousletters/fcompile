#pragma once

#ifndef SRC_OTHERS_H_
#define SRC_OTHERS_H_

#include "basic.h"

#define Tb 1
#define base_Tin 128
#define Tout 32
#define MAX_BN_DW 16
#define MAX_DAT_DW 16
#define T_quant_block    128 //base_Tin //equal to the base_Tin
#define AXI_BURST_LEN_SOFTMAX 4
#define AXI_DAT_WIDTH (MAX_DAT_DW*Tout*Tb)
#define Pixel_Data_Bytes ((AXI_DAT_WIDTH)>>3)        
#define log2_AXI_BURST_LEN  5
#define Elementwise_reg_bias 128 
///// function for software//////////

enum ElementOP {
    element_add = 0,
    element_minus = 1,
    element_mul = 2
};

void FPGA_Run_Elementwise(int ElementWise_Mode, struct Mapped_Feature* feature_a, struct Mapped_Feature* feature_b, struct Mapped_Feature* feature_out, int Out_and_In_Mode, HANDLE device=0);


#endif
