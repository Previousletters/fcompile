`include "CNN_defines.vh"

module softmax_fsm
(
	input clk,
	input rst_n,

    input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout, //reg 9
    input [`log2_H-1:0]h_in,                      //reg 10
    input [`log2_W-1:0]w_in,                      //reg 11  
    input [`Log2_Trans_pixel-1:0]pixel_in,        //reg 19
    input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout, //reg 20
    input [`log2_H-1:0]h_out,                      //reg 21
    input [`log2_W-1:0]w_out,                      //reg 22  
    input [`Log2_Trans_pixel-1:0]ch_in,           //reg 23
    input [`log2_S-1:0]in_scale,
    input [`log2_S-1:0]out_scale,
    
    input [`MAX_DAT_DW*`Tout-1:0]dat_in,
    input dat_in_vld,
    output dat_in_rdy,
    input [`log2AXI_BURST_LEN-1:0]wr_addr,
    input [`Log2_Trans_pixel-1:0]ch_addr_now, 
    
    output reg[`MAX_DAT_DW*`Tout-1:0]dat_out,
    output reg dat_out_vld
   
);

wire [`MAX_DAT_DW-1:0]softmax_in[`AXI_BURST_LEN-1:0];
wire              softmax_in_vld[`AXI_BURST_LEN-1:0];
wire [`MAX_DAT_DW-1:0]softmax_out[`AXI_BURST_LEN-1:0];
wire              softmax_out_vld[`AXI_BURST_LEN-1:0];

reg [`Log2_Trans_pixel-1:0]ch_addr[`AXI_BURST_LEN:0];
always @(posedge clk or negedge rst_n)
if(~rst_n)
    ch_addr[0]<=0;
else
    ch_addr[0]<=ch_addr_now;

wire[`AXI_BURST_LEN-1:0] buf_rdy;
assign dat_in_rdy=|buf_rdy;

genvar i;
generate
    for(i=0;i<`AXI_BURST_LEN;i=i+1)//`AXI_BURST_LEN=`Tout
    begin:in_buf
    
        always @(posedge clk)
            ch_addr[i+1]<=ch_addr[i];
    
        softmax_in_buf u_in_buf
        (
            .clk(clk),
            .rst_n(rst_n),
            
            .ch_addr(ch_addr[i]),
            .CH_in(ch_in),
            
            .wr_vld(dat_in_vld&wr_addr==i),
            .wr_rdy(buf_rdy[i]),
            .wr_dat(dat_in),
            .buf_out_vld(softmax_in_vld[i]),
            .buf_out(softmax_in[i])
        );
	end
endgenerate 

generate
    for(i=0;i<`AXI_BURST_LEN;i=i+1)
    begin:cal

        Softmax u_Softmax
        (
            .clk(clk),
            .rst_n(rst_n),
            .ch_in(ch_in),
            .scale_in(in_scale[3:0]),
            .scale_out(out_scale[3:0]),
            .data_in(softmax_in[i]),
            .valid_in(softmax_in_vld[i]),
            .data_out(softmax_out[i]),
            .valid_out(softmax_out_vld[i])
        );

	end
endgenerate 

wire [`MAX_DAT_DW*`Tout-1:0]buf_dat_out[`AXI_BURST_LEN-1:0];
wire [`AXI_BURST_LEN-1:0]buf_dat_out_vld;
generate
    for(i=0;i<`AXI_BURST_LEN;i=i+1)
    begin:out_buf

        softmax_out_buf u_out_buf
        (
            .clk(clk),
            .rst_n(rst_n),
            .CH_in(ch_in),
            .CH_in_div_Tout(CH_in_div_Tout),
            .dat_in(softmax_out[i]),
            .dat_in_vld(softmax_out_vld[i]),
            
            .dat_out(buf_dat_out[i]),
            .dat_out_vld(buf_dat_out_vld[i])
        );
	end
endgenerate 

reg [`MAX_DAT_DW*`Tout-1:0]tp_dat_out;
integer j;
always @(*)
begin
	tp_dat_out=0;
	for(j=0;j<`AXI_BURST_LEN;j=j+1)
	begin
		tp_dat_out = tp_dat_out | buf_dat_out[j];
	end
end
always @(*)
    dat_out=tp_dat_out;
    
always @(*)
    dat_out_vld=|buf_dat_out_vld;
    
endmodule

