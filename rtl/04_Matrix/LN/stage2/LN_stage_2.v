`include "CNN_defines.vh"

module LN_stage_2
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`Log2_LN_pixel-1:0]pixel_in,
    
    input [`log2_S-1:0]k_factor_scale, //gama or wt
    input [`log2_S-1:0]bias_scale,
    input [`log2_S-1:0]out_scale,
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
    input [`MAX_DAT_DW*`Tout-1:0]mcif2LN_rd_resp_pd,
    output LN_rd_fifo_pop,
    
    //to onchip SRAM
    output rd_dat_en,
    output [`Log2_LN_pixel-1:0]rd_dat_addr,
    input [`MAX_DAT_DW-1:0]mean,
    input mean_vld,
    input [`MAX_DAT_DW*2-1:0]recip_sqrt,

    output rd_wt_en,
    output [`Log2_LN_ch_max-`log2Tout-1:0]rd_wt_addr,
    output rd_bias_en,
    output [`Log2_LN_ch_max-`log2Tout-1:0]rd_bias_addr,
    input [`MAX_DAT_DW*`Tout-1:0]wt,
    input [`MAX_DAT_DW*`Tout-1:0]bias,
    input wt_vld,
    input bias_vld,
    
    output [`MAX_DAT_DW*`Tout-1:0]dat_out,
    output dat_out_vld,
    input credit_vld
	
);

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


wire [`MAX_DAT_DW*`Tout-1:0]original_feature;
wire original_feature_vld;

LN_ctrl u_LN_ctrl
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .done(done),
    .credit_vld(credit_vld),
    
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),

    .rd_resp_vld(mcif2LN_rd_resp_vld),
    .rd_resp_rdy(mcif2LN_rd_resp_rdy),
    .rd_resp_pd(mcif2LN_rd_resp_pd),
    .rd_fifo_pop(LN_rd_fifo_pop),
    
	.original_feature(original_feature),
    .original_feature_vld(original_feature_vld),
    .rd_dat_en(rd_dat_en),
    .rd_dat_addr(rd_dat_addr),
    
    .rd_wt_en(rd_wt_en),
    .rd_wt_addr(rd_wt_addr),
    .rd_bias_en(rd_bias_en),
    .rd_bias_addr(rd_bias_addr)
);


LN_calculation u_LN_calculation
(
    .clk(clk),
    .rst_n(rst_n),

    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .k_factor_scale(k_factor_scale),
    .bias_scale(bias_scale),
    .out_scale(out_scale),

	.original_feature(original_feature),
    .original_feature_vld(original_feature_vld),
    .mean(mean),
    .mean_vld(mean_vld),
    .recip_sqrt(recip_sqrt),
    .bias(bias),
    .wt(wt),
    .wt_vld(wt_vld),
    .bias_vld(bias_vld),
    
    .dat_out(dat_out),
    .dat_out_vld(dat_out_vld)
);


endmodule