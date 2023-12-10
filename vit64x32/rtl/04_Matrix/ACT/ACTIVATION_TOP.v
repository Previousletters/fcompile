`include "CNN_defines.vh"

module ACTIVATION_TOP
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
    input [`log2_S-1:0]in_scale,
    input [`log2_S-1:0]out_scale,
    
    output wdma_done,
    output reg working,

    //rd CMD to MCIF
    output ACT2mcif_rd_req_vld,
    input ACT2mcif_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]ACT2mcif_rd_req_pd,

    //rd response from MCIF
    input mcif2ACT_rd_resp_vld,
    output mcif2ACT_rd_resp_rdy,
    input [`MAX_DAT_DW *`Tout-1:0]mcif2ACT_rd_resp_pd,
    output ACT_rd_fifo_pop,

    //write path to MCIF
    output ACT2mcif_wr_req_vld,
    input ACT2mcif_wr_req_rdy,
    output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]ACT2mcif_wr_req_pd,
    input ACT2mcif_wr_rsp_complete
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

wire wr_done;
wire wr_working;
wire wr_en;

wire buf_out_vld;
wire buf_out_rdy;
wire [`MAX_DAT_DW*`Tout-1:0]buf_out;

wire [`MAX_DAT_DW *`Tout-1:0]ACT_out;
wire [`Tout-1:0]ACT_out_vld;

wire [`MAX_DAT_DW *`Tout-1:0]dat2wdma;
wire dat2wdma_vld;
wire dat2wdma_rdy;

wire credit_vld;


activation_rdma u_activation_rdma
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
    
    .ACT2mcif_rd_req_vld(ACT2mcif_rd_req_vld),
    .ACT2mcif_rd_req_rdy(ACT2mcif_rd_req_rdy),
    .ACT2mcif_rd_req_pd(ACT2mcif_rd_req_pd)
);

activation_dat2buf u_dat2buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .CH_out_div_Tout(CH_in_div_Tout),
    .h_out(h_out),
    .w_out(w_out),
    .pixel_out(pixel_out),
    
    .rd_resp_vld(mcif2ACT_rd_resp_vld),
    .rd_resp_rdy(mcif2ACT_rd_resp_rdy),
    .rd_resp_pd(mcif2ACT_rd_resp_pd),
    .rd_fifo_pop(ACT_rd_fifo_pop),
    
    .done(wr_done),
    .working(wr_working),
    .dat_out(buf_out),
    .dat_out_vld(buf_out_vld),
    .credit_vld(credit_vld)
);

//genvar i;
//generate
//    for(i=0;i<`Tout;i=i+1)
//    begin:act    
//        generate_dat_shift #
//        (
//            .DATA_WIDTH(`MAX_DAT_DW),
//            .DEPTH(8)
//        )dat_out_shift_reg
//        (
//            .clk(clk),
//            .rst_n(rst_n),
//            .data_in(buf_out[i*`MAX_DAT_DW+:`MAX_DAT_DW]),
//            .data_out(ACT_out[i*`MAX_DAT_DW+:`MAX_DAT_DW])
//        );
//	end
//endgenerate 
        
//generate_vld_shift #
//(
//    .DATA_WIDTH(1),
//    .DEPTH(8)
//)shift_reg
//(
//    .clk(clk),
//    .rst_n(rst_n),
//    .data_in(buf_out_vld),
//    .data_out(ACT_out_vld)
//);


genvar i;
generate
    for(i=0;i<`Tout;i=i+1)
    begin:act
        Gelu u_Gelu
        (
            .clk       (clk),
            .rst_n     (rst_n),
            .in_scale  (in_scale[2:0]),
            .x         (buf_out[i*`MAX_DAT_DW+:`MAX_DAT_DW]),
            .in_vld    (buf_out_vld),
        
            .y_reg1    (ACT_out[i*`MAX_DAT_DW+:`MAX_DAT_DW]),
            .out_vld   (ACT_out_vld[i])
        );
	end
endgenerate 

assign credit_vld=dat2wdma_vld&dat2wdma_rdy;
hs_fifo #
(
	.DATA_WIDTH(`MAX_DAT_DW *`Tout),
	.FIFO_DEPTH(4),
	.log2_FIFO_DEPTH(2)
)hs_fifo_cacc_out
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(ACT_out_vld[0]),
    .data_in(ACT_out),
    .data_in_rdy(),
    
    .data_out_vld(dat2wdma_vld),
    .data_out(dat2wdma),
    .data_out_rdy(dat2wdma_rdy)
);


activation_wdma u_wdma
(
    .clk(clk),
    .rst_n(rst_n),
    .start(start),

    .ch_div_Tout(CH_in_div_Tout),
    .h(h_out),
    .w(w_out),
    .feature_base_addr(feature_out_base_addr),
    .feature_surface_stride(feature_out_surface_stride),
    .feature_line_stride(feature_out_line_stride),

	.dat_in_vld(dat2wdma_vld),
	.dat_in_pd(dat2wdma),
	.dat_in_rdy(dat2wdma_rdy),
	
    .ACT2mcif_wr_req_vld(ACT2mcif_wr_req_vld),
    .ACT2mcif_wr_req_rdy(ACT2mcif_wr_req_rdy),
    .ACT2mcif_wr_req_pd(ACT2mcif_wr_req_pd),
    .ACT2mcif_wr_rsp_complete(ACT2mcif_wr_rsp_complete),
    
    .wdma_done(wdma_done)
    
);

endmodule
