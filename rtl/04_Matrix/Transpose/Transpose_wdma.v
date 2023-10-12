`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module Transpose_wdma
(
	input clk,
	input rst_n,
    
	//from CSR
	input start,
	input [31:0]wt_num_div_Tout,	
	input [31:0]wt_base_addr,
	
	//to CSR
	output wdma_done,

	//from Trans_mac_and_acc
	input dat_in_vld,
	input [`Tout*`MAX_DAT_DW-1:0]dat_in_pd,
	output dat_in_rdy,

	//write path to MCIF
	output Trans2mcif_wr_req_vld,
	input Trans2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Trans2mcif_wr_req_pd,
	input Trans2mcif_wr_rsp_complete
);


wire [`log2AXI_BURST_LEN-1:0]cmd_length;
reg cmd_en,dat_en;

reg [`log2AXI_BURST_LEN-1:0]burst_len_cnt;
wire burst_len_cnt_will_update_now=dat_in_vld&dat_in_rdy;
wire burst_len_cnt_is_max_now=(burst_len_cnt==cmd_length);
always @(posedge clk or negedge rst_n)
if(~rst_n)
	burst_len_cnt<=0;
else
	if(burst_len_cnt_will_update_now)
	begin
		if(burst_len_cnt_is_max_now)
			burst_len_cnt<=0;
		else
			burst_len_cnt<=burst_len_cnt+1;
	end
	
	
reg [31:0]cnt;
wire cnt_will_update_now=burst_len_cnt_will_update_now&burst_len_cnt_is_max_now;
wire cnt_is_max_now=(cnt==((wt_num_div_Tout-1)>>`log2AXI_BURST_LEN) );
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

wire done=cnt_will_update_now&cnt_is_max_now;
reg working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	working<=1'b0;
else
	if(start)
		working<=1'b1;
	else
		if(done)
			working<=1'b0;


always @(posedge clk or negedge rst_n)
if(~rst_n)
	{cmd_en,dat_en}<=2'b10;
else
	if(cmd_en & Trans2mcif_wr_req_vld & Trans2mcif_wr_req_rdy)
		{cmd_en,dat_en}<=2'b01;
	else
		if(burst_len_cnt_will_update_now&burst_len_cnt_is_max_now)
			{cmd_en,dat_en}<=2'b10;


assign dat_in_rdy=dat_en&Trans2mcif_wr_req_rdy;

assign cmd_length=(cnt_is_max_now?(wt_num_div_Tout[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1));
wire [31:0]cmd_addr=cnt*(`AXI_BURST_LEN*(`MAX_DAT_DW *`Tout)/8);

wire cmd_nonposted=cnt_is_max_now;


wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]cmd_req_pd={1'b1,{(`MAX_DAT_DW *`Tout-32){1'b0}},wt_base_addr,cmd_nonposted,cmd_length,cmd_addr};
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]dat_req_pd={{(2+`log2AXI_BURST_LEN+32){1'b0}},dat_in_pd};
assign Trans2mcif_wr_req_vld=(dat_en&dat_in_vld) | (working&cmd_en);
assign Trans2mcif_wr_req_pd=cmd_en?cmd_req_pd:dat_req_pd;

assign wdma_done=Trans2mcif_wr_rsp_complete;

endmodule
