`include "CNN_defines.vh"

module Transpose_fsm
(
	input clk,
	input rst_n,
    
    input [`Log2_Trans_pixel-1:0]pixel_in,//197
    input [`Log2_Trans_pixel-1:0]pixel_out,//192
    
    output rd_vld,
    output [`base_log2Tin-1:0]rd_addr,
    output [`base_log2Tin:0]rd_addr_max,
    input  rd_rdy,

    input credit_vld
);

//////////////////// output and read BUF////////////////////////

reg [2:0]credit_cnt;
wire FSM_go=credit_cnt!=0;
wire credit_cnt_decrease=FSM_go&rd_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	credit_cnt<=3;
else
	case({credit_cnt_decrease,credit_vld})
	2'b01:begin if(credit_cnt!=3) credit_cnt<=credit_cnt+1;end
	2'b10:credit_cnt<=credit_cnt-1;
	endcase
	

reg [`base_log2Tin-1:0]block_in_cnt;
wire block_in_cnt_will_update_now=FSM_go&rd_rdy;
wire block_in_cnt_is_max_now=(block_in_cnt==`base_Tin/`Tout-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		block_in_cnt<=0;
	else
		if(block_in_cnt_will_update_now)
		begin
			if(block_in_cnt_is_max_now)
				block_in_cnt<=0;
			else
				block_in_cnt<=block_in_cnt+1;
		end
end

wire [`log2Tout-1:0]current_out_len_minus1;
reg [`log2Tout-1:0]block_out_cnt;
wire block_out_cnt_will_update_now=block_in_cnt_will_update_now&block_in_cnt_is_max_now;
wire block_out_cnt_is_max_now=(block_out_cnt==current_out_len_minus1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		block_out_cnt<=0;
	else
		if(block_out_cnt_will_update_now)
		begin
			if(block_out_cnt_is_max_now)
				block_out_cnt<=0;
			else
				block_out_cnt<=block_out_cnt+1;
		end
end


wire [`log2_CH-1:0]CHin_minus1=pixel_out-1;//192
reg [`log2_CH-1:0]CHin_cnt;
wire CHin_cnt_will_update_now=block_out_cnt_will_update_now&block_out_cnt_is_max_now;
wire CHin_cnt_is_max_now=(CHin_cnt==CHin_minus1>>`base_log2Tin);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		CHin_cnt<=0;
	else
		if(CHin_cnt_will_update_now)
		begin
			if(CHin_cnt_is_max_now)
				CHin_cnt<=0;
			else
				CHin_cnt<=CHin_cnt+1;
		end
end


wire [`log2_CH-1:0]CHout_minus1=pixel_in-1;//197
reg [`log2_CH-1:0]CHout_cnt;
wire CHout_cnt_will_update_now=CHin_cnt_will_update_now&CHin_cnt_is_max_now;
wire CHout_cnt_is_max_now=(CHout_cnt==CHout_minus1>>`log2Tout);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		CHout_cnt<=0;
	else
		if(CHout_cnt_will_update_now)
		begin
			if(CHout_cnt_is_max_now)
				CHout_cnt<=0;
			else
				CHout_cnt<=CHout_cnt+1;
		end
end

wire rd_done=CHout_cnt_will_update_now&CHout_cnt_is_max_now;
assign current_out_len_minus1=CHout_cnt_is_max_now?(CHout_minus1[`log2Tout-1:0]):(`Tout-1);
assign rd_addr=block_out_cnt*(`base_Tin/`Tout)+block_in_cnt;
assign rd_addr_max=CHout_cnt_is_max_now?((current_out_len_minus1+1)*(`base_Tin/`Tout)):`base_Tin;
assign rd_vld=FSM_go&rd_rdy;

endmodule