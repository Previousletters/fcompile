`include "TOP_defines.vh"
`define KV_cache_mode 1
`define Token 49
`define Feature_Head 4

`define Height (`Token)
`define Width_in  (`MAX_CH_per_HEAD)
`define Width_out (`Token)

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win `MAX_TOKEN
`define Feature_Win `MAX_TOKEN
`define Hin 1
`define CHin (`MAX_CH_per_HEAD)
`define CHout (`MAX_CH_per_HEAD)

`define Wout (`MAX_TOKEN)
`define Feature_Wout `MAX_TOKEN
`define Hout (1)
`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define RELU_EN 0

`define DAT_IN_BASE_ADDR 32'h000_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_HEAD_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define WT_CHin_div_Tblock ((`WT_CHin_Padding_with_Tin+`T_quant_block-1)/`T_quant_block)

`define WT_BASE_ADDR 32'h200_0000

`define Pos_Num `MAX_TOKEN
`define POS_IN_BASE_ADDR 32'h400_0000
`define POS_LINE_STRIDE  (`Pixel_Data_Bytes*`Pos_Num)

`define DAT_OUT_BASE_ADDR 32'h800_0000//(`KV_cache_mode==1?`DAT_IN_BASE_ADDR:32'h800_0000) 
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_HEAD_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_scale `Conv_out_scale

`define half_clk_period 5

module testbench_EMB;

parameter M_AXI_ID_WIDTH = 4;     // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

`include "basic_tasks.vh"
`include "tasks_EMB.vh"
always #5 clk=~clk;

integer flag=1;
bit [31:0]rdata;

real                           real_dat_in[`Feature_Head][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Feature_Head][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Feature_Head][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]     dat_in_mem[`Feature_Head*`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Feature_Head][`Hin][`Win][`CHin_Padding_with_Tout];

real                           real_pos_in[`Pos_Num][`CHin/2];
bit [`DAT_DW_L0-1:0]                pos_in[`Pos_Num][`CHin/2];
bit [`MAX_DAT_DW*`Tout-1:0]     pos_in_mem[`Pos_Num*`CHin_div_Tout/2];
bit [`DAT_DW_L0-1:0]           pos_in_test[`Pos_Num][`CHin_Padding_with_Tout/2];


real          real_fp16_software_out[`Feature_Head][`Hout][`Wout][`CHout];
real          real_fp32_software_out[`Feature_Head][`Hout][`Wout][`CHout];

bit [`DAT_DW_L1-1:0]              dat_out[`Feature_Head][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]      dat_out_mem[`Feature_Head*`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Feature_Head][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Feature_Head][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;


initial begin
    rst_n=1;
    #20 rst_n=0;
    #20 rst_n=1;
    Run_PosEmb_Hardware(`POS_IN_BASE_ADDR,`POS_LINE_STRIDE,
                         `DAT_IN_BASE_ADDR,`DAT_IN_HEAD_STRIDE,`DAT_IN_LINE_STRIDE,
                         `DAT_OUT_BASE_ADDR,`DAT_OUT_HEAD_STRIDE,`DAT_OUT_LINE_STRIDE);
    $display("Run PosEmb Finish! ");
    $finish;
end
endmodule