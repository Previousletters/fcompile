`include "CNN_defines.vh"

module Systolic_CONV_MAC_top
(
	input clk,
	input rst_n,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit    
    input relu,
    input [`MAX_log2BN_DW-1:0]out_data_width, //'b111 means 8bit, 'b11 means 4bit, 'b1 means 2bit
    
	//data from FSM
	input dat_vld,
	input [`base_Tin-1:0]dat_vld_copy,
	input [`base_Tin*`MAX_DAT_DW-1:0]dat,
	input CHinKyKx_max_now,
	input Wout_loop_start,
	input Wout_loop_end,
	input CHinKyKxWout_loop_end,
	
	//wt from FSM
	input wt_vld,
	input [`base_Tin*`MAX_WT_DW-1:0]wt,
	input [`log2Tout-1:0]wt_cnt,

	//from CSR
	input [`log2_scale-1:0]conv_out_shift,

	//dat to SDP
	output conv_out_dat_vld,
	output [`MAX_BN_DW *`Tout-1:0]conv_out_dat
	
);
(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d1,Tin_factor_d2,Tin_factor_d3;
always@(posedge clk) Tin_factor_d1<=Tin_factor;
always@(posedge clk) Tin_factor_d2<=Tin_factor_d1;
always@(posedge clk) Tin_factor_d3<=Tin_factor_d2;


//dat to CACC
wire [`Tout*(`MAX_DW2+`base_log2Tin)-1:0]mac2acc_dat;    

wire mac2acc_dat_vld;
wire mac2acc_CHinKyKx_max_now;
wire mac2acc_Wout_loop_end;
wire mac2acc_CHinKyKxWout_loop_end;
    
generate_vld_shift #
(
    .DATA_WIDTH(4),
    .DEPTH(`MAC_LATENCY)
)shift_reg
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({dat_vld,CHinKyKx_max_now,Wout_loop_end,CHinKyKxWout_loop_end}),
    .data_out({mac2acc_dat_vld,mac2acc_CHinKyKx_max_now,mac2acc_Wout_loop_end,mac2acc_CHinKyKxWout_loop_end})
);

Column_Systolic_MAC_Control Column_Systolic_MAC_Control
(
    .clk(clk),
    .rst_n(rst_n),
    .Tin_factor(Tin_factor_d3),
    
    //dat from BUF
    .dat_vld(dat_vld),
    .dat(dat),
    .Wout_loop_start(Wout_loop_start),
    .Wout_loop_end(Wout_loop_end),
    
    //wt from BUF
    .wt_vld(wt_vld),
    .wt(wt),
    .wt_cnt(wt_cnt),
    
    .dat_o(mac2acc_dat)
);

Accumulation Accumulation
(
	.clk(clk),
	.rst_n(rst_n),
    .relu(relu),
	//from CSR
	.conv_out_shift(conv_out_shift),
    .out_data_width(out_data_width),
    
	//dat from CMAC
	.dat_vld_i(mac2acc_dat_vld),
	.Wout_loop_end_i(mac2acc_Wout_loop_end),
	.CHinKyKx_max_now_i(mac2acc_CHinKyKx_max_now),
	.CHinKyKxWout_loop_end_i(mac2acc_CHinKyKxWout_loop_end),
	.dat_i(mac2acc_dat),
    
	//dat to SDP
	.conv_out_dat_vld(conv_out_dat_vld),
	.conv_out_dat(conv_out_dat)
);

endmodule