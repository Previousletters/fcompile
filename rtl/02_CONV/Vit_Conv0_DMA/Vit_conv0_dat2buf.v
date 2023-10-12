`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module Vit_conv0_dat2buf
(
	input clk,
	input rst_n,
    input conv_mode,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit    
    
	//from CSR
	input start,
	input [`log2_W-1:0]Win,
	
	input [`log2_K-1:0]Kx,
	input [`log2_H-1:0]Hin,
    input [(`log2_W+`log2_H-1):0]Hin_x_Win,
	input [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1,		//ceil(CH_in%Tin /Tout)
	input [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin,				//ceil(CH_in/Tin)

	//to CSR
	output dma_dat_done,

	//rd response from MCIF
	input rd_resp_vld,
	output rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]rd_resp_pd,
	output rd_fifo_pop,

	//dat to BUF 
	output reg dma2buf_DAT_wr_en,
	output reg [`log2BUF_DEP-1:0]dma2buf_DAT_wr_addr,
	output reg [`base_Tin*`MAX_DAT_DW-1:0]dma2buf_DAT_wr_data,

	//row information to BUF
	output reg row_num_updt,
	output reg [`log2_H-1:0]row_num,
	output reg chin_num_updt,
	output reg [`log2_CH-1:0]chin_num
);
assign rd_fifo_pop=rd_resp_vld&rd_resp_rdy;
reg working;
assign rd_resp_rdy=working;

wire [`base_log2Tin+1:0]Tin_max=Kx*4;
reg [`Max_log2Tin_minus_log2Tout:0]kk;
wire kk_is_max_now=(kk==(`base_Tin>>`log2Tout)-1);//if Kx=8, Tin_max=32;   if Kx=16, Tin_max=64
wire kk_will_update_now=rd_resp_vld&rd_resp_rdy;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	kk<=0;
else
	if(kk_will_update_now)
	begin
		if(kk_is_max_now)
			kk<=0;
		else
			kk<=kk+1;
	end

reg [`log2_W-1:0]win;
reg [`log2_W-1:0]Win_max;
always@(*)
    case(Kx)
        8:Win_max=Win>>3;
        16:Win_max=Win>>4;
        default:Win_max=Win;
    endcase


reg [`log2_W-1:0] win_bias;
wire win_will_update_now=kk_will_update_now&kk_is_max_now;
wire win_is_max_now=(win==Win_max-1);
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	win<=0;
end
else
begin
	if(win_will_update_now)
	begin
		if(win_is_max_now)
		begin
			win<=0;
		end
		else
		begin
			win<=win+1;
		end
	end
end



reg [`log2_H-1:0]hin;
reg [`log2_H+`log2_W-1:0]hin_bias;
wire hin_will_update_now=win_is_max_now&win_will_update_now;
wire hin_is_max_now=(hin==Hin-1);
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	hin<=0;
	hin_bias<=0;
end
else
begin
	if(hin_will_update_now)
	begin
		if(hin_is_max_now)
		begin
			hin<=0;
			hin_bias<=0;
		end
		else
		begin
			hin<=hin+1;
			hin_bias<=hin_bias+Win_max;
		end
	end
end

assign dma_dat_done=hin_will_update_now&hin_is_max_now;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	working<=1'b0;
else
	if(start)
		working<=1'b1;
	else
		if(dma_dat_done)
			working<=1'b0;
			
			
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	dma2buf_DAT_wr_en<=1'b0;
	dma2buf_DAT_wr_addr<=0;
	row_num_updt<=0;
	row_num<=0;
	chin_num_updt<=0;
	chin_num<=0;
end
else
begin
	dma2buf_DAT_wr_en<=win_will_update_now;
	dma2buf_DAT_wr_addr<=hin_bias+win;
	row_num_updt<=hin_will_update_now;
	row_num<=hin;
    chin_num_updt<=kk_will_update_now;
    chin_num<=0;
end

always @(posedge clk)
if(kk_will_update_now)
    dma2buf_DAT_wr_data[`Tout*`MAX_WT_DW*kk+:`Tout*`MAX_WT_DW]<=rd_resp_pd;


endmodule

