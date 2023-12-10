`timescale 1ns / 1ps

module Softmax
(
    input                   clk,
    input                   rst_n,
    
    input [`Log2_Trans_pixel-1:0]ch_in,
    
    input [3:0]             scale_in,//scale_in 和 scale_out 在数据输入期间需要一直保持 scale_in认为其最小为1，最大为10
    input [3:0]             scale_out,//认为其最小为7，最大为12
    input signed [7:0]      data_in,
    input                   valid_in,//valid_in在这里需要是因为需要判断什么时候开始计数
                                    //支持流水线输入，即valid_in一直拉高
    output reg [7:0]        data_out,
    output                  valid_out
);

reg valid_in_delay1;
reg valid_in_delay2;
reg valid_in_delay3;
reg valid_in_delay4;
reg valid_in_delay5;
reg valid_in_delay6;
reg valid_in_delay7;
reg valid_in_delay8;
reg valid_in_delay9;
reg valid_in_delay10;			
reg [9:0] cnt;
reg signed [7:0] data_in_max;
reg signed [17:0] x_max_S7Q10;
reg signed [17:0] x_max_S7Q10_delay1;
reg signed [17:0] x_max_S7Q10_delay2;
reg signed [17:0] x_max_S7Q10_delay3;
reg signed [17:0] x_max_S7Q10_delay4;
reg signed [17:0] x_max_S7Q10_delay5;
reg signed [17:0] x_max_S7Q10_delay6;
reg  [19:0] e_sum_U8Q12;
reg [1:0] stage_delay1;
reg [1:0] stage_delay2;
reg [1:0] stage_delay3;
reg [1:0] stage_delay4;
reg [1:0] stage_delay5;
reg [1:0] stage_delay6;
reg [1:0] stage_delay7;
reg [1:0] stage_delay8;
reg [1:0] stage_delay9;
reg [1:0] stage_delay10;
reg [3:0] scale_out_delay1;
reg [3:0] scale_out_delay2;
reg [3:0] scale_out_delay3;
reg [3:0] scale_out_delay4;
reg [3:0] scale_out_delay5;
reg [3:0] scale_out_delay6;
reg [3:0] scale_out_delay7;
reg [3:0] scale_out_delay8;
reg [3:0] scale_out_delay9;
reg [7:0] exp_out;

wire [1:0] stage;//等于1表示第一次输入数据，2第二次，3第三次
wire signed [8:0] x_max_9b;
wire  [11:0] exp_U0Q12;
wire signed [11:0] exp_U0Q12_out;
wire [15:0] e_sum_U8Q8;
wire [12:0] ln_U3Q10;
wire signed [18:0] x_max_ln_S8Q10;
wire signed [17:0] x_max_ln_S7Q10;

assign x_max_ln_S8Q10 = x_max_S7Q10_delay6-$signed({5'b0_0000,ln_U3Q10}); //注意小数点位置
assign x_max_ln_S7Q10 = x_max_ln_S8Q10[18] == 0 ? 0:(x_max_ln_S8Q10 < -131072) ? 18'b1000_0000_0000_0000_00:x_max_ln_S8Q10;
assign stage = cnt<=(ch_in-1)?1 : ((ch_in-1)<cnt && cnt <=ch_in*2-1 ? 2 : 3);
assign x_max_9b = data_in-data_in_max;
assign e_sum_U8Q8 = e_sum_U8Q12[19:4];
assign valid_out = (stage_delay10==3 & valid_in_delay10)?1:0;


always @(*) begin                                                                                                
    case (scale_out_delay9)
            12: exp_out = (|exp_U0Q12_out[11:7])?8'b0111_1111:{1'b0,exp_U0Q12_out[6:0]};                                 
            11: exp_out = (|exp_U0Q12_out[11:8])?8'b0111_1111:{1'b0,exp_U0Q12_out[7:1]};                                 
            10: exp_out = (|exp_U0Q12_out[11:9])?8'b0111_1111:{1'b0,exp_U0Q12_out[8:2]};                                 
             9: exp_out = (|exp_U0Q12_out[11:10])?8'b0111_1111:{1'b0, exp_U0Q12_out[9:3]};                               
             8: exp_out = (exp_U0Q12_out[11])?8'b0111_1111:{1'b0, exp_U0Q12_out[10:4]};                                  
       default: exp_out = {1'b0, exp_U0Q12_out[11:5]};     //out_scale == 7 
    endcase                                                                                                      
end                                                                                                      

always@(posedge clk)begin
    data_out<=exp_out;
end

always@(posedge clk)begin
    stage_delay1<=stage;
    stage_delay2<=stage_delay1;
    stage_delay3<=stage_delay2;
    stage_delay4<=stage_delay3;
    stage_delay5<=stage_delay4;
    stage_delay6<=stage_delay5;
    stage_delay7<=stage_delay6;
    stage_delay8<=stage_delay7;
    stage_delay9<=stage_delay8;
    stage_delay10<=stage_delay9;
end

always@(posedge clk)begin
    scale_out_delay1<=scale_out;
    scale_out_delay2<=scale_out_delay1;
    scale_out_delay3<=scale_out_delay2;
    scale_out_delay4<=scale_out_delay3;
    scale_out_delay5<=scale_out_delay4;
    scale_out_delay6<=scale_out_delay5;
    scale_out_delay7<=scale_out_delay6;
    scale_out_delay8<=scale_out_delay7;
    scale_out_delay9<=scale_out_delay8;
end

always@(posedge clk)begin
    valid_in_delay1<=valid_in;    
	valid_in_delay2<=valid_in_delay1;
    valid_in_delay3<=valid_in_delay2;
    valid_in_delay4<=valid_in_delay3;
    valid_in_delay5<=valid_in_delay4;
    valid_in_delay6<=valid_in_delay5;
    valid_in_delay7<=valid_in_delay6;
    valid_in_delay8<=valid_in_delay7;
    valid_in_delay9<=valid_in_delay8;
    valid_in_delay10<=valid_in_delay9;
end

wire cnt_is_max_now= (cnt==ch_in*3-1);
wire cnt_will_update_now=valid_in;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    cnt<='d0;
else
    if(cnt_will_update_now)
    begin
        if(cnt_is_max_now)
            cnt<='d0;
        else
            cnt<=cnt+'d1;
    end

always@(posedge clk or negedge rst_n) 
if(~rst_n) 
    data_in_max<=8'b11111111;
else
    if(cnt==ch_in*3-1)
        data_in_max<=8'b11111111;
    else 
        if(valid_in&stage==1)
            data_in_max<=(data_in_max>data_in)?data_in_max:data_in;

always @(*) begin
    if (scale_in < 10) begin
        x_max_S7Q10 = $signed(x_max_9b) <<< (5'd10- scale_in);
    end
    else begin // in_scale == 10
        x_max_S7Q10 = x_max_9b;
    end
end

always@(posedge clk)begin
    x_max_S7Q10_delay1<=x_max_S7Q10;
    x_max_S7Q10_delay2<=x_max_S7Q10_delay1;
    x_max_S7Q10_delay3<=x_max_S7Q10_delay2;
    x_max_S7Q10_delay4<=x_max_S7Q10_delay3;
    x_max_S7Q10_delay5<=x_max_S7Q10_delay4;
    x_max_S7Q10_delay6<=x_max_S7Q10_delay5;
end

always@(posedge clk or negedge rst_n) 
if(~rst_n)
    e_sum_U8Q12<=0;
else
    if(stage_delay4==2 & valid_in_delay4)
        e_sum_U8Q12<=exp_U0Q12+e_sum_U8Q12;
    else 
        if(stage_delay4==1)
            e_sum_U8Q12<=0;


Exp_module u_Exp_module_1(
	.clk(clk),
    .x_S7Q10(x_max_S7Q10_delay1), //有符号数
    .y_U0Q12_reg1(exp_U0Q12) //无符号数
);

Log_module u_Log_module(//latency=2
	.clk(clk),
    .x_U8Q8(e_sum_U8Q8),    //无符号数，一定大于1，所以w不用考虑小于0的情况
    .y_U3Q10(ln_U3Q10)   //无符号数，但是一定大于0
);

Exp_module u_Exp_module_2(
	.clk(clk),
    .x_S7Q10(x_max_ln_S7Q10), //有符号数
    .y_U0Q12_reg1(exp_U0Q12_out) //无符号数
);
endmodule
