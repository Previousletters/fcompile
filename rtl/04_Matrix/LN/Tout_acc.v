`include "CNN_defines.vh"


////////////////////////////////////////
//////   ACC by 2 adder,  max64    /////
////////////////////////////////////////
module Tout_acc #
(
	parameter DATA_WIDTH=256
)

(
	input clk,
    input[(DATA_WIDTH*`Tout)-1:0] i_dat,
    output [(DATA_WIDTH+`log2Tout)-1:0]o_dat
);


reg signed [DATA_WIDTH-1:0] tp_sum  [(`Tout/8)-1:0][8-1:0];

genvar i,j;
generate
    for(i=0;i<`Tout/8;i=i+1)
    begin:dat_of_8Tin
        for(j=0;j<8;j=j+1)
        begin:dat_in8    
            always@(*)
            begin
                tp_sum[i][j]<=i_dat[(i*DATA_WIDTH*8+j*DATA_WIDTH)+DATA_WIDTH-1:(i*DATA_WIDTH*8+j*DATA_WIDTH)];
            end
        end
    end
endgenerate

reg signed  [DATA_WIDTH+1-1 :0] sum_1 [(`Tout/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_2 [(`Tout/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_3 [(`Tout/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_4 [(`Tout/8)-1:0];

reg signed  [DATA_WIDTH+2-1 :0] sum_5 [(`Tout/8)-1:0];
reg signed  [DATA_WIDTH+2-1 :0] sum_6 [(`Tout/8)-1:0];

reg signed  [DATA_WIDTH+3-1 :0] sum_7 [(`Tout/8)-1:0];

reg signed [DATA_WIDTH+`log2Tout-2-1:0] dat_out0;
reg signed [DATA_WIDTH+`log2Tout-2-1:0] dat_out1;
reg signed [DATA_WIDTH+`log2Tout-2-1:0] dat_out2;
reg signed [DATA_WIDTH+`log2Tout-2-1:0] dat_out3;
reg signed [DATA_WIDTH+`log2Tout-1-1:0] dat_out4;
reg signed [DATA_WIDTH+`log2Tout-1-1:0] dat_out5;
reg signed [DATA_WIDTH+`log2Tout-1:0] dat_out;

 generate
     for(i=0;i<(`Tout/8);i=i+1)
     begin:acc  
         always @(posedge clk)
         begin
                 sum_1[i] <= $signed(tp_sum[i][0] ) + $signed(tp_sum[i][1]);
                 sum_2[i] <= $signed(tp_sum[i][2] ) + $signed(tp_sum[i][3]);
                 sum_3[i] <= $signed(tp_sum[i][4] ) + $signed(tp_sum[i][5]);
                 sum_4[i] <= $signed(tp_sum[i][6] ) + $signed(tp_sum[i][7]);
                 sum_5[i] <= $signed(sum_1[i]) + $signed(sum_2[i]);
                 sum_6[i] <= $signed(sum_3[i]) + $signed(sum_4[i]);
                 sum_7[i] <= $signed(sum_5[i]) + $signed(sum_6[i]);
  
                 dat_out0 <= $signed(sum_7[0]) + $signed(sum_7[1]);           //if Tout==64 or 32
                 dat_out1 <= $signed(sum_7[2]) + $signed(sum_7[3]);           //if Tout==64 or 32
               
//                 dat_out2 <= $signed(sum_7[4]) + $signed(sum_7[5]);           //if Tout==64
//                 dat_out3 <= $signed(sum_7[6]) + $signed(sum_7[7]);           //if Tout==64
//                 dat_out4 <= $signed(dat_out0) + $signed(dat_out1);           //if Tout==64
//                 dat_out5 <= $signed(dat_out2) + $signed(dat_out3);           //if Tout==64
//                 dat_out <= $signed(dat_out4) + $signed(dat_out5);            //if Tout==64
               
                 dat_out <= $signed(dat_out0) + $signed(dat_out1);          // if Tout==32

         end
     end
endgenerate  

//always @(posedge clk)    dat_out <= $signed(sum_7[0]) + $signed(sum_7[1]);      // if Tout==16

assign o_dat=dat_out;

endmodule

