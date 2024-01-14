#pragma once

#ifndef SRC_CONV_H_
#define SRC_CONV_H_

#include <driver/basic.h>

namespace hbm {

#define Tb 1
#define base_Tin 128
#define Tout     32
#define T_quant_block 128
#define HBM_AXI_DATA_WIDTH 256
#define WT_quant_scale_DW 16
#define MAX_DAT_DW 16
#define MAX_WT_DW 4
#define MAX_BN_DW 16
#define DAT_BRAM_NUM 1
#define HBM_Port 32
#define WT_BRAM_NUM HBM_Port
#define AXI_BURST_LEN Tout
#define AXI_BN_WIDTH (MAX_BN_DW*Tout*Tb)
#define AXI_DAT_WIDTH (MAX_DAT_DW*Tout*Tb)
#define BN_FIFO_DEP ((AXI_BURST_LEN*MAX_DAT_DW*Tb)/(MAX_BN_DW*2))
#define BN_FIFO_NUM ((MAX_BN_DW*2)/(MAX_DAT_DW*Tb))
#define Pixel_Data_Bytes ((AXI_DAT_WIDTH)>>3)        
#define WT_CH_Tgroup (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW)
#define DAT_BRAM_DEPTH ((1<<22)/base_Tin/MAX_DAT_DW/DAT_BRAM_NUM)  //18: 256Kb for ASIC.
#define WT_BRAM_DEPTH ((1<<24)/HBM_AXI_DATA_WIDTH/WT_BRAM_NUM)  //18: 256Kb for ASIC.
#define BN_SURFACE_STRIDE ((Tout*MAX_BN_DW*2) >> 3)

enum PathMode {
    mode_mvm = 0b00100011111,
    mode_mvm_bn = 0b01100011111,
    mode_mvm_bn_res = 0b11100011111,
};

#define Run_on_FPGA

///// function for hardware//////////

void FPGA_RunHBM_MVM(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_MVM_BN_Res(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Skip_Factor, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

};

#endif /* SRC_VPU_H_ */
