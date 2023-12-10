`include "CNN_defines.vh"

module activation_dat2buf
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`log2_W-1:0]w_out,
    input [`log2_H-1:0]h_out,
    input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout,
    input [`Log2_Trans_pixel-1:0]pixel_out,
    input [`Log2_Trans_pixel-1:0]pixel_in,
    
	//rd response from MCIF
	input rd_resp_vld,
	output rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]rd_resp_pd,
	output rd_fifo_pop,

    output done,
    output reg working,
    output dat_out_vld,
    output [`MAX_DAT_DW*`Tout-1:0]dat_out,
    input credit_vld
    
);

assign rd_fifo_pop=rd_resp_vld&rd_resp_rdy;

reg [5-1:0]credit_cnt;
wire FSM_go=|credit_cnt;//credit_cnt!=0

wire credit_cnt_decrease=rd_resp_vld&rd_resp_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	credit_cnt<=`Gelu_delay;
else
	case({credit_cnt_decrease,credit_vld})
	2'b01:begin if(credit_cnt!=`Gelu_delay) credit_cnt<=credit_cnt+1;end
	2'b10:credit_cnt<=credit_cnt-1;
	endcase
	
	
//////////////////// input and write BUF////////////////////////
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;
            

assign rd_resp_rdy=FSM_go&working;


reg [16-1:0]w_cnt;
wire w_cnt_will_update_now=rd_resp_vld&rd_resp_rdy;
wire w_cnt_is_max_now=(w_cnt==w_in-1);
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

reg [`log2_CH-1:0]ch_cnt;
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

assign done=ch_cnt_will_update_now&ch_cnt_is_max_now;  


///////////////////////////////////////////////

	
	
assign dat_out_vld=rd_resp_vld&rd_resp_rdy;
assign dat_out=rd_resp_pd;

endmodule

