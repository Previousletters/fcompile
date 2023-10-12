`include "CNN_defines.vh"

module Concat_rdma
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [31:0]clstoken_base_addr,
	input [31:0]base_addr,
	input [25:0]surface_stride,
	input [15:0]line_stride,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_W-1:0]w_in,
    
	//rd CMD to MCIF
	output Concat2mcif_rd_req_vld,
	input Concat2mcif_rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]Concat2mcif_rd_req_pd
);


reg [1:0]cnt;

reg [`log2_W-1:0]w_burst_cnt;
wire [`log2_W-1:0]w_burst_len_now=(cnt==0)?0:((w_in-1)>>`log2AXI_BURST_LEN);

wire w_burst_cnt_will_update_now=Concat2mcif_rd_req_vld&Concat2mcif_rd_req_rdy;
wire w_burst_cnt_is_max_now=(w_burst_cnt==w_burst_len_now);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_burst_cnt<=0;
	else
		if(w_burst_cnt_will_update_now)
		begin
			if(w_burst_cnt_is_max_now)
				w_burst_cnt<=0;
			else
				w_burst_cnt<=w_burst_cnt+1;
		end
end

wire cnt_will_update_now=w_burst_cnt_will_update_now&w_burst_cnt_is_max_now;
wire cnt_is_max_now=(cnt==1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		cnt<=0;
	else
		if(cnt_will_update_now)
		begin
			if(cnt_is_max_now)
				cnt<=0;
			else
				cnt<=cnt+1;
		end
end


reg [`Log2_LN_ch_max-`log2Tout-1:0]ch_cnt;
reg [31:0]ch_cnt_bias;
wire ch_cnt_will_update_now=cnt_will_update_now&cnt_is_max_now;
wire ch_cnt_is_max_now=(ch_cnt==(CH_in_div_Tout - 1));
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
				ch_cnt_bias<=ch_cnt_bias+surface_stride;
			end
		end
end


reg working;
wire done;
assign done=ch_cnt_will_update_now&ch_cnt_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;


wire [`log2AXI_BURST_LEN-1:0]cls_cmd_len=0;
wire [31:0]cls_cmd_addr=ch_cnt*(`Tout*(`MAX_DAT_DW)/8);

wire [`log2AXI_BURST_LEN-1:0]cmd_len=w_burst_cnt_is_max_now?(w_in[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);
wire [31:0]cmd_addr=ch_cnt_bias+(w_burst_cnt*(`AXI_BURST_LEN*`Tout*(`MAX_DAT_DW)/8));

assign Concat2mcif_rd_req_vld=working;
assign Concat2mcif_rd_req_pd=cnt==0?{cls_cmd_len,clstoken_base_addr,cls_cmd_addr}:{cmd_len,base_addr,cmd_addr};

endmodule

