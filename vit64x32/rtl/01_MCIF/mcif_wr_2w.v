`include "CNN_defines.vh"

module mcif_wr_2w #
(
	parameter M_AXI_ID_WIDTH=1,
	parameter M_AXI_DATA_WIDTH = (`MAX_DAT_DW *`Tout)
)
(
	input clk,
	input rst_n,

	//////////////////////////////////////////////////
	input wr_req_vld0,
	output wr_req_rdy0,
	input [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]wr_req_pd0,
	output wr_rsp_complete0,
	//////////////////////////////////////////////////
	input wr_req_vld1,
	output wr_req_rdy1,
	input [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]wr_req_pd1,
	output wr_rsp_complete1,
	//////////////////////////////////////////////////

	//AW channel
	output [M_AXI_ID_WIDTH-1 : 0] M_AXI_AWID,
	output [32-1 : 0] M_AXI_AWADDR,
	output [`log2AXI_BURST_LEN-1 : 0] M_AXI_AWLEN,
	output [2 : 0] M_AXI_AWSIZE,//=clogb2((M_AXI_DATA_WIDTH/8)-1);
	output [1 : 0] M_AXI_AWBURST,//=2'b01;
	output  M_AXI_AWLOCK,//1'b0;
	output [3 : 0] M_AXI_AWCACHE,//=4'b0010
	output [2 : 0] M_AXI_AWPROT,//=3'h0;
	output [3 : 0] M_AXI_AWQOS,//=4'h0;
	output  M_AXI_AWVALID,
	input  M_AXI_AWREADY,
	
	//Wr channel
	output [M_AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA,
	output [M_AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB,
	output  M_AXI_WLAST,
	output  M_AXI_WVALID,
	input  M_AXI_WREADY,
	input [M_AXI_ID_WIDTH-1 : 0] M_AXI_BID,//ignore
	input [1 : 0] M_AXI_BRESP,//ignore
	input  M_AXI_BVALID,//Bvalid and Bread means a a write response.
	output  M_AXI_BREADY//Bvalid and Bread means a a write response.
);

wire wr_cmd_fifo_0_data_in_rdy;
wire wr_cmd_fifo_1_data_in_rdy;

wire wr_dat_fifo_0_data_in_rdy;
wire wr_dat_fifo_1_data_in_rdy;

wire wr_cmd_fifo_0_data_out_vld;
wire wr_cmd_fifo_1_data_out_vld;

wire wr_cmd_fifo_0_data_out_rdy;
wire wr_cmd_fifo_1_data_out_rdy;

wire [`log2AXI_BURST_LEN+32+32+1-1:0]wr_cmd_fifo_0_data_out;
wire [`log2AXI_BURST_LEN+32+32+1-1:0]wr_cmd_fifo_1_data_out;

wire wr_dat_fifo_0_data_out_vld;
wire wr_dat_fifo_1_data_out_vld;

wire wr_dat_fifo_0_data_out_rdy;
wire wr_dat_fifo_1_data_out_rdy;

wire [`MAX_DAT_DW *`Tout-1:0]wr_dat_fifo_0_data_out;
wire [`MAX_DAT_DW *`Tout-1:0]wr_dat_fifo_1_data_out;

hs_fifo #
(
	.DATA_WIDTH(`log2AXI_BURST_LEN+32+32+1),
	.FIFO_DEPTH(4),
    .log2_FIFO_DEPTH(2)
)wr_cmd_fifo_0
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(wr_req_vld0&wr_req_pd0[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]),
	.data_in(wr_req_pd0[`log2AXI_BURST_LEN+32+32+1-1:0]),
	.data_in_rdy(wr_cmd_fifo_0_data_in_rdy),

	.data_out_vld(wr_cmd_fifo_0_data_out_vld),
	.data_out(wr_cmd_fifo_0_data_out),
	.data_out_rdy(wr_cmd_fifo_0_data_out_rdy)
);

hs_fifo #
(
	.DATA_WIDTH(`MAX_DAT_DW *`Tout),
	.FIFO_DEPTH(4),
    .log2_FIFO_DEPTH(2)
)wr_dat_fifo_0
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(wr_req_vld0&(~wr_req_pd0[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1])),
	.data_in(wr_req_pd0[`MAX_DAT_DW *`Tout-1:0]),
	.data_in_rdy(wr_dat_fifo_0_data_in_rdy),

	.data_out_vld(wr_dat_fifo_0_data_out_vld),
	.data_out(wr_dat_fifo_0_data_out),
	.data_out_rdy(wr_dat_fifo_0_data_out_rdy)
);

hs_fifo #
(
	.DATA_WIDTH(`log2AXI_BURST_LEN+32+32+1),
	.FIFO_DEPTH(4),
    .log2_FIFO_DEPTH(2)
)wr_cmd_fifo_1
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(wr_req_vld1&wr_req_pd1[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]),
	.data_in(wr_req_pd1[`log2AXI_BURST_LEN+32+32+1-1:0]),
	.data_in_rdy(wr_cmd_fifo_1_data_in_rdy),

	.data_out_vld(wr_cmd_fifo_1_data_out_vld),
	.data_out(wr_cmd_fifo_1_data_out),
	.data_out_rdy(wr_cmd_fifo_1_data_out_rdy)
);

hs_fifo #
(
	.DATA_WIDTH(`MAX_DAT_DW *`Tout),
	.FIFO_DEPTH(4),
    .log2_FIFO_DEPTH(2)
)wr_dat_fifo_1
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(wr_req_vld1&(~wr_req_pd1[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1])),
	.data_in(wr_req_pd1[`MAX_DAT_DW *`Tout-1:0]),
	.data_in_rdy(wr_dat_fifo_1_data_in_rdy),

	.data_out_vld(wr_dat_fifo_1_data_out_vld),
	.data_out(wr_dat_fifo_1_data_out),
	.data_out_rdy(wr_dat_fifo_1_data_out_rdy)
);

assign wr_req_rdy0=wr_req_pd0[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]?wr_cmd_fifo_0_data_in_rdy:wr_dat_fifo_0_data_in_rdy;
assign wr_req_rdy1=wr_req_pd1[2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1]?wr_cmd_fifo_1_data_in_rdy:wr_dat_fifo_1_data_in_rdy;

wire wr_arb_req_0=wr_cmd_fifo_0_data_out_vld;
wire wr_arb_req_1=wr_cmd_fifo_1_data_out_vld;

wire wr_arb_gnt_0;
wire wr_arb_gnt_1;

assign wr_cmd_fifo_0_data_out_rdy=wr_arb_gnt_0&M_AXI_AWREADY;
assign wr_cmd_fifo_1_data_out_rdy=wr_arb_gnt_1&M_AXI_AWREADY;

wire wlen_wid_noposted_fifo_full;
wire wlen_wid_noposted_fifo_empty;
wire [`log2AXI_BURST_LEN+2+1-1:0]wlen_wid_noposted_fifo_data_out;

mcif_rr_arb2 wr_rr_arb2
(
	.clk(clk),
	.rst_n(rst_n),

	.gnt_valid(M_AXI_AWVALID),
	.gnt_ready(M_AXI_AWREADY),

	.arb_req({wr_arb_req_1,wr_arb_req_0}),
	.arb_gnt({wr_arb_gnt_1,wr_arb_gnt_0})
);

wire noposted_aw_ch;
fifo_dff #
(
	.DATA_WIDTH(`log2AXI_BURST_LEN+2+1),
	.FIFO_DEPTH(8),
	.log2_FIFO_DEPTH(3)
)wlen_wid_noposted_fifo
(
	.clk(clk),
	.rst_n(rst_n),

	.push(M_AXI_AWVALID&M_AXI_AWREADY),
	.data_in({noposted_aw_ch,{wr_arb_gnt_1,wr_arb_gnt_0},M_AXI_AWLEN}),
	.full(wlen_wid_noposted_fifo_full),

	.pop(M_AXI_WLAST&M_AXI_WVALID&M_AXI_WREADY),
	.data_out(wlen_wid_noposted_fifo_data_out),
	.empty(wlen_wid_noposted_fifo_empty)
);

assign noposted_aw_ch=(wr_arb_gnt_0?wr_cmd_fifo_0_data_out[`log2AXI_BURST_LEN+32]:0) |
			(wr_arb_gnt_1?wr_cmd_fifo_1_data_out[`log2AXI_BURST_LEN+32]:0);

wire noposted_data_ch=wlen_wid_noposted_fifo_empty?(M_AXI_AWVALID?noposted_aw_ch:0):wlen_wid_noposted_fifo_data_out[`log2AXI_BURST_LEN+2];
wire [2-1:0]cur_wid_data_ch=wlen_wid_noposted_fifo_empty?(M_AXI_AWVALID?{wr_arb_gnt_1,wr_arb_gnt_0}:2'b0):wlen_wid_noposted_fifo_data_out[`log2AXI_BURST_LEN+2-1:`log2AXI_BURST_LEN];
wire [`log2AXI_BURST_LEN-1:0]cur_burst_len_data_ch=wlen_wid_noposted_fifo_empty?(M_AXI_AWVALID?M_AXI_AWLEN:0):wlen_wid_noposted_fifo_data_out[`log2AXI_BURST_LEN-1:0];

wire [M_AXI_ID_WIDTH-1:0]wid_0=wr_arb_gnt_0?0:0;
wire [M_AXI_ID_WIDTH-1:0]wid_1=wr_arb_gnt_1?1:0;

wire [`log2AXI_BURST_LEN-1:0]wlen_0=wr_arb_gnt_0?wr_cmd_fifo_0_data_out[`log2AXI_BURST_LEN+32-1:32]:0;
wire [`log2AXI_BURST_LEN-1:0]wlen_1=wr_arb_gnt_1?wr_cmd_fifo_1_data_out[`log2AXI_BURST_LEN+32-1:32]:0;

wire [31:0]base_awaddr_0=wr_cmd_fifo_0_data_out[`log2AXI_BURST_LEN+64+1-1:`log2AXI_BURST_LEN+32+1];
wire [31:0]base_awaddr_1=wr_cmd_fifo_1_data_out[`log2AXI_BURST_LEN+64+1-1:`log2AXI_BURST_LEN+32+1];
wire [31:0]awaddr_0=wr_arb_gnt_0?(wr_cmd_fifo_0_data_out[31:0]+base_awaddr_0):32'd0;
wire [31:0]awaddr_1=wr_arb_gnt_1?(wr_cmd_fifo_1_data_out[31:0]+base_awaddr_1):32'd0;

wire wvalid_0=(cur_wid_data_ch==4'b01)?wr_dat_fifo_0_data_out_vld:0;
wire wvalid_1=(cur_wid_data_ch==4'b10)?wr_dat_fifo_1_data_out_vld:0;

wire [M_AXI_DATA_WIDTH-1:0]wdata_0=(cur_wid_data_ch==4'b01)?wr_dat_fifo_0_data_out:0;
wire [M_AXI_DATA_WIDTH-1:0]wdata_1=(cur_wid_data_ch==4'b10)?wr_dat_fifo_1_data_out:0;

assign wr_dat_fifo_0_data_out_rdy=(cur_wid_data_ch==4'b01)&M_AXI_WREADY;
assign wr_dat_fifo_1_data_out_rdy=(cur_wid_data_ch==4'b10)&M_AXI_WREADY;

assign wr_rsp_complete0=noposted_data_ch & (M_AXI_WLAST&M_AXI_WVALID&M_AXI_WREADY) & (cur_wid_data_ch==4'b01);
assign wr_rsp_complete1=noposted_data_ch & (M_AXI_WLAST&M_AXI_WVALID&M_AXI_WREADY) & (cur_wid_data_ch==4'b10);

reg [`log2AXI_BURST_LEN-1:0]wlen_cnt;
always @(posedge clk or negedge rst_n)                                                                                                     
if(~rst_n)
	wlen_cnt<=0;
else
	if(M_AXI_WVALID&M_AXI_WREADY)
	begin
		if(M_AXI_WLAST)
			wlen_cnt<=0;
		else
			wlen_cnt<=wlen_cnt+1;
	end

reg axi_bready;
always @(posedge clk or negedge rst_n)                                                                                                     
if(~rst_n)                                                           
	axi_bready<=1'b0;                                                                                                                          
else
	if(M_AXI_BVALID&&~axi_bready)                                                                               
		axi_bready <= 1'b1;                                                                                                                                       
	else
		if(axi_bready)
			axi_bready<=1'b0;      

//AW channel
assign M_AXI_AWADDR=awaddr_0|awaddr_1;
assign M_AXI_AWLEN=wlen_0|wlen_1;
assign M_AXI_AWID=wid_0|wid_1;
assign M_AXI_AWVALID=(wr_arb_gnt_1|wr_arb_gnt_0)&(~wlen_wid_noposted_fifo_full);

//Wr channel
assign M_AXI_WVALID=wvalid_0|wvalid_1;
assign M_AXI_WDATA=wdata_0|wdata_1;
assign M_AXI_WSTRB={(M_AXI_DATA_WIDTH/8){1'b1}};
assign M_AXI_WLAST=M_AXI_WVALID&(wlen_cnt==cur_burst_len_data_ch);
assign M_AXI_BREADY=axi_bready;

//////////////////////////////////////////////////

assign M_AXI_AWSIZE=(`MAX_log2DAT_DW +`log2Tout-3); //clogb2((M_AXI_DATA_WIDTH/8)-1);
assign M_AXI_AWBURST=2'b01;
assign M_AXI_AWLOCK=1'b0;
assign M_AXI_AWCACHE=4'b0010;
assign M_AXI_AWPROT=3'h0;
assign M_AXI_AWQOS=4'h0;

endmodule
