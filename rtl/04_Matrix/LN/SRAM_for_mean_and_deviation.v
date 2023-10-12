`include "CNN_defines.vh"

module SRAM_for_mean_and_deviation
(
	input clk,
	input rst_n,

	//dat from stage-1
	input dat_in_vld,
 	input [`MAX_DAT_DW-1:0]mean_dat_in,
 	input [`MAX_DAT_DW+2-1:0]deviation_dat_in,
    input [`Log2_LN_pixel-1:0]wr_addr,
    
	input rd_en,
	input [`Log2_LN_pixel-1:0]rd_addr,
	output [`MAX_DAT_DW  -1:0] mean_dat_out,
	output [`MAX_DAT_DW*2-1:0] recip_deviation_dat_out,
	output dat_out_vld
);
wire sqrt_is_zero,sqrt_is_zero_delay;
assign sqrt_is_zero=deviation_dat_in==0;
wire [`MAX_DAT_DW*2-1:0]recip_sqrt,tp_recip_sqrt;
wire [2-1:0]reminder;

//div_gen_0 u_div //16clk
//(
//    .aclk(clk),
//    .s_axis_dividend_tdata(16'b1111_1111_1111_1111),
//    .s_axis_dividend_tvalid(dat_in_vld),
//    .s_axis_divisor_tdata(deviation_dat_in),
//    .s_axis_divisor_tvalid(dat_in_vld),
    
//    .m_axis_dout_tdata({tp_recip_sqrt,reminder}),
//    .m_axis_dout_tvalid()
//);

divider_main # //16clk
(
    .N(`MAX_DAT_DW*2),           //dividend
    .M(`MAX_DAT_DW+2),           //divisor
    .N_ACT(`MAX_DAT_DW*3+1)
)divider
(
  .clk(clk),
  .rstn(rst_n),

  .en(dat_in_vld),
  .dividend(16'b1111_1111_1111_1111),
  .divisor(deviation_dat_in),

  .quotient(tp_recip_sqrt)
);

wire [`MAX_DAT_DW-1:0]mean;
wire wr_en;
wire [`Log2_LN_pixel-1:0]tp_wr_addr;
generate_vld_shift #
(
    .DATA_WIDTH(`MAX_DAT_DW+1+`Log2_LN_pixel+1),
    .DEPTH(`LN_div_delay)
)shift_VLD_reg
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({dat_in_vld,mean_dat_in,wr_addr,sqrt_is_zero}),
    .data_out({wr_en,mean,tp_wr_addr,sqrt_is_zero_delay})
);

assign recip_sqrt=sqrt_is_zero_delay?16'b1111_1111_1111_1111:tp_recip_sqrt;

wire [`MAX_DAT_DW*3-1:0]wr_dat={mean,recip_sqrt};

wire [`MAX_DAT_DW*3-1:0]rd_dat;
wire rd_dat_vld;


center_buf  #
(
   .DATA_WIDTH(`MAX_DAT_DW*3),
   .DEPTH((1<<`Log2_LN_pixel)),
   .log2_DEPTH(`Log2_LN_pixel)
)u_center_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    //Wr Port
    .wr_en(wr_en),
    .wr_addr(tp_wr_addr),
    .wr_dat(wr_dat),

    //Rd Port
    .rd_en(rd_en),
    .rd_addr(rd_addr),
    .rd_dat_vld(rd_dat_vld),
    .rd_dat(rd_dat)
);   

assign mean_dat_out=rd_dat[`MAX_DAT_DW*3-1:`MAX_DAT_DW*2];
assign recip_deviation_dat_out=rd_dat[`MAX_DAT_DW*2-1:0];
assign dat_out_vld=rd_dat_vld;

endmodule
