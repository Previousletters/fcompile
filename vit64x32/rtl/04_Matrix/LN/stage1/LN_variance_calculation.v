`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module LN_variance_calculation
(
	input clk,
	input rst_n,

    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,

	input mean_vld,
 	input [`MAX_DAT_DW-1:0]mean,
    input [`MAX_DAT_DW*2-1:0]mean_square,
    input dat_square_acc_vld,
    input [`MAX_DAT_DW*2+`Log2_LN_ch_max-1:0]dat_square_acc,
    
    output [`MAX_DAT_DW-1:0]mean_delay,
	output [`MAX_DAT_DW*2+4-1:0]variance,
	output [`MAX_DAT_DW  +2-1:0]std_deviation,
 	output std_deviation_vld,
	
	output [`Log2_LN_pixel-1:0]wr_addr,
	output variance_done
);
///////count for stage1////////////
reg [`Log2_LN_pixel-1:0]cnt;
wire cnt_will_update_now=mean_vld&dat_square_acc_vld;
wire cnt_is_max_now=(cnt==h_in*w_in-1);
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

wire cnt_done=mean_vld&dat_square_acc_vld&cnt_is_max_now;

////// end count ///////

wire [`MAX_DAT_DW*2+4-1:0]tp_variance=dat_square_acc-mean_square;
generate_vld_shift #
(
    .DATA_WIDTH((`MAX_DAT_DW*2+4)+`MAX_DAT_DW+2+`Log2_LN_pixel),
    .DEPTH(`Sqrt_delay)
)shift_VLD_reg
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({tp_variance, mean,       mean_vld&dat_square_acc_vld, cnt_done,     cnt}),
    .data_out({variance,   mean_delay, std_deviation_vld,      variance_done, wr_addr})
);

sqrt_unsigned u_sqrt //6clk 
(
    .clk(clk),
    .rst_n(rst_n),
    .valid_in(mean_vld),
    .x(tp_variance),
    .y_10Q10(std_deviation)
);
//cordic_0 u_cordic //6clk 
//(
//  .aclk(clk),
//  .s_axis_cartesian_tvalid(mean_vld),
//  .s_axis_cartesian_tdata(tp_variance),
//  .m_axis_dout_tvalid(),
//  .m_axis_dout_tdata(std_deviation)
//);

endmodule
