`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module LN_mean_acc
(
	input clk,
	input rst_n,

	//dat from CMAC
	input dat_vld_i,
 	input [`MAX_DAT_DW+`log2Tout-1:0]dat_i,
    input [`MAX_DAT_DW*2:0]recip_CH_int16,
    
	input Stripe_loop_end,
	input CH_acc_max_now,
	input CH_and_Stripe_loop_end,

	//dat to SDP
	output reg dat_out_vld,
	output reg [`MAX_DAT_DW-1:0]dat_out,
	output reg [`MAX_DAT_DW*2-1:0]dat_out_square
);

wire wr_en;
wire [`log2Tout-1:0]wr_addr;
wire [(`MAX_DAT_DW+`Log2_LN_ch_max)-1:0]wr_dat;
wire rd_en;
wire [`log2Tout-1:0]rd_addr;
wire [(`MAX_DAT_DW+`Log2_LN_ch_max)-1:0]rd_dat;
wire rd_dat_vld;


center_buf  #
(
   .DATA_WIDTH((`MAX_DAT_DW+`Log2_LN_ch_max)),
   .DEPTH(`Tout),
   .log2_DEPTH(`log2Tout)
)u_center_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    //Wr Port
    .wr_en(wr_en),
    .wr_addr(wr_addr),
    .wr_dat(wr_dat),

    //Rd Port
    .rd_en(rd_en),
    .rd_addr(rd_addr),
    .rd_dat_vld(rd_dat_vld),
    .rd_dat(rd_dat)                
);   

reg [`log2Tout-1:0]ptr;
reg [`log2Tout-1:0]ptr_d1;
always @(posedge clk) ptr_d1<=ptr;

reg load_new_data, load_new_data_d1;
reg [(`MAX_DAT_DW+`Log2_LN_ch_max)-1:0]adder_out;
reg [(`MAX_DAT_DW+`Log2_LN_ch_max)-1:0]adder_in_0;
reg [(`MAX_DAT_DW+`Log2_LN_ch_max)-1:0]origin_dat_in;
reg [`MAX_DAT_DW+`log2Tout-1:0]adder_in_1;

always @(posedge clk) load_new_data_d1<=load_new_data;
always @(*) adder_in_0=load_new_data_d1?0:rd_dat;

always @(posedge clk)
    adder_in_1<=dat_i;
	
always @(*)
begin
    adder_out= $signed(adder_in_0)+$signed(adder_in_1);
    origin_dat_in=$signed(dat_i);
end

always @(posedge clk or negedge rst_n)
if(~rst_n)
    load_new_data<=1'b1;
else
    if(CH_and_Stripe_loop_end & dat_vld_i)
        load_new_data<=1'b1;
    else
        if(Stripe_loop_end & dat_vld_i)
            load_new_data<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
    ptr<='d0;
else
    if(Stripe_loop_end & dat_vld_i)
        ptr<='d0;
    else
        if(dat_vld_i)
            ptr<=ptr+'d1;

assign rd_en=1'b1;
assign rd_addr=ptr;

reg dat_vld_i_d1;
always @(posedge clk) dat_vld_i_d1<=dat_vld_i;

assign wr_en=(dat_vld_i&load_new_data)|(dat_vld_i_d1&(!load_new_data));
assign wr_addr=(dat_vld_i&load_new_data)?ptr:((dat_vld_i_d1&(!load_new_data))?ptr_d1:0);
assign wr_dat=(dat_vld_i&load_new_data)?origin_dat_in:((dat_vld_i_d1&(!load_new_data))?adder_out:0);


///////////////////////////
reg [(`MAX_DAT_DW*2+`Log2_LN_ch_max)-1:0]tp_dat_out;
reg [`MAX_DAT_DW-1:0]tp_mean;
reg [(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:0]tp_dat_square;
reg [(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:0]round_dat_square;
reg [`MAX_DAT_DW*2-1:0]tp_dat_square_out;

always @(posedge clk)
    tp_dat_out<=$signed(adder_out)*$signed({1'b0,recip_CH_int16});

always @(posedge clk)
    tp_mean<=tp_dat_out[(`MAX_DAT_DW*2+`Log2_LN_ch_max-1):`MAX_DAT_DW*2]+tp_dat_out[(`MAX_DAT_DW*2-1)];


always @(posedge clk)
    tp_dat_square<=$signed(tp_dat_out)*$signed(tp_dat_out);


always@(*)
begin
    round_dat_square=tp_dat_square[(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:`MAX_DAT_DW*4] + tp_dat_square[(`MAX_DAT_DW*4-1)];
    
    if( (!round_dat_square[(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1]) & (|round_dat_square[((`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-2):(`MAX_DAT_DW*4-1)]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
        tp_dat_square_out<={1'b0,{(`MAX_DAT_DW*2-1){1'b1}}};//`MAX_DW'h7fff;
    else
        tp_dat_square_out<=round_dat_square[(`MAX_DAT_DW*2-1):0];   
end

always @(posedge clk)
begin
	dat_out<=tp_mean;
    dat_out_square<=tp_dat_square_out;
end


reg dat_out_vld_d0,dat_out_vld_d1,dat_out_vld_d2;      
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	dat_out_vld_d0<=1'b0;
	dat_out_vld_d1<=1'b0;
	dat_out_vld_d2<=1'b0;
	dat_out_vld<=1'b0;
end
else
begin
	dat_out_vld_d0<=dat_vld_i&CH_acc_max_now;
    dat_out_vld_d1<=dat_out_vld_d0;
    dat_out_vld_d2<=dat_out_vld_d1;
    dat_out_vld<=dat_out_vld_d2;
end


endmodule
