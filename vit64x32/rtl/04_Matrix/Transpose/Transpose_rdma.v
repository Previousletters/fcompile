`include "CNN_defines.vh"

module Transpose_rdma
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [31:0]base_addr,
	input [25:0]surface_stride,
	input [15:0]line_stride,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`Log2_Trans_pixel-1:0]pixel_in,
    
	//rd CMD to MCIF
	output Trans2mcif_rd_req_vld,
	input Trans2mcif_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]Trans2mcif_rd_req_pd
);
reg [`log2_CH-1:0]CH_cnt;
wire CH_cnt_will_update_now=Trans2mcif_rd_req_vld&Trans2mcif_rd_req_rdy;
wire CH_cnt_is_max_now=(CH_cnt==CH_in_div_Tout-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		CH_cnt<=0;
	else
		if(CH_cnt_will_update_now)
		begin
			if(CH_cnt_is_max_now)
				CH_cnt<=0;
			else
				CH_cnt<=CH_cnt+1;
		end
end

reg [16-1:0]CH_cnt_bias;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    CH_cnt_bias<=0;
else
    if(CH_cnt_will_update_now)
    begin
        if(CH_cnt_is_max_now)
            CH_cnt_bias<=0;
        else
            CH_cnt_bias<=CH_cnt_bias+surface_stride;
    end

wire [`log2_W-1:0]Win_minus1=w_in-1;
reg [`log2_CH-1:0]w;
wire w_will_update_now=CH_cnt_will_update_now&CH_cnt_is_max_now;
wire w_is_max_now=(w==Win_minus1>>`log2AXI_BURST_LEN);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w<=0;
	else
		if(w_will_update_now)
		begin
			if(w_is_max_now)
				w<=0;
			else
				w<=w+1;
		end
end
reg [16-1:0]w_bias;
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_bias<=0;
	else
		if(w_will_update_now)
		begin
			if(w_is_max_now)
				w_bias<=0;
			else
				w_bias<=w_bias+(`AXI_BURST_LEN*`Tout*`MAX_DAT_DW/8);
		end
end

reg working;
wire done;
assign done=w_will_update_now&w_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;

wire [`log2AXI_BURST_LEN-1:0]cmd_len=w_is_max_now?(Win_minus1[`log2AXI_BURST_LEN-1:0]):(`AXI_BURST_LEN-1);
wire [31:0]cmd_addr=(CH_cnt_bias+w_bias);

assign Trans2mcif_rd_req_vld=working;
assign Trans2mcif_rd_req_pd={cmd_len,base_addr,cmd_addr};

endmodule

