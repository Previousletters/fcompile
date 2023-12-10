`include "CNN_defines.vh"

module LN_TOP
(
	input clk,
	input rst_n,

    //from CSR
    input  LN_stage0_start,					              //reg 0
    output LN_stage0_done,
    output LN_stage0_working,
    input  LN_stage1and2_start,
    output wdma_done,
    output reg LN_stage1and2_working,
    
    input [31:0]wt_base_addr,                     //reg 2
    input [31:0]feature_in_base_addr,             //reg 3
    input [25:0]feature_in_surface_stride,        //reg 4
    input [15:0]feature_in_line_stride,           //reg 5
    input [31:0]feature_out_base_addr,            //reg 6
    input [25:0]feature_out_surface_stride,       //reg 7
    input [15:0]feature_out_line_stride,          //reg 8
    input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout, //reg 9,
    input [`log2_H-1:0]h_in,                      //reg 10
    input [`log2_W-1:0]w_in,                      //reg 11
    input [`log2_S-1:0]k_factor_scale,            //reg 13
    input [`log2_S-1:0]bias_scale,                //reg 15
    input [`log2_S-1:0]out_scale,                 //reg 16
    input [`MAX_DW:0]recip_CH_int8,               //reg 17
    input [`MAX_DW*2:0]recip_CH_int16,            //reg 18
    input [`Log2_LN_pixel-1:0]pixel_in,           //reg 19
    

    
	//rd CMD to MCIF
	output reg LN2mcif_rd_req_vld,
	input LN2mcif_rd_req_rdy,
	output reg [`log2AXI_BURST_LEN+32+32-1:0]LN2mcif_rd_req_pd,

	//rd response from MCIF
	input mcif2LN_rd_resp_vld,
	output reg mcif2LN_rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]mcif2LN_rd_resp_pd,
	output reg LN_rd_fifo_pop,

	//write path to MCIF
	output LN2mcif_wr_req_vld,
	input LN2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]LN2mcif_wr_req_pd,
	input LN2mcif_wr_rsp_complete
);

always @(posedge clk or negedge rst_n)
if(~rst_n)
    LN_stage1and2_working<=1'b0;
else
    if(LN_stage1and2_start)
        LN_stage1and2_working<=1'b1;
    else
        if(wdma_done)
            LN_stage1and2_working<=1'b0;
            
wire [`MAX_DAT_DW-1:0] mean_delay;
wire std_deviation_vld;
wire [`MAX_DAT_DW  +2-1:0]std_deviation;
wire [`MAX_DAT_DW*2+4-1:0]variance;
wire variance_vld;
wire [`Log2_LN_pixel-1:0]wr_addr;

wire stage0_working=LN_stage0_working;
wire stage1_start=LN_stage1and2_start;
wire stage1_working;
wire stage1_done;
wire stage2_start=stage1_done;
wire stage2_working;
wire stage2_done;

//rd CMD to MCIF
wire stage0_LN2mcif_rd_req_vld,stage1_LN2mcif_rd_req_vld,stage2_LN2mcif_rd_req_vld;
wire stage0_LN2mcif_rd_req_rdy,stage1_LN2mcif_rd_req_rdy,stage2_LN2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]stage0_LN2mcif_rd_req_pd,stage1_LN2mcif_rd_req_pd,stage2_LN2mcif_rd_req_pd;
//rd response from MCIF
wire stage0_mcif2LN_rd_resp_vld,stage1_mcif2LN_rd_resp_vld,stage2_mcif2LN_rd_resp_vld;
wire stage0_mcif2LN_rd_resp_rdy,stage1_mcif2LN_rd_resp_rdy,stage2_mcif2LN_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]stage0_mcif2LN_rd_resp_pd,stage1_mcif2LN_rd_resp_pd,stage2_mcif2LN_rd_resp_pd;
wire stage0_LN_rd_fifo_pop,stage1_LN_rd_fifo_pop,stage2_LN_rd_fifo_pop;

assign stage0_LN2mcif_rd_req_rdy=stage0_working&LN2mcif_rd_req_rdy;
assign stage1_LN2mcif_rd_req_rdy=stage1_working&LN2mcif_rd_req_rdy;
assign stage2_LN2mcif_rd_req_rdy=stage2_working&LN2mcif_rd_req_rdy;

assign stage0_mcif2LN_rd_resp_vld=stage0_working&mcif2LN_rd_resp_vld;
assign stage1_mcif2LN_rd_resp_vld=stage1_working&mcif2LN_rd_resp_vld;
assign stage2_mcif2LN_rd_resp_vld=stage2_working&mcif2LN_rd_resp_vld;

assign stage0_mcif2LN_rd_resp_pd=stage0_working?mcif2LN_rd_resp_pd:0;
assign stage1_mcif2LN_rd_resp_pd=stage1_working?mcif2LN_rd_resp_pd:0;
assign stage2_mcif2LN_rd_resp_pd=stage2_working?mcif2LN_rd_resp_pd:0;

wire rd_dat_en;
wire [`Log2_LN_pixel-1:0]rd_dat_addr;
wire [`MAX_DAT_DW  -1:0]mean;
wire [`MAX_DAT_DW*2-1:0]recip_sqrt;
wire mean_vld;

wire [`Log2_LN_ch_max-`log2Tout-1:0]rd_wt_addr;
wire rd_wt_en;
wire [`Log2_LN_ch_max-`log2Tout-1:0]rd_bias_addr;
wire rd_bias_en;
wire [`MAX_DAT_DW *`Tout-1:0]bias_rd_dat;
wire [`MAX_DAT_DW *`Tout-1:0]wt_rd_dat;
wire rd_wt_vld;
wire rd_bias_vld;

wire credit_vld;
wire [`MAX_DAT_DW *`Tout-1:0]stage2_out;
wire stage2_out_vld;
wire stage2_out_rdy;

wire [`MAX_DAT_DW *`Tout-1:0]dat2wdma_out;
wire dat2wdma_vld;
wire dat2wdma_rdy;

always@(*)
    case({stage2_working,stage1_working,stage0_working})
    3'b001:begin
            LN2mcif_rd_req_vld  = stage0_LN2mcif_rd_req_vld;
            LN2mcif_rd_req_pd   = stage0_LN2mcif_rd_req_pd;
            mcif2LN_rd_resp_rdy = stage0_mcif2LN_rd_resp_rdy;
            LN_rd_fifo_pop      = stage0_LN_rd_fifo_pop;
            end
    3'b010:begin
            LN2mcif_rd_req_vld  = stage1_LN2mcif_rd_req_vld;
            LN2mcif_rd_req_pd   = stage1_LN2mcif_rd_req_pd;
            mcif2LN_rd_resp_rdy = stage1_mcif2LN_rd_resp_rdy;
            LN_rd_fifo_pop      = stage1_LN_rd_fifo_pop;
            end
    3'b100:begin
            LN2mcif_rd_req_vld  = stage2_LN2mcif_rd_req_vld;
            LN2mcif_rd_req_pd   = stage2_LN2mcif_rd_req_pd;
            mcif2LN_rd_resp_rdy = stage2_mcif2LN_rd_resp_rdy;
            LN_rd_fifo_pop      = stage2_LN_rd_fifo_pop;
            end
    default:begin
            LN2mcif_rd_req_vld  = 0;
            LN2mcif_rd_req_pd   = 0;
            mcif2LN_rd_resp_rdy = 0;
            LN_rd_fifo_pop      = 0;
            end
    endcase


LN_stage_0 u_LN_stage_0 // get the gama and beta
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(LN_stage0_start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .wt_base_addr(wt_base_addr),
    
    //rd CMD to MCIF
    .LN2mcif_rd_req_vld(stage0_LN2mcif_rd_req_vld),
    .LN2mcif_rd_req_pd(stage0_LN2mcif_rd_req_pd),
    .LN2mcif_rd_req_rdy(stage0_LN2mcif_rd_req_rdy),
    
    .mcif2LN_rd_resp_vld(stage0_mcif2LN_rd_resp_vld),
    .mcif2LN_rd_resp_rdy(stage0_mcif2LN_rd_resp_rdy),
    .mcif2LN_rd_resp_pd(stage0_mcif2LN_rd_resp_pd),
    .LN_rd_fifo_pop(stage0_LN_rd_fifo_pop),
    
    .stage0_working(LN_stage0_working),
    .stage0_done(LN_stage0_done),
    
    .rd_wt_addr(rd_wt_addr),
    .rd_wt_en(rd_wt_en),
    .rd_bias_addr(rd_bias_addr),
    .rd_bias_en(rd_bias_en),
    .bias_rd_dat(bias_rd_dat),
    .wt_rd_dat(wt_rd_dat),
    .rd_wt_vld(rd_wt_vld),
    .rd_bias_vld(rd_bias_vld)

);

LN_stage_1 u_LN_stage_1 // calculate the mean and variance
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(stage1_start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .recip_CH_int16(recip_CH_int16), 
    .feature_in_base_addr(feature_in_base_addr),
    .feature_in_surface_stride(feature_in_surface_stride),
    .feature_in_line_stride(feature_in_line_stride),
    
    //rd CMD to MCIF
    .LN2mcif_rd_req_vld(stage1_LN2mcif_rd_req_vld),
    .LN2mcif_rd_req_pd(stage1_LN2mcif_rd_req_pd),
    .LN2mcif_rd_req_rdy(stage1_LN2mcif_rd_req_rdy),
    
    .mcif2LN_rd_resp_vld(stage1_mcif2LN_rd_resp_vld),
    .mcif2LN_rd_resp_rdy(stage1_mcif2LN_rd_resp_rdy),
    .mcif2LN_rd_resp_pd(stage1_mcif2LN_rd_resp_pd),
    .LN_rd_fifo_pop(stage1_LN_rd_fifo_pop),
    
    .variance(variance),
    .std_deviation(std_deviation),
    .mean_delay(mean_delay),
    .std_deviation_vld(std_deviation_vld),

    .wr_addr(wr_addr),
    
    .stage1_working(stage1_working),
    .stage1_done(stage1_done)
    
);


SRAM_for_mean_and_deviation u_onchip_SRAM
(
    .clk(clk),
    .rst_n(rst_n),

    .dat_in_vld(std_deviation_vld),
    .mean_dat_in(mean_delay),
    .deviation_dat_in(std_deviation),
    .wr_addr(wr_addr),
    
    .rd_en(rd_dat_en),
    .rd_addr(rd_dat_addr),
    .mean_dat_out(mean),
    .recip_deviation_dat_out(recip_sqrt),
    .dat_out_vld(mean_vld)
);

LN_stage_2 u_LN_stage_2 // LN operation
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(stage2_start),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .recip_CH_int16(recip_CH_int16), 
    .pixel_in(pixel_in),
    .k_factor_scale(k_factor_scale),  
   
    .bias_scale(bias_scale),      
    .out_scale(out_scale),   
    .feature_in_base_addr(feature_in_base_addr),
    .feature_in_surface_stride(feature_in_surface_stride),
    .feature_in_line_stride(feature_in_line_stride),
    
    //rd CMD to MCIF
    .LN2mcif_rd_req_vld(stage2_LN2mcif_rd_req_vld),
    .LN2mcif_rd_req_pd(stage2_LN2mcif_rd_req_pd),
    .LN2mcif_rd_req_rdy(stage2_LN2mcif_rd_req_rdy),
    
    .mcif2LN_rd_resp_vld(stage2_mcif2LN_rd_resp_vld),
    .mcif2LN_rd_resp_rdy(stage2_mcif2LN_rd_resp_rdy),
    .mcif2LN_rd_resp_pd(stage2_mcif2LN_rd_resp_pd),
    .LN_rd_fifo_pop(stage2_LN_rd_fifo_pop),
    
    .rd_dat_en(rd_dat_en),
    .rd_dat_addr(rd_dat_addr),   
    .mean(mean),
    .mean_vld(mean_vld),
    .recip_sqrt(recip_sqrt),

    .rd_wt_en(rd_wt_en),
    .rd_wt_addr(rd_wt_addr),
    .rd_bias_en(rd_bias_en),
    .rd_bias_addr(rd_bias_addr),
    .bias(bias_rd_dat),
    .wt(wt_rd_dat),
    .wt_vld(rd_wt_vld),
    .bias_vld(rd_bias_vld),
    
    .dat_out(stage2_out),
    .dat_out_vld(stage2_out_vld),
    .credit_vld(credit_vld)
    
);

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
    
    .data_in_vld(stage2_out_vld),
    .data_in(stage2_out),
    .data_in_rdy(),
    
    .data_out_vld(dat2wdma_vld),
    .data_out(dat2wdma_out),
    .data_out_rdy(dat2wdma_rdy)
);

LN_wdma u_wdma
(
    .clk(clk),
    .rst_n(rst_n),
    
    .start(stage2_start),
    .done(stage2_done),
    .working(stage2_working),
    
    .w(w_in),
    .h(h_in),
    
    .ch_div_Tout(CH_in_div_Tout),
    .feature_base_addr(feature_out_base_addr),
    .feature_surface_stride(feature_out_surface_stride),
    .feature_line_stride(feature_out_line_stride),
    
    .wdma_done(wdma_done),
    
    .dat_in_vld(dat2wdma_vld),
    .dat_in_pd(dat2wdma_out),
    .dat_in_rdy(dat2wdma_rdy),
    
    .LN2mcif_wr_req_vld(LN2mcif_wr_req_vld),
    .LN2mcif_wr_req_rdy(LN2mcif_wr_req_rdy),
    .LN2mcif_wr_req_pd(LN2mcif_wr_req_pd),
    .LN2mcif_wr_rsp_complete(LN2mcif_wr_rsp_complete)
);

endmodule

