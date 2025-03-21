`include "CNN_defines.vh"

module generate_dat_shift #
(
	parameter DATA_WIDTH=256,
	parameter DEPTH=16
)
(
    input clk,
    input rst_n,

    input [DATA_WIDTH-1:0]data_in,
    output [DATA_WIDTH-1:0]data_out
);
reg [DATA_WIDTH-1:0]dat_reg[DEPTH-1:0];

always@(posedge clk)
    dat_reg[0]<=data_in;
        
genvar i;
generate
    for(i=1;i<DEPTH;i=i+1) 
    begin:shift
        always@(posedge clk)
            dat_reg[i]<=dat_reg[i-1];
    end
endgenerate

assign data_out=dat_reg[DEPTH-1];

endmodule
