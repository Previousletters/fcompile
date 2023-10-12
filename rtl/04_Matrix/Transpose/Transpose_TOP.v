`include "CNN_defines.vh"

module Transpose_TOP
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
    
    input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout, //reg 9
    input [`log2_H-1:0]h_in,                      //reg 10
    input [`log2_W-1:0]w_in,                      //reg 11  
    input [`Log2_Trans_pixel-1:0]pixel_in,        //reg 19
    input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout, //reg 20
    input [`log2_H-1:0]h_out,                      //reg 21
    input [`log2_W-1:0]w_out,                      //reg 22  
    input [`Log2_Trans_pixel-1:0]pixel_out,        //reg 23
    input [31:0]wt_num_div_Tout,                   //reg 24
    
    output wdma_done,
    output reg working,

	//rd CMD to MCIF
	output Trans2mcif_rd_req_vld,
	input Trans2mcif_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]Trans2mcif_rd_req_pd,

	//rd response from MCIF
	input mcif2Trans_rd_resp_vld,
	output mcif2Trans_rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]mcif2Trans_rd_resp_pd,
	output Trans_rd_fifo_pop,

	//write path to MCIF
	output Trans2mcif_wr_req_vld,
	input Trans2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Trans2mcif_wr_req_pd,
	input Trans2mcif_wr_rsp_complete
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
            
wire [`base_log2Tin-1:0]wr_addr;
wire [`base_log2Tin:0]wr_addr_max;
wire [`MAX_DAT_DW*`Tout-1:0]wr_dat;
wire wr_vld;
wire wr_rdy;

wire [`base_log2Tin-1:0]rd_addr;
wire [`base_log2Tin:0]rd_addr_max;
wire rd_vld;
wire rd_rdy;

wire buf_out_vld;
wire buf_out_rdy;
wire [`MAX_DAT_DW*`Tout-1:0]buf_out;

wire [`MAX_DAT_DW *`Tout-1:0]dat2wdma;
wire dat2wdma_vld;
wire dat2wdma_rdy;

wire credit_vld=dat2wdma_vld&dat2wdma_rdy;


Transpose_rdma u_rdma
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .base_addr(feature_in_base_addr),
    .surface_stride(feature_in_surface_stride),
    .line_stride(feature_in_line_stride),
    
    .Trans2mcif_rd_req_vld(Trans2mcif_rd_req_vld),
    .Trans2mcif_rd_req_rdy(Trans2mcif_rd_req_rdy),
    .Trans2mcif_rd_req_pd(Trans2mcif_rd_req_pd)
);

Transpose_dat2buf u_dat2buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_out),
    .w_out(w_out),
    .pixel_out(pixel_out),
    
    .rd_resp_vld(mcif2Trans_rd_resp_vld),
    .rd_resp_rdy(mcif2Trans_rd_resp_rdy),
    .rd_resp_pd(mcif2Trans_rd_resp_pd),
    .rd_fifo_pop(Trans_rd_fifo_pop),

    .wr_addr(wr_addr),
    .wr_addr_max(wr_addr_max),
    .wr_dat(wr_dat),
    .wr_vld(wr_vld),
    .wr_rdy(wr_rdy)

);


Transpose_buf#
(
	.DAT_WIDTH(`Tout*`MAX_DAT_DW),
	.MEM_DEPTH(`base_Tin),
	.log2_MEM_DEPTH(`base_log2Tin)
)u_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    .wr_vld(wr_vld),
    .waddr(wr_addr),
    .wdata(wr_dat),
    .waddr_max(wr_addr_max),
    .wr_rdy(wr_rdy),
    
    .rd_vld(rd_vld),
    .rd_rdy(rd_rdy),
    .raddr(rd_addr),
    .raddr_max(rd_addr_max),
    
    .rdata(buf_out),
    .rd_dat_out_vld(buf_out_vld)
    
);


Transpose_fsm u_fsm
(
    .clk(clk),
    .rst_n(rst_n),
    
    .pixel_in(pixel_in),
    .pixel_out(pixel_out),
    
    .rd_vld(rd_vld),
    .rd_addr(rd_addr),
    .rd_addr_max(rd_addr_max),
    .rd_rdy(rd_rdy),
            
    .credit_vld(credit_vld)

);


hs_fifo #
(
	.DATA_WIDTH(`MAX_DAT_DW *`Tout),
	.FIFO_DEPTH(4),
	.log2_FIFO_DEPTH(2)
)hs_fifo_cacc_out
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(buf_out_vld),
    .data_in(buf_out),
    .data_in_rdy(),
    
    .data_out_vld(dat2wdma_vld),
    .data_out(dat2wdma),
    .data_out_rdy(dat2wdma_rdy)
);
		
Transpose_wdma u_wdma
(
    .clk(clk),
    .rst_n(rst_n),
    .start(start),

    .wt_num_div_Tout(wt_num_div_Tout),
    .wt_base_addr(feature_out_base_addr),

	.dat_in_vld(dat2wdma_vld),
	.dat_in_pd(dat2wdma),
	.dat_in_rdy(dat2wdma_rdy),
	
    .Trans2mcif_wr_req_vld(Trans2mcif_wr_req_vld),
    .Trans2mcif_wr_req_rdy(Trans2mcif_wr_req_rdy),
    .Trans2mcif_wr_req_pd(Trans2mcif_wr_req_pd),
    .Trans2mcif_wr_rsp_complete(Trans2mcif_wr_rsp_complete),
    
    .wdma_done(wdma_done)
    
);

endmodule

