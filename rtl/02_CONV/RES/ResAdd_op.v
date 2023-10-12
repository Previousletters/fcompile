`include "CNN_defines.vh"


module ResAdd_op
(
	input clk,
	input rst_n,

	//from CSR
	input [`log2_scale-1:0]short_cut_scale,
	input [`log2_scale-1:0]main_dat_scale,
	input [`log2_scale-1:0]res_add_shift,
	input [1:0]ew_op_mode,		//0: add, 1: mul, 2: minus, 3: not defined
    input relu_en,
    
	//input data stream
	input s0_dat_vld,
	input [`Tout*`MAX_BN_DW-1:0]s0_dat_pd,
	output s0_dat_rdy,

	input s1_dat_vld,
	input [`Tout*`MAX_DAT_DW-1:0]s1_dat_pd,
	output s1_dat_rdy,

	//output data stream
	output dat_out_vld,
	output reg [`Tout*`MAX_DAT_DW-1:0]dat_out_pd,
	input dat_out_rdy
);

wire hs_fifo_data_in_vld;
reg [`Tout*(`MAX_DAT_DW+`MAX_BN_DW)-1:0]hs_fifo_data_in;
wire hs_fifo_data_in_rdy;

wire hs_fifo_data_out_vld;
wire [`Tout*(`MAX_DAT_DW+`MAX_BN_DW)-1:0]hs_fifo_data_out;
wire hs_fifo_data_out_rdy;

assign hs_fifo_data_in_vld=s0_dat_vld&s1_dat_vld;

assign s0_dat_rdy=hs_fifo_data_in_vld&hs_fifo_data_in_rdy;
assign s1_dat_rdy=s0_dat_rdy;

integer i;
reg [(`MAX_DAT_DW+`MAX_BN_DW-2):0]tp1;//shifted s0
reg [(`MAX_DAT_DW+`MAX_BN_DW-2):0]tp2;//shifted and inverted(?) s1
reg [(`MAX_DAT_DW+`MAX_BN_DW-1):0]tp3;//OP(s0,s1)
always @(*)
begin
	for(i=0;i<`Tout;i=i+1)
	begin
		tp1=$signed(s0_dat_pd[(`MAX_BN_DW)*i+:(`MAX_BN_DW)])<<<short_cut_scale;
		tp2=$signed(s1_dat_pd[(`MAX_DAT_DW)*i+:(`MAX_DAT_DW)])<<<main_dat_scale;
		//if(ew_op_mode==2'd2)
		//	tp2=(~tp2)+32'b1;
		case(ew_op_mode)
			2'd0:tp3=$signed(tp1)+$signed(tp2);
		//	2'd1:tp3=$signed(tp1)*$signed(tp2);
		//	2'd2:tp3=tp1-tp2;
		//	2'd3:tp3=0;
		    default:tp3=0;
		endcase
		hs_fifo_data_in[(`MAX_DAT_DW+`MAX_BN_DW)*i+:(`MAX_DAT_DW+`MAX_BN_DW)]=tp3;
		////right shift and round
		//tp4=$signed(tp3)>>>res_add_shift;
		//if((res_add_shift!=0)&&(tp4!=32'h7fff_ffff))
		//	tp4=tp4+tp3[res_add_shift-1];

		////saturate
		//if(tp4[31]&&(!(&tp4[30:15])))//tp4 is negetive and tp4<16'h8000
		//	tp_sat=16'h8000;
		//else
		//	if((!tp4[31])&&(|tp4[30:15]))//tp4 is positive and tp4>16'h7fff
		//		tp_sat=16'h7fff;
		//	else
		//		tp_sat=tp4[15:0];

		//dat_out_pd[16*i+:16]=tp_sat;
	end
end

hs_fifo #
(
	.DATA_WIDTH(`Tout*(`MAX_DAT_DW+`MAX_BN_DW)),
	.FIFO_DEPTH(2)
)hs_fifo
(
    .clk(clk),
    .rst_n(rst_n),
    
    .data_in_vld(hs_fifo_data_in_vld),
    .data_in(hs_fifo_data_in),
    .data_in_rdy(hs_fifo_data_in_rdy),
    
    .data_out_vld(hs_fifo_data_out_vld),
    .data_out(hs_fifo_data_out),
    .data_out_rdy(hs_fifo_data_out_rdy)
);

assign dat_out_vld=hs_fifo_data_out_vld;
assign hs_fifo_data_out_rdy=dat_out_rdy;

reg [(`MAX_DAT_DW+`MAX_BN_DW-1):0]tp3_2,tp3_3;
reg [(`MAX_DAT_DW+`MAX_BN_DW-1):0]tp4;
reg [(`MAX_DAT_DW-1):0]tp_sat;


integer ii;
always @(*)
begin
	for(ii=0;ii<`Tout;ii=ii+1)
	begin:shift
		//right shift and round
		tp3_2=hs_fifo_data_out[(`MAX_DAT_DW+`MAX_BN_DW)*ii+:(`MAX_DAT_DW+`MAX_BN_DW)];
		tp3_3=$signed(tp3_2)>>>res_add_shift;
		if((res_add_shift!=0)&&(tp3_3!={1'b0, {((`MAX_DAT_DW+`MAX_BN_DW)-1){1'b1}} }))
			tp4=tp3_3+tp3_2[res_add_shift-1];

		//saturate
		if(tp4[((`MAX_DAT_DW+`MAX_BN_DW)-1)]&&(!(&tp4[((`MAX_DAT_DW+`MAX_BN_DW)-2):(`MAX_DAT_DW-1)])))//tp4 is negetive and tp4<16'h8000
			tp_sat={1'b1, {(`MAX_DAT_DW-1){1'b0}} };
		else
			if((!tp4[((`MAX_DAT_DW+`MAX_BN_DW)-1)])&&(|tp4[((`MAX_DAT_DW+`MAX_BN_DW)-2):(`MAX_DAT_DW-1)]))//tp4 is positive and tp4>16'h7fff
				tp_sat={1'b0, {(`MAX_DAT_DW-1){1'b1}} };
			else
				tp_sat=tp4[(`MAX_DAT_DW-1):0];

		dat_out_pd[(`MAX_DAT_DW)*ii+:(`MAX_DAT_DW)]=(relu_en&tp_sat[`MAX_DAT_DW-1])? `MAX_DAT_DW'b0: tp_sat;
	end
end

endmodule

