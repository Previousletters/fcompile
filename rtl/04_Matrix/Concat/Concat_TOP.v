`include "CNN_defines.vh"

module Concat_TOP
(
	input clk,
	input rst_n,

    //from CSR
    input start,					              //reg 0
    input [31:0]feature_in_base_addr,             //reg 3
    input [25:0]feature_in_surface_stride,        //reg 4
    input [15:0]feature_in_line_stride,           //reg 5
    input [31:0]feature_out_base_addr,            //reg 6
    input [25:0]feature_out_surface_stride,       //reg 7
    input [15:0]feature_out_line_stride,          //reg 8
    input [31:0]clstoken_base_addr,               //reg 24   
    
    input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout, //reg 9
    input [`log2_W-1:0]w_in,                      //reg 11
    
    output wdma_done,
    output reg working,

	//rd CMD to MCIF
	output Concat2mcif_rd_req_vld,
	input Concat2mcif_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]Concat2mcif_rd_req_pd,

	//rd response from MCIF
	input mcif2Concat_rd_resp_vld,
	output mcif2Concat_rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]mcif2Concat_rd_resp_pd,
	output Concat_rd_fifo_pop,

	//write path to MCIF
	output Concat2mcif_wr_req_vld,
	input Concat2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Concat2mcif_wr_req_pd,
	input Concat2mcif_wr_rsp_complete
);

always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(wdma_done)
            working<=1'b0;


Concat_rdma u_rdma
(
    .clk(clk),
    .rst_n(rst_n),    
    .start(start),    

    .clstoken_base_addr(clstoken_base_addr),
    .base_addr(feature_in_base_addr),
    .surface_stride(feature_in_surface_stride),
    .line_stride(feature_in_line_stride),
    .w_in(w_in),
    .CH_in_div_Tout(CH_in_div_Tout),
    
    .Concat2mcif_rd_req_vld(Concat2mcif_rd_req_vld),
    .Concat2mcif_rd_req_rdy(Concat2mcif_rd_req_rdy),
    .Concat2mcif_rd_req_pd(Concat2mcif_rd_req_pd)
);

assign Concat_rd_fifo_pop=mcif2Concat_rd_resp_vld&mcif2Concat_rd_resp_rdy;

Concat_wdma u_wdma
(
    .clk(clk),
    .rst_n(rst_n),
    .start(start),

    .ch_div_Tout(CH_in_div_Tout),
    .w_in(w_in),
    
    .feature_base_addr(feature_out_base_addr),
    .feature_surface_stride(feature_out_surface_stride),
    .feature_line_stride(feature_out_line_stride),

	.dat_in_vld(mcif2Concat_rd_resp_vld),
	.dat_in_pd(mcif2Concat_rd_resp_pd),
	.dat_in_rdy(mcif2Concat_rd_resp_rdy),
	
    .Concat2mcif_wr_req_vld(Concat2mcif_wr_req_vld),
    .Concat2mcif_wr_req_rdy(Concat2mcif_wr_req_rdy),
    .Concat2mcif_wr_req_pd(Concat2mcif_wr_req_pd),
    .Concat2mcif_wr_rsp_complete(Concat2mcif_wr_rsp_complete),
    
    .wdma_done(wdma_done)
    
);

endmodule

