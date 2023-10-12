`include "CNN_defines.vh"

module mcif_rr_arb2
(
	input clk,
	input rst_n,

	input gnt_valid,
	input gnt_ready,

	input [1:0]arb_req,
	output [1:0]arb_gnt
);

reg [1:0]cur_arb_id;
wire [1:0]nxt_arb_id;

reg arb_en;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	arb_en<=1'b1;
else
	if(gnt_valid)
	begin
		if(gnt_ready)
			arb_en<=1'b1;
		else
			arb_en<=1'b0;	
	end

mcif_arb_comb2 mcif_arb_comb2
(
	.cur_arb_id(cur_arb_id),
	.arb_req(arb_req),
	.arb_en(arb_en),
	.nxt_arb_id(nxt_arb_id)
);

always @(posedge clk or negedge rst_n)
if(~rst_n)
	cur_arb_id<=2'd0;
else
	cur_arb_id<=nxt_arb_id;

assign arb_gnt[0]=(nxt_arb_id==2'd0)&arb_req[0];
assign arb_gnt[1]=(nxt_arb_id==2'd1)&arb_req[1];

endmodule
