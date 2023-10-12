`include "CNN_defines.vh"

module MATRIX_TOP
(
	input clk,
	input rst_n,

    input Matrix_In_from_BRAM_mode,
    input Matrix_Out_to_BRAM_mode,
    output [32-1:0]matrix_read_base_addr,
    output [32-1:0]matrix_write_base_addr,
    
	//from CSB Master
	input csb2csr_req_vld,
	output csb2csr_req_rdy,
	input [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd,
	output csr2csb_resp_vld,
	output [31:0]csr2csb_resp_pd,

	//rd CMD to MCIF
	output reg Matrix2mcif_rd_req_vld,
	input Matrix2mcif_rd_req_rdy,
	output reg [`log2AXI_BURST_LEN+32+32-1:0]Matrix2mcif_rd_req_pd,

	//rd response from MCIF
	input mcif2Matrix_rd_resp_vld,
	output reg mcif2Matrix_rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]mcif2Matrix_rd_resp_pd,
	output reg Matrix_rd_fifo_pop,

	//write path to MCIF
	output reg Matrix2mcif_wr_req_vld,
	input Matrix2mcif_wr_req_rdy,
	output reg [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Matrix2mcif_wr_req_pd,
	input Matrix2mcif_wr_rsp_complete
);
//rd CMD to MCIF
wire LN2mcif_rd_req_vld;
wire LN2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]LN2mcif_rd_req_pd;
//rd response from MCIF
wire mcif2LN_rd_resp_vld;
wire mcif2LN_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]mcif2LN_rd_resp_pd;
wire LN_rd_fifo_pop;
//write path to MCIF
wire LN2mcif_wr_req_vld;
wire LN2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]LN2mcif_wr_req_pd;
wire LN2mcif_wr_rsp_complete;

//rd CMD to MCIF
wire Trans2mcif_rd_req_vld;
wire Trans2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]Trans2mcif_rd_req_pd;
//rd response from MCIF
wire mcif2Trans_rd_resp_vld;
wire mcif2Trans_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]mcif2Trans_rd_resp_pd;
wire Trans_rd_fifo_pop;
//write path to MCIF
wire Trans2mcif_wr_req_vld;
wire Trans2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Trans2mcif_wr_req_pd;
wire Trans2mcif_wr_rsp_complete;
	
//rd CMD to MCIF
wire Concat2mcif_rd_req_vld;
wire Concat2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]Concat2mcif_rd_req_pd;
//rd response from MCIF
wire mcif2Concat_rd_resp_vld;
wire mcif2Concat_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]mcif2Concat_rd_resp_pd;
wire Concat_rd_fifo_pop;
//write path to MCIF
wire Concat2mcif_wr_req_vld;
wire Concat2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Concat2mcif_wr_req_pd;
wire Concat2mcif_wr_rsp_complete;

//rd CMD to MCIF
wire Softmax2mcif_rd_req_vld;
wire Softmax2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]Softmax2mcif_rd_req_pd;
//rd response from MCIF
wire mcif2Softmax_rd_resp_vld;
wire mcif2Softmax_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]mcif2Softmax_rd_resp_pd;
wire Softmax_rd_fifo_pop;
//write path to MCIF
wire Softmax2mcif_wr_req_vld;
wire Softmax2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Softmax2mcif_wr_req_pd;
wire Softmax2mcif_wr_rsp_complete;


//rd CMD to MCIF
wire ACT2mcif_rd_req_vld;
wire ACT2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]ACT2mcif_rd_req_pd;
//rd response from MCIF
wire mcif2ACT_rd_resp_vld;
wire mcif2ACT_rd_resp_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]mcif2ACT_rd_resp_pd;
wire ACT_rd_fifo_pop;
//write path to MCIF
wire ACT2mcif_wr_req_vld;
wire ACT2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]ACT2mcif_wr_req_pd;
wire ACT2mcif_wr_rsp_complete;

//from CSR
wire Trans_start,LN_start,Concat_start,Softmax_start,ACT_start;       //reg 0

wire [31:0]wt_base_addr;                      //reg 2
wire [31:0]feature_in_base_addr;              //reg 3
wire [25:0]feature_in_surface_stride;         //reg 4
wire [15:0]feature_in_line_stride;            //reg 5
wire [31:0]feature_out_base_addr;             //reg 6
wire [25:0]feature_out_surface_stride;        //reg 7
wire [15:0]feature_out_line_stride;           //reg 8
wire [`log2_CH-`log2Tout-1:0]CH_in_div_Tout;  //reg 9;
wire [`log2_H-1:0]h_in;                       //reg 10
wire [`log2_W-1:0]w_in;                       //reg 11  
wire [`MAX_DW-1:0]k_factor;                   //reg 12
wire [`log2_S-1:0]k_factor_scale;             //reg 13
wire [`MAX_DW-1:0]bias;                       //reg 14
wire [`log2_S-1:0]bias_scale;                 //reg 15
wire [`log2_S-1:0]out_scale;                  //reg 16
wire [`MAX_DW:0]recip_CH_int8;                //reg 17
wire [`MAX_DW*2:0]recip_CH_int16;             //reg 18
wire [`Log2_LN_pixel-1:0]pixel_in;            //reg 19
wire [`log2_CH-`log2Tout-1:0]CH_out_div_Tout; //reg 20
wire [`log2_H-1:0]h_out;                      //reg 21
wire [`log2_W-1:0]w_out;                      //reg 22  
wire [`Log2_Trans_pixel-1:0]pixel_out;        //reg 23
wire [31:0]clstoken_base_addr;                //reg 24
wire [31:0] wt_num_div_Tout;                  //reg 25


wire LN_stage0_start;
wire LN_stage0_working;
wire LN_stage0_done;
wire LN_stage1and2_start;
wire LN_stage1and2_working;
wire LN_stage1and2_done;

reg wdma_done;
wire LN_wdma_done,Trans_wdma_done,Concat_wdma_done,Softmax_wdma_done,ACT_wdma_done;
wire LN_working,Trans_working,Concat_working,Softmax_working,ACT_working;
assign LN_working=LN_stage0_working|LN_stage1and2_working;

always@(*)
    case({ACT_working,Softmax_working,Concat_working,LN_working,Trans_working})
        5'b00001: wdma_done=Trans_wdma_done;
        5'b00010: wdma_done=LN_wdma_done;
        5'b00100: wdma_done=Concat_wdma_done;
        5'b01000: wdma_done=Softmax_wdma_done;
        5'b10000: wdma_done=ACT_wdma_done;
        default:wdma_done=0;
    endcase
    
assign LN2mcif_rd_req_rdy     =LN_working    & Matrix2mcif_rd_req_rdy;
assign mcif2LN_rd_resp_vld    =LN_working    & mcif2Matrix_rd_resp_vld;
assign mcif2LN_rd_resp_pd     =LN_working    ? mcif2Matrix_rd_resp_pd:0;

assign Trans2mcif_rd_req_rdy  =Trans_working & Matrix2mcif_rd_req_rdy;
assign mcif2Trans_rd_resp_vld =Trans_working & mcif2Matrix_rd_resp_vld;
assign mcif2Trans_rd_resp_pd  =Trans_working ? mcif2Matrix_rd_resp_pd:0;

assign Concat2mcif_rd_req_rdy =Concat_working & Matrix2mcif_rd_req_rdy;
assign mcif2Concat_rd_resp_vld=Concat_working & mcif2Matrix_rd_resp_vld;
assign mcif2Concat_rd_resp_pd =Concat_working ? mcif2Matrix_rd_resp_pd:0;

assign Softmax2mcif_rd_req_rdy =Softmax_working & Matrix2mcif_rd_req_rdy;
assign mcif2Softmax_rd_resp_vld=Softmax_working & mcif2Matrix_rd_resp_vld;
assign mcif2Softmax_rd_resp_pd =Softmax_working ? mcif2Matrix_rd_resp_pd:0;

assign ACT2mcif_rd_req_rdy =ACT_working & Matrix2mcif_rd_req_rdy;
assign mcif2ACT_rd_resp_vld=ACT_working & mcif2Matrix_rd_resp_vld;
assign mcif2ACT_rd_resp_pd =ACT_working ? mcif2Matrix_rd_resp_pd:0;

//////////////////////////////////////////////////////////////////////////////////////

assign LN2mcif_wr_req_rdy     =LN_working       &  Matrix2mcif_wr_req_rdy;
assign LN2mcif_wr_rsp_complete=LN_working       &  Matrix2mcif_wr_rsp_complete;

assign Trans2mcif_wr_req_rdy  =Trans_working    &  Matrix2mcif_wr_req_rdy;
assign Trans2mcif_wr_rsp_complete=Trans_working &  Matrix2mcif_wr_rsp_complete;

assign Concat2mcif_wr_req_rdy     =Concat_working & Matrix2mcif_wr_req_rdy;
assign Concat2mcif_wr_rsp_complete=Concat_working & Matrix2mcif_wr_rsp_complete;

assign Softmax2mcif_wr_req_rdy     =Softmax_working & Matrix2mcif_wr_req_rdy;
assign Softmax2mcif_wr_rsp_complete=Softmax_working & Matrix2mcif_wr_rsp_complete;

assign ACT2mcif_wr_req_rdy     =ACT_working & Matrix2mcif_wr_req_rdy;
assign ACT2mcif_wr_rsp_complete=ACT_working & Matrix2mcif_wr_rsp_complete;


always@(*)
    case({ACT_working,Softmax_working,Concat_working,LN_working,Trans_working})
    5'b00001:begin
            Matrix2mcif_rd_req_vld  = Trans2mcif_rd_req_vld;
            Matrix2mcif_rd_req_pd   = Trans2mcif_rd_req_pd;
            mcif2Matrix_rd_resp_rdy = mcif2Trans_rd_resp_rdy;
            Matrix_rd_fifo_pop      = Trans_rd_fifo_pop;
            Matrix2mcif_wr_req_vld  = Trans2mcif_wr_req_vld;
            Matrix2mcif_wr_req_pd   = Trans2mcif_wr_req_pd;
            end
    5'b00010:begin
            Matrix2mcif_rd_req_vld  = LN2mcif_rd_req_vld;
            Matrix2mcif_rd_req_pd   = LN2mcif_rd_req_pd;
            mcif2Matrix_rd_resp_rdy = mcif2LN_rd_resp_rdy;
            Matrix_rd_fifo_pop      = LN_rd_fifo_pop;
            Matrix2mcif_wr_req_vld  = LN2mcif_wr_req_vld;
            Matrix2mcif_wr_req_pd   = LN2mcif_wr_req_pd;
            end
    5'b00100:begin
            Matrix2mcif_rd_req_vld  = Concat2mcif_rd_req_vld;
            Matrix2mcif_rd_req_pd   = Concat2mcif_rd_req_pd;
            mcif2Matrix_rd_resp_rdy = mcif2Concat_rd_resp_rdy;
            Matrix_rd_fifo_pop      = Concat_rd_fifo_pop;
            Matrix2mcif_wr_req_vld  = Concat2mcif_wr_req_vld;
            Matrix2mcif_wr_req_pd   = Concat2mcif_wr_req_pd;
            end
    5'b01000:begin
            Matrix2mcif_rd_req_vld  = Softmax2mcif_rd_req_vld;
            Matrix2mcif_rd_req_pd   = Softmax2mcif_rd_req_pd;
            mcif2Matrix_rd_resp_rdy = mcif2Softmax_rd_resp_rdy;
            Matrix_rd_fifo_pop      = Softmax_rd_fifo_pop;
            Matrix2mcif_wr_req_vld  = Softmax2mcif_wr_req_vld;
            Matrix2mcif_wr_req_pd   = Softmax2mcif_wr_req_pd;
            end
    5'b10000:begin
            Matrix2mcif_rd_req_vld  = ACT2mcif_rd_req_vld;
            Matrix2mcif_rd_req_pd   = ACT2mcif_rd_req_pd;
            mcif2Matrix_rd_resp_rdy = mcif2ACT_rd_resp_rdy;
            Matrix_rd_fifo_pop      = ACT_rd_fifo_pop;
            Matrix2mcif_wr_req_vld  = ACT2mcif_wr_req_vld;
            Matrix2mcif_wr_req_pd   = ACT2mcif_wr_req_pd;
            end
    default:begin
            Matrix2mcif_rd_req_vld  = 0;
            Matrix2mcif_rd_req_pd   = 0;
            mcif2Matrix_rd_resp_rdy = 0;
            Matrix_rd_fifo_pop      = 0;
            Matrix2mcif_wr_req_vld  = 0;
            Matrix2mcif_wr_req_pd   = 0;
            end
    endcase

assign matrix_read_base_addr=feature_in_base_addr;
assign matrix_write_base_addr=feature_out_base_addr;
Matrix_reg u_Matrix_reg
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSB Master
	.csb2csr_req_vld(csb2csr_req_vld),
	.csb2csr_req_rdy(csb2csr_req_rdy),
	.csb2csr_req_pd(csb2csr_req_pd),
	.csr2csb_resp_vld(csr2csb_resp_vld),
	.csr2csb_resp_pd(csr2csb_resp_pd),

	//to datapath
	.LN_stage0_start(LN_stage0_start),
	.LN_stage0_done(LN_stage0_done),
	.LN_stage1and2_start(LN_stage1and2_start),
	.Trans_start(Trans_start),
	.Concat_start(Concat_start),
	.Softmax_start(Softmax_start),
	.Activation_start(ACT_start),
	
	.wt_base_addr(wt_base_addr),
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
	.ch_in_div_Tout(CH_in_div_Tout),
	.h_in(h_in),
	.w_in(w_in),
    .pixel_in(pixel_in),
    .k_factor_scale(k_factor_scale),  
    .bias_scale(bias_scale),      
    .out_scale(out_scale),          
    .recip_CH_int8(recip_CH_int8),      
    .recip_CH_int16(recip_CH_int16),     
    
    .CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_out),
    .w_out(w_out),
    .pixel_out(pixel_out),
    
    .clstoken_base_addr(clstoken_base_addr),
    .wt_num_div_Tout(wt_num_div_Tout),
	//from datapath
	.wdma_done(wdma_done),
	
    .Matrix_In_from_BRAM_mode(Matrix_In_from_BRAM_mode),
    .Matrix_Out_to_BRAM_mode(Matrix_Out_to_BRAM_mode)
);


LN_TOP LN_TOP
(
    .clk(clk),
    .rst_n(rst_n),

	//to datapath
    .LN_stage0_start(LN_stage0_start),
	.LN_stage0_working(LN_stage0_working),
	.LN_stage0_done(LN_stage0_done),
    .LN_stage1and2_start(LN_stage1and2_start),
    .LN_stage1and2_working(LN_stage1and2_working),
    .wdma_done(LN_wdma_done),
    
	.wt_base_addr(wt_base_addr),
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
	.CH_in_div_Tout(CH_in_div_Tout),
	.h_in(h_in),
	.w_in(w_in),
    .pixel_in(pixel_in),
    .k_factor_scale(k_factor_scale),  

    .bias_scale(bias_scale),      
    .out_scale(out_scale),          
    .recip_CH_int8(recip_CH_int8),      
    .recip_CH_int16(recip_CH_int16),     
    
    //rd CMD to MCIF
    .LN2mcif_rd_req_vld(LN2mcif_rd_req_vld),
    .LN2mcif_rd_req_rdy(LN2mcif_rd_req_rdy),
    .LN2mcif_rd_req_pd(LN2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2LN_rd_resp_vld(mcif2LN_rd_resp_vld),
    .mcif2LN_rd_resp_rdy(mcif2LN_rd_resp_rdy),
    .mcif2LN_rd_resp_pd(mcif2LN_rd_resp_pd),
    .LN_rd_fifo_pop(LN_rd_fifo_pop),

    //write path to MCIF
    .LN2mcif_wr_req_vld(LN2mcif_wr_req_vld),
    .LN2mcif_wr_req_rdy(LN2mcif_wr_req_rdy),
    .LN2mcif_wr_req_pd(LN2mcif_wr_req_pd),
    .LN2mcif_wr_rsp_complete(LN2mcif_wr_rsp_complete)
);


Transpose_TOP Trans_TOP
(
    .clk(clk),
    .rst_n(rst_n),

	//to datapath
	.start(Trans_start),
    .working(Trans_working),
    .wdma_done(Trans_wdma_done),
	
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
	
	.CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
	.CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_out),
    .w_out(w_out),
    .pixel_out(pixel_out),
	.wt_num_div_Tout(wt_num_div_Tout),
	
    //rd CMD to MCIF
    .Trans2mcif_rd_req_vld(Trans2mcif_rd_req_vld),
    .Trans2mcif_rd_req_rdy(Trans2mcif_rd_req_rdy),
    .Trans2mcif_rd_req_pd(Trans2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2Trans_rd_resp_vld(mcif2Trans_rd_resp_vld),
    .mcif2Trans_rd_resp_rdy(mcif2Trans_rd_resp_rdy),
    .mcif2Trans_rd_resp_pd(mcif2Trans_rd_resp_pd),
    .Trans_rd_fifo_pop(Trans_rd_fifo_pop),

    //write path to MCIF
    .Trans2mcif_wr_req_vld(Trans2mcif_wr_req_vld),
    .Trans2mcif_wr_req_rdy(Trans2mcif_wr_req_rdy),
    .Trans2mcif_wr_req_pd(Trans2mcif_wr_req_pd),
    .Trans2mcif_wr_rsp_complete(Trans2mcif_wr_rsp_complete)
);


Concat_TOP Concat_TOP
(
    .clk(clk),
    .rst_n(rst_n),

	//to datapath
	.start(Concat_start),
    .working(Concat_working),
    .wdma_done(Concat_wdma_done),

    .clstoken_base_addr(clstoken_base_addr),
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
	.CH_in_div_Tout(CH_in_div_Tout),
    .w_in(w_in),
	
    //rd CMD to MCIF
    .Concat2mcif_rd_req_vld(Concat2mcif_rd_req_vld),
    .Concat2mcif_rd_req_rdy(Concat2mcif_rd_req_rdy),
    .Concat2mcif_rd_req_pd(Concat2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2Concat_rd_resp_vld(mcif2Concat_rd_resp_vld),
    .mcif2Concat_rd_resp_rdy(mcif2Concat_rd_resp_rdy),
    .mcif2Concat_rd_resp_pd(mcif2Concat_rd_resp_pd),
    .Concat_rd_fifo_pop(Concat_rd_fifo_pop),

    //write path to MCIF
    .Concat2mcif_wr_req_vld(Concat2mcif_wr_req_vld),
    .Concat2mcif_wr_req_rdy(Concat2mcif_wr_req_rdy),
    .Concat2mcif_wr_req_pd(Concat2mcif_wr_req_pd),
    .Concat2mcif_wr_rsp_complete(Concat2mcif_wr_rsp_complete)
);

SOFTMAX_TOP Softmax_TOP
(
	.clk(clk),
	.rst_n(rst_n),

    //from datapath
	.start(Softmax_start),   
    .working(Softmax_working),
    .wdma_done(Softmax_wdma_done), 

	//to datapath
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .ch_in(pixel_out),
    .pixel_in(pixel_in),
    .CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_in),
    .w_out(w_in),

    .in_scale(k_factor_scale),
    .out_scale(out_scale),
    
	//rd CMD to MCIF
    .Softmax2mcif_rd_req_vld(Softmax2mcif_rd_req_vld),
    .Softmax2mcif_rd_req_rdy(Softmax2mcif_rd_req_rdy),
    .Softmax2mcif_rd_req_pd(Softmax2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2Softmax_rd_resp_vld(mcif2Softmax_rd_resp_vld),
    .mcif2Softmax_rd_resp_rdy(mcif2Softmax_rd_resp_rdy),
    .mcif2Softmax_rd_resp_pd(mcif2Softmax_rd_resp_pd),
    .Softmax_rd_fifo_pop(Softmax_rd_fifo_pop),

    //write path to MCIF
    .Softmax2mcif_wr_req_vld(Softmax2mcif_wr_req_vld),
    .Softmax2mcif_wr_req_rdy(Softmax2mcif_wr_req_rdy),
    .Softmax2mcif_wr_req_pd(Softmax2mcif_wr_req_pd),
    .Softmax2mcif_wr_rsp_complete(Softmax2mcif_wr_rsp_complete)
);



ACTIVATION_TOP ACTIVATION_TOP
(
	.clk(clk),
	.rst_n(rst_n),

    //from datapath
	.start(ACT_start),   
    .working(ACT_working),
    .wdma_done(ACT_wdma_done), 

	//to datapath
	.feature_in_base_addr(feature_in_base_addr),
	.feature_in_surface_stride(feature_in_surface_stride),
	.feature_in_line_stride(feature_in_line_stride),
	.feature_out_base_addr(feature_out_base_addr),
	.feature_out_surface_stride(feature_out_surface_stride),
	.feature_out_line_stride(feature_out_line_stride),
    .CH_in_div_Tout(CH_in_div_Tout),
    .h_in(h_in),
    .w_in(w_in),
    .pixel_in(pixel_in),
    .CH_out_div_Tout(CH_out_div_Tout),
    .h_out(h_in),
    .w_out(w_in),
    .pixel_out(pixel_in),
    .in_scale(k_factor_scale),
    .out_scale(out_scale),
    
	//rd CMD to MCIF
    .ACT2mcif_rd_req_vld(ACT2mcif_rd_req_vld),
    .ACT2mcif_rd_req_rdy(ACT2mcif_rd_req_rdy),
    .ACT2mcif_rd_req_pd(ACT2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2ACT_rd_resp_vld(mcif2ACT_rd_resp_vld),
    .mcif2ACT_rd_resp_rdy(mcif2ACT_rd_resp_rdy),
    .mcif2ACT_rd_resp_pd(mcif2ACT_rd_resp_pd),
    .ACT_rd_fifo_pop(ACT_rd_fifo_pop),

    //write path to MCIF
    .ACT2mcif_wr_req_vld(ACT2mcif_wr_req_vld),
    .ACT2mcif_wr_req_rdy(ACT2mcif_wr_req_rdy),
    .ACT2mcif_wr_req_pd(ACT2mcif_wr_req_pd),
    .ACT2mcif_wr_rsp_complete(ACT2mcif_wr_rsp_complete)
);
endmodule

