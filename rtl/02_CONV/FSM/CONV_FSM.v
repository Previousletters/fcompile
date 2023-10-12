`include "CNN_defines.vh"

module CONV_FSM
(
	input clk,
	input rst_n,

	//from CSR
	input fsm_dat_start,
	input fsm_wt_start,
    input conv_mode,
    
	input [`log2_P-1:0]pad_up,
	input [`log2_P-1:0]pad_left,
	input [`log2_S-1:0]Sx,
	input [`log2_S-1:0]Sy,
	input [`log2_K-1:0]Kx,
	input [`log2_K-1:0]Ky,
	input [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin,				//ceil(CH_in/Tin)
	input [`log2_CH-1:0]CH_out,
	input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout,				//ceil(CH_out/Tout)
	input [`log2_W-1:0]Wout,
	input [(`log2_W+`log2_H-1):0]Hout_x_Wout,					//Wout*Hout
	
	input [`log2_H-1:0]Hout,	
	input [`log2_H-1:0]Hin,
	input [`log2_W-1:0]Win,
    input [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout,			//Wout*Hout/Tout
    
	//to CSR
	output fsm_dat_done,
	output fsm_wt_done,
    
	//to BUF for Backpressure
	output CHinKyKx_max_now_comb,

	//conv feature addr to BUF
	output feature_data_vld,
	output [`log2_H:0]feature_hin,
	output [`log2_W:0]feature_win,
	output [`log2_CH-1:0]feature_chin,
	output Wout_loop_end_comb,
	input feature_data_rdy,
    
	//wt_addr to BUF
	output wt_addr_vld,
	output [`log2BUF_DEP-1:0]wt_addr,
	output wt_last_comb,
	input wt_addr_rdy,

	//data to CMAC
	output CHinKyKx_max_now,
	output Wout_loop_start,
	output Wout_loop_end,
	output CHinKyKxWout_loop_end,
	//wt to CMAC
	output [`log2Tout-1:0]wt_cnt
);

fsm_dat fsm_dat
(
	.clk(clk),
	.rst_n(rst_n),
    
	//from CSR
	.start(fsm_dat_start),
	.pad_up(pad_up),
	.pad_left(pad_left),
	.Sx(Sx),
	.Sy(Sy),
	.Kx(Kx),
	.Ky(Ky),
	.CHin_div_Tin(CH_in_div_Tin),				//ceil(CH_in/Tin)
	.CHout_div_Tout(CH_out_div_Tout),				//ceil(CH_out/Tout)
	.Wout(Wout),
	.Hout_x_Wout(Hout_x_Wout),					//Wout*Hout
	
	.Hout(Hout),	//ceil(Hout*Wout/Tp)
    .Hout_x_Wout_div_Tout(Hout_x_Wout_div_Tout),//Wout*Hout/Tout

	//to CSR
	.done(fsm_dat_done),

	//to BUF for Backpressure
	.CHinKyKx_max_now_comb(CHinKyKx_max_now_comb),

	//axis to BUF
	.feature_data_vld(feature_data_vld),
	.feature_hin(feature_hin),
	.feature_win(feature_win),
	.feature_chin(feature_chin),
	.Wout_loop_end_comb(Wout_loop_end_comb),
	.feature_data_rdy(feature_data_rdy),
    
	//data to CMAC
	.CHinKyKx_max_now(CHinKyKx_max_now),
	.Wout_loop_start(Wout_loop_start),
	.Wout_loop_end(Wout_loop_end),
	.CHinKyKxWout_loop_end(CHinKyKxWout_loop_end)
);

fsm_wt fsm_wt
(
	.clk(clk),
	.rst_n(rst_n),
    
	//from CSR
	.start(fsm_wt_start),
	.Kx(Kx),
	.Ky(Ky),
	.CH_in_div_Tin(CH_in_div_Tin),				//ceil(CH_in/Tin)
	.CH_out(CH_out),
	.CH_out_div_Tout(CH_out_div_Tout),				//ceil(CH_out/Tout)
	.Hout(Hout),
    .Hout_x_Wout_div_Tout(Hout_x_Wout_div_Tout),//Wout*Hout/Tout
    
	//to CSR
	.done(fsm_wt_done),

	//wt_addr to BUF
	.wt_addr_vld(wt_addr_vld),
	.wt_addr(wt_addr),
	.wt_last_comb(wt_last_comb),
	.wt_addr_rdy(wt_addr_rdy),

	//wt to CMAC
	.wt_cnt(wt_cnt)

);

endmodule
