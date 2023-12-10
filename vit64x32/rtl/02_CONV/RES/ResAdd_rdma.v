`include "CNN_defines.vh"


module ResAdd_rdma
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_W-1:0]W,
	input [`log2_H-1:0]H,
	input [`log2_CH-`log2Tout-1:0]CH_div_Tout,
	input [31:0]dat_base_addr,
	input [31:0]surface_stride_in,
	input [15:0]line_stride_in,

    output done,

	//rd CMD to MCIF
	output rd_req_vld,
	input rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd
);

reg [`log2_W-`log2AXI_BURST_LEN-1:0]w_burst_cnt;
reg [31:0]w_burst_cnt_bias;
wire w_burst_cnt_will_update_now=rd_req_vld&rd_req_rdy;
wire w_burst_cnt_is_max_now=(w_burst_cnt==((W-1)>>`log2AXI_BURST_LEN));//(Win+K-1)%K-1=(w_wdma-1)%K

reg [`log2_H-1:0]h_cnt;
reg [31:0]h_cnt_bias;
wire h_cnt_will_update_now=w_burst_cnt_will_update_now&w_burst_cnt_is_max_now;
wire h_cnt_is_max_now=(h_cnt==(H-1));

reg [`log2_CH-1:0]ch_cnt;
reg [31:0]ch_cnt_bias;
wire ch_cnt_will_update_now=h_cnt_will_update_now&h_cnt_is_max_now;
wire ch_cnt_is_max_now=(ch_cnt==CH_div_Tout-1);


always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
	begin
		w_burst_cnt<=0;
		w_burst_cnt_bias<=0;
	end
	else
		if(w_burst_cnt_will_update_now)
		begin
			if(w_burst_cnt_is_max_now)
			begin
				w_burst_cnt<=0;
				w_burst_cnt_bias<=0;
			end
			else
			begin
				w_burst_cnt<=w_burst_cnt+1;
				w_burst_cnt_bias<=w_burst_cnt_bias+(`AXI_BURST_LEN)*(`Pixel_Data_Bytes);
			end
		end
end

always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
	begin
		h_cnt<=0;
		h_cnt_bias<=0;
	end
	else
		if(h_cnt_will_update_now)
		begin
			if(h_cnt_is_max_now)
			begin
				h_cnt<=0;
				h_cnt_bias<=0;
			end
			else
			begin
				h_cnt<=h_cnt+1;
				h_cnt_bias<=h_cnt_bias+line_stride_in;
			end
		end
end	

always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
	begin
		ch_cnt<=0;
		ch_cnt_bias<=0;
	end
	else
		if(ch_cnt_will_update_now)
		begin
			if(ch_cnt_is_max_now)
			begin
				ch_cnt<=0;
				ch_cnt_bias<=0;
			end
			else
			begin
				ch_cnt<=ch_cnt+1;
				ch_cnt_bias<=ch_cnt_bias+surface_stride_in;
			end
		end
end

reg working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	working<=1'b0;
else
	if(start)
		working<=1'b1;
	else
		if(done)
			working<=1'b0;

assign done=ch_cnt_will_update_now&ch_cnt_is_max_now;

wire [`log2AXI_BURST_LEN-1:0]cmd_length=(w_burst_cnt_is_max_now?(W[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1));
//wire [31:0]cmd_addr=feature_in_base_addr+feature_in_surface_stride*ch_cnt+feature_in_line_stride*h_cnt
//					+(col_burst_cnt<<`log2AXI_BURST_LEN)*(`Tout*(`log2DAT_DW-2));
wire [31:0]cmd_addr=ch_cnt_bias+h_cnt_bias+w_burst_cnt_bias;

assign rd_req_vld=working;
assign rd_req_pd={cmd_length,dat_base_addr,cmd_addr};

endmodule

