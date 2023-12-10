`include "CNN_defines.vh"


//////////////////////////////////////
////   ACC by 2 adder,  max128    /////
//////////////////////////////////////
//module Tin_acc #
//(
//	parameter DATA_WIDTH=256
//)

//(
//	input clk,
//    input[(DATA_WIDTH*`base_Tin)-1:0] i_dat,
//    output [(DATA_WIDTH+`base_log2Tin)-1:0]o_dat
//);


//reg signed [DATA_WIDTH-1:0] tp_sum  [(`base_Tin/8)-1:0][8-1:0];

//genvar i,j;
//generate
//    for(i=0;i<`base_Tin/8;i=i+1)
//    begin:dat_of_8Tin
//        for(j=0;j<8;j=j+1)
//        begin:dat_in8    
//            always@(posedge clk)
//            begin
//                tp_sum[i][j]<=i_dat[(i*DATA_WIDTH*8+j*DATA_WIDTH)+DATA_WIDTH-1:(i*DATA_WIDTH*8+j*DATA_WIDTH)];
//            end
//        end
//    end
//endgenerate

//reg signed  [DATA_WIDTH+1-1 :0] sum_1 [(`base_Tin/8)-1:0];
//reg signed  [DATA_WIDTH+1-1 :0] sum_2 [(`base_Tin/8)-1:0];
//reg signed  [DATA_WIDTH+1-1 :0] sum_3 [(`base_Tin/8)-1:0];
//reg signed  [DATA_WIDTH+1-1 :0] sum_4 [(`base_Tin/8)-1:0];

//reg signed  [DATA_WIDTH+2-1 :0] sum_5 [(`base_Tin/8)-1:0];
//reg signed  [DATA_WIDTH+2-1 :0] sum_6 [(`base_Tin/8)-1:0];

//reg signed  [DATA_WIDTH+3-1 :0] sum_7 [(`base_Tin/8)-1:0];

//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out0;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out1;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out2;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out3;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out4;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out5;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out5a;
//reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out5b;

//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out6;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out7;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out8;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out9;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out10;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out11;
//reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out12;
//reg signed [DATA_WIDTH+`base_log2Tin-1:0] dat_out;

// generate
//     for(i=0;i<(`base_Tin/8);i=i+1)
//     begin:acc  
//         always @(posedge clk)
//         begin
//                 sum_1[i] <= $signed(tp_sum[i][0] ) + $signed(tp_sum[i][1]);
//                 sum_2[i] <= $signed(tp_sum[i][2] ) + $signed(tp_sum[i][3]);
//                 sum_3[i] <= $signed(tp_sum[i][4] ) + $signed(tp_sum[i][5]);
//                 sum_4[i] <= $signed(tp_sum[i][6] ) + $signed(tp_sum[i][7]);
//                 sum_5[i] <= $signed(sum_1[i]) + $signed(sum_2[i]);
//                 sum_6[i] <= $signed(sum_3[i]) + $signed(sum_4[i]);
//                 sum_7[i] <= $signed(sum_5[i]) + $signed(sum_6[i]);
 
////                 dat_out0 <= $signed(sum_7[0]) + $signed(sum_7[1]);   // if Tin=80
////                 dat_out1 <= $signed(sum_7[2]) + $signed(sum_7[3]);   // if Tin=80
////                 dat_out2 <= $signed(sum_7[4]) + $signed(sum_7[5]);   // if Tin=80
////                 dat_out3 <= $signed(sum_7[6]) + $signed(sum_7[7]);   // if Tin=80
////                 dat_out4 <= $signed(sum_7[8]) + $signed(sum_7[9]);   // if Tin=80             

////                 dat_out6 <= $signed(dat_out0) + $signed(dat_out1);   // if Tin=80
////                 dat_out7 <= $signed(dat_out2) + $signed(dat_out3);   // if Tin=80
////                 dat_out8 <= $signed(dat_out4);                       // if Tin=80

////                 dat_out9 <= $signed(dat_out6) + $signed(dat_out7);   // if Tin=80              
////                 dat_out10 <= $signed(dat_out8);                      // if Tin=80

////                 dat_out <= $signed(dat_out9) + $signed(dat_out10);   // if Tin=80
                    
//                 dat_out0 <= $signed(sum_7[0]) + $signed(sum_7[1]);   // if Tin=96
//                 dat_out1 <= $signed(sum_7[2]) + $signed(sum_7[3]);   // if Tin=96
//                 dat_out2 <= $signed(sum_7[4]) + $signed(sum_7[5]);   // if Tin=96
//                 dat_out3 <= $signed(sum_7[6]) + $signed(sum_7[7]);   // if Tin=96
//                 dat_out4 <= $signed(sum_7[8]) + $signed(sum_7[9]);   // if Tin=96
//                 dat_out5 <= $signed(sum_7[10]) + $signed(sum_7[11]); // if Tin=96                

//                 dat_out6 <= $signed(dat_out0) + $signed(dat_out1);   // if Tin=96
//                 dat_out7 <= $signed(dat_out2) + $signed(dat_out3);   // if Tin=96
//                 dat_out8 <= $signed(dat_out4) + $signed(dat_out5);   // if Tin=96

//                 dat_out9 <= $signed(dat_out6) + $signed(dat_out7);   // if Tin=96              
//                 dat_out10 <= $signed(dat_out8);                      // if Tin=96

//                 dat_out <= $signed(dat_out9) + $signed(dat_out10);   // if Tin=96


////                 dat_out0 <= $signed(sum_7[0]) + $signed(sum_7[1]);   // if Tin=128
////                 dat_out1 <= $signed(sum_7[2]) + $signed(sum_7[3]);   // if Tin=128
////                 dat_out2 <= $signed(sum_7[4]) + $signed(sum_7[5]);   // if Tin=128
////                 dat_out3 <= $signed(sum_7[6]) + $signed(sum_7[7]);   // if Tin=128
////                 dat_out4 <= $signed(sum_7[8]) + $signed(sum_7[9]);   // if Tin=128
////                 dat_out5 <= $signed(sum_7[10]) + $signed(sum_7[11]); // if Tin=128                
////                 dat_out5a <= $signed(sum_7[12]) + $signed(sum_7[13]);// if Tin=128  
////                 dat_out5b <= $signed(sum_7[14]) + $signed(sum_7[15]);// if Tin=128  
                 
////                 dat_out6 <= $signed(dat_out0) + $signed(dat_out1);   // if Tin=128
////                 dat_out7 <= $signed(dat_out2) + $signed(dat_out3);   // if Tin=128
////                 dat_out8 <= $signed(dat_out4) + $signed(dat_out5);   // if Tin=128
////                 dat_out9 <= $signed(dat_out5a) + $signed(dat_out5b); // if Tin=128   
                 
////                 dat_out10 <= $signed(dat_out6) + $signed(dat_out7);  // if Tin=128  
////                 dat_out11 <= $signed(dat_out8) + $signed(dat_out9);  // if Tin=128  

////                 dat_out <= $signed(dat_out10) + $signed(dat_out11);  // if Tin=128
                                  
//         end
//     end
//endgenerate  

//assign o_dat=dat_out;

//endmodule



////////////////////////////////////////
//////   ACC by 2 adder,  max64    /////
////////////////////////////////////////
module Tin_acc #
(
	parameter DATA_WIDTH=256
)

(
	input clk,
    input[(DATA_WIDTH*`base_Tin)-1:0] i_dat,
    output [(DATA_WIDTH+`base_log2Tin)-1:0]o_dat
);


reg signed [DATA_WIDTH-1:0] tp_sum  [(`base_Tin/8)-1:0][8-1:0];

genvar i,j;
generate
    for(i=0;i<`base_Tin/8;i=i+1)
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

reg signed  [DATA_WIDTH+1-1 :0] sum_1 [(`base_Tin/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_2 [(`base_Tin/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_3 [(`base_Tin/8)-1:0];
reg signed  [DATA_WIDTH+1-1 :0] sum_4 [(`base_Tin/8)-1:0];

reg signed  [DATA_WIDTH+2-1 :0] sum_5 [(`base_Tin/8)-1:0];
reg signed  [DATA_WIDTH+2-1 :0] sum_6 [(`base_Tin/8)-1:0];

reg signed  [DATA_WIDTH+3-1 :0] sum_7 [(`base_Tin/8)-1:0];

reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out0;
reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out1;
reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out2;
reg signed [DATA_WIDTH+`base_log2Tin-2-1:0] dat_out3;
reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out4;
reg signed [DATA_WIDTH+`base_log2Tin-1-1:0] dat_out5;
reg signed [DATA_WIDTH+`base_log2Tin-1:0] dat_out;

 generate
     for(i=0;i<(`base_Tin/8);i=i+1)
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
  
                 dat_out0 <= $signed(sum_7[0]) + $signed(sum_7[1]);           //if base_Tin==64 or 32
                 dat_out1 <= $signed(sum_7[2]) + $signed(sum_7[3]);           //if base_Tin==64 or 32
               
                 dat_out2 <= $signed(sum_7[4]) + $signed(sum_7[5]);           //if base_Tin==64
                 dat_out3 <= $signed(sum_7[6]) + $signed(sum_7[7]);           //if base_Tin==64
                 dat_out4 <= $signed(dat_out0) + $signed(dat_out1);           //if base_Tin==64
                 dat_out5 <= $signed(dat_out2) + $signed(dat_out3);           //if base_Tin==64
                 dat_out <= $signed(dat_out4) + $signed(dat_out5);            //if base_Tin==64
               
//                 dat_out <= $signed(dat_out0) + $signed(dat_out1);          // if base_Tin==32

               
         end
     end
endgenerate  

//always @(posedge clk)    dat_out <= $signed(sum_7[0]) + $signed(sum_7[1]);      // if base_Tin==16

assign o_dat=dat_out;

endmodule



////////////////////////////////////////
/////////// ACC by fout adder///////////
////////////////////////////////////////

//module Tin_acc 
//(
//	input clk,
//    input[(DATA_WIDTH*`base_Tin)-1:0] i_dat,
//    output [(DATA_WIDTH+`base_log2Tin)-1:0]o_dat
//);

//reg signed [DATA_WIDTH-1:0] tp  [(`base_Tin/16)-1:0][16-1:0];

//genvar i,j;
//generate
//    for(i=0;i<`base_Tin/16;i=i+1)
//    begin:dat_of_16Tin
//        for(j=0;j<16;j=j+1)
//        begin:dat_in16    
//            always@(posedge clk)
//            begin
//                tp[i][j]=i_dat[(i*DATA_WIDTH*16+j*DATA_WIDTH)+DATA_WIDTH-1:(i*DATA_WIDTH*16+j*DATA_WIDTH)];
//            end
//        end
//    end
//endgenerate


//reg signed[(DATA_WIDTH+2)-1:0]sum_0[(`base_Tin/16)-1:0];
//reg signed[(DATA_WIDTH+2)-1:0]sum_1[(`base_Tin/16)-1:0];
//reg signed[(DATA_WIDTH+2)-1:0]sum_2[(`base_Tin/16)-1:0];
//reg signed[(DATA_WIDTH+2)-1:0]sum_3[(`base_Tin/16)-1:0];
//reg signed[(DATA_WIDTH+2+2)-1:0]sum_of_16[(`base_Tin/16)-1:0];


//begin
//    for(i=0;i<`base_Tin/16;i=i+1)
//    begin:acc_dat_in16
//        always@(posedge clk)
//        begin
//            sum_0[i] <= ($signed(tp[i][0] )  + $signed(tp[i][1]) ) + ($signed(tp[i][2]) + $signed(tp[i][3]) );
//            sum_1[i] <= ($signed(tp[i][4] )  + $signed(tp[i][5]) ) + ($signed(tp[i][6]) + $signed(tp[i][7]) );
//            sum_2[i] <= ($signed(tp[i][8] )  + $signed(tp[i][9]) ) + ($signed(tp[i][10])+ $signed(tp[i][11]));
//            sum_3[i] <= ($signed(tp[i][12] ) + $signed(tp[i][13])) + ($signed(tp[i][14])+ $signed(tp[i][15]));
    
//            sum_of_16[i] <= ($signed(sum_0[i]) + $signed(sum_1[i]))+ ($signed(sum_2[i])+ $signed(sum_3[i]));
//        end
//    end

//end

//reg [(DATA_WIDTH+`base_log2Tin)-1:0]final_result;

///////`ifdef Tin16   
////always@(*)
////    final_result <= sum_of_16[0];


///////////`ifdef Tin32   
////always@(posedge clk)
////    final_result <= ($signed(sum_of_16[0]) + $signed(sum_of_16[1]));


///////`ifdef Tin64   
//always@(posedge clk)
//    final_result <= (($signed(sum_of_16[0]) + $signed(sum_of_16[1])) + ($signed(sum_of_16[2]) + $signed(sum_of_16[3])));


///////////`ifdef Tin128  
////reg [((DATA_WIDTH+`base_log2Tin))-1:0]tp1;
////reg [((DATA_WIDTH+`base_log2Tin))-1:0]tp2;
////always@(posedge clk)
////begin
////    tp1 <= (($signed(sum_of_16[0]) + $signed(sum_of_16[1])) + ($signed(sum_of_16[2]) + $signed(sum_of_16[3])));
////    tp2 <= ( ($signed(sum_of_16[4]) + $signed(sum_of_16[5])) + ($signed(sum_of_16[6]) + $signed(sum_of_16[7])) );    
////    final_result <=$signed(tp1) + $signed(tp2);
////end


//assign o_dat=final_result;

//endmodule
