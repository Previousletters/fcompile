`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module softmax_out_buf
(
	input clk,
	input rst_n,
    
    input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout, //reg 9
    input [`Log2_Softmax_pixel-1:0]CH_in,

	input dat_in_vld,
	input [`MAX_DAT_DW-1:0]dat_in,
    
    output reg dat_out_vld,
    output reg[`Tout*`MAX_DAT_DW-1:0]dat_out
);
        
//////////write buf//////////
wire [`log2Tout-1:0]current_CH_len_minus1;
reg [`log2Tout-1:0]wr_cnt;
wire wr_cnt_will_update_now=dat_in_vld;
wire wr_cnt_is_max_now=(wr_cnt==current_CH_len_minus1);
always @(posedge clk or negedge rst_n)
begin
    if(~rst_n)
        wr_cnt<=0;
    else
        if(wr_cnt_will_update_now)
        begin
            if(wr_cnt_is_max_now)
                wr_cnt<=0;
            else
                wr_cnt<=wr_cnt+1;
        end
end

reg [`log2_CH-1:0]ch_cnt;
wire ch_cnt_will_update_now=wr_cnt_will_update_now&wr_cnt_is_max_now;
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

wire [`log2_CH-1:0]CH_max_minus1=CH_in-1;
assign current_CH_len_minus1=ch_cnt_is_max_now?(CH_max_minus1[`log2Tout-1:0]):(`Tout-1);



////////// fill zero//////////
reg fill_working;
reg [`log2Tout-1:0]fill_cnt;
wire fill_cnt_will_update_now=fill_working;
wire fill_cnt_is_max_now=(fill_cnt==`Tout-(CH_max_minus1[`log2Tout-1:0]+1)-1);

wire fill_start=ch_cnt_is_max_now&wr_cnt_will_update_now&wr_cnt_is_max_now&(CH_max_minus1[`log2Tout-1:0]!=`Tout-1);
wire fill_done=fill_cnt_will_update_now&fill_cnt_is_max_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    fill_working<=1'b0;
else
if(fill_start)
    fill_working<=1'b1;
else
    if(fill_done)
        fill_working<=1'b0;
        
always @(posedge clk or negedge rst_n)
begin
    if(~rst_n)
        fill_cnt<=0;
    else
        if(fill_cnt_will_update_now)
        begin
            if(fill_cnt_is_max_now)
                fill_cnt<=0;
            else
                fill_cnt<=fill_cnt+1;
        end
end

reg[`MAX_DAT_DW-1:0]tp_dat_out[`Tout-1:0];
wire wr_vld=dat_in_vld|fill_working;
wire[`log2Tout-1:0]wr_addr=dat_in_vld?wr_cnt:(fill_working?fill_cnt+CH_max_minus1[`log2Tout-1:0]+1:0);
wire[`MAX_DAT_DW-1:0]wr_dat=dat_in_vld?dat_in:0;

always@(posedge clk)
if(wr_vld)
    tp_dat_out[wr_addr]<=wr_dat;

genvar i;
generate
    for(i=0;i<`Tout;i=i+1)
    begin:out_buf
        always@(*)
            dat_out[i*`MAX_DAT_DW+:`MAX_DAT_DW]=dat_out_vld?tp_dat_out[i]:0;
	end
endgenerate 

always@(posedge clk or negedge rst_n)
if(~rst_n)
    dat_out_vld<=0;
else
    dat_out_vld<=(wr_cnt_will_update_now&wr_cnt_is_max_now&(~fill_working)&(~fill_start))|(fill_working&fill_done);

endmodule
