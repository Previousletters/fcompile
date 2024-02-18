#pragma once

#ifndef SRC_OTHERS_H_
#define SRC_OTHERS_H_

#include "basic.h"

#define Elementwise_reg_bias 128 
///// function for software//////////

enum ElementOP {
    element_add = 0,
    element_minus = 1,
    element_mul = 2
};

void FPGA_Run_Elementwise(int ElementWise_Mode, struct Mapped_Feature* feature_a, struct Mapped_Feature* feature_b, struct Mapped_Feature* feature_out, int Out_and_In_Mode, HANDLE device=0);


#endif
