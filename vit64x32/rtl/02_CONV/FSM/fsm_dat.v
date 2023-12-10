`include "CNN_defines.vh"


module fsm_dat
(
	input clk,
	input rst_n,
    
	//from CSR
	input start,
	input [`log2_P-1:0]pad_up,
	input [`log2_P-1:0]pad_left,
	input [`log2_S-1:0]Sx,
	input [`log2_S-1:0]Sy,
	input [`log2_K-1:0]Kx,
	input [`log2_K-1:0]Ky,
	input [`log2_CH-`base_log2Tin-1:0]CHin_div_Tin,				//ceil(CHin/Tin)
	input [`log2_CH-`log2Tout-1:0]CHout_div_Tout,				//ceil(CHout/Tout)
	input [`log2_W-1:0]Wout,
	input [(`log2_W+`log2_H-1):0]Hout_x_Wout,					//Wout*Hout
	input [`log2_H-1:0]Hout,	
    input [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout,			//Wout*Hout/Tout

	//to CSR
	output done,

	//to BUF for Backpressure
	output CHinKyKx_max_now_comb,

	//axis to BUF
	output feature_data_vld,
	output [`log2_H:0]feature_hin,
	output [`log2_W:0]feature_win,
	output [`log2_CH-1:0]feature_chin,
	output Wout_loop_end_comb,
	input feature_data_rdy,

	//data to CMAC
	output CHinKyKx_max_now,
	output Wout_loop_start,
	output Wout_loop_end,
	output CHinKyKxWout_loop_end
);

wire [`log2Tout-1:0]stripe_len_now;
reg [`log2Tout-1:0]pp;
wire pp_is_max_now= (pp==stripe_len_now);
wire pp_will_update_now=feature_data_vld&feature_data_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    pp<='d0;
else
    if(pp_will_update_now)
    begin
        if(pp_is_max_now)
            pp<='d0;
        else
            pp<=pp+'d1;
    end

reg [`log2_K-1:0]kx;
wire kx_is_max_now=(kx==Kx-1);
wire kx_will_update_now=(pp_is_max_now&pp_will_update_now);
always @(posedge clk or negedge rst_n)
if(~rst_n)
    kx<='d0;
else
    if(kx_will_update_now)
    begin
        if(kx_is_max_now)
            kx<='d0;
        else
            kx<=kx+'d1;
    end

reg [`log2_K-1:0]ky;
wire ky_is_max_now=(ky==Ky-1);
wire ky_will_update_now=kx_is_max_now&kx_will_update_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    ky<='d0;
else
    if(ky_will_update_now)
    begin
        if(ky_is_max_now)
            ky<='d0;
        else
            ky<=ky+'d1;
    end

reg [`log2_CH-1:0]chin; //index of group input channel
wire chin_is_max_now=(chin==(CHin_div_Tin-1));
wire chin_will_update_now=ky_is_max_now&ky_will_update_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    chin<='d0;
else
    if(chin_will_update_now)
    begin
        if(chin_is_max_now)
            chin<='d0;
        else
            chin<=chin+'d1;
    end

reg [(`log2_W+`log2_H-`log2Tout-1):0]p; //Wout*Hout/Tout
wire p_is_max_now=(p==(Hout_x_Wout_div_Tout-1));
wire p_will_update_now=chin_is_max_now&chin_will_update_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    p<='d0;
else
    if(p_will_update_now)
    begin
        if(p_is_max_now)
            p<='d0;
        else
            p<=p+'d1;
    end

assign stripe_len_now=p_is_max_now?(Hout_x_Wout[`log2Tout-1:0]-1):(`Tout-1);

reg [`log2_CH-1:0]chout;//index of group output channel
wire chout_is_max_now=(chout==(CHout_div_Tout-1));
wire chout_will_update_now=(p_is_max_now&p_will_update_now);
always @(posedge clk or negedge rst_n)
if(~rst_n)
    chout<='d0;
else
    if(chout_will_update_now)
    begin
        if(chout_is_max_now)
            chout<='d0;
        else
            chout<=chout+'d1;
    end

reg [`log2_W-1:0]w_restore;
reg [`log2_H-1:0]h_restore;
reg [`log2_W-1:0]w;//=cur_pixel%w_out;
reg [`log2_H-1:0]h;//=cur_pixel/w_out;

reg [`log2_W-1:0]w_inc;
reg [`log2_H-1:0]h_inc;
always @(*)
begin
    if(w==Wout-1)
    begin
        w_inc='d0;
        h_inc=h+'d1;
    end
    else
    begin
        w_inc=w+'d1;
        h_inc=h;
    end
end

always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    w_restore<='d0;
    h_restore<='d0;
    w<='d0;
    h<='d0;
end
else
    if(pp_will_update_now)
    begin
        if(p_will_update_now)
        begin
            if(p_is_max_now)
            begin
                w_restore<='d0;
                h_restore<='d0;
                w<='d0;
                h<='d0;
            end
            else
            begin
                w<=w_inc;
                h<=h_inc;
                w_restore<=w_inc;
                h_restore<=h_inc;
            end
        end
        else
        begin
            if(pp_is_max_now)
            begin
                w<=w_restore;
                h<=h_restore;
            end
            else
            begin
                w<=w_inc;
                h<=h_inc;
            end
        end
    end


reg dat_fsm_working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    dat_fsm_working<=1'b0;
else
    if(start)
        dat_fsm_working<=1'b1;
    else
        if(done)
            dat_fsm_working<=1'b0;

assign done=chout_is_max_now&chout_will_update_now;
assign feature_data_vld=dat_fsm_working;

assign feature_hin=h*Sy-pad_up+ky; //signed
assign feature_win=w*Sx-pad_left+kx; //signed
assign feature_chin=chin;

assign CHinKyKx_max_now_comb=kx_is_max_now&ky_is_max_now&chin_is_max_now;
assign Wout_loop_end_comb=(pp_is_max_now&pp_will_update_now);
wire CHinKyKxWout_loop_end_comb=chin_is_max_now&chin_will_update_now;
wire Wout_loop_start_comb=((pp==0)&pp_will_update_now);

generate_vld_shift #
(
    .DATA_WIDTH(4),
    .DEPTH(`TOTAL_BUF_RD_LATENCY)
)shift_reg
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({CHinKyKx_max_now_comb,Wout_loop_start_comb,Wout_loop_end_comb,CHinKyKxWout_loop_end_comb}),
    .data_out({CHinKyKx_max_now,Wout_loop_start,Wout_loop_end,CHinKyKxWout_loop_end})
);


endmodule
