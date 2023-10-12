`include "CNN_defines.vh"

module LN_ctrl
(
	input clk,
	input rst_n,

	input start,
    output done,
	input credit_vld,
	
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,		//ceil(ch_in/Tout)
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,

	//rd response from MCIF
	input rd_resp_vld,
	output rd_resp_rdy,
	input [`MAX_DAT_DW*`Tout-1:0]rd_resp_pd,
	output rd_fifo_pop,
	
	//dat2LN calculation
	output reg[`MAX_DAT_DW*`Tout-1:0]original_feature, //delay 1clk
	output reg original_feature_vld,                   //delay 1clk
    output rd_dat_en,
    output [`Log2_LN_pixel-1:0]rd_dat_addr,
    output reg rd_wt_en,                                  //delay 1clk
    output reg [`Log2_LN_ch_max-`log2Tout-1:0]rd_wt_addr, //delay 1clk
    output reg rd_bias_en,                                //delay 3clk
    output reg [`Log2_LN_ch_max-`log2Tout-1:0]rd_bias_addr//delay 3clk
);

reg [3:0]credit_cnt;
wire FSM_go=|credit_cnt;//credit_cnt!=0

wire credit_cnt_decrease=rd_resp_vld&rd_resp_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    credit_cnt<=4;
else
    case({credit_cnt_decrease,credit_vld})
        2'b01:begin if(credit_cnt!=4) credit_cnt<=credit_cnt+1;end
        2'b10:credit_cnt<=credit_cnt-1;
    endcase



wire [`log2AXI_BURST_LEN-1:0]current_burst_len;

reg [`log2_W-1:0]w_cnt;
wire w_cnt_will_update_now=rd_resp_vld&rd_resp_rdy;
wire w_cnt_is_max_now=(w_cnt==current_burst_len);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_cnt<=0;
	else
		if(w_cnt_will_update_now)
		begin
			if(w_cnt_is_max_now)
				w_cnt<=0;
			else
				w_cnt<=w_cnt+1;
		end
end

reg [`Log2_LN_ch_max-`log2Tout-1:0]ch_cnt;
wire ch_cnt_will_update_now=w_cnt_will_update_now&w_cnt_is_max_now;
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

reg [`log2_W-1:0]w_burst_cnt;
wire w_burst_cnt_will_update_now=ch_cnt_will_update_now&ch_cnt_is_max_now;
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

assign current_burst_len=(w_burst_cnt_is_max_now)?(w_in[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);


reg [`log2_W-1:0]h_cnt;
wire h_cnt_will_update_now=w_burst_cnt_will_update_now&w_burst_cnt_is_max_now;
wire h_cnt_is_max_now=(h_cnt==h_in-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		h_cnt<=0;
	else
		if(h_cnt_will_update_now)
		begin
			if(h_cnt_is_max_now)
				h_cnt<=0;
			else
				h_cnt<=h_cnt+1;
		end
end


reg working;
assign done=h_cnt_will_update_now&h_cnt_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;
            
assign rd_resp_rdy=working&FSM_go;
assign rd_fifo_pop=rd_resp_vld&rd_resp_rdy;



always@(posedge clk) original_feature<=rd_resp_pd;
always@(posedge clk) original_feature_vld<=rd_resp_vld&rd_resp_rdy;

assign rd_dat_en=rd_resp_vld&rd_resp_rdy;
assign rd_dat_addr=h_cnt*w_in+w_burst_cnt*`AXI_BURST_LEN+w_cnt;

always@(posedge clk) rd_wt_en<=rd_resp_vld&rd_resp_rdy;  //delay 1clk
always@(posedge clk) rd_wt_addr<=ch_cnt;                 //delay 1clk

reg tp_rd_bias_en_d1,tp_rd_bias_en_d2;
reg [`Log2_LN_ch_max-`log2Tout-1:0]tp_rd_bias_addr_d1,tp_rd_bias_addr_d2;
always@(posedge clk) 
begin
    tp_rd_bias_en_d1<=rd_resp_vld&rd_resp_rdy;  //delay 3clk
    tp_rd_bias_en_d2<=tp_rd_bias_en_d1;
    rd_bias_en      <=tp_rd_bias_en_d2;
    
    tp_rd_bias_addr_d1<=ch_cnt;                 //delay 3clk
    tp_rd_bias_addr_d2<=tp_rd_bias_addr_d1;
    rd_bias_addr      <=tp_rd_bias_addr_d2;
    
end

endmodule

