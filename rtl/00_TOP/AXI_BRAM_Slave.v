`include "CNN_defines.vh"

module AXI_BRAM_Slave
(
	input wire clk,
	input wire rst_n,
	input wire wr_req_vld,
	input wire [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]wr_req_pd,
	output wire wr_complete,
	input [32-1:0]onchip_write_base_addr,
	
	input [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd,
	input rd_req_vld,
	output rd_req_rdy,
	input [32-1:0]onchip_read_base_addr,
	output rd_dat_vld,
	input rd_dat_rdy,
	output reg[`MAX_DAT_DW *`Tout-1:0]rd_dat
	
);

reg  [`MAX_DAT_DW *`Tout-1:0] memory [(1<<`log2_AUX_BRAM_DEPTH)-1:0];

/*  start write */
//cmd_req_pd={1'b1,{(`MAX_DAT_DW *`Tout-32){1'b0}},out_addr,cmd_nonposted,cmd_length,cmd_addr};
//dat_req_pd={{(2+`log2AXI_BURST_LEN+32){1'b0}},tp_dat};

wire wr_cmd_vld=wr_req_vld&wr_req_pd[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]==1;
reg [31:0]wr_addr_in_bytes;
reg [`log2AXI_BURST_LEN-1:0]wr_len;
wire wr_dat_vld=wr_req_vld&wr_req_pd[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]==0;
wire [`MAX_DAT_DW *`Tout-1:0]wr_dat=wr_dat_vld?wr_req_pd[`MAX_DAT_DW *`Tout-1:0]:0;
wire [`log2_AUX_BRAM_DEPTH-1:0]wr_addr=(wr_addr_in_bytes+onchip_write_base_addr)>>(`log2Tout+`MAX_log2DW-3);

always @(posedge clk)
if(wr_cmd_vld)
begin
    wr_addr_in_bytes<=wr_req_pd[31:0];
    wr_len<=wr_req_pd[32+`log2AXI_BURST_LEN-1:32];
end

wire wr_done;
reg wr_working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	wr_working<=1'b0;
else
	if(wr_cmd_vld)
		wr_working<=1'b1;
	else
		if(wr_done)
			wr_working<=1'b0;

            
reg [`log2AXI_BURST_LEN-1:0]wr_cnt;
wire wr_cnt_will_update_now=wr_dat_vld&wr_working;
wire wr_cnt_is_max_now=(wr_cnt==wr_len);
always @(posedge clk or negedge rst_n)
if(~rst_n)
	wr_cnt<=0;
else
	if(wr_cnt_will_update_now)
	begin
		if(wr_cnt_is_max_now)
			wr_cnt<=0;
		else
			wr_cnt<=wr_cnt+1;
	end
assign wr_done=wr_cnt_will_update_now&wr_cnt_is_max_now;

wire last_wr_cmd=wr_cmd_vld&wr_req_pd[`log2AXI_BURST_LEN+32];
reg last_wr_working;
wire last_wr_done;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	last_wr_working<=1'b0;
else
	if(last_wr_cmd)
		last_wr_working<=1'b1;
	else
		if(last_wr_done)
			last_wr_working<=1'b0;
			
assign last_wr_done=last_wr_working&wr_done;
assign wr_complete=last_wr_done;

wire [`log2_AUX_BRAM_DEPTH-1:0]current_wr_addr=wr_addr+wr_cnt;
always @(posedge clk)
if(wr_dat_vld)
	memory[current_wr_addr]<=wr_dat;


/*  start read */
wire rd_cmd_vld=rd_req_vld&rd_req_rdy;
reg [31:0]rd_addr_in_bytes;
reg [`log2AXI_BURST_LEN-1:0]rd_len;
wire [`log2_AUX_BRAM_DEPTH-1:0] rd_addr=(rd_addr_in_bytes+onchip_read_base_addr)>>(`log2Tout+`MAX_log2DW-3);
always @(posedge clk)
if(rd_cmd_vld)
begin
    rd_addr_in_bytes<=rd_req_pd[31:0];
    rd_len<=rd_req_pd[64+`log2AXI_BURST_LEN-1:64];
end

//reg [`log2AXI_BURST_LEN-1:0]cnt;
//always @(posedge clk or negedge rst_n)
//if(~rst_n)
//	cnt<=0;
//else
//	if(1'b1)
//	begin
//		if(cnt==2)
//			cnt<=0;
//		else
//			cnt<=cnt+1;
//	end

wire rd_done;
reg rd_working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	rd_working<=1'b0;
else
	if(rd_cmd_vld)
		rd_working<=1'b1;
	else
		if(rd_done)
			rd_working<=1'b0;

assign rd_dat_vld=rd_working&rd_dat_rdy;

reg [`log2AXI_BURST_LEN-1:0]rd_cnt;
wire rd_cnt_will_update_now=rd_dat_vld&rd_dat_rdy&rd_working;
wire rd_cnt_is_max_now=(rd_cnt==rd_len);
always @(posedge clk or negedge rst_n)
if(~rst_n)
	rd_cnt<=0;
else
	if(rd_cnt_will_update_now)
	begin
		if(rd_cnt_is_max_now)
			rd_cnt<=0;
		else
			rd_cnt<=rd_cnt+1;
	end

assign rd_done=rd_cnt_will_update_now&rd_cnt_is_max_now;
assign rd_req_rdy=~rd_working;

wire [`log2_AUX_BRAM_DEPTH-1:0]current_rd_addr=rd_addr+rd_cnt;
always @(posedge clk)
if(rd_dat_vld&rd_dat_rdy)
	rd_dat<=memory[current_rd_addr];




endmodule
