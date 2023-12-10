`include "CNN_defines.vh"

module Transpose_dat2buf
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

    output wr_vld,
    output [`base_log2Tin-1:0]wr_addr,
    output [`base_log2Tin:0]wr_addr_max,
    output [`MAX_DAT_DW*`Tout-1:0]wr_dat,
    input  wr_rdy
);

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
reg [`log2_W-1:0]burst_cnt;
wire burst_cnt_will_update_now=ch_cnt_will_update_now&ch_cnt_is_max_now;
wire burst_cnt_is_max_now=(burst_cnt==((w_in-1)>>`log2AXI_BURST_LEN));
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		burst_cnt<=0;
	else
		if(burst_cnt_will_update_now)
		begin
			if(burst_cnt_is_max_now)
				burst_cnt<=0;
			else
				burst_cnt<=burst_cnt+1;
		end
end

assign current_in_burst_len_minus1=(burst_cnt_is_max_now)?(w_in[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);


reg working;
wire done=burst_cnt_will_update_now&burst_cnt_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;


assign rd_resp_rdy=working&wr_rdy;
/*
reg [`MAX_DAT_DW*`Tout-1:0] feature_buf [`Tout-1:0][`Tin/Tout-1:0];

      Tin >= Tout >= Burst_len
      
suppose Tin=32, Tout=16, Burst_len=8

                  original input        =============>                 transpose
                  
                   Tout_group0                                 Burst0             Burst1      
b  w 0 |ch0|ch1|...|ch7|  |ch8|ch9|...|ch15|         ch 0 |w0|w1|...  |w7|   |w8|w9|...|  |w15|
u  w 1 |                                   |         ch 1 |              |   |                |  
r  ... |                                   |         ...  |              |   |                |  
s      |                                   |      T       |              |   |                |  
t      |                                   |      o       |              |   |                |  
0   w 7|||||||||||||||||||||||||||||||||||||      u  ch 7 |              |   |                | 
                                                  t                      |   |                |
b   w 8|ch0|ch1|...|ch7|  |ch8|ch9|...|ch15|      0  ch 8 |              |   |                |
u   w 9|                                   |         ch 9 |              |   |                | 
r   ...|                                   |          ... |              |   |                | 
s      |                                   |              |              |   |                | 
t   w14|                                   |         ch14 |              |   |                | 
1   w15|||||||||||||||||||||||||||||||||||||         ch15 ||||||||||||||||   ||||||||||||||||| 


   w  0 1 2    15    0  1  2    15   0  1  2    15
  ch  0 0 0     0    1  1  1    1    2  2  2     2
addr  0 2 4    30    1  3  5    31   0  2  4    30
*/
wire [2:0]Tin_div_Tout=`base_Tin/`Tout;
reg  [`base_log2Tin-1:0]tp_wr_addr;
always@(*)
    case(Tin_div_Tout)
    3'b001:tp_wr_addr=w_in_cnt;
    3'b010:tp_wr_addr=w_in_cnt*2+ch_cnt[0];
    3'b100:tp_wr_addr=w_in_cnt*4+ch_cnt[1:0];
    default:tp_wr_addr=w_in_cnt;
    endcase
    
assign wr_addr=tp_wr_addr;
assign wr_dat=rd_resp_pd;
assign wr_vld=rd_resp_vld&rd_resp_rdy;
assign wr_addr_max=(current_in_burst_len_minus1+1)*(`base_Tin/`Tout);

endmodule

