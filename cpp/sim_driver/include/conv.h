#pragma once

#ifndef SRC_CONV_H_
#define SRC_CONV_H_

#include "basic.h"

enum PathMode {
    mode_mvm = 0b00100011111,
    mode_mvm_f2w = 0b00100011111,
    mode_mvm_f2w_old = 0b00110011111,
    mode_mvm_bn = 0b01100011111,
    mode_mvm_bn_res = 0b11100011111,
    mode_mvm_bn_res_trp = 0b11100011111,
    mode_mvm_bn_res_trp_old = 0b11101011111,
    mode_mvm_bn_res_aug = 0b111100011111,
};

#define Run_on_FPGA

///// function for hardware//////////
#ifdef Run_on_FPGA

void FPGA_RunHBM_MVM(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_afterF2W(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN_Res(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN_Res_AugMax(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, struct Mapped_Feature* feature_aug, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN_Res_ArgMax(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, struct Mapped_Feature* feature_aug, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN_Res_afterTRP(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

#endif
#endif
