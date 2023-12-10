`include "CNN_defines.vh"

 module Gelu_Top
(
	input clk,
	input rst_n,
	input gelu,
    input [2:0]    in_scale, //范围0�?7
    input [`MAX_DAT_DW *`Tout-1:0]    dat_in,
    output[`MAX_DAT_DW *`Tout-1:0]    dat_out,
	input  in_vld,
    output out_vld

);

wire [`MAX_DAT_DW *`Tout-1:0]gelu_out;
wire [`Tout-1:0]gelu_out_vld;
genvar i;
generate
    for(i=0;i<`Tout;i=i+1)
    begin:act
        Gelu u_Gelu
        (
            .clk       (clk),
            .rst_n     (rst_n),
            .in_scale  (in_scale),
            .x         (gelu?dat_in[i*`MAX_DAT_DW+:`MAX_DAT_DW]: {(`MAX_DAT_DW){1'b0}}),
            .in_vld    (in_vld),
        
            .y_reg1    (gelu_out[i*`MAX_DAT_DW+:`MAX_DAT_DW]),
            .out_vld   (gelu_out_vld[i])
        );
	end
endgenerate 

assign dat_out=gelu_out;
assign out_vld=gelu_out_vld[0];

endmodule
