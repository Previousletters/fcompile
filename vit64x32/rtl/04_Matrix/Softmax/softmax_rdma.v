`include "CNN_defines.vh"

module softmax_rdma
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
    input [`Log2_Softmax_pixel-1:0]pixel_in,
    input [`Log2_Softmax_pixel-1:0]ch_in,
    
	//rd CMD to MCIF
	output Softmax2mcif_rd_req_vld,
	input Softmax2mcif_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]Softmax2mcif_rd_req_pd
);

reg [`log2_CH-1:0]CH_cnt;
wire CH_cnt_will_update_now=Softmax2mcif_rd_req_vld&Softmax2mcif_rd_req_rdy;
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

reg [2:0]cycle;
wire cycle_will_update_now=CH_cnt_will_update_now&CH_cnt_is_max_now;
wire cycle_is_max_now=(cycle==3-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		cycle<=0;
	else
		if(cycle_will_update_now)
		begin
			if(cycle_is_max_now)
				cycle<=0;
			else
				cycle<=cycle+1;
		end
end

wire [`log2_W-1:0]Win_minus1=w_in-1;
reg [`log2_CH-1:0]w;
wire w_will_update_now=cycle_will_update_now&cycle_is_max_now;
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

wire [`log2_W-1:0]Hin_minus1=h_in-1;
reg [`log2_CH-1:0]h;
wire h_will_update_now=w_will_update_now&w_is_max_now;
wire h_is_max_now=(h==Hin_minus1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		h<=0;
	else
		if(h_will_update_now)
		begin
			if(h_is_max_now)
				h<=0;
			else
				h<=h+1;
		end
end
reg [16-1:0]h_bias;
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		h_bias<=0;
	else
		if(h_will_update_now)
		begin
			if(h_is_max_now)
				h_bias<=0;
			else
				h_bias<=h_bias+w_in*(`Tout*`MAX_DAT_DW/8);
		end
end

reg working;
wire done;
assign done=h_will_update_now&h_is_max_now;  
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
wire [31:0]cmd_addr=(h_bias)+(CH_cnt_bias+w_bias);

assign Softmax2mcif_rd_req_vld=working;
assign Softmax2mcif_rd_req_pd={cmd_len,base_addr,cmd_addr};

endmodule

