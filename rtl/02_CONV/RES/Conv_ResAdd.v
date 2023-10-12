`include "CNN_defines.vh"


module Conv_ResAdd_TOP
(
	input clk,
	input rst_n,
    input start,
    input conv_res_mode,
    
	//from Conv
	input conv_out_dat_vld,
	input [`MAX_BN_DW*`Tout-1:0]conv_out_dat,
    output conv_out_dat_rdy,

	//from REG
	input [`log2_W-1:0]Wout,
	input [`log2_H-1:0]Hout,
	input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout,
	input [31:0]ResAdd_dat_base_addr,
    input [31:0]ResAdd_surface_stride_in,
    input [15:0]ResAdd_line_stride_in,
	input ResAdd_relu_en,
	input [`log2_scale-1:0]ResAdd_in_scale,
	input [`log2_scale-1:0]Conv_out_scale,
	input [`log2_scale-1:0]ResAdd_out_shift,
	
	//rd CMD to MCIF (res)
    output dma2mcif_ResAdd_rd_req_vld,
    input dma2mcif_ResAdd_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_ResAdd_rd_req_pd,

	//from ResAdd_RD
    input mcif2ResAdd_rd_resp_vld,
    output mcif2ResAdd_rd_resp_rdy,
    input [`AXI_DAT_WIDTH-1:0]mcif2ResAdd_rd_resp_pd,
    output ResAdd_rd_fifo_pop,
    
	//to wdma
	output res_out_dat_vld,
	output [`MAX_DAT_DW*`Tout-1:0]res_out_dat,
	input res_out_dat_rdy
);

wire ResAdd_rdma_done;
ResAdd_rdma ResAdd_rdma
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.start(start),
	.done(ResAdd_rdma_done),
	.H(Hout),
	.W(Wout),
	.CH_div_Tout(CH_out_div_Tout),
	.dat_base_addr(ResAdd_dat_base_addr),
	.surface_stride_in(ResAdd_surface_stride_in),
	.line_stride_in(ResAdd_line_stride_in),

	//rd CMD to MCIF
	.rd_req_vld(dma2mcif_ResAdd_rd_req_vld),
	.rd_req_rdy(dma2mcif_ResAdd_rd_req_rdy),
	.rd_req_pd(dma2mcif_ResAdd_rd_req_pd)
);


wire hs_fifo_ResAdd_data_out_vld;
wire [`AXI_DAT_WIDTH-1:0]hs_fifo_ResAdd_data_out_pd;
wire hs_fifo_ResAdd_data_out_rdy;
assign ResAdd_rd_fifo_pop=hs_fifo_ResAdd_data_out_vld&hs_fifo_ResAdd_data_out_rdy;
hs_fifo #
(
	.DATA_WIDTH(`AXI_DAT_WIDTH),
	.FIFO_DEPTH(`BN_FIFO_DEP),
    .log2_FIFO_DEPTH(`log2_BN_FIFO_DEP)
)hs_fifo_ResAdd
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(mcif2ResAdd_rd_resp_vld),
    .data_in(mcif2ResAdd_rd_resp_pd),
    .data_in_rdy(mcif2ResAdd_rd_resp_rdy),
    
    .data_out_vld(hs_fifo_ResAdd_data_out_vld),
    .data_out(hs_fifo_ResAdd_data_out_pd),
    .data_out_rdy(hs_fifo_ResAdd_data_out_rdy)
);

ResAdd_op ResAdd
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.short_cut_scale(ResAdd_in_scale),
	.main_dat_scale(Conv_out_scale),
	.res_add_shift(ResAdd_out_shift),
	.ew_op_mode(2'b0),		//0: add(), 1: mul(), 2: minus(), 3: not defined
	.relu_en(ResAdd_relu_en),

	// data stream
	.s0_dat_vld(conv_out_dat_vld),
	.s0_dat_pd(conv_out_dat),
	.s0_dat_rdy(conv_out_dat_rdy),

	.s1_dat_vld(hs_fifo_ResAdd_data_out_vld),
	.s1_dat_pd(hs_fifo_ResAdd_data_out_pd),
	.s1_dat_rdy(hs_fifo_ResAdd_data_out_rdy),

	// data stream
	.dat_out_vld(res_out_dat_vld),
	.dat_out_pd(res_out_dat),
	.dat_out_rdy(res_out_dat_rdy)
);


endmodule
