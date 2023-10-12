`include "CNN_defines.vh"

module mcif_feature_4r_2w #
(
	parameter M_AXI_ID_WIDTH=4,
	parameter M_AXI_DATA_WIDTH = `MAX_DAT_DW *`Tout
)
(
	input clk,
	input rst_n,

	//////////////////////////////////////////////////
	input rd_req_vld0,
	output rd_req_rdy0,
	input [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd0,

	output rd_resp_vld0,
	input rd_resp_rdy0,
	output [M_AXI_DATA_WIDTH-1:0]rd_resp_pd0,
	input rd_fifo_pop0,
	//////////////////////////////////////////////////
	input rd_req_vld1,
	output rd_req_rdy1,
	input [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd1,

	output rd_resp_vld1,
	input rd_resp_rdy1,
	output [M_AXI_DATA_WIDTH-1:0]rd_resp_pd1,
	input rd_fifo_pop1,
	//////////////////////////////////////////////////
	input rd_req_vld2,
	output rd_req_rdy2,
	input [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd2,

	output rd_resp_vld2,
	input rd_resp_rdy2,
	output [M_AXI_DATA_WIDTH-1:0]rd_resp_pd2,
	input rd_fifo_pop2,
	//////////////////////////////////////////////////
    input rd_req_vld3,
    output rd_req_rdy3,
    input [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd3,

    output rd_resp_vld3,
    input rd_resp_rdy3,
    output [M_AXI_DATA_WIDTH-1:0]rd_resp_pd3,
    input rd_fifo_pop3,
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
	output  M_AXI_BREADY,//Bvalid and Bread means a a write response.
	
	//AR channel
	output [M_AXI_ID_WIDTH-1 : 0] M_AXI_ARID,
	output [32-1 : 0] M_AXI_ARADDR,
	output [`log2AXI_BURST_LEN-1 : 0] M_AXI_ARLEN,
	output [2 : 0] M_AXI_ARSIZE,//=clogb2((M_AXI_DATA_WIDTH/8)-1);
	output [1 : 0] M_AXI_ARBURST,//=2'b01;
	output  M_AXI_ARLOCK,//=1'b0;
	output [3 : 0] M_AXI_ARCACHE,//=4'b0010;
	output [2 : 0] M_AXI_ARPROT,//=3'h0;
	output [3 : 0] M_AXI_ARQOS,//=4'h0;
	output  M_AXI_ARVALID,
	input  M_AXI_ARREADY,
	
	//Rd channel
	input [M_AXI_ID_WIDTH-1 : 0] M_AXI_RID,
	input [M_AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA,
	input [1 : 0] M_AXI_RRESP,//ignore
	input  M_AXI_RLAST,
	input  M_AXI_RVALID,
	output  M_AXI_RREADY
);

mcif_rd_4r #
(
	.M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .M_AXI_DATA_WIDTH(`MAX_DAT_DW *`Tout)
)mcif_rd
(
	.clk(clk),
    .rst_n(rst_n),
    
    //////////////////////////////////////////////////
    .rd_req_vld0(rd_req_vld0),
    .rd_req_rdy0(rd_req_rdy0),
    .rd_req_pd0(rd_req_pd0),
    
    .rd_resp_vld0(rd_resp_vld0),
    .rd_resp_rdy0(rd_resp_rdy0),
    .rd_resp_pd0(rd_resp_pd0),
    .rd_fifo_pop0(rd_fifo_pop0),
    //////////////////////////////////////////////////
    .rd_req_vld1(rd_req_vld1),
    .rd_req_rdy1(rd_req_rdy1),
    .rd_req_pd1(rd_req_pd1),
    
    .rd_resp_vld1(rd_resp_vld1),
    .rd_resp_rdy1(rd_resp_rdy1),
    .rd_resp_pd1(rd_resp_pd1),
    .rd_fifo_pop1(rd_fifo_pop1),
    //////////////////////////////////////////////////
    .rd_req_vld2(rd_req_vld2),
    .rd_req_rdy2(rd_req_rdy2),
    .rd_req_pd2(rd_req_pd2),
    
    .rd_resp_vld2(rd_resp_vld2),
    .rd_resp_rdy2(rd_resp_rdy2),
    .rd_resp_pd2(rd_resp_pd2),
    .rd_fifo_pop2(rd_fifo_pop2),
    //////////////////////////////////////////////////
    .rd_req_vld3(rd_req_vld3),
    .rd_req_rdy3(rd_req_rdy3),
    .rd_req_pd3(rd_req_pd3),
    
    .rd_resp_vld3(rd_resp_vld3),
    .rd_resp_rdy3(rd_resp_rdy3),
    .rd_resp_pd3(rd_resp_pd3),
    .rd_fifo_pop3(rd_fifo_pop3),
    //////////////////////////////////////////////////
    
    //AR channel
    .M_AXI_ARID(M_AXI_ARID),
    .M_AXI_ARADDR(M_AXI_ARADDR),
    .M_AXI_ARLEN(M_AXI_ARLEN),
    .M_AXI_ARSIZE(M_AXI_ARSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_ARBURST(M_AXI_ARBURST),//=2'b01;
    .M_AXI_ARLOCK(M_AXI_ARLOCK),//=1'b0;
    .M_AXI_ARCACHE(M_AXI_ARCACHE),//=4'b0010;
    .M_AXI_ARPROT(M_AXI_ARPROT),//=3'h0;
    .M_AXI_ARQOS(M_AXI_ARQOS),//=4'h0;
    .M_AXI_ARVALID(M_AXI_ARVALID),
    .M_AXI_ARREADY(M_AXI_ARREADY),
    
    //Rd channel
    .M_AXI_RID(M_AXI_RID),
    .M_AXI_RDATA(M_AXI_RDATA),
    .M_AXI_RRESP(M_AXI_RRESP),//ignore
    .M_AXI_RLAST(M_AXI_RLAST),
    .M_AXI_RVALID(M_AXI_RVALID),
    .M_AXI_RREADY(M_AXI_RREADY)
);

mcif_wr_2w #
(
	.M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
	.M_AXI_DATA_WIDTH(`MAX_DAT_DW *`Tout)
)mcif_wr
(
    .clk(clk),
    .rst_n(rst_n),
    
    //////////////////////////////////////////////////
    .wr_req_vld0(wr_req_vld0),
    .wr_req_rdy0(wr_req_rdy0),
    .wr_req_pd0(wr_req_pd0),
    .wr_rsp_complete0(wr_rsp_complete0),
    //////////////////////////////////////////////////
    .wr_req_vld1(wr_req_vld1),
    .wr_req_rdy1(wr_req_rdy1),
    .wr_req_pd1(wr_req_pd1),
    .wr_rsp_complete1(wr_rsp_complete1),
    //////////////////////////////////////////////////
    
    //AW channel
    .M_AXI_AWID(M_AXI_AWID),
    .M_AXI_AWADDR(M_AXI_AWADDR),
    .M_AXI_AWLEN(M_AXI_AWLEN),
    .M_AXI_AWSIZE(M_AXI_AWSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_AWBURST(M_AXI_AWBURST),//=2'b01;
    .M_AXI_AWLOCK(M_AXI_AWLOCK),//1'b0;
    .M_AXI_AWCACHE(M_AXI_AWCACHE),//=4'b0010
    .M_AXI_AWPROT(M_AXI_AWPROT),//=3'h0;
    .M_AXI_AWQOS(M_AXI_AWQOS),//=4'h0;
    .M_AXI_AWVALID(M_AXI_AWVALID),
    .M_AXI_AWREADY(M_AXI_AWREADY),
    
    //Wr channel
    .M_AXI_WDATA(M_AXI_WDATA),
    .M_AXI_WSTRB(M_AXI_WSTRB),
    .M_AXI_WLAST(M_AXI_WLAST),
    .M_AXI_WVALID(M_AXI_WVALID),
    .M_AXI_WREADY(M_AXI_WREADY),
    .M_AXI_BID(M_AXI_BID),//ignore
    .M_AXI_BRESP(M_AXI_BRESP),//ignore
    .M_AXI_BVALID(M_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M_AXI_BREADY(M_AXI_BREADY)//Bvalid and Bread means a a write response.
);

endmodule
