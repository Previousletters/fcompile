`include "CNN_defines.vh"


module fsm_wt
(
	input clk,
	input rst_n,
    
	//from CSR
	input start,
	input [`log2_K-1:0]Kx,
	input [`log2_K-1:0]Ky,
	input [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin,				//ceil(CH_in/Tin)
	input [`log2_CH-1:0]CH_out,
	input [`log2_CH-`log2Tout-1:0]CH_out_div_Tout,				//ceil(CH_out/Tout)
	input [`log2_H-1:0]Hout,	//Hout
    input [(`log2_W+`log2_H-`log2Tout-1):0]Hout_x_Wout_div_Tout,			//Wout*Hout/Tout

	//to CSR
	output done,

	//wt_addr to BUF
	output wt_addr_vld,
	output [`log2BUF_DEP-1:0]wt_addr,
	output wt_last_comb,
	input wt_addr_rdy,

	//wt to CMAC
	output [`log2Tout-1:0]wt_cnt
	//output wt_last
);

wire [`log2Tout-1:0]max_num_of_chout_now;

reg [`log2Tout-1:0]tout;// tout ranges from 0 to Tout
wire tout_is_max_now= (tout==max_num_of_chout_now);
wire tout_will_update_now=wt_addr_vld&wt_addr_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    tout<='d0;
else
    if(tout_will_update_now)
    begin
        if(tout_is_max_now)
            tout<='d0;
        else
            tout<=tout+'d1;
    end

reg [`log2_KyKx-1:0]Ky_mul_Kx;
reg [`log2_KyKx+`log2_CH-1:0]Ky_mul_Kx_mul_CHin_div_Tin;
always @(posedge clk)
begin
    Ky_mul_Kx<=Kx*Ky;
    Ky_mul_Kx_mul_CHin_div_Tin<=Ky_mul_Kx*CH_in_div_Tin;
end

reg [`log2_KyKx+`log2_CH-1:0]KyKxCHin_div_Tin;
wire KyKxCHin_div_Tin_is_max_now= (KyKxCHin_div_Tin==Ky_mul_Kx_mul_CHin_div_Tin-1);
wire KyKxCHin_div_Tin_will_update_now= tout_is_max_now&tout_will_update_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    KyKxCHin_div_Tin<='d0;
else
    if(KyKxCHin_div_Tin_will_update_now)
    begin
        if(KyKxCHin_div_Tin_is_max_now)
            KyKxCHin_div_Tin<='d0;
        else
            KyKxCHin_div_Tin<=KyKxCHin_div_Tin+'d1;
    end

reg [(`log2_W+`log2_H-`log2Tout-1):0]p;
wire p_is_max_now=(p==(Hout_x_Wout_div_Tout-1));
wire p_will_update_now=KyKxCHin_div_Tin_is_max_now&KyKxCHin_div_Tin_will_update_now;
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
    
reg [`log2_CH-1:0]chout;//index of group output height
reg [`log2_CH-1:0]chout_bias;
wire chout_is_max_now= (chout==CH_out_div_Tout-1);
wire chout_will_update_now= (p_is_max_now&p_will_update_now);
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    chout<='d0;
    chout_bias <= 'b0;
end
else
begin
    if(chout_will_update_now)
    begin
        if(chout_is_max_now)
        begin
            chout<='d0;
            chout_bias <= 'd0;
        end
        else
        begin
            chout<=chout+'d1;
            chout_bias <= chout_bias + `Tout;
        end
    end   
end

//assign max_num_of_chout_now=chout_is_max_now?(CH_out[`log2Tout-1:0]-1):(`Tout-1);
assign max_num_of_chout_now=chout_is_max_now?((CH_out-1) - chout_bias):(`Tout-1);

reg [`log2BUF_DEP-1:0]addr;
reg [`log2BUF_DEP-1:0]addr_bias;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    begin
        addr<='d0;
        addr_bias<='d0;
    end
else
    if(start)
    begin
        addr<='d0;
        addr_bias<='d0;
    end
    else
        if(tout_will_update_now)
        begin
            if(chout_will_update_now)
            begin
                addr<=addr+1;
                addr_bias<=addr+1;
            end
            else
                if(p_will_update_now)
                    addr<=addr_bias;
                else
                    addr<=addr+1;
        end

reg wt_fsm_working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    wt_fsm_working<=1'b0;
else
    if(start)
        wt_fsm_working<=1'b1;
    else
        if(done)
            wt_fsm_working<=1'b0;

assign done=chout_is_max_now&chout_will_update_now;
assign wt_addr_vld=wt_fsm_working;
assign wt_addr=addr;

wire [`log2Tout-1:0]wt_cnt_comb=tout;
assign wt_last_comb=KyKxCHin_div_Tin_will_update_now;

(* MAX_FANOUT=128 *)reg [`TOTAL_BUF_RD_LATENCY*`log2Tout-1:0]wt_cnt_d;
assign wt_cnt=wt_cnt_d[`TOTAL_BUF_RD_LATENCY*`log2Tout-1 -: `log2Tout];
//assign wt_last=wt_last_d[`TOTAL_BUF_RD_LATENCY-1];

generate
if(`TOTAL_BUF_RD_LATENCY==1)
    begin
        always @(posedge clk or negedge rst_n)
        if(~rst_n)
            wt_cnt_d<=0;
        else
            wt_cnt_d<=wt_cnt_comb;
    end
else
    begin
        always @(posedge clk or negedge rst_n)
        if(~rst_n)
            wt_cnt_d<=0;
        else
            wt_cnt_d<={wt_cnt_d[(`TOTAL_BUF_RD_LATENCY-1)*`log2Tout-1:0],wt_cnt_comb};
    end
endgenerate

endmodule
