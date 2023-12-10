`include "CNN_defines.vh"

module CONV_REG
(
	input clk,
	input rst_n,

    output Conv_In_from_BRAM_mode,
    output Conv_Out_to_BRAM_mode,

	//from CSB Master
	input csb2csr_req_vld,
	output csb2csr_req_rdy,
	input [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd,
	output reg csr2csb_resp_vld,
	output reg [31:0]csr2csb_resp_pd,

	//to datapath
	output dma_DAT_start,//reg 0			
	output dma_WT_start,//reg 0	
	output buf_init,//reg 0
	output fsm_dat_start,//reg 0
	output fsm_wt_start,//reg 0
    
    output conv_mode,
	output conv_res_mode,
	output [3:0]Tin_factor,//reg 2
	output [`log2BRAM_NUM-1:0]dat_buf_num,//reg 3
	
	output [`log2_W-1:0]Win,//reg 4
	output [`log2_H-1:0]Hin,//reg 5
	output [(`log2_W+`log2_H-1):0]Hin_x_Win,//reg 6					//Win*Hin
	output [(`log2_W+`log2_H-1):0]effect_pixel,//reg 48
	output [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,//reg 7		//ceil(CH_in/Tout)
	output [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin,//reg 8				//ceil(CH_in/Tin)
	output [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1,	//reg31		//ceil(CH_in%Tin/Tout)
	output [`log2_P-1:0]pad_up,//reg 9
	output [`log2_P-1:0]pad_left,//reg 10
	output [`log2_S-1:0]Sx,//reg 11
	output [`log2_S-1:0]Sy,//reg 12
	output [`log2_K-1:0]Kx,//reg 13
	output [`log2_K-1:0]Ky,//reg 14
	
	output [`log2_W-1:0]Wout,//reg 15
	output [(`log2_W+`log2_H-1):0]Hout_x_Wout,//reg 16	
    
	output [`log2_H-1:0]Hout,//reg 17			//Hout
	output [`log2_CH-1:0]CH_out,//reg 18
	output [(`log2_CH-`log2Tout-1):0]CH_out_div_Tout,//reg 19				//ceil(CH_out/Tout)
	output [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout,//reg 33			//Wout*Hout/Tout

	output [31:0]dat_base_addr,//reg 20
	output [31:0]surface_stride_in,//reg 21
	output [15:0]line_stride_in,//reg 22
	
	output [31:0]wt_num_div_Tout,//reg 23					//weight size in bytes
	output [31:0]wt_base_addr,//reg 24
	output [31:0]wt_num_div_Tin,//reg 32
	output dma_dat_reuse,//reg 25
	output dma_wt_reuse,//reg 25

	output [`log2_scale-1:0]conv_out_shift,//reg 26

	// from datapath
	input dma_dat_done,//reg 1
	input dma_wt_done,//reg 1
	input fsm_dat_done,//reg 1
	input fsm_wt_done,//reg 1
	
    // from write DMA
    input wdma_cycle_done,
	input wdma_done,
	output wdma_start,
	output [31:0]dat_out_base_addr,//reg 34
	output [31:0]surface_stride_out,//reg 35
	output [15:0]line_stride_out,//reg 36
	output relu, //reg38
	output gelu,
	
	output ResAdd_start,
    output [31:0]res_dat_base_addr,//reg 40
    output [31:0]res_surface_stride_in,//reg 41
    output [15:0]res_line_stride_in,//reg 42
    output res_relu_en,//reg 43
    output [`log2_scale-1:0]res_in_scale,//reg 44
    output [`log2_scale-1:0]conv_out_scale,//reg 45
    output [`log2_scale-1:0]res_out_shift,//reg 46
    
    output Vit_conv0_mode,//reg 47
    output reg [`log2_K-1:0]Vit_conv0_Kx,
    output reg [`log2_S-1:0]Vit_conv0_Sx,
    output reg [(`log2_W-1):0]Vit_conv0_Win,    
    output reg [(`log2_W+`log2_H-1):0]Vit_conv0_Hin_x_Win, 
    
	output [`MAX_log2BN_DW-1:0]out_data_width //'b111 means 8bit, 'b11 means 4bit, 'b1 means 2bit
);

wire [`log2CSR_REG_NUM:0]req_addr=csb2csr_req_pd[`log2CSR_REG_NUM:0];
wire [31:0]wdata=csb2csr_req_pd[(`log2CSR_REG_NUM+32):(`log2CSR_REG_NUM+1)];
wire wr_rd=csb2csr_req_pd[`log2CSR_REG_NUM+32+1];

assign csb2csr_req_rdy=1'b1;


always @(posedge clk or negedge rst_n)
if(~rst_n)
	csr2csb_resp_vld<=1'b0;
else
	csr2csb_resp_vld<=csb2csr_req_vld&csb2csr_req_rdy&(~wr_rd);

reg dma_dat_done_r;//reg 1
reg dma_wt_done_r;//reg 1
reg fsm_dat_done_r;//reg 1
reg fsm_wt_done_r;//reg 1
reg wdma_done_r;//reg 37

(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_r;//reg 2
reg [`log2BRAM_NUM-1:0]dat_buf_num_r;//reg 3
reg [`log2_W-1:0]Win_r;//reg 4
reg [`log2_H-1:0]Hin_r;//reg 5
reg [(`log2_W+`log2_H-1):0]Hin_x_Win_r;//reg 6					//Win*Hin
reg [`log2_CH-`log2Tout-1:0]CH_in_div_Tout_r;//reg 7		//ceil(CH_in/Tout)
reg [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin_r;//reg 8					//ceil(CH_in/Tin)
reg [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1_r;//reg 31
reg [`log2_P-1:0]pad_up_r;//reg 9
reg [`log2_P-1:0]pad_left_r;//reg 10
reg [`log2_S-1:0]Sx_r;//reg 11
reg [`log2_S-1:0]Sy_r;//reg 12
reg [`log2_K-1:0]Kx_r;//reg 13
reg [`log2_K-1:0]Ky_r;//reg 14
reg [`log2_W-1:0]Wout_r;//reg 15
reg [(`log2_W+`log2_H-1):0]Hout_x_Wout_r;//reg 16					//Wout*Hout
reg [`log2_H-1:0]Hout_r;//reg 17			//Hout
reg [`log2_CH-1:0]CH_out_r;//reg 18
reg [`log2_CH-`log2Tout-1:0]CH_out_div_Tout_r;//reg 19				//ceil(CH_out/Tout)
reg [31:0]dat_base_addr_r;//reg 20
reg [31:0]surface_stride_in_r;//reg 21
reg [15:0]line_stride_in_r;//reg 22
reg [31:0]wt_num_div_Tout_r;//reg 23					//weight size in bytes
reg [31:0]wt_num_div_Tin_r;//reg32
reg [31:0]wt_base_addr_r;//reg 24
reg dma_dat_reuse_r;//reg 25
reg dma_wt_reuse_r;//reg 25
reg [`log2_scale-1:0]conv_out_shift_r;//reg 26
reg [31:0]dma_dat_performance_cnt_r;//reg27
reg [31:0]dma_wt_performance_cnt_r;//reg28
reg [31:0]fsm_dat_performance_cnt_r;//reg29
reg [31:0]csw_wt_performance_cnt_r;//reg30
reg [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout_r;//reg 33			//Wout*Hout/Tout
reg [31:0]dat_out_base_addr_r;//reg 34
reg [31:0]surface_stride_out_r;//reg 35
reg [15:0]line_stride_out_r;//reg 36
reg relu_r;//reg 38
reg gelu_r;
reg [`MAX_log2BN_DW-1:0]out_data_width_r; //reg 39
reg [31:0]res_dat_base_addr_r;//reg 40
reg [31:0]res_surface_stride_in_r;//reg 41
reg [15:0]res_line_stride_in_r;//reg 42
reg res_relu_en_r;//reg 43
reg [`log2_scale-1:0]res_in_scale_r;//reg 44
reg [`log2_scale-1:0]conv_out_scale_r;//reg 45
reg [`log2_scale-1:0]res_out_shift_r;//reg 46
reg Conv_In_from_BRAM_mode_r;//reg 47
reg Conv_Out_to_BRAM_mode_r;//reg 47
reg [(`log2_W+`log2_H-1):0]effect_pixel_r;//reg 48

always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    Tin_factor_r<='b0;//reg 2
	dat_buf_num_r<='b0;//reg 3
	Win_r<='b0;//reg 4
	Hin_r<='b0;//reg 5
	Hin_x_Win_r<='b0;//reg 6
	CH_in_div_Tout_r<='b0;//reg 7
	CH_in_div_Tin_r<='b0;//reg 8
	CH_in_res_Tin_div_Tout_minus1_r<='b0;//reg31
	pad_up_r<='b0;//reg 9
	pad_left_r<='b0;//reg 10
	Sx_r<='b0;//reg 11
	Sy_r<='b0;//reg 12
	Kx_r<='b0;//reg 13
	Ky_r<='b0;//reg 14
	Wout_r<='b0;//reg 15
	Hout_x_Wout_r<='b0;//reg 16
	Hout_r<='b0;//reg 17
	CH_out_r<='b0;//reg 18
	CH_out_div_Tout_r<='b0;//reg 19
	dat_base_addr_r<='b0;//reg 20
	surface_stride_in_r<='b0;//reg 21
	line_stride_in_r<='b0;//reg 22
	wt_num_div_Tout_r<='b0;//reg 23
	wt_num_div_Tin_r<='b0;//reg32
	wt_base_addr_r<='b0;//reg 24
	dma_dat_reuse_r<='b0;//reg 25
	dma_wt_reuse_r<='b0;//reg 25
	conv_out_shift_r<='d0;//reg 26
	Hout_x_Wout_div_Tout_r<='b0;//reg 33			//Wout*Hout/Tout
	dat_out_base_addr_r<='b0;//reg 34
	surface_stride_out_r<='b0;//reg 35
	line_stride_out_r<='b0;//reg 36
	relu_r<='b0;//reg 38
    gelu_r<='b0;//reg 38
	out_data_width_r<=0;//reg 39
	res_dat_base_addr_r<=0;//reg 40
    res_surface_stride_in_r<=0;//reg 41
    res_line_stride_in_r<=0;//reg 42
    res_relu_en_r<=0;//reg 43
    res_in_scale_r<=0;//reg 44
    conv_out_scale_r<=0;//reg 45
    res_out_shift_r<=0;//reg 46
    Conv_In_from_BRAM_mode_r<=0;//reg 47
    Conv_Out_to_BRAM_mode_r<=0;//reg 47
    effect_pixel_r<=0;//reg 48
end
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd)
	begin
		case(req_addr)
		    2:Tin_factor_r<=wdata;
			3:dat_buf_num_r<=wdata;
			4:Win_r<=wdata;
			5:Hin_r<=wdata;
			6:Hin_x_Win_r<=wdata;
			7:CH_in_div_Tout_r<=wdata;
			8:CH_in_div_Tin_r<=wdata;
			31:CH_in_res_Tin_div_Tout_minus1_r<=wdata;
			9:pad_up_r<=wdata;
			10:pad_left_r<=wdata;
			11:Sx_r<=wdata;
			12:Sy_r<=wdata;
			13:Kx_r<=wdata;
			14:Ky_r<=wdata;
			15:Wout_r<=wdata;
			16:Hout_x_Wout_r<=wdata;
			17:Hout_r<=wdata;
			18:CH_out_r<=wdata;
			19:CH_out_div_Tout_r<=wdata;
			20:dat_base_addr_r<=wdata;
			21:surface_stride_in_r<=wdata;
			22:line_stride_in_r<=wdata;
			23:wt_num_div_Tout_r<=wdata;
			32:wt_num_div_Tin_r<=wdata;
			24:wt_base_addr_r<=wdata;
			25:{dma_wt_reuse_r,dma_dat_reuse_r}<=wdata;
			26:conv_out_shift_r<=wdata;
			33:Hout_x_Wout_div_Tout_r<=wdata;//reg 33			//Wout*Hout/Tout
			34:dat_out_base_addr_r<=wdata;//reg 34
			35:surface_stride_out_r<=wdata;//reg 35
			36:line_stride_out_r<=wdata;//reg 36
			38:begin gelu_r<=wdata[1]; relu_r<=wdata[0];end//reg 38
			39:out_data_width_r<=wdata;//reg 39
			40:res_dat_base_addr_r<=wdata;//reg 40
            41:res_surface_stride_in_r<=wdata;//reg 41
            42:res_line_stride_in_r<=wdata;//reg 42
            43:res_relu_en_r<=wdata;
            44:res_in_scale_r<=wdata;
            45:conv_out_scale_r<=wdata;
            46:res_out_shift_r<=wdata;
            47:begin Conv_Out_to_BRAM_mode_r<=wdata[1]; Conv_In_from_BRAM_mode_r<=wdata[0];end
            48:effect_pixel_r<=wdata;
		endcase
	end

always @(posedge clk or negedge rst_n)
if(~rst_n)
	csr2csb_resp_pd<=32'b0;
else
	if(csb2csr_req_vld&csb2csr_req_rdy&(~wr_rd))
	begin
		case(req_addr)
			1:csr2csb_resp_pd<={28'b0,fsm_wt_done_r,fsm_dat_done_r,dma_wt_done_r,dma_dat_done_r};
			2:csr2csb_resp_pd<={{(32-4){1'b0}},Tin_factor_r};
			3:csr2csb_resp_pd<={{(32-`log2BRAM_NUM){1'b0}},dat_buf_num_r};
			4:csr2csb_resp_pd<={{(32-`log2_W){1'b0}},Win_r};
			5:csr2csb_resp_pd<={{(32-`log2_H){1'b0}},Hin_r};
			6:csr2csb_resp_pd<={{(32-(`log2_W+`log2_H)){1'b0}},Hin_x_Win_r};
			7:csr2csb_resp_pd<={{(32-`log2_CH+`log2Tout){1'b0}},CH_in_div_Tout_r};
			8:csr2csb_resp_pd<={{(32-`log2_CH+`base_log2Tin){1'b0}},CH_in_div_Tin_r};
			31:csr2csb_resp_pd<={{(32-`Max_log2Tin_minus_log2Tout-1){1'b0}},CH_in_res_Tin_div_Tout_minus1_r};
			9:csr2csb_resp_pd<={{(32-`log2_P){1'b0}},pad_up_r};
			10:csr2csb_resp_pd<={{(32-`log2_P){1'b0}},pad_left_r};
			11:csr2csb_resp_pd<={{(32-`log2_S){1'b0}},Sx_r};
			12:csr2csb_resp_pd<={{(32-`log2_S){1'b0}},Sy_r};
			13:csr2csb_resp_pd<={{(32-`log2_K){1'b0}},Kx_r};
			14:csr2csb_resp_pd<={{(32-`log2_K){1'b0}},Ky_r};
			15:csr2csb_resp_pd<={{(32-`log2_W){1'b0}},Wout_r};
			16:csr2csb_resp_pd<={{(32-(`log2_W+`log2_H)){1'b0}},Hout_x_Wout_r};
			17:csr2csb_resp_pd<={{(32-`log2_H){1'b0}},Hout_r};
			18:csr2csb_resp_pd<={{(32-`log2_CH){1'b0}},CH_out_r};
			19:csr2csb_resp_pd<={{(32-`log2_CH+`log2Tout){1'b0}},CH_out_div_Tout_r};
			20:csr2csb_resp_pd<=dat_base_addr_r;
			21:csr2csb_resp_pd<=surface_stride_in_r;
			22:csr2csb_resp_pd<=line_stride_in_r;
			23:csr2csb_resp_pd<=wt_num_div_Tout_r;
			32:csr2csb_resp_pd<=wt_num_div_Tin_r;
			24:csr2csb_resp_pd<=wt_base_addr_r;
			25:csr2csb_resp_pd<={30'b0,dma_wt_reuse_r,dma_dat_reuse_r};
			26:csr2csb_resp_pd<={{(32-`log2_scale){1'b0}},conv_out_shift_r};
			27:csr2csb_resp_pd<=dma_dat_performance_cnt_r;
			28:csr2csb_resp_pd<=dma_wt_performance_cnt_r;
			29:csr2csb_resp_pd<=fsm_dat_performance_cnt_r;
			30:csr2csb_resp_pd<=csw_wt_performance_cnt_r;
			33:csr2csb_resp_pd<={{(32-(`log2_W+`log2_H-`log2Tout)){1'b0}},Hout_x_Wout_div_Tout_r};
			34:csr2csb_resp_pd<=dat_out_base_addr_r;
			35:csr2csb_resp_pd<=surface_stride_out_r;
			36:csr2csb_resp_pd<=line_stride_out_r;
			37:csr2csb_resp_pd<={31'b0,wdma_done_r};
			38:csr2csb_resp_pd<={30'b0,gelu_r,relu_r};
			39:csr2csb_resp_pd<={{(32-(`MAX_log2BN_DW)){1'b0}},out_data_width_r};
			40:csr2csb_resp_pd<=res_dat_base_addr_r;
            41:csr2csb_resp_pd<=res_surface_stride_in_r;
            42:csr2csb_resp_pd<=res_line_stride_in_r;
            43:csr2csb_resp_pd<={31'b0,res_relu_en_r};
            44:csr2csb_resp_pd<={{(32-`log2_scale){1'b0}},res_in_scale_r};
            45:csr2csb_resp_pd<={{(32-`log2_scale){1'b0}},conv_out_scale_r};
            46:csr2csb_resp_pd<={{(32-`log2_scale){1'b0}},res_out_shift_r};
            47:csr2csb_resp_pd<={30'b0,Conv_Out_to_BRAM_mode_r,Conv_In_from_BRAM_mode_r};
            48:csr2csb_resp_pd<={{(32-(`log2_W+`log2_H)){1'b0}},effect_pixel_r};
			default:csr2csb_resp_pd<=32'b0;
		endcase
	end

assign dma_DAT_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[0];//reg 0			
assign dma_WT_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[1];//reg 0		
assign buf_init=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[2];//reg 0
assign fsm_dat_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[3];//reg 0
assign fsm_wt_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[4];//reg 0
assign wdma_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[0];//reg 0
assign ResAdd_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[5];//reg 0
wire Vit_conv0_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[6];//reg 0


reg Vit_conv0_mode_r;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	Vit_conv0_mode_r<=1'b0;
else
	if(Vit_conv0_start)
		Vit_conv0_mode_r<=1'b1;
	else
		if(wdma_done)
			Vit_conv0_mode_r<=1'b0;
assign Vit_conv0_mode=Vit_conv0_mode_r;

reg conv_res_mode_r;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_res_mode_r<=1'b0;
else
	if(ResAdd_start)
		conv_res_mode_r<=1'b1;
	else
		if(wdma_done)
			conv_res_mode_r<=1'b0;
assign conv_res_mode=conv_res_mode_r;

reg conv_mode_r;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_mode_r<=1'b0;
else
	if(dma_DAT_start)
		conv_mode_r<=1'b1;
	else
		if(wdma_done)
			conv_mode_r<=1'b0;
assign conv_mode=conv_mode_r;
			
always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_dat_done_r<=1'b0;
else
	if(dma_DAT_start)
		dma_dat_done_r<=1'b0;
	else
		if(dma_dat_done|dma_dat_reuse_r)
			dma_dat_done_r<=1'b1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_wt_done_r<=1'b0;
else
	if(dma_WT_start)
		dma_wt_done_r<=1'b0;
	else
		if(dma_wt_done|dma_wt_reuse_r)
			dma_wt_done_r<=1'b1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	wdma_done_r<=1'b0;
else
	if(wdma_start)
		wdma_done_r<=1'b0;
	else
		if(wdma_done)
			wdma_done_r<=1'b1;
			
always @(posedge clk or negedge rst_n)
if(~rst_n)
	fsm_dat_done_r<=1'b0;
else
	if(fsm_dat_start)
		fsm_dat_done_r<=1'b0;
	else
		if(fsm_dat_done)	
			fsm_dat_done_r<=1'b1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	fsm_wt_done_r<=1'b0;
else
	if(fsm_wt_start)
		fsm_wt_done_r<=1'b0;
	else
		if(fsm_wt_done)	
			fsm_wt_done_r<=1'b1;

reg dma_DAT_Working;
reg dma_WT_Working;
reg fsm_DAT_Working;
reg fsm_WT_Working;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_DAT_Working<=1'b0;
else
	if(dma_DAT_start)
		dma_DAT_Working<=1'b1;
	else
		if(dma_dat_done)
			dma_DAT_Working<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_WT_Working<=1'b0;
else
	if(dma_WT_start)
		dma_WT_Working<=1'b1;
	else
		if(dma_wt_done)
			dma_WT_Working<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	fsm_DAT_Working<=1'b0;
else
	if(fsm_dat_start)
		fsm_DAT_Working<=1'b1;
	else
		if(fsm_dat_done)
			fsm_DAT_Working<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	fsm_WT_Working<=1'b0;
else
	if(fsm_wt_start)
		fsm_WT_Working<=1'b1;
	else
		if(fsm_wt_done)
			fsm_WT_Working<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_dat_performance_cnt_r<=32'd0;//reg27
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==27))
		dma_dat_performance_cnt_r<=wdata[31:0];
	else
		if(dma_DAT_Working)
			dma_dat_performance_cnt_r<=dma_dat_performance_cnt_r+1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	dma_wt_performance_cnt_r<=32'd0;//reg28
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==28))
		dma_wt_performance_cnt_r<=wdata[31:0];
	else
		if(dma_WT_Working)
			dma_wt_performance_cnt_r<=dma_wt_performance_cnt_r+1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	fsm_dat_performance_cnt_r<=32'd0;//reg29
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==29))
		fsm_dat_performance_cnt_r<=wdata[31:0];
	else
		if(fsm_DAT_Working)
			fsm_dat_performance_cnt_r<=fsm_dat_performance_cnt_r+1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	csw_wt_performance_cnt_r<=32'd0;//reg30
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==30))
		csw_wt_performance_cnt_r<=wdata[31:0];
	else
		if(fsm_WT_Working)
			csw_wt_performance_cnt_r<=csw_wt_performance_cnt_r+1;	


//assign Tin_factor=Tin_factor_r;//reg 2
//assign dat_buf_num=dat_buf_num_r;//reg 3
//assign Win=Win_r;//reg 4
//assign Hin=Hin_r;//reg 5
//assign Hin_x_Win=Hin_x_Win_r;//reg 6
//assign CH_in_div_Tout=CH_in_div_Tout_r;//reg 7
//assign CH_in_div_Tin=CH_in_div_Tin_r;//reg 8
//assign CH_in_res_Tin_div_Tout_minus1=CH_in_res_Tin_div_Tout_minus1_r;//reg 31
//assign pad_up=pad_up_r;//reg 9
//assign pad_left=pad_left_r;//reg 10
//assign Sx=Sx_r;//reg 11
//assign Sy=Sy_r;//reg 12
//assign Kx=Kx_r;//reg 13
//assign Ky=Ky_r;//reg 14
//assign Wout=Wout_r;//reg 15
//assign Hout_x_Wout_last_stripe_minus1=Hout_x_Wout_last_stripe_minus1_r[`log2Tout-1:0];//reg 16
//assign last_stripe_vld_cycles=Hout_x_Wout_last_stripe_minus1_r[2*`log2Tout-1:`log2Tout];//reg 16

//assign Hout=Hout_r;//reg 17
//assign CH_out=CH_out_r;//reg 18
//assign CH_out_div_Tout=CH_out_div_Tout_r;//reg 19
//assign dat_base_addr=dat_base_addr_r;//reg 20
//assign surface_stride_in=surface_stride_in_r;//reg 21
//assign line_stride_in=line_stride_in_r;//reg 22
//assign wt_num_div_Tout=wt_num_div_Tout_r;//reg 23
//assign wt_num_div_Tin=wt_num_div_Tin_r;//reg32
//assign wt_base_addr=wt_base_addr_r;//reg 24
//assign dma_dat_reuse=dma_dat_reuse_r;//reg 25
//assign dma_wt_reuse=dma_wt_reuse_r;//reg 25
//assign conv_out_shift=conv_out_shift_r;//reg 26
//assign Hout_x_Wout_div_Tout=Hout_x_Wout_div_Tout_r;//reg 33
//assign dat_out_base_addr=dat_out_base_addr_r;//reg 34
//assign surface_stride_out=surface_stride_out_r;//reg 35
//assign line_stride_out=line_stride_out_r;//reg 36
//assign relu=relu_r;
//assign out_data_width=out_data_width_r;

(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d;//reg 2
(* MAX_FANOUT = 64 *)reg [`log2BRAM_NUM-1:0]dat_buf_num_d;//reg 3
(* MAX_FANOUT = 64 *)reg [`log2_W-1:0]Win_d;//reg 4
(* MAX_FANOUT = 64 *)reg [`log2_H-1:0]Hin_d;//reg 5
(* MAX_FANOUT = 64 *)reg [(`log2_W+`log2_H-1):0]Hin_x_Win_d;//reg 6					//Win*Hin
(* MAX_FANOUT = 64 *)reg [`log2_CH-`log2Tout-1:0]CH_in_div_Tout_d;//reg 7		//ceil(CH_in/Tout)
(* MAX_FANOUT = 64 *)reg [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin_d;//reg 8					//ceil(CH_in/Tin)
(* MAX_FANOUT = 64 *)reg [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1_d;//reg 31
(* MAX_FANOUT = 64 *)reg [`log2_P-1:0]pad_up_d;//reg 9
(* MAX_FANOUT = 64 *)reg [`log2_P-1:0]pad_left_d;//reg 10
(* MAX_FANOUT = 64 *)reg [`log2_S-1:0]Sx_d;//reg 11
(* MAX_FANOUT = 64 *)reg [`log2_S-1:0]Sy_d;//reg 12
(* MAX_FANOUT = 64 *)reg [`log2_K-1:0]Kx_d;//reg 13
(* MAX_FANOUT = 64 *)reg [`log2_K-1:0]Ky_d;//reg 14
(* MAX_FANOUT = 64 *)reg [`log2_W-1:0]Wout_d;//reg 15
(* MAX_FANOUT = 64 *)reg [(`log2_W+`log2_H-1):0]Hout_x_Wout_d;//reg 16					//Wout*Hout
(* MAX_FANOUT = 64 *)reg [`log2_H-1:0]Hout_d;//reg 17			//Hout
(* MAX_FANOUT = 64 *)reg [`log2_CH-1:0]CH_out_d;//reg 18
(* MAX_FANOUT = 64 *)reg [`log2_CH-`log2Tout-1:0]CH_out_div_Tout_d;//reg 19				//ceil(CH_out/Tout)
(* MAX_FANOUT = 64 *)reg [31:0]dat_base_addr_d;//reg 20
(* MAX_FANOUT = 64 *)reg [31:0]surface_stride_in_d;//reg 21
(* MAX_FANOUT = 64 *)reg [15:0]line_stride_in_d;//reg 22
(* MAX_FANOUT = 64 *)reg [31:0]wt_num_div_Tout_d;//reg 23					//weight size in bytes
(* MAX_FANOUT = 64 *)reg [31:0]wt_num_div_Tin_d;//reg32
(* MAX_FANOUT = 64 *)reg [31:0]wt_base_addr_d;//reg 24
(* MAX_FANOUT = 64 *)reg dma_dat_reuse_d;//reg 25
(* MAX_FANOUT = 64 *)reg dma_wt_reuse_d;//reg 25
(* MAX_FANOUT = 64 *)reg [`log2_scale-1:0]conv_out_shift_d;//reg 26
(* MAX_FANOUT = 64 *)reg [31:0]dma_dat_performance_cnt_d;//reg27
(* MAX_FANOUT = 64 *)reg [31:0]dma_wt_performance_cnt_d;//reg28
(* MAX_FANOUT = 64 *)reg [31:0]fsm_dat_performance_cnt_d;//reg29
(* MAX_FANOUT = 64 *)reg [31:0]csw_wt_performance_cnt_d;//reg30
(* MAX_FANOUT = 64 *)reg [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout_d;//reg 33			//Wout*Hout/Tout
(* MAX_FANOUT = 64 *)reg [31:0]dat_out_base_addr_d;//reg 34
(* MAX_FANOUT = 64 *)reg [31:0]surface_stride_out_d;//reg 35
(* MAX_FANOUT = 64 *)reg [15:0]line_stride_out_d;//reg 36
(* MAX_FANOUT = 64 *)reg relu_d;//reg 38
(* MAX_FANOUT = 64 *)reg gelu_d;//reg 38
(* MAX_FANOUT = 64 *)reg [`MAX_log2BN_DW-1:0]out_data_width_d; //reg 39
(* MAX_FANOUT = 64 *)reg [31:0]res_dat_base_addr_d;//reg 40
(* MAX_FANOUT = 64 *)reg [31:0]res_surface_stride_in_d;//reg 41
(* MAX_FANOUT = 64 *)reg [15:0]res_line_stride_in_d;//reg 42
(* MAX_FANOUT = 64 *)reg res_relu_en_d;//reg 43
(* MAX_FANOUT = 64 *)reg [`log2_scale-1:0]res_in_scale_d;//reg 44
(* MAX_FANOUT = 64 *)reg [`log2_scale-1:0]conv_out_scale_d;//reg 45
(* MAX_FANOUT = 64 *)reg [`log2_scale-1:0]res_out_shift_d;//reg 46
(* MAX_FANOUT = 64 *)reg Conv_In_from_BRAM_mode_d;//reg 47
(* MAX_FANOUT = 64 *)reg Conv_Out_to_BRAM_mode_d;//reg 47
(* MAX_FANOUT = 64 *)reg [(`log2_W+`log2_H-1):0]effect_pixel_d;//reg 47
always @(posedge clk)
begin
    Tin_factor_d<=Tin_factor_r;//reg 2
	dat_buf_num_d<=dat_buf_num_r;//reg 3
	Win_d<=Win_r;//reg 4
	Hin_d<=Hin_r;//reg 5
	Hin_x_Win_d<=Hin_x_Win_r;//reg 6
	CH_in_div_Tout_d<=CH_in_div_Tout_r;//reg 7
	CH_in_div_Tin_d<=CH_in_div_Tin_r;//reg 8
	CH_in_res_Tin_div_Tout_minus1_d<=CH_in_res_Tin_div_Tout_minus1_r;//reg31
	pad_up_d<=pad_up_r;//reg 9
	pad_left_d<=pad_left_r;//reg 10
	Sx_d<=Sx_r;//reg 11
	Sy_d<=Sy_r;//reg 12
	Kx_d<=Kx_r;//reg 13
	Ky_d<=Ky_r;//reg 14
	Wout_d<=Wout_r;//reg 15
	Hout_x_Wout_d<=Hout_x_Wout_r;//reg 16
	Hout_d<=Hout_r;//reg 17
	CH_out_d<=CH_out_r;//reg 18
	CH_out_div_Tout_d<=CH_out_div_Tout_r;//reg 19
	dat_base_addr_d<=dat_base_addr_r;//reg 20
	surface_stride_in_d<=surface_stride_in_r;//reg 21
	line_stride_in_d<=line_stride_in_r;//reg 22
	wt_num_div_Tout_d<=wt_num_div_Tout_r;//reg 23
	wt_num_div_Tin_d<=wt_num_div_Tin_r;//reg32
	wt_base_addr_d<=wt_base_addr_r;//reg 24
	dma_dat_reuse_d<=dma_dat_reuse_r;//reg 25
	dma_wt_reuse_d<=dma_wt_reuse_r;//reg 25
	conv_out_shift_d<=conv_out_shift_r;//reg 26
	Hout_x_Wout_div_Tout_d<=Hout_x_Wout_div_Tout_r;//reg 33			//Wout*Hout/Tout
	dat_out_base_addr_d<=dat_out_base_addr_r;//reg 34
	surface_stride_out_d<=surface_stride_out_r;//reg 35
	line_stride_out_d<=line_stride_out_r;//reg 36
	relu_d<=relu_r;//reg 38
	gelu_d<=gelu_r;//reg 38
	out_data_width_d<=out_data_width_r;//reg 39
	res_dat_base_addr_d<=res_dat_base_addr_r;//reg 40
    res_surface_stride_in_d<=res_surface_stride_in_r;//reg 41
    res_line_stride_in_d<=res_line_stride_in_r;//reg 42
    res_relu_en_d<=res_relu_en_r;//reg 43
    res_in_scale_d<=res_in_scale_r;//reg 44
    conv_out_scale_d<=conv_out_scale_r;//reg 45
    res_out_shift_d<=res_out_shift_r;//reg 46
    Conv_In_from_BRAM_mode_d<=Conv_In_from_BRAM_mode_r;//reg 47
    Conv_Out_to_BRAM_mode_d<=Conv_Out_to_BRAM_mode_r;//reg 47
    effect_pixel_d<=effect_pixel_r;//reg 48
end

assign Tin_factor=Tin_factor_d;//reg 2
assign dat_buf_num=dat_buf_num_d;//reg 3
assign Win=Win_d;//reg 4
assign Hin=Hin_d;//reg 5
assign Hin_x_Win=Hin_x_Win_d;//reg 6
assign CH_in_div_Tout=CH_in_div_Tout_d;//reg 7
assign CH_in_div_Tin=CH_in_div_Tin_d;//reg 8
assign CH_in_res_Tin_div_Tout_minus1=CH_in_res_Tin_div_Tout_minus1_d;//reg 31
assign pad_up=pad_up_d;//reg 9
assign pad_left=pad_left_d;//reg 10
assign Sx=Sx_d;//reg 11
assign Sy=Sy_d;//reg 12
assign Kx=Kx_d;//reg 13
assign Ky=Ky_d;//reg 14
assign Wout=Wout_d;//reg 15
assign Hout_x_Wout=Hout_x_Wout_d;//reg 16

assign Hout=Hout_d;//reg 17
assign CH_out=CH_out_d;//reg 18
assign CH_out_div_Tout=CH_out_div_Tout_d;//reg 19
assign dat_base_addr=dat_base_addr_d;//reg 20
assign surface_stride_in=surface_stride_in_d;//reg 21
assign line_stride_in=line_stride_in_d;//reg 22
assign wt_num_div_Tout=wt_num_div_Tout_d;//reg 23
assign wt_num_div_Tin=wt_num_div_Tin_d;//reg32
assign wt_base_addr=wt_base_addr_d;//reg 24
assign dma_dat_reuse=dma_dat_reuse_d;//reg 25
assign dma_wt_reuse=dma_wt_reuse_d;//reg 25
assign conv_out_shift=conv_out_shift_d;//reg 26
assign Hout_x_Wout_div_Tout=Hout_x_Wout_div_Tout_d;//reg 33
assign dat_out_base_addr=dat_out_base_addr_d;//reg 34
assign surface_stride_out=surface_stride_out_d;//reg 35
assign line_stride_out=line_stride_out_d;//reg 36
assign relu=relu_d;
assign gelu=gelu_d;
assign out_data_width=out_data_width_d;
assign res_dat_base_addr=res_dat_base_addr_d;//reg 40
assign res_surface_stride_in=res_surface_stride_in_d;//reg 41
assign res_line_stride_in=res_line_stride_in_d;//reg 42
assign res_relu_en=res_relu_en_d;//reg 43
assign res_in_scale=res_in_scale_d;//reg 44
assign conv_out_scale=conv_out_scale_d;//reg 45
assign res_out_shift=res_out_shift_d;//reg 46
assign Conv_In_from_BRAM_mode=Conv_In_from_BRAM_mode_d;//reg 47
assign Conv_Out_to_BRAM_mode=Conv_Out_to_BRAM_mode_d;//reg 47
assign effect_pixel=effect_pixel_d;//reg 48

always@(posedge clk)
begin
    Vit_conv0_Kx <= Vit_conv0_mode?1:Kx;
    Vit_conv0_Sx <= Vit_conv0_mode?1:Sx;
end

always@(posedge clk)
    case(Kx)
        8:begin
            Vit_conv0_Win         <= Vit_conv0_mode? Win>>3: Win;    
            Vit_conv0_Hin_x_Win   <= Vit_conv0_mode? Hin_x_Win>>3: Hin_x_Win;
          end
        16:begin
              Vit_conv0_Win       <= Vit_conv0_mode? Win>>4: Win;    
              Vit_conv0_Hin_x_Win <= Vit_conv0_mode? Hin_x_Win>>4: Hin_x_Win;
            end      
        default:begin
              Vit_conv0_Win       <= Win;    
              Vit_conv0_Hin_x_Win <= Hin_x_Win;
            end     
    endcase    
endmodule
