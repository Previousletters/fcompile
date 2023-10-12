`include "CNN_defines.vh"

module SOFTMAX_TOP
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
    input [`Log2_Trans_pixel-1:0]ch_in,           //reg 23
    input [`log2_S-1:0]in_scale,
    input [`log2_S-1:0]out_scale,
    
    output wdma_done,
    output reg working,

    //rd CMD to MCIF
    output Softmax2mcif_rd_req_vld,
    input Softmax2mcif_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]Softmax2mcif_rd_req_pd,

    //rd response from MCIF
    input mcif2Softmax_rd_resp_vld,
    output mcif2Softmax_rd_resp_rdy,
    input [`MAX_DAT_DW *`Tout-1:0]mcif2Softmax_rd_resp_pd,
    output Softmax_rd_fifo_pop,

    //write path to MCIF
    output Softmax2mcif_wr_req_vld,
    input Softmax2mcif_wr_req_rdy,
    output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Softmax2mcif_wr_req_pd,
    input Softmax2mcif_wr_rsp_complete
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

wire wr_vld;
wire wr_rdy;
wire [`log2AXI_BURST_LEN-1:0]wr_addr;
wire [`Log2_Softmax_pixel-1:0]pixel_addr_now;
wire [`Log2_Softmax_pixel-1:0]ch_addr_now;
wire [`MAX_DAT_DW*`Tout-1:0]wr_dat;

wire [2:0]cycle;

wire [`MAX_DAT_DW*`Tout-1:0]softmax_dat_out;
wire softmax_dat_out_vld;

wire [`MAX_DAT_DW *`Tout-1:0]dat2wdma;
wire dat2wdma_vld;
wire dat2wdma_rdy;

wire credit_vld;


softmax_rdma u_rdma
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .ch_in(ch_in),
    
    .h_in(h_in),
    .w_in(w_in),
    
    .pixel_in(pixel_in),
    .base_addr(feature_in_base_addr),
    .surface_stride(feature_in_surface_stride),
    .line_stride(feature_in_line_stride),
    
    .Softmax2mcif_rd_req_vld(Softmax2mcif_rd_req_vld),
    .Softmax2mcif_rd_req_rdy(Softmax2mcif_rd_req_rdy),
    .Softmax2mcif_rd_req_pd(Softmax2mcif_rd_req_pd)
);

softmax_dat2buf u_dat2buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .h_in(h_in),
    .w_in(w_in),
    .CH_in_div_Tout(CH_in_div_Tout),
    .ch_in(ch_in),
    
    .rd_resp_vld(mcif2Softmax_rd_resp_vld),
    .rd_resp_rdy(mcif2Softmax_rd_resp_rdy),
    .rd_resp_pd(mcif2Softmax_rd_resp_pd),
    .rd_fifo_pop(Softmax_rd_fifo_pop),
    
    .wr_vld(wr_vld),
    .wr_rdy(wr_rdy),
    .wr_addr(wr_addr), //`AXI_BURST_LEN
    .pixel_addr(pixel_addr_now),
    .ch_addr(ch_addr_now),//current group ch index
    .cycle(cycle),
    .wr_dat(wr_dat),
    
    .credit_vld(credit_vld)
);

softmax_fsm u_fsm
(
	.clk(clk),
	.rst_n(rst_n),

    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_out),
    .w_out(w_out),
    .ch_in(ch_in),
    .in_scale(in_scale),
    .out_scale(out_scale),
    
    .dat_in(wr_dat),
    .dat_in_vld(wr_vld),
    .dat_in_rdy(wr_rdy),
    .wr_addr(wr_addr),
    .ch_addr_now(ch_addr_now), 
    
    .dat_out(softmax_dat_out),
    .dat_out_vld(softmax_dat_out_vld)
);


assign credit_vld=dat2wdma_vld&dat2wdma_rdy;
hs_fifo #
(
	.DATA_WIDTH(`MAX_DAT_DW *`Tout),
	.FIFO_DEPTH(`Softmax_delay+`AXI_BURST_LEN+2+2),
	.log2_FIFO_DEPTH(6)
)hs_fifo_cacc_out
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(softmax_dat_out_vld),
    .data_in(softmax_dat_out),
    .data_in_rdy(),
    
    .data_out_vld(dat2wdma_vld),
    .data_out(dat2wdma),
    .data_out_rdy(dat2wdma_rdy)
);


softmax_wdma u_wdma
(
    .clk(clk),
    .rst_n(rst_n),
    .start(start),

    .ch_div_Tout(CH_in_div_Tout),
    .h(h_out),
    .w(w_out),
    .pixel_in(pixel_in),
    
    .feature_base_addr(feature_out_base_addr),
    .feature_surface_stride(feature_out_surface_stride),
    .feature_line_stride(feature_out_line_stride),

	.dat_in_vld(dat2wdma_vld),
	.dat_in_pd(dat2wdma),
	.dat_in_rdy(dat2wdma_rdy),
	
    .Softmax2mcif_wr_req_vld(Softmax2mcif_wr_req_vld),
    .Softmax2mcif_wr_req_rdy(Softmax2mcif_wr_req_rdy),
    .Softmax2mcif_wr_req_pd(Softmax2mcif_wr_req_pd),
    .Softmax2mcif_wr_rsp_complete(Softmax2mcif_wr_rsp_complete),
    
    .wdma_done(wdma_done)
    
);

endmodule
