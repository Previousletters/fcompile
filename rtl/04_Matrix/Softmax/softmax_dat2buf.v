`include "CNN_defines.vh"

module softmax_dat2buf
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`Log2_Softmax_pixel-1:0]ch_in,

	//rd response from MCIF
	input rd_resp_vld,
	output rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]rd_resp_pd,
	output rd_fifo_pop,

    output wr_vld,
    input  wr_rdy,
    output [`log2AXI_BURST_LEN-1:0]wr_addr,
    output reg[2:0]cycle,
    output [`Log2_Softmax_pixel-1:0]pixel_addr,
    output [`Log2_Softmax_pixel-1:0]ch_addr,
    output [`MAX_DAT_DW*`Tout-1:0]wr_dat,

    input  credit_vld
);

reg [5+`log2Tout-1:0]credit_cnt;
wire FSM_go=|credit_cnt;//credit_cnt!=0

wire credit_cnt_decrease=rd_resp_vld&rd_resp_rdy&(cycle==2);
always @(posedge clk or negedge rst_n)
if(~rst_n)
    credit_cnt<=(`Softmax_delay+`AXI_BURST_LEN+2);
else
    case({credit_cnt_decrease,credit_vld})
        2'b01:begin if(credit_cnt!=(`Softmax_delay+`AXI_BURST_LEN+2)) credit_cnt<=credit_cnt+1;end
        2'b10:credit_cnt<=credit_cnt-1;
    endcase




assign rd_fifo_pop=rd_resp_vld&rd_resp_rdy;

wire [`log2AXI_BURST_LEN-1:0]current_in_burst_len_minus1;
reg [`Log2_Trans_pixel-1:0]w_in_cnt;
wire w_in_cnt_will_update_now=rd_resp_vld&rd_resp_rdy;
wire w_in_cnt_is_max_now=(w_in_cnt==current_in_burst_len_minus1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_in_cnt<=0;
	else
		if(w_in_cnt_will_update_now)
		begin
			if(w_in_cnt_is_max_now)
				w_in_cnt<=0;
			else
				w_in_cnt<=w_in_cnt+1;
		end
end


reg [`log2_CH-1:0]ch_cnt;
wire ch_cnt_will_update_now=w_in_cnt_will_update_now&w_in_cnt_is_max_now;
wire ch_cnt_is_max_now=(ch_cnt==CH_in_div_Tout-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		ch_cnt<=0;
	else
		if(ch_cnt_will_update_now)
		begin
			if(ch_cnt_is_max_now)
				ch_cnt<=0;
			else
				ch_cnt<=ch_cnt+1;
		end
end


wire cycle_will_update_now=ch_cnt_will_update_now&ch_cnt_is_max_now;
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

reg [`log2_W-1:0]w_burst_cnt;
wire w_burst_cnt_will_update_now=cycle_will_update_now&cycle_is_max_now;
wire w_burst_cnt_is_max_now=(w_burst_cnt==((w_in-1)>>`log2AXI_BURST_LEN));
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

reg [`log2_H-1:0]h;
wire h_will_update_now=w_burst_cnt_will_update_now&w_burst_cnt_is_max_now;
wire h_is_max_now=(h==h_in-1);
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

assign current_in_burst_len_minus1=(w_burst_cnt_is_max_now)?(w_in[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);


reg working;
wire done=h_will_update_now&h_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;
            
assign rd_resp_rdy=FSM_go&wr_rdy&working;

assign wr_addr=w_in_cnt;
assign pixel_addr=h*w_in+(w_burst_cnt*`AXI_BURST_LEN+w_in_cnt);
assign ch_addr=ch_cnt;
assign wr_dat=rd_resp_pd;
assign wr_vld=rd_resp_vld&rd_resp_rdy;


endmodule

