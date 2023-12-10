`include "CNN_defines.vh"

module buf_stripe_control
(
	input clk,
	input rst_n,

	//for Backpressure
	input feature_data_vld,
	input feature_data_rdy,
	input CHinKyKx_max_now_comb,
	input credit_vld,

	input Wout_loop_end_comb,
	input wt_last_comb,
	
	output dat_ch_go,
	output wt_ch_go
);

reg [1:0]cnt; // the current number of wt_group in conv.
always @(posedge clk or negedge rst_n)
if(~rst_n)
	cnt<=2'd0;
else
	case({wt_last_comb,Wout_loop_end_comb})
		2'b01:cnt<=cnt-2'd1;
		2'b10:cnt<=cnt+2'd1;
	endcase

reg [`log2FSM_CREDIT_NUM:0]credit_cnt;
wire FSM_go=|credit_cnt;//credit_cnt!=0

assign dat_ch_go=(|cnt) & FSM_go;//cnt=1 or 2, get enough wt, no need to update wt
assign wt_ch_go=~cnt[1] & FSM_go;//cnt=0 or 1, wt is not enough

wire credit_cnt_decrease=feature_data_vld&feature_data_rdy&CHinKyKx_max_now_comb;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	credit_cnt<=`FSM_CREDIT_NUM;
else
	case({credit_cnt_decrease,credit_vld})
	2'b01:begin if(credit_cnt!=`FSM_CREDIT_NUM) credit_cnt<=credit_cnt+1;end
	2'b10:credit_cnt<=credit_cnt-1;
	endcase
	

endmodule
