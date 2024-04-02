`include "TOP_defines.vh"
`define Token   1
`define Height   `Token
`define Width_in 1024//

`define RMS_Norm 0
`define Layer_Norm (1-`RMS_Norm)

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define LN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win `Height
`define Hin (1)
`define CHin `Width_in
`define CHout `CHin

`define Wout (`Win)
`define Hout (`Hin)
`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define DAT_IN_BASE_ADDR 32'h000_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define LN_WT_BASE_ADDR 32'h100_0000
`define LN_SURFACE_STRIDE (`Tout*`MAX_BN_DW*2)>>3
`define LN_num_per_AXI_DW (`AXI_DAT_WIDTH/(2*`LN_DW)) // LN_num_per_AXI_DW <= `Tout
`define LN_ch_group_times (`CHout_Padding/`LN_num_per_AXI_DW)

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

`define RELU_EN 0


`define half_clk_period 5

module testbench_LN;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

`include "tasks_LN.vh"
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

real real_LN_weight[`CHout];
real real_LN_bias[`CHout];
bit [`MAX_BN_DW-1:0] LN_weight[`CHout];
bit [`MAX_BN_DW-1:0] LN_bias[`CHout];
bit [`AXI_DAT_WIDTH-1:0] LN_wt_in_mem[2*((`CHout_Padding+`Tout-1)/`Tout)];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]     dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

real real_fp16_software_LN_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_LN_out[`Tb][`Hout][`Wout][`CHout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout];
bit [(`DAT_DW_L1-1):0]before_relu[`Tb][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;

initial begin
    Run_LN_Soft(real_dat_in,real_LN_weight,real_LN_bias,dat_out_soft,real_fp16_software_LN_out,real_fp32_software_LN_out);
    rst_n=1;
    #20 rst_n=0;
    #20 rst_n=1;
    Run_LN(`Win,`Hin,`CHin,`LN_WT_BASE_ADDR,`DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
           `DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);
    $display("Run LN Finish! ");
    $finish;
end
endmodule