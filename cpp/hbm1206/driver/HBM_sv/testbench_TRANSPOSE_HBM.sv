`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`timescale 1ns/1ps
`define Height    35//5//
`define Width_in  (2048+128)//4//

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define LN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win (((`Height+`Tout-1)/`Tout)*`Tout)
`define Hin (1)
`define CHin ((`Width_in+`T_quant_block-1)/`T_quant_block*`T_quant_block)
`define CHout (((`Height+`Tout-1)/`Tout)*`Tout)

`define Wout (1)
`define Hout (1)
`define Ky   (1)
`define Kx   (1)

`define WT_CHin_div_Tblock (`CHin/`T_quant_block)
`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define Tblock_div_Tin (`T_quant_block/`Tin)

`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define DAT_IN_BASE_ADDR 32'h000_0000
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)

`define WT_BASE_ADDR 32'h200_0000

`define DAT_OUT_BASE_ADDR 32'h400_0000
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Hout*`Wout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define Block_wt_bits (`Tout*`MAX_DAT_DW+`Tout*4*`T_quant_block)
`define Total_wt_bits (`WT_CHin_div_Tblock*`CHout_div_Tout*`Block_wt_bits)

//`define WT_CH_Tgroup (`T_quant_block*`HBM_AXI_DATA_WIDTH/`WT_quant_scale_DW) // =2048 CHins
`define WT_scale_group_nums ((`WT_CHin_Padding_with_Tin+`WT_CH_Tgroup-1)/`WT_CH_Tgroup)
`define WT_CH_Tgroup_div_Tblock (`WT_CH_Tgroup/`T_quant_block) //2048/128=16

`define Group_WT_Bytes     (`WT_CH_Tgroup*`WT_DW/8)
`define Group_Scale_Bytes (`HBM_AXI_DATA_WIDTH/8)
`define Group_WT_and_Scale_Bytes (`Group_WT_Bytes+`Group_Scale_Bytes)
`define Last_Group_CHin        (`WT_CHin_Padding_with_Tin%`WT_CH_Tgroup)
`define Last_Group_WT_Bytes    (`Last_Group_CHin*`WT_DW/8)
`define Last_Group_Scale_Bytes (`HBM_AXI_DATA_WIDTH/8)
`define Last_Group_WT_and_Scale_Bytes (`Last_Group_WT_Bytes+`Last_Group_Scale_Bytes)

`define CHin_WT_Bytes     (`WT_CHin_Padding_with_Tin*`WT_DW/8)
`define CHin_Scale_Bytes (`HBM_AXI_DATA_WIDTH*`WT_scale_group_nums/8)
`define CHin_WT_and_Scale_Bytes (`CHin_WT_Bytes+`CHin_Scale_Bytes)
`define log2_WT_base_addr_Bank_Step 8
`define WT_base_addr_Bank_Step (1<<`log2_WT_base_addr_Bank_Step)
`define HBM00_WT_BASE_ADDR 32'h00_0000


///////////////// no use ////////////////////
`define RELU_EN 0
`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_TRANSPOSE_HBM;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

////////////////////////////////////////

`include "tasks_TRANSPOSE_HBM.vh"
`include "basic_tasks.vh"
`include "activation_tasks.vh"
always #(`half_clk_period) clk=~clk;

integer flag=1;
bit [31:0]rdata;

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real      real_wt_chout_chin[1][1][`CHout_Padding][`CHin_Padding_with_Tout];//[Ky][Kx][CHout][CHin]
bit [3:0] INT4_wt_chout_chin[1][1][`CHout_Padding][`CHin_Padding_with_Tout];//[Ky][Kx][CHout][CHin]
bit [`WT_DW*`Tout-1:0]wt_mem[`CHout_div_Tout*`WT_CHin_div_Tin*`Tout*`Tin_div_Tout];

real real_tp_max;
real      real_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block];
real         real_scale[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout];
real  new_real_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block];
bit [15:0]FP16_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block];
bit [15:0]   FP16_scale[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout];
bit [3:0] INT4_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block];
bit [31:0]cnt_wt,cnt_i;


bit [`WT_DW*`WT_CHin_Padding_with_Tin-1:0]HBM_wt_mem[`CHout_div_Tout][`Tout/`HBM_Port][`HBM_Port];
int wt_start_ch_in,wt_end_ch_in;
bit [`WT_quant_scale_DW*`Tout-1:0] wt_FP_scale_men[`CHout_div_Tout *`WT_CHin_div_Tblock];
bit [`HBM_AXI_DATA_WIDTH-1:0]      HBM_wt_FP_scale[`CHout_div_Tout][`WT_scale_group_nums][`Tout/`HBM_Port][`HBM_Port];
bit [31:0] wt_addr_bias;
bit [31:0] scale_addr_bias;

bit [31:0] Sofware_out_DDR00[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR01[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR02[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR03[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR04[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR05[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR06[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR07[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR08[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR09[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR10[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR11[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR12[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR13[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR14[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR15[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR16[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR17[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR18[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR19[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR20[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR21[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR22[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR23[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR24[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR25[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR26[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR27[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR28[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR29[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR30[`WT_CHin_div_Tblock*`Block_wt_bits];
bit [31:0] Sofware_out_DDR31[`WT_CHin_div_Tblock*`Block_wt_bits];

initial
begin

    Run_Transpose(`Win,`Hin,`CHin,`DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,`CHout,`HBM00_WT_BASE_ADDR);

   #10 $finish;
end

initial
begin
#100000000 $finish;
end

endmodule
