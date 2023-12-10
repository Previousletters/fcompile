`include "CNN_defines.vh"

module Systolic_PE_Column
(
    input clk,
    input rst_n,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit     
    
    input [(`base_Tin*`MAX_WT_DW)*`Tn-1:0]wt,
    input [`base_Tin*`MAX_DAT_DW-1:0]left_dat_in,
    output reg [`base_Tin*`MAX_DAT_DW-1:0]right_dat_out,
    output [(`base_log2Tin+`MAX_DW2)*`Tn-1:0]down_dat_out

);

(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_copy[`Tn/2-1:0];
genvar n;
generate
    for(n=0;n<`Tn/2;n=n+1)
    begin:cacc
        always@(posedge clk)
            Tin_factor_copy[n]<=Tin_factor;
    
        Two_Col_MAC_Calculation Col_Parallel_MAC
        (
            .clk(clk),
            .rst_n(rst_n),
            .Tin_factor(Tin_factor_copy[n]),
            
            .dat(left_dat_in),
            .wt(wt[n*(`base_Tin*`MAX_WT_DW*2)+:(`base_Tin*`MAX_WT_DW*2)]),
            .dat_o(down_dat_out[n*(`base_log2Tin+`MAX_DW2)*2+:(`base_log2Tin+`MAX_DW2)*2])
        );
    end
endgenerate

always@(posedge clk or negedge rst_n)
if(~rst_n)
    right_dat_out<=0;
else
    right_dat_out<=left_dat_in;
    
endmodule
