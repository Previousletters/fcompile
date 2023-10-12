`timescale 1ns / 1ps
module Exp_module(//latency = 3
	input 					clk,      
    input  signed [17:0]    x_S7Q10, //�з�����
    output reg signed [11:0]    y_U0Q12_reg1 //�޷�����
);

wire signed [22:0] x_log2e_S8Q14; //�з�������һ��С�ڵ���0
reg  signed [22:0] x_log2e_S8Q14_reg1; //�з�������һ��С�ڵ���0
assign x_log2e_S8Q14  = x_S7Q10 * 6'sd23;

always@(posedge clk)begin
	x_log2e_S8Q14_reg1 <= x_log2e_S8Q14;
end

wire [21:0] x_log2e_U8Q14_abs; //x_log2e_6Q10�ľ���ֵ�޷�������������һλ����λ����Ϊ�����ܳ���-256
assign x_log2e_U8Q14_abs = ~x_log2e_S8Q14_reg1+1;

wire [7:0] x_int_8Q0 = x_log2e_U8Q14_abs[21:14];

wire [13:0] x_decimal_0Q14 = x_log2e_U8Q14_abs[13:0];//С�����֣��޷�����

wire [14:0] temp_1Q14 = 15'b100_0000_0000_0000 - {2'b0,x_decimal_0Q14[13:1]};//1+0.5*x_decimal,�޷�����
reg  [14:0] temp_1Q14_reg1;
wire [3:0] x_int_4Q0;//�޷�����
assign x_int_4Q0 = (x_int_8Q0 > 12)? 4'd12 : x_int_8Q0;//�ض�Ϊ12

wire [11:0] temp_2_int_1Q11; //2��ڤ,���ֵΪ1
reg  [11:0] temp_2_int_1Q11_reg1; //2��ڤ,���ֵΪ1
assign temp_2_int_1Q11 = 12'b1000_0000_0000 >> x_int_4Q0;
always@(posedge clk)begin
	temp_2_int_1Q11_reg1<=temp_2_int_1Q11;
	temp_1Q14_reg1<=temp_1Q14;
end

wire [26:0] temp_y_2Q25 = temp_2_int_1Q11_reg1 * temp_1Q14_reg1;//�޷�����


wire signed [11:0] y_U0Q12;
assign y_U0Q12 = temp_y_2Q25[25] == 1'b1 ? 12'b1111_1111_1111:temp_y_2Q25[24:13];

always@(posedge clk)begin 
	y_U0Q12_reg1<=y_U0Q12;
end

endmodule
