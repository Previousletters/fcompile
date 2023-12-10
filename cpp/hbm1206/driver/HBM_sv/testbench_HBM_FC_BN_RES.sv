`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`define Height 1//
`define Width_in  (256)//4//
`define Width_out (512+32)//3//

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win `Height//`Tout
`define Hin ((`Height+`Win-1)/`Win)
`define CHin `Width_in
`define CHout `Width_out

`define Wout (`Win)
`define Hout (`Hin)
`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define RELU_EN 0

`define DAT_IN_BASE_ADDR 32'h100_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define WT_CHin_div_Tblock ((`WT_CHin_Padding_with_Tin+`T_quant_block-1)/`T_quant_block)
`define Tblock_div_Tin (`T_quant_block/`Tin)

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

//`define WT_scale_bits (`CHout_Padding*`WT_CHin_div_Tblock*`WT_quant_scale_DW)
`define WT_scale_bits (`CHout_Padding*`HBM_AXI_DATA_WIDTH*`WT_scale_group_nums)
`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_SIZE_IN_BYTES (((`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_DW)>>3)+((`WT_scale_bits)>>3))
`define WT_NUM_DIV_TIN   (`CHout_Padding*`WT_CHin_div_Tin)
`define WT_scale 5
`define HBM00_WT_BASE_ADDR 32'h00_0000

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

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_HBM_FC_BN_RES;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;


`include "tasks_driver_HBM_FC_BN_RES.vh"
`include "basic_tasks.vh"
`include "MVM_tasks.vh"
`include "activation_tasks.vh"
always #(`half_clk_period) clk=~clk;

integer flag=1;
int cnt_i;
bit [31:0]rdata;

bit [`WT_DW-1:0]       weight_reorg[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin];
bit [`WT_DW*`Tin-1:0] weight_Tin[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout];
bit [`WT_DW*`Tout-1:0]  weight_Tout[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin_div_Tout];	

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

bit [`WT_DW-1:0] wt[`CHout][`CHin];
bit [`WT_DW-1:0] wt_soft[`CHout][`CHin];
bit [`WT_DW*`Tout-1:0]wt_mem[`CHout_div_Tout*`WT_CHin_div_Tin*`Tout*`Tin_div_Tout];
bit [`WT_DW*`WT_CHin_Padding_with_Tin-1:0]HBM_wt_mem[`CHout_div_Tout][`Tout/`HBM_Port][`HBM_Port];
int wt_start_ch_in,wt_end_ch_in;

real                              real_wt_FP_scale[`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
bit [`WT_quant_scale_DW-1:0]           wt_FP_scale[`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
bit [`WT_quant_scale_DW*`Tout-1:0] wt_FP_scale_men[`CHout_div_Tout *`WT_CHin_div_Tblock];
bit [`HBM_AXI_DATA_WIDTH-1:0]      HBM_wt_FP_scale[`CHout_div_Tout][`WT_scale_group_nums][`Tout/`HBM_Port][`HBM_Port];
bit [31:0] wt_addr_bias;
bit [31:0] scale_addr_bias;

real real_bn_weight[`CHout_Padding];
real real_bn_bias[`CHout_Padding];
bit [`MAX_BN_DW-1:0] bn_weight[`CHout_Padding];
bit [`MAX_BN_DW-1:0] bn_bias[`CHout_Padding];
bit [`MAX_BN_DW*2-1:0] bn_wt_and_bias[`CHout_div_Tout*`Tout];
bit [`AXI_DAT_WIDTH-1:0]bn_wt_and_bias_mem[`BN_ch_group_times];

real real_Res_Add_in[`Tb][`Hout][`Wout][`CHout];
bit [`MAX_DAT_DW-1:0] Res_Add_dat[`Tb][`Hout][`Wout][`CHout];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_Res_Add_mem[`Tb][`Wout*`Hout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]Res_Add_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`AXI_DAT_WIDTH-1:0]Res_Add_dat_mem[`Wout*`Hout*((`CHout_Padding+`Tout-1)/`Tout)];

real          real_fp16_software_conv_out[`Tb][`Hout][`Wout][`CHout];
real          real_fp32_software_conv_out[`Tb][`Hout][`Wout][`CHout];
real          real_fp16_software_conv_relu_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_conv_bn_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_res_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_conv_bn_res_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_res_relu_out[`Tb][`Hout][`Wout][`CHout];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]     dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout];
bit [(`DAT_DW_L1-1):0]before_relu[`Tb][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;

initial
begin
	RunHBM_FC_BN_RES(`CHin_Padding_with_Tout,`Hin,`Win,`CHout_Padding,`RELU_EN,
			`DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
			`HBM00_WT_BASE_ADDR,`WT_SIZE_IN_BYTES,`WT_NUM_DIV_TIN,`BN_BASE_ADDR,
			`Res_Add_BASE_ADDR,`Res_Add_SURFACE_STRIDE,`Res_Add_LINE_STRIDE,
			`DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);

   #1 $finish;
end

initial
begin
#100000000 $finish;
end

endmodule
