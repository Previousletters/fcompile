`include "CNN_defines.vh"


module Two_Col_INT248_MAC
(
    input clk,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit
    input[`MAX_DAT_DW-1:0]INT8_dat_in,
    input [`MAX_WT_DW-1:0]INT8_wt1,
    input [`MAX_WT_DW-1:0]INT8_wt0,
    output reg [`MAX_DW2-1:0]INT8_sum_out1,
    output reg [`MAX_DW2-1:0]INT8_sum_out0,
    
    input[`MAX_DAT_DW-1:0]INT4_dat_in,
    input [`MAX_WT_DW-1:0]INT4_wt1,
    input [`MAX_WT_DW-1:0]INT4_wt0,
    output reg[`MAX_DAT_DW-1 :0]INT4_sum_out1,
    output reg[`MAX_DAT_DW-1 :0]INT4_sum_out0,
    
    input [         4-1:0]INT2_dat_in,
    input [`MAX_WT_DW-1:0]INT2_wt1,
    input [`MAX_WT_DW-1:0]INT2_wt0,
    output reg [    4-1:0]INT2_sum_out1,
    output reg [    4-1:0]INT2_sum_out0,
    
    input [`MAX_DAT_DW-1:0]INT1_dat_in,
    input [`MAX_WT_DW-1:0]INT1_wt1,
    input [`MAX_WT_DW-1:0]INT1_wt0,
    output reg [    5-1:0]INT1_sum_out1,
    output reg [    5-1:0]INT1_sum_out0
);
//////////////////////////////////////////
///////////// DSP for INT8    ////////////
//////////////////////////////////////////
`ifdef ZYNQ
    wire signed[24:0]INT8_W0 = {{17{INT8_wt0[7]}}, INT8_wt0};
    wire signed[24:0]INT8_W1 = {INT8_wt1, 17'd0};
`else
    wire signed[26:0]INT8_W0 = {{19{INT8_wt0[7]}}, INT8_wt0};
    wire signed[26:0]INT8_W1 = {INT8_wt1, 19'd0};
`endif
wire signed[7:0]INT8_F  = INT8_dat_in;
    
reg signed[26:0] W0;
reg signed[26:0] W1;
reg signed[17:0] F;

(* use_dsp = "yes" *) reg  signed [40 : 0] tp;
always@(posedge clk )begin
    tp <= $signed((INT8_W1 + INT8_W0)) * $signed(INT8_F);
end
    
`ifdef ZYNQ
    wire signed [15 : 0] INT8_tp_d0w0 = tp[15: 0];
    wire signed [15 : 0] INT8_tp_d0w1 = tp[32:17];
`else
    wire signed [15 : 0] INT8_tp_d0w0 = tp[15: 0];
    wire signed [15 : 0] INT8_tp_d0w1 = tp[34:19];
`endif

always@(*)
begin   
    INT8_sum_out0 <=  INT8_tp_d0w0;
    INT8_sum_out1 <=  (INT8_tp_d0w1 + INT8_tp_d0w0[15]);
//    INT8_sum_out1 <= (INT8_wt1==8'b1000_0000 & INT8_wt0[7])? -(INT8_tp_d0w1 + INT8_tp_d0w0[15]):(INT8_tp_d0w1 + INT8_tp_d0w0[15]);
end

//////////////////////////////////////////
///////// Simplest INT8 MUL    ///////////
//////////////////////////////////////////
//(* use_dsp="yes" *) reg [`MAX_DW2-1:0]tp_INT8_sum_out0,tp_INT8_sum_out1;
//always @(posedge clk)
//begin        
//    tp_INT8_sum_out0<=$signed(INT8_wt0) * $signed(INT8_dat_in);
//    tp_INT8_sum_out1<=$signed(INT8_wt1) * $signed(INT8_dat_in);
//end
//always @(*) INT8_sum_out0=tp_INT8_sum_out0;
//always @(*) INT8_sum_out1=tp_INT8_sum_out1;

//////////////////////////////////////////
///////// Simplest INT4 MUL    ///////////
//////////////////////////////////////////
//`ifdef INT4_Enable
//    reg [6:0] INT4_mul[1:0][1:0];
//    always @(*)
//    begin
//        INT4_mul[0][0]<=$signed(INT4_dat_in[3:0]) * $signed(INT4_wt0[3:0]);
//        INT4_mul[0][1]<=$signed(INT4_dat_in[7:4]) * $signed(INT4_wt0[7:4]);
//        INT4_mul[1][0]<=$signed(INT4_dat_in[3:0]) * $signed(INT4_wt1[3:0]);
//        INT4_mul[1][1]<=$signed(INT4_dat_in[7:4]) * $signed(INT4_wt1[7:4]);
//    end
    
//    always @(posedge clk)
//    begin        
//        INT4_sum_out0<=$signed(INT4_mul[0][0]) + $signed(INT4_mul[0][1]);
//        INT4_sum_out1<=$signed(INT4_mul[1][0]) + $signed(INT4_mul[1][1]);
//    end
//`endif

//////////////////////////////////////////
///////// Simplest INT2 MUL    ///////////
//////////////////////////////////////////
//reg [1:0]tp_INT2_mul[1:0][3:0];
//always @(*)
//begin
//    tp_INT2_mul[0][0]=INT2_dat_in[0]==1?INT2_wt0[1:0]:0;
//    tp_INT2_mul[0][1]=INT2_dat_in[1]==1?INT2_wt0[3:2]:0;
//    tp_INT2_mul[0][2]=INT2_dat_in[2]==1?INT2_wt0[5:4]:0;
//    tp_INT2_mul[0][3]=INT2_dat_in[3]==1?INT2_wt0[7:6]:0;
//    tp_INT2_mul[1][0]=INT2_dat_in[0]==1?INT2_wt1[1:0]:0;
//    tp_INT2_mul[1][1]=INT2_dat_in[1]==1?INT2_wt1[3:2]:0;
//    tp_INT2_mul[1][2]=INT2_dat_in[2]==1?INT2_wt1[5:4]:0;
//    tp_INT2_mul[1][3]=INT2_dat_in[3]==1?INT2_wt1[7:6]:0;
//end

//reg [2:0]tp_INT2_sum[1:0][1:0];
//always @(*)
//begin
//    tp_INT2_sum[0][0]<=($signed(tp_INT2_mul[0][0]) + $signed(tp_INT2_mul[0][1]));
//    tp_INT2_sum[0][1]<=($signed(tp_INT2_mul[0][2]) + $signed(tp_INT2_mul[0][3]));

//    tp_INT2_sum[1][0]<=($signed(tp_INT2_mul[1][0]) + $signed(tp_INT2_mul[1][1]));
//    tp_INT2_sum[1][1]<=($signed(tp_INT2_mul[1][2]) + $signed(tp_INT2_mul[1][3]));
    
//end

//always @(posedge clk)
//begin
//    INT2_sum_out0<=$signed(tp_INT2_sum[0][0]) + $signed(tp_INT2_sum[0][1]);
//    INT2_sum_out1<=$signed(tp_INT2_sum[1][0]) + $signed(tp_INT2_sum[1][1]);
//end

//////////////////////////////////////////
///////// Simplest INT1 MUL    ///////////
//////////////////////////////////////////
//reg tp_INT1_mul[1:0][7:0];
//always @(*)
//begin
//    tp_INT1_mul[0][0]=INT1_dat_in[0]==INT1_wt0[0]?1:0;
//    tp_INT1_mul[0][1]=INT1_dat_in[1]==INT1_wt0[1]?1:0;
//    tp_INT1_mul[0][2]=INT1_dat_in[2]==INT1_wt0[2]?1:0;
//    tp_INT1_mul[0][3]=INT1_dat_in[3]==INT1_wt0[3]?1:0;
//    tp_INT1_mul[0][4]=INT1_dat_in[4]==INT1_wt0[4]?1:0;
//    tp_INT1_mul[0][5]=INT1_dat_in[5]==INT1_wt0[5]?1:0;
//    tp_INT1_mul[0][6]=INT1_dat_in[6]==INT1_wt0[6]?1:0;
//    tp_INT1_mul[0][7]=INT1_dat_in[7]==INT1_wt0[7]?1:0;

//    tp_INT1_mul[1][0]=INT1_dat_in[0]==INT1_wt1[0]?1:0;
//    tp_INT1_mul[1][1]=INT1_dat_in[1]==INT1_wt1[1]?1:0;
//    tp_INT1_mul[1][2]=INT1_dat_in[2]==INT1_wt1[2]?1:0;
//    tp_INT1_mul[1][3]=INT1_dat_in[3]==INT1_wt1[3]?1:0;
//    tp_INT1_mul[1][4]=INT1_dat_in[4]==INT1_wt1[4]?1:0;
//    tp_INT1_mul[1][5]=INT1_dat_in[5]==INT1_wt1[5]?1:0;
//    tp_INT1_mul[1][6]=INT1_dat_in[6]==INT1_wt1[6]?1:0;
//    tp_INT1_mul[1][7]=INT1_dat_in[7]==INT1_wt1[7]?1:0;
//end

//always @(posedge clk)
//begin
//    INT1_sum_out0<=((tp_INT1_mul[0][0]+tp_INT1_mul[0][1])+(tp_INT1_mul[0][2]+tp_INT1_mul[0][3])) + 
//                   ((tp_INT1_mul[0][4]+tp_INT1_mul[0][5])+(tp_INT1_mul[0][6]+tp_INT1_mul[0][7]));
//    INT1_sum_out1<=((tp_INT1_mul[1][0]+tp_INT1_mul[1][1])+(tp_INT1_mul[1][2]+tp_INT1_mul[1][3])) + 
//                   ((tp_INT1_mul[1][4]+tp_INT1_mul[1][5])+(tp_INT1_mul[1][6]+tp_INT1_mul[1][7]));
//end


endmodule

