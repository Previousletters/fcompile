`timescale 1ns / 1ps //纯组合逻辑
module Log_module(//latency=2
	input clk,
    input [15:0]    x_U8Q8,    //无符号数，一定大于1，所以w不用考虑小于0的情况
    output [12:0]   y_U3Q10    //无符号数，但是一定大于0
);

reg [2:0] w; //无符号数
//无符号数,k-1
reg [14:0] k_1_0Q15;
//x一定大于1,所以不用考虑x小于1的情况


always @(*) begin
    if (x_U8Q8[15]==1'b1)begin 
        w = 7;
        k_1_0Q15 = x_U8Q8[14:0];
    end
    else if (x_U8Q8[14]==1'b1)begin
        w = 6;
        k_1_0Q15 = {x_U8Q8[13:0],1'b0};
    end
    else if (x_U8Q8[13]==1'b1)begin
        w = 5;
        k_1_0Q15 = {x_U8Q8[12:0],2'b00};
    end
    else if (x_U8Q8[12]==1'b1)begin
        w = 4;
        k_1_0Q15 = {x_U8Q8[11:0],3'b000};
    end
    else if (x_U8Q8[11]==1'b1)begin
        w = 3;
        k_1_0Q15 = {x_U8Q8[10:0],4'b0000};
    end
    else if (x_U8Q8[10]==1'b1)begin
        w = 2;
        k_1_0Q15 = {x_U8Q8[9:0],5'b00000};
    end
    else if (x_U8Q8[9]==1'b1)begin
        w = 1;
        k_1_0Q15 = {x_U8Q8[8:0],6'b000000};
    end
    else begin
        w = 0;
        k_1_0Q15 = {x_U8Q8[7:0],7'b0000000};
    end
end

//无符号数，k-1+w
wire [17:0] k_1_w_3Q15 = {w,k_1_0Q15};
reg  [17:0] k_1_w_3Q15_reg1;
always@(posedge clk)begin
	k_1_w_3Q15_reg1<=k_1_w_3Q15;
end
wire [21:0] P_3Q19;
reg  [21:0] P_3Q19_reg1;
assign P_3Q19 = k_1_w_3Q15_reg1 * 4'b1011; // 11/16 约为 ln2
always@(posedge clk)begin
	P_3Q19_reg1<=P_3Q19;
end
assign y_U3Q10 = P_3Q19_reg1[21:9];
endmodule
