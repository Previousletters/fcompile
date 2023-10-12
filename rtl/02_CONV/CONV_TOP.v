`include "CNN_defines.vh"
/*

                  ************
                  *   ctrl   *
     -------------* register *--------------
     |            *          *			   |
     |            *          *			   |
     |            ************			   |
     |					|		     	   |
     |					|   			   |
***********       ***********         **********
*         *       *         *         *        *
*  conv   *==wt ==*   MAC   *=========*  data  *
* datpath *       *  module *         * center *===out for BN
*         *==dat==*         *    =====*        *
*         *       *         *         *        *
***********       ***********         **********

*/

module CONV_TOP
(
	input clk,
	input rst_n,
	
    output Conv_In_from_BRAM_mode,
    output Conv_Out_to_BRAM_mode,
	//from CSB Master
	input csb2csr_req_vld,
	output csb2csr_req_rdy,
	input [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd,
	output csr2csb_resp_vld,
	output [31:0]csr2csb_resp_pd,

	//rd CMD to MCIF (dat)
	output dma2mcif_dat_rd_req_vld,
	input dma2mcif_dat_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_dat_rd_req_pd,

	//rd response from MCIF (dat)
	input mcif2dma_dat_rd_resp_vld,
	output mcif2dma_dat_rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]mcif2dma_dat_rd_resp_pd,
	output dma_dat_rd_fifo_pop,

	//rd CMD to MCIF (wt)
	output dma2mcif_wt_rd_req_vld,
	input dma2mcif_wt_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_wt_rd_req_pd,

	//rd response from MCIF (wt)
	input mcif2dma_wt_rd_resp_vld,
	output mcif2dma_wt_rd_resp_rdy,
	input [`MAX_WT_DW *`Tout-1:0]mcif2dma_wt_rd_resp_pd,
	output dma_wt_rd_fifo_pop,

	//rd CMD to MCIF (res)
    output dma2mcif_res_rd_req_vld,
    input dma2mcif_res_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_res_rd_req_pd,

    //rd response from MCIF (res)
    input mcif2dma_res_rd_resp_vld,
    output mcif2dma_res_rd_resp_rdy,
    input [`MAX_WT_DW *`Tout-1:0]mcif2dma_res_rd_resp_pd,
    output dma_res_rd_fifo_pop,

	//write path to MCIF
	output conv2mcif_wr_req_vld,
	input conv2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]conv2mcif_wr_req_pd,
	input conv2mcif_wr_rsp_complete,
	
	output [31:0]dat_base_addr,
    output [31:0]dat_out_base_addr
);

//from CSR
wire dma_DAT_start;
wire dma_WT_start;
wire ResAdd_start;
wire buf_init;
wire fsm_dat_start;
wire fsm_wt_start;
wire conv_res_mode;
wire conv_mode;
wire Vit_conv0_mode;

wire [3:0]Tin_factor;// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit    
wire [`log2BRAM_NUM-1:0]dat_buf_num;
wire [`log2_W-1:0]Win;
wire [`log2_H-1:0]Hin;
wire [(`log2_W+`log2_H-1):0]Hin_x_Win;					//Win*Hin
wire [`log2_CH-`log2Tout-1:0]CH_in_div_Tout;		//ceil(CH_in/Tout)
wire [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1;		//ceil(CH_in%Tin /Tout)
wire [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin;					//ceil(CH_in/Tin)

wire [`log2_P-1:0]pad_up;
wire [`log2_P-1:0]pad_left;
wire [`log2_S-1:0]Sx;
wire [`log2_S-1:0]Sy;
wire [`log2_K-1:0]Kx;
wire [`log2_K-1:0]Ky;

wire [`log2_W-1:0]Wout;
wire [(`log2_W+`log2_H-1):0]Hout_x_Wout;   
wire [(`log2_W+`log2_H-1):0]effect_pixel;

wire [`log2_K-1:0]Vit_conv0_Kx;
wire [`log2_S-1:0]Vit_conv0_Sx;
wire [(`log2_W-1):0]Vit_conv0_Win;    
wire [(`log2_W+`log2_H-1):0]Vit_conv0_Hin_x_Win; 

wire [`log2_H-1:0]Hout;			//ceil(Hout*Wout/Tp)
wire [`log2_CH-1:0]CH_out;
wire [`log2_CH-`log2Tout-1:0]CH_out_div_Tout;				//ceil(CH_out/Tout)
wire [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout;			//Wout*Hout/Tout
wire relu,gelu;
wire [`MAX_log2BN_DW-1:0]out_data_width; //'b111 means 8bit, 'b11 means 4bit, 'b1 means 2bit

//wire [31:0]dat_base_addr;
wire [31:0]surface_stride_in;
wire [15:0]line_stride_in;
//wire [31:0]dat_out_base_addr;
wire [31:0]surface_stride_out;
wire [15:0]line_stride_out;
wire [31:0]res_dat_base_addr;
wire [31:0]res_surface_stride_in;
wire [15:0]res_line_stride_in;

wire [31:0]wt_num_div_Tout;					//weight size in bytes
wire [31:0]wt_num_div_Tin;
wire [31:0]wt_base_addr;

wire dma_dat_reuse;
wire dma_wt_reuse;

wire [`log2_scale-1:0]conv_out_shift;
wire res_relu_en;
wire [`log2_scale-1:0]res_in_scale;
wire [`log2_scale-1:0]conv_out_scale;
wire [`log2_scale-1:0]res_out_shift;


//to CSR
wire dma_dat_done;
wire dma_wt_done;
wire fsm_dat_done;
wire fsm_wt_done;

//dat to BUF 
wire dma2buf_DAT_wr_en;
wire [`log2BUF_DEP-1:0]dma2buf_DAT_wr_addr;
wire [`base_Tin*`MAX_DAT_DW-1:0]dma2buf_DAT_wr_data;

//row information to BUF
wire row_num_updt;
wire [`log2_H-1:0]row_num;
wire chin_num_updt;
wire [`log2_CH-1:0]chin_num;

//wt to BUF
wire dma2buf_WT_wr_en;
wire [`log2BUF_DEP-1:0]dma2buf_WT_wr_addr;
wire [`base_Tin*`MAX_WT_DW-1:0]dma2buf_WT_wr_data;

//wt information to BUF
wire wt_addr_updt;
wire [`log2BUF_DEP-1:0]wt_addr_dma;
wire CHinKyKx_max_now_comb;

//for FSM Backpressure
wire credit_vld;

//conv feature addr from FSM
wire feature_data_vld;
wire [`log2_H:0]feature_hin;
wire [`log2_W:0]feature_win;
wire [`log2_CH-1:0]feature_chin;
wire Wout_loop_end_comb;
wire feature_data_rdy;

//wt_addr from FSM
wire wt_addr_vld;
wire wt_addr_rdy;
wire wt_last_comb;
wire [`log2BUF_DEP-1:0]wt_addr;

//dat to CMAC
wire BUF2MAC_dat_vld;
wire [`base_Tin*`MAX_DAT_DW-1:0]BUF2MAC_dat;

//wt to CMAC
wire BUF2MAC_wt_vld;
wire [`base_Tin-1:0]dat_vld_copy;
wire [`base_Tin*`MAX_WT_DW-1:0]BUF2MAC_wt;

//data to CMAC
wire CHinKyKx_max_now;
wire Wout_loop_start;
wire Wout_loop_end;
wire CHinKyKxWout_loop_end;

//wt to CMAC
wire [`log2Tout-1:0]wt_cnt;

//dat to CACC
wire CMAC2CACC_dat_vld;
wire CMAC2CACC_CHinKyKx_max_now;
wire CMAC2CACC_Wout_loop_end;
wire CMAC2CACC_CHinKyKxWout_loop_end;
wire [`Tout*(`MAX_DW2+`base_log2Tin)-1:0]CMAC2CACC_dat;

//conv_wdma
wire wdma_cycle_done;
wire wdma_start;
wire wdma_done;	
wire conv_out_dat_vld;
wire [`MAX_BN_DW*`Tout-1:0]conv_out_dat;

wire res_dat_out_vld;
wire [`MAX_DAT_DW*`Tout-1:0]res_dat_out;
wire res_dat_out_rdy;

CONV_REG CONV_REG_top
(
	.clk(clk),
	.rst_n(rst_n),
    .Conv_In_from_BRAM_mode(Conv_In_from_BRAM_mode),
    .Conv_Out_to_BRAM_mode(Conv_Out_to_BRAM_mode),
    
	//from CSB Master
	.csb2csr_req_vld(csb2csr_req_vld),
	.csb2csr_req_rdy(csb2csr_req_rdy),
	.csb2csr_req_pd(csb2csr_req_pd),
	.csr2csb_resp_vld(csr2csb_resp_vld),
	.csr2csb_resp_pd(csr2csb_resp_pd),

	//to datapath
	.dma_DAT_start(dma_DAT_start),
	.dma_WT_start(dma_WT_start),
	.buf_init(buf_init),
	.fsm_dat_start(fsm_dat_start),
	.fsm_wt_start(fsm_wt_start),
    .conv_res_mode(conv_res_mode),
    .ResAdd_start(ResAdd_start),
    .conv_mode(conv_mode),
    .Vit_conv0_mode(Vit_conv0_mode),
    
    .Tin_factor(Tin_factor),
	.dat_buf_num(dat_buf_num),
	.relu(relu),
	.gelu(gelu),
	.out_data_width(out_data_width),
	
	.Win(Win),
	.Hin(Hin),
	.Hin_x_Win(Hin_x_Win),					//Win*Hin
	.CH_in_div_Tout(CH_in_div_Tout),				//ceil(CH_in/Tout)
	.CH_in_div_Tin(CH_in_div_Tin),				//ceil(CH_in/Tin)
    .Hout_x_Wout_div_Tout(Hout_x_Wout_div_Tout),		//Wout*Hout/Tout
    .effect_pixel(effect_pixel),
    
	.pad_up(pad_up),
	.pad_left(pad_left),
	.Sx(Sx),
	.Sy(Sy),
	.Kx(Kx),
	.Ky(Ky),
	
	.Wout(Wout),
	.Hout_x_Wout(Hout_x_Wout),					//Wout*Hout
    .Vit_conv0_Kx(Vit_conv0_Kx),
    .Vit_conv0_Sx(Vit_conv0_Sx),
    .Vit_conv0_Win(Vit_conv0_Win),    
    .Vit_conv0_Hin_x_Win(Vit_conv0_Hin_x_Win), 
    
	.Hout(Hout),			//ceil(Hout*Wout/Tp)
	.CH_out(CH_out),
	.CH_out_div_Tout(CH_out_div_Tout),				//ceil(CH_out/Tout)
	.CH_in_res_Tin_div_Tout_minus1(CH_in_res_Tin_div_Tout_minus1),			//ceil(CH_in/Tin/Tout)
	
	.dat_base_addr(dat_base_addr),
	.surface_stride_in(surface_stride_in),
	.line_stride_in(line_stride_in),

	.res_dat_base_addr(res_dat_base_addr),
	.res_surface_stride_in(res_surface_stride_in),
	.res_line_stride_in(res_line_stride_in),
		
	.wt_num_div_Tout(wt_num_div_Tout),					//weight size in bytes
	.wt_num_div_Tin(wt_num_div_Tin),
	.wt_base_addr(wt_base_addr),

	.dma_dat_reuse(dma_dat_reuse),
	.dma_wt_reuse(dma_wt_reuse),

	.conv_out_shift(conv_out_shift),
    .res_relu_en(res_relu_en),
    .res_in_scale(res_in_scale),
    .conv_out_scale(conv_out_scale),
    .res_out_shift(res_out_shift),
    
	//from datapath
	.dma_dat_done(dma_dat_done),
	.dma_wt_done(dma_wt_done),
	.fsm_dat_done(fsm_dat_done),
	.fsm_wt_done(fsm_wt_done),
	
	.wdma_cycle_done(wdma_cycle_done),
	.wdma_done(wdma_done),
	.wdma_start(wdma_start),
    .dat_out_base_addr(dat_out_base_addr),
    .surface_stride_out(surface_stride_out),
    .line_stride_out(line_stride_out)
);

CONV_DMA CONV_DMA_top
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.dma_DAT_start(dma_DAT_start),
	.dma_WT_start(dma_WT_start),
    .conv_mode(conv_mode),
    .Vit_conv0_mode(Vit_conv0_mode),
    .Tin_factor(Tin_factor),
    
    .Kx(Kx),
	.Win(Win),
	.Hin(Hin),
	.Hin_x_Win(Hin_x_Win),
	.CH_in_res_Tin_div_Tout_minus1(CH_in_res_Tin_div_Tout_minus1),			//ceil(CH_in/Tin/Tout)
	.CH_in_div_Tin(CH_in_div_Tin),				//ceil(CH_in/Tin)
	.CH_in_div_Tout(CH_in_div_Tout),				//ceil(CH_in/Tout)
	.dat_base_addr(dat_base_addr),
	.surface_stride_in(surface_stride_in),
	.line_stride_in(line_stride_in),
	.dat_buf_num(dat_buf_num),
	.wt_num_div_Tout(wt_num_div_Tout),					//weight size in bytes
	.wt_base_addr(wt_base_addr),
	.dma_dat_reuse(dma_dat_reuse),
	.dma_wt_reuse(dma_wt_reuse),
    .wt_num_div_Tin(wt_num_div_Tin),
    
	//to CSR
	.dma_dat_done(dma_dat_done),
	.dma_wt_done(dma_wt_done),
    
	//rd CMD to MCIF (dat)
	.dma2mcif_dat_rd_req_vld(dma2mcif_dat_rd_req_vld),
	.dma2mcif_dat_rd_req_rdy(dma2mcif_dat_rd_req_rdy),
	.dma2mcif_dat_rd_req_pd(dma2mcif_dat_rd_req_pd),

	//rd response from MCIF (dat)
	.mcif2dma_dat_rd_resp_vld(mcif2dma_dat_rd_resp_vld),
	.mcif2dma_dat_rd_resp_rdy(mcif2dma_dat_rd_resp_rdy),
	.mcif2dma_dat_rd_resp_pd(mcif2dma_dat_rd_resp_pd),
	.dma_dat_rd_fifo_pop(dma_dat_rd_fifo_pop),

	//rd CMD to MCIF (wt)
	.dma2mcif_wt_rd_req_vld(dma2mcif_wt_rd_req_vld),
	.dma2mcif_wt_rd_req_rdy(dma2mcif_wt_rd_req_rdy),
	.dma2mcif_wt_rd_req_pd(dma2mcif_wt_rd_req_pd),

	//rd response from MCIF (wt)
	.mcif2dma_wt_rd_resp_vld(mcif2dma_wt_rd_resp_vld),
	.mcif2dma_wt_rd_resp_rdy(mcif2dma_wt_rd_resp_rdy),
	.mcif2dma_wt_rd_resp_pd(mcif2dma_wt_rd_resp_pd),
	.dma_wt_rd_fifo_pop(dma_wt_rd_fifo_pop),

	//dat to BUF 
	.dma2buf_DAT_wr_en(dma2buf_DAT_wr_en),
	.dma2buf_DAT_wr_addr(dma2buf_DAT_wr_addr),
	.dma2buf_DAT_wr_data(dma2buf_DAT_wr_data),

	//row information to BUF
	.row_num_updt(row_num_updt),
	.row_num(row_num),
    .chin_num_updt(chin_num_updt),
    .chin_num(chin_num),
    
	//wt to BUF
	.dma2buf_WT_wr_en(dma2buf_WT_wr_en),
	.dma2buf_WT_wr_addr(dma2buf_WT_wr_addr),
	.dma2buf_WT_wr_data(dma2buf_WT_wr_data),

	//wt information to BUF
	.wt_addr_updt(wt_addr_updt),
	.wt_addr_dma(wt_addr_dma)
);

CONV_BUF CONV_BUF_top
(
	.clk(clk),
	.rst_n(rst_n),
    .conv_mode(conv_mode),
    
	//for Backpressure
	.CHinKyKx_max_now_comb(CHinKyKx_max_now_comb),
	.credit_vld(credit_vld),

	//from CSR
	.fsm_logic_init(buf_init),
	.dat_buf_num(dat_buf_num),
	.Win(Vit_conv0_Win),
	.Hin(Hin),
	.Hin_x_Win(Vit_conv0_Hin_x_Win),					//Win*Hin
	.dma_dat_reuse(dma_dat_reuse),
	.dma_wt_reuse(dma_wt_reuse),

	//dat from DMA 
	.dma2buf_DAT_wr_en(dma2buf_DAT_wr_en),
	.dma2buf_DAT_wr_addr(dma2buf_DAT_wr_addr),
	.dma2buf_DAT_wr_data(dma2buf_DAT_wr_data),

	//row information from DMA
	.row_num_updt(row_num_updt),
	.row_num(row_num),
    .chin_num_updt(chin_num_updt),
    .chin_num(chin_num),
    
	//wt from DMA
	.dma2buf_WT_wr_en(dma2buf_WT_wr_en),
	.dma2buf_WT_wr_addr(dma2buf_WT_wr_addr),
	.dma2buf_WT_wr_data(dma2buf_WT_wr_data),

	//wt information from DMA
	.wt_addr_updt(wt_addr_updt),
	.wt_addr_dma(wt_addr_dma),
	
	//conv feature from FSM
	.feature_data_vld(feature_data_vld),
	.feature_hin(feature_hin),
	.feature_win(feature_win),
	.feature_chin(feature_chin),
	.Wout_loop_end_comb(Wout_loop_end_comb),
	.feature_data_rdy(feature_data_rdy),
    
	//wt_addr from FSM
	.wt_addr_vld(wt_addr_vld),
	.wt_addr_rdy(wt_addr_rdy),
	.wt_last_comb(wt_last_comb),
	.wt_addr(wt_addr),
    
	//dat to FSM
	.BUF2MAC_dat_vld(BUF2MAC_dat_vld),
	.dat_vld_copy(dat_vld_copy),
	.BUF2MAC_dat(BUF2MAC_dat),

	//wt to FSM
	.BUF2MAC_wt_vld(BUF2MAC_wt_vld),
	.BUF2MAC_wt(BUF2MAC_wt)
);

CONV_FSM CONV_FSM_top
(
	.clk(clk),
	.rst_n(rst_n),
    
	//from CSR
	.fsm_dat_start(fsm_dat_start),
	.fsm_wt_start(fsm_wt_start),
    .conv_mode(conv_mode),
    
	.pad_up(pad_up),
	.pad_left(pad_left),
	.Sx(Vit_conv0_Sx),
	.Sy(Sy),
	.Kx(Vit_conv0_Kx),
	.Ky(Ky),
	.CH_in_div_Tin(CH_in_div_Tin),				//ceil(CH_in/Tin)
	.CH_out(CH_out),
	.CH_out_div_Tout(CH_out_div_Tout),				//ceil(CH_out/Tout)
	.Wout(Wout),
	.Hout_x_Wout(Hout_x_Wout),

	.Hout(Hout),			//ceil(Hout*Wout/Tp)
	.Hin(Hin),
	.Win(Vit_conv0_Win),
    .Hout_x_Wout_div_Tout(Hout_x_Wout_div_Tout),		//Wout*Hout/Tout
    
	//to CSR
	.fsm_dat_done(fsm_dat_done),
	.fsm_wt_done(fsm_wt_done),
    
	//to BUF for Backpressure
	.CHinKyKx_max_now_comb(CHinKyKx_max_now_comb),
	
	//conv feature addr to BUF
	.feature_data_vld(feature_data_vld),
	.feature_hin(feature_hin),
	.feature_win(feature_win),
	.feature_chin(feature_chin),
	.Wout_loop_end_comb(Wout_loop_end_comb),
	.feature_data_rdy(feature_data_rdy),
    
	//wt_addr to BUF
	.wt_addr_vld(wt_addr_vld),
	.wt_addr(wt_addr),
	.wt_last_comb(wt_last_comb),
	.wt_addr_rdy(wt_addr_rdy),

	//data to CMAC
	.CHinKyKx_max_now(CHinKyKx_max_now),
	.Wout_loop_start(Wout_loop_start),
	.Wout_loop_end(Wout_loop_end),
	.CHinKyKxWout_loop_end(CHinKyKxWout_loop_end),

	//wt to CMAC
	.wt_cnt(wt_cnt)
);

`ifdef Column_Systolic_MAC
    Systolic_CONV_MAC_top CONV_MAC_top
    (
        .clk(clk),
        .rst_n(rst_n),
        .Tin_factor(Tin_factor),
        .relu(relu),
        .out_data_width(out_data_width),
        
        //data from FSM
        .dat_vld(BUF2MAC_dat_vld),
        .dat_vld_copy(dat_vld_copy),
        .dat(BUF2MAC_dat),
        .CHinKyKx_max_now(CHinKyKx_max_now),
        .Wout_loop_start(Wout_loop_start),
        .Wout_loop_end(Wout_loop_end),
        .CHinKyKxWout_loop_end(CHinKyKxWout_loop_end),
        
        //wt from FSM
        .wt_vld(BUF2MAC_wt_vld),
        .wt(BUF2MAC_wt),
        .wt_cnt(wt_cnt),
        
        //from CSR
        .conv_out_shift(conv_out_shift),
        
        //dat to SDP
        .conv_out_dat_vld(conv_out_dat_vld),
        .conv_out_dat(conv_out_dat)
    );
`else
    Parallel_CONV_MAC_top CONV_MAC_top
    (
        .clk(clk),
        .rst_n(rst_n),
        .Tin_factor(Tin_factor),
        .relu(relu),
        .out_data_width(out_data_width),
        
        //data from FSM
        .dat_vld(BUF2MAC_dat_vld),
        .dat(BUF2MAC_dat),
        .CHinKyKx_max_now(CHinKyKx_max_now),
        .Wout_loop_start(Wout_loop_start),
        .Wout_loop_end(Wout_loop_end),
        .CHinKyKxWout_loop_end(CHinKyKxWout_loop_end),
        
        //wt from FSM
        .wt_vld(BUF2MAC_wt_vld),
        .wt(BUF2MAC_wt),
        .wt_cnt(wt_cnt),
        
        //from CSR
        .conv_out_shift(conv_out_shift),
        
        //dat to SDP
        .conv_out_dat_vld(conv_out_dat_vld),
        .conv_out_dat(conv_out_dat)
    );

`endif

wire [`MAX_DAT_DW *`Tout-1:0]gelu_out;
wire gelu_out_vld;
Gelu_Top u_Gelu_Top
(
    .clk(clk),
    .rst_n(rst_n),
    .gelu(gelu),
    .in_scale(res_in_scale[2:0]),
    .dat_in(conv_out_dat),
    .dat_out(gelu_out),
    .in_vld(conv_out_dat_vld),
    .out_vld(gelu_out_vld)
);

wire [`MAX_DAT_DW *`Tout-1:0]conv_out_delay;
generate_dat_shift #
(
    .DATA_WIDTH(`MAX_DAT_DW *`Tout),
    .DEPTH(`Gelu_delay)
)dat_out_
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in(conv_out_dat),
    .data_out(conv_out_delay)
);

      
wire hs_fifo_cacc_out_data_out_vld;
wire [`MAX_BN_DW *`Tout-1:0]hs_fifo_cacc_out_data_out_pd;
wire hs_fifo_cacc_out_data_out_rdy;
assign credit_vld=hs_fifo_cacc_out_data_out_vld&hs_fifo_cacc_out_data_out_rdy;

hs_fifo #
(
	.DATA_WIDTH(`MAX_BN_DW *`Tout),
	.FIFO_DEPTH(`CACC_OUT_DEP),
	.log2_FIFO_DEPTH(`log2_CACC_OUT_DEP)
)hs_fifo_cacc_out
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(gelu_out_vld),
    .data_in(gelu?gelu_out:conv_out_delay),
    .data_in_rdy(),
    
    .data_out_vld(hs_fifo_cacc_out_data_out_vld),
    .data_out(hs_fifo_cacc_out_data_out_pd),
    .data_out_rdy(hs_fifo_cacc_out_data_out_rdy)
);

wire wdma_data_in_vld=conv_res_mode?res_dat_out_vld:hs_fifo_cacc_out_data_out_vld;
wire [`MAX_DAT_DW *`Tout-1:0]wdma_data_in=conv_res_mode?res_dat_out:hs_fifo_cacc_out_data_out_pd;
wire wdma_data_in_rdy;

wire res_data_in_vld=conv_res_mode?hs_fifo_cacc_out_data_out_vld:0;
wire [`MAX_BN_DW *`Tout-1:0]res_data_in=conv_res_mode?hs_fifo_cacc_out_data_out_pd:0;
wire res_data_in_rdy;
assign hs_fifo_cacc_out_data_out_rdy=conv_res_mode?res_data_in_rdy:wdma_data_in_rdy;
assign res_dat_out_rdy=conv_res_mode?wdma_data_in_rdy:0;

Conv_ResAdd_TOP CONV_RES
(
	 .clk(clk),
	 .rst_n(rst_n),
     .start(ResAdd_start),
     .conv_res_mode(conv_res_mode),
     
	//from Conv 
	 .conv_out_dat_vld(res_data_in_vld),
	 .conv_out_dat(res_data_in),
	 .conv_out_dat_rdy(res_data_in_rdy),

	//from REG
	 .Wout(Wout),
	 .Hout(Hout),
	 .CH_out_div_Tout(CH_out_div_Tout),
	 .ResAdd_dat_base_addr(res_dat_base_addr),
     .ResAdd_surface_stride_in(res_surface_stride_in),
     .ResAdd_line_stride_in(res_line_stride_in),
	 .ResAdd_relu_en(res_relu_en),                 //bn_bias_relu(), bn_wt_relu
	 .ResAdd_in_scale(res_in_scale),
	 .Conv_out_scale(conv_out_scale),
	 .ResAdd_out_shift(res_out_shift),

	//rd CMD to MCIF (res)
     .dma2mcif_ResAdd_rd_req_vld(dma2mcif_res_rd_req_vld),
     .dma2mcif_ResAdd_rd_req_rdy(dma2mcif_res_rd_req_rdy),
     .dma2mcif_ResAdd_rd_req_pd(dma2mcif_res_rd_req_pd),

	//from ResAdd_RD
     .mcif2ResAdd_rd_resp_vld(mcif2dma_res_rd_resp_vld),
     .mcif2ResAdd_rd_resp_rdy(mcif2dma_res_rd_resp_rdy),
     .mcif2ResAdd_rd_resp_pd(mcif2dma_res_rd_resp_pd),
     .ResAdd_rd_fifo_pop(dma_res_rd_fifo_pop),
    
	//to wdma
	 .res_out_dat_vld(res_dat_out_vld),
	 .res_out_dat(res_dat_out),
	 .res_out_dat_rdy(res_dat_out_rdy)
);


CONV_WDMA CONV_WDMA
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.wdma_start(wdma_start),
	.w_wdma(Wout),
	.h_wdma(Hout),
    .effect_pixel(effect_pixel),
	.ch_wdma_div_Tout(CH_out_div_Tout),	//ceil(ch_wdma/Tout)
	.feature_wdma_base_addr(dat_out_base_addr),
	.feature_wdma_surface_stride(surface_stride_out),
	.feature_wdma_line_stride(line_stride_out),
	
	//to CSR
	.wdma_done(wdma_done),
    .wdma_cycle_done(wdma_cycle_done),
	
	//from core_nonlinear
	.dat_in_vld(wdma_data_in_vld),
	.dat_in_pd(wdma_data_in),
	.dat_in_rdy(wdma_data_in_rdy),

	//write path to MCIF
	.conv2mcif_wr_req_vld(conv2mcif_wr_req_vld),
	.conv2mcif_wr_req_rdy(conv2mcif_wr_req_rdy),
	.conv2mcif_wr_req_pd(conv2mcif_wr_req_pd),
	.conv2mcif_wr_rsp_complete(conv2mcif_wr_rsp_complete)
);

endmodule
