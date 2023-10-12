`include "CNN_defines.vh"

module LN_stage_1
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`MAX_DW*2:0]recip_CH_int16,
    input [31:0]feature_in_base_addr,
    input [25:0]feature_in_surface_stride,
    input [15:0]feature_in_line_stride,
    
    //rd CMD to MCIF
    output LN2mcif_rd_req_vld,
    input LN2mcif_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]LN2mcif_rd_req_pd,
    
    //rd response from MCIF
    input mcif2LN_rd_resp_vld,
    output mcif2LN_rd_resp_rdy,
    input [`MAX_DAT_DW *`Tout-1:0]mcif2LN_rd_resp_pd,
    output LN_rd_fifo_pop,
    
	output [`MAX_DAT_DW-1:0]mean_delay,
	output [`MAX_DAT_DW*2+4-1:0]variance,
	output [`MAX_DAT_DW  +2-1:0]std_deviation,
 	output std_deviation_vld,
	
	output [`Log2_LN_pixel-1:0]wr_addr,
	output reg stage1_working,
	output stage1_done
	
);

always @(posedge clk or negedge rst_n)
if(~rst_n)
    stage1_working<=1'b0;
else
    if(start)
        stage1_working<=1'b1;
    else
        if(stage1_done)
            stage1_working<=1'b0;


wire Stripe_loop_end;
wire CH_max_now;
wire CH_Stripe_loop_end;
wire [`MAX_DAT_DW+`log2Tout-1:0]dat_sum;
wire dat_sum_vld;
wire [`MAX_DAT_DW*2+`log2Tout-1:0]dat_square_sum;
wire dat_square_sum_vld;

wire [`MAX_DAT_DW*2-1:0]mean_square;
wire [`MAX_DAT_DW-1:0]mean;
wire mean_vld;
wire [`MAX_DAT_DW*2+`Log2_LN_ch_max-1:0]dat_square_acc;
wire square_vld;


LN_rdma LN_rdma
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.start(start),
	.base_addr(feature_in_base_addr),
	.surface_stride(feature_in_surface_stride),
	.line_stride(feature_in_line_stride),
	.CH_in_div_Tout(CH_in_div_Tout),				//ceil(ch_in/Tout)
	.h_in(h_in),
    .w_in(w_in),

	//rd CMD to MCIF
	.LN2mcif_rd_req_vld(LN2mcif_rd_req_vld),
    .LN2mcif_rd_req_pd(LN2mcif_rd_req_pd),
	.LN2mcif_rd_req_rdy(LN2mcif_rd_req_rdy)

);


LN_dat_sum_and_square u_LN_dat_sum_and_square
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
        
    .rd_resp_vld(mcif2LN_rd_resp_vld),
    .rd_resp_rdy(mcif2LN_rd_resp_rdy),
    .rd_resp_pd(mcif2LN_rd_resp_pd),
    .rd_fifo_pop(LN_rd_fifo_pop),
    
    .dat_sum(dat_sum),
    .dat_sum_vld(dat_sum_vld),
    .dat_square_sum(dat_square_sum),
    .dat_square_sum_vld(dat_square_sum_vld),
    .Stripe_loop_end(Stripe_loop_end),
    .CH_max_now(CH_max_now),
    .CH_Stripe_loop_end(CH_Stripe_loop_end)
);

LN_mean_acc u_LN_mean_acc
(
    .clk(clk),
    .rst_n(rst_n),
    .dat_i(dat_sum),
    .dat_vld_i(dat_sum_vld),
    
    .Stripe_loop_end(Stripe_loop_end),
    .CH_acc_max_now(CH_max_now),
    .CH_and_Stripe_loop_end(CH_Stripe_loop_end),
    .recip_CH_int16(recip_CH_int16),
    
    .dat_out_vld(mean_vld),
    .dat_out(mean),
    .dat_out_square(mean_square)
);

LN_variance_acc u_LN_variance_acc
(
    .clk(clk),
    .rst_n(rst_n),
    .dat_i(dat_square_sum),
    .dat_vld_i(dat_square_sum_vld),
    
    .Stripe_loop_end(Stripe_loop_end),
    .CH_acc_max_now(CH_max_now),
    .CH_and_Stripe_loop_end(CH_Stripe_loop_end),
    .recip_CH_int16(recip_CH_int16),
    
    .dat_out_vld(square_vld),
    .dat_out(dat_square_acc)
);

LN_variance_calculation u_LN_var_calculation
(
    .clk(clk),
    .rst_n(rst_n),
    
    .h_in(h_in),
    .w_in(w_in),
    
    .mean_vld(mean_vld),
    .mean(mean),
    .mean_square(mean_square),
    .dat_square_acc_vld(square_vld),
    .dat_square_acc(dat_square_acc),

    .variance(variance),
    .std_deviation(std_deviation),
    .mean_delay(mean_delay),
    .std_deviation_vld(std_deviation_vld),

    .wr_addr(wr_addr),
    .variance_done(stage1_done)
);

endmodule