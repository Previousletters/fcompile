#pragma once

#ifndef SRC_CONV_H_
#define SRC_CONV_H_

#include "basic.h"

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
#define AXI_DAT_WIDTH (MAX_DAT_DW*Tout*Tb)
#define Pixel_Data_Bytes ((AXI_DAT_WIDTH)>>3)        
#define WT_CH_Tgroup (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW)
#define DAT_BRAM_DEPTH ((1<<20)/base_Tin/MAX_DAT_DW/DAT_BRAM_NUM)  //18: 256Kb for ASIC.
#define WT_BRAM_DEPTH ((1<<25)/HBM_AXI_DATA_WIDTH/WT_BRAM_NUM)  //18: 256Kb for ASIC.
#define BN_SURFACE_STRIDE ((Tout*MAX_BN_DW*2) >> 3)

enum PathMode {
    mode_fc = 0b111111,
    mode_fc_s_V = 0b10111111,
    mode_fc_QKt = 0b01111111,
    mode_mvm = 0b011111,
    mode_mvm_s_V = 0b10011111,
    mode_mvm_QKt = 0b01011111
};

struct MVM_Cfg
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
    int Skip_Factor;
};

///// function for software//////////
struct MVM_Cfg Get_MVM_Cfg(int Hin, int Win, int CHin, int CHout, int in_feature_dw, int wt_dw, int wt_scale_dw, int Skip_Factor);
void Save_MVM_Cfg(const char* cfg_name, struct MVM_Cfg conv_cfg, FILE* fp);

#define Run_on_FPGA

///// function for hardware//////////
#ifdef Run_on_FPGA
void FPGA_MVM(struct MVM_Cfg conv_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_MVM_single_time(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
	int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
	int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, 
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
	int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device
);

void FPGA_MVM_BN(struct MVM_Cfg conv_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_MVM_BN_single_time(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
	int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
	int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, int BN_base_addr,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
	int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device
);

void FPGA_MVM_BN_Res(struct MVM_Cfg conv_cfg, int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_MVM_BN_Res_single_time(
    int CHin, int Hin, int Win, int CHout, int Wout, int Hout, 
	int Out_to_BRAM, int In_from_BRAM, int relu_en, int mode,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride,
	int wt_base_addr, int wt_size_in_bits, int wt_num_div_Tin, int BN_base_addr,
    int ResAdd_base, int ResAdd_surface_stride, int ResAdd_line_stride,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, 
	int single_dat_buf_num, int dma_dat_reuse, int dma_wt_reuse, int Skip_Factor, int log2_WT_base_addr_Bank_Step, int Left_WT_Base_Addr, HANDLE device
);

void FPGA_RunHBM_FC(int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_FC_BN(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

void FPGA_RunHBM_FC_BN_Res(int relu_en, int mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_bn, struct Mapped_Feature* feature_add, struct Mapped_Feature* feature_out, int Out_to_BRAM, int In_from_BRAM, int out_base_addr, int in_base_addr, HANDLE device=0, int log2_WT_base_addr_Bank_Step=8, int Left_WT_Base_Addr=0);

#endif
#endif /* SRC_VPU_H_ */
