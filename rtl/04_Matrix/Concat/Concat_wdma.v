`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module Concat_wdma
(
	input clk,
	input rst_n,
    
	//from CSR
	input start,
	input [`log2_W-1:0]w_in,
	input [`log2_CH-`log2Tout-1:0]ch_div_Tout,
	
	input [31:0]feature_base_addr,
	input [25:0]feature_surface_stride,
	input [15:0]feature_line_stride,
	
	//to CSR
	output wdma_done,

	//from Concat_mac_and_acc
	input dat_in_vld,
	input [`Tout*`MAX_DAT_DW-1:0]dat_in_pd,
	output dat_in_rdy,

	//write path to MCIF
	output Concat2mcif_wr_req_vld,
	input Concat2mcif_wr_req_rdy,
	output [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]Concat2mcif_wr_req_pd,
	input Concat2mcif_wr_rsp_complete
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


reg [`log2_W-`log2AXI_BURST_LEN-1:0]burst_times_cnt; //Output Wout dim.  Each Tout block contains several Burst_len. 
reg [31:0] burst_times_cnt_bias;
wire burst_times_cnt_will_update_now=burst_len_cnt_will_update_now&burst_len_cnt_is_max_now;
wire burst_times_cnt_is_max_now=(burst_times_cnt==w_in>>`log2AXI_BURST_LEN);
always @(posedge clk or negedge rst_n)
begin
    if(~rst_n)
    begin
        burst_times_cnt<=0;
        burst_times_cnt_bias<=0;
    end
    else
        if(burst_times_cnt_will_update_now)
        begin
            if(burst_times_cnt_is_max_now)
            begin
                burst_times_cnt<=0;
                burst_times_cnt_bias<=0;
            end
            else
            begin
                burst_times_cnt<=burst_times_cnt+1;
                burst_times_cnt_bias<=burst_times_cnt_bias+(`AXI_BURST_LEN)*(`Pixel_Data_Bytes);
            end
        end
end


reg [`log2_CH-1:0]ch_cnt;
reg [31:0]ch_cnt_bias;
wire ch_cnt_will_update_now=burst_times_cnt_will_update_now&burst_times_cnt_is_max_now;
wire ch_cnt_is_max_now=(ch_cnt==ch_div_Tout-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
	begin
		ch_cnt<=0;
		ch_cnt_bias<=0;
	end
	else
		if(ch_cnt_will_update_now)
		begin
			if(ch_cnt_is_max_now)
			begin
				ch_cnt<=0;
				ch_cnt_bias<=0;
			end
			else
			begin
				ch_cnt<=ch_cnt+1;
				ch_cnt_bias<=ch_cnt_bias+feature_surface_stride;
			end
		end
end


wire done=ch_cnt_will_update_now&ch_cnt_is_max_now;

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
	if(cmd_en & Concat2mcif_wr_req_vld & Concat2mcif_wr_req_rdy)
		{cmd_en,dat_en}<=2'b01;
	else
		if(burst_len_cnt_will_update_now&burst_len_cnt_is_max_now)
			{cmd_en,dat_en}<=2'b10;


assign dat_in_rdy=dat_en&Concat2mcif_wr_req_rdy;

assign cmd_length=(burst_times_cnt_is_max_now?(w_in[`log2AXI_BURST_LEN-1:0]):(`AXI_BURST_LEN-1));
wire [31:0]cmd_addr=burst_times_cnt_bias+ch_cnt_bias;
					
wire cmd_nonposted=burst_times_cnt_is_max_now&ch_cnt_is_max_now;


wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]cmd_req_pd={1'b1,{(`MAX_DAT_DW *`Tout-32){1'b0}},feature_base_addr,cmd_nonposted,cmd_length,cmd_addr};
wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]dat_req_pd={{(2+`log2AXI_BURST_LEN+32){1'b0}},dat_in_pd};
assign Concat2mcif_wr_req_vld=(dat_en&dat_in_vld) | (working&cmd_en);
assign Concat2mcif_wr_req_pd=cmd_en?cmd_req_pd:dat_req_pd;

assign wdma_done=Concat2mcif_wr_rsp_complete;

endmodule
