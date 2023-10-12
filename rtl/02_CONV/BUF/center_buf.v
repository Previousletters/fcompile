`include "CNN_defines.vh"

module center_buf #
(
	parameter DATA_WIDTH=256,
	parameter DEPTH = 32,
	parameter log2_DEPTH = 5
)
(
	input clk,
	input rst_n,

	//Wr Port
	input wr_en,
	input [log2_DEPTH-1:0]wr_addr,
	input [DATA_WIDTH-1:0]wr_dat,

	//Rd Port
	input rd_en,
	input [log2_DEPTH-1:0]rd_addr,
	output reg rd_dat_vld,
	output [DATA_WIDTH-1:0]rd_dat
);

reg [DATA_WIDTH-1:0]rd_dat_r;
reg [DATA_WIDTH-1:0]mem[DEPTH-1:0];

always @(posedge clk)
if(rd_en)
	rd_dat_r<=mem[rd_addr];

always @(posedge clk or negedge rst_n)
if(~rst_n)
	rd_dat_vld<=1'b0;
else
	rd_dat_vld<=rd_en;

always @(posedge clk)
if(wr_en)
	mem[wr_addr]<=wr_dat;

assign rd_dat=rd_dat_r;

endmodule
