`include "TOP_defines.vh"
`define Height   32//
`define Width_in 128//

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
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

`define DAT_IN_BASE_ADDR 32'h100_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

`define RELU_EN 0

`define WT_scale_bits (`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_quant_scale_DW/`base_Tin)
`define WT_BASE_ADDR 32'h200_0000
`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define WT_SIZE_IN_BYTES (((`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_DW)>>3)+((`WT_scale_bits)>>3))
`define WT_NUM_DIV_TIN   (`CHout_Padding*`WT_CHin_div_Tin)
`define WT_scale 5

`define Conv_out_scale 0 // make sure wt_scale + in_scale >= out_scale

`define BN_BASE_ADDR 32'h400_0000
`define BN_SURFACE_STRIDE (`Tout*`MAX_BN_DW*2)>>3
`define BN_num_per_AXI_DW (`AXI_DAT_WIDTH/(2*`BN_DW)) // BN_num_per_AXI_DW <= `Tout
`define BN_ch_group_times (`CHout_Padding/`BN_num_per_AXI_DW)
`define BN_WT_scale 3//(`MAX_BN_DW-1)
`define BN_BIAS_scale 2//2

`define Res_Add_BASE_ADDR 32'h600_0000
`define Res_Add_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define Res_Add_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define Res_Add_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define Res_Add_scale 0          

`define half_clk_period 5

module testbench_ACT;

parameter M_AXI_ID_WIDTH = 4;     // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

`include "basic_tasks.vh"
`include "tasks_ACT.vh"
always #5 clk=~clk;

integer flag=1;
bit [31:0]rdata;

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real real_wt[16];
real real_bias[16];
real real_x_region[16];

bit [`MAX_DAT_DW-1:0] FP16_wt[16];      
bit [`MAX_DAT_DW-1:0] FP16_bias[16];    
bit [`MAX_DAT_DW-1:0] FP16_x_region[16];
bit [`AXI_DAT_WIDTH-1:0] parameter_in_mem[3];

real          real_fp16_software_out[`Tb][`Hout][`Wout][`CHout_Padding];
real          real_fp32_software_out[`Tb][`Hout][`Wout][`CHout_Padding];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]      dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout_Padding];
real real_software_out,real_hardware_out,difference,relative_error;


initial begin
    rst_n=1;
    #20 rst_n=0;
    #20 rst_n=1;
    Run_Activation_Hardware(`WT_BASE_ADDR,
                      `DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
                      `DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);
    $display("Run Softmax Finish! ");
    $finish;
end
endmodule