`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`timescale 1ns/1ps
`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win 64
`define Hin 5
`define CHin 64
`define ElementWise_Mode 1 //0: add, 1: minus, 2: mul

`define CHout `CHin
`define Wout `Win
`define Hout `Hin

`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)


`define A_DAT_IN_BASE_ADDR 32'h000_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)

`define B_DAT_IN_BASE_ADDR 32'h100_0000

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)

`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_ElementWise;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

//////////////////////////////////////////////////

`include "tasks_driver_ElementWise.vh"
`include "basic_tasks.vh"
always #(`half_clk_period) clk=~clk;

int flag=1;
bit [31:0]rdata;

real                           real_dat_in_A[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in_A[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem_A[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0] dat_in_mem_A[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test_A[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real                           real_dat_in_B[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in_B[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem_B[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0] dat_in_mem_B[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test_B[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real          real_fp16_software_elementwise_out[`Tb][`Hout][`Wout][`CHout];
real          real_fp32_software_elementwise_out[`Tb][`Hout][`Wout][`CHout];
bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]     dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout];
bit [(`DAT_DW_L1-1):0]before_relu[`Tb][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;

initial
begin

	RunElementwise_Hardware(`Hin,`Win,`CHin_div_Tout,`ElementWise_Mode,
		      `A_DAT_IN_BASE_ADDR,`B_DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
		      `DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);

   #10 $finish;
end

initial
begin
#100000000 $finish;
end

endmodule
