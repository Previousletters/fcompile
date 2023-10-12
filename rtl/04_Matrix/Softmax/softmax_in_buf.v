`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module softmax_in_buf
(
	input clk,
	input rst_n,
    
    input [`Log2_Softmax_pixel-1:0]CH_in,
    input [`Log2_Softmax_pixel-1:0]ch_addr,

	input wr_vld,
	output wr_rdy,
	input [`Tout*`MAX_DAT_DW-1:0]wr_dat,
    
    output reg buf_out_vld,
    output reg[`MAX_DAT_DW-1:0]buf_out
);
/*            write buf            */
reg [`MAX_DAT_DW*`Tout-1:0]tp_buf;
always @(posedge clk)
if(wr_vld)
    tp_buf<=wr_dat;


/*             read buf            */
wire [`log2Tout-1:0]current_CH_len_minus1;
wire [`log2_CH-1:0]CH_max_minus1=CH_in-1;
assign current_CH_len_minus1=((ch_addr+1)*`Tout>CH_in)?(CH_max_minus1[`log2Tout-1:0]):(`Tout-1);

reg [`log2Tout-1:0]rd_cnt;
reg rd_working;
wire rd_cnt_will_update_now=rd_working;
wire rd_cnt_is_max_now=(rd_cnt==`Tout-1);
wire rd_done=rd_cnt_will_update_now&rd_cnt_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    rd_working<=1'b0;
else
if(wr_vld)
    rd_working<=1'b1;
else
    if(rd_done)
        rd_working<=1'b0;
        

always @(posedge clk or negedge rst_n)
begin
    if(~rst_n)
        rd_cnt<=0;
    else
        if(rd_cnt_will_update_now)
        begin
            if(rd_cnt_is_max_now)
                rd_cnt<=0;
            else
                rd_cnt<=rd_cnt+1;
        end
end


always @(posedge clk)
if(rd_working)
    buf_out<=tp_buf[rd_cnt*`MAX_DAT_DW+:`MAX_DAT_DW];
    
always @(posedge clk)
    buf_out_vld<=rd_working&(rd_cnt<=current_CH_len_minus1);    

assign wr_rdy=~rd_working;

endmodule
