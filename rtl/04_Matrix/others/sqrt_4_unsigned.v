`timescale 1ns / 1ps
//����1��4�Ŀ����ţ����������Ҫ�ﵽǧ��֮һ pow(2,-10)����ô���뾫����Ҫ�ﵽpow(2,-9)
//����ʽ��ϣ�1/np.power(2,7) * np.power(x,3) - 3/np.power(2,5)* np.power(x,2)+ 163/np.power(2,8)*x+58/np.power(2,7)���0.1%-0.4%
module sqrt_4_unsigned(
    input  clk,
    input  reset,
    input  [10:0] x_2Q9, // 11bit�޷�����������λ2��С��λ9
    output reg [10:0] y_1Q10 
);
wire [8:0]  x_2Q7 = x_2Q9[10:2];//����ȡ��һ�����ʵľ���
reg [8:0] x_2Q7_reg1;
always@(posedge clk or posedge reset)begin
    if(reset)begin
        x_2Q7_reg1<=0;
    end
    else begin
        x_2Q7_reg1<=x_2Q7;
    end
end
reg [17:0] tempQ14;
reg [28:0] temp1Q28;

reg [21:0] tempQ18;
reg [23:0] temp1Q23;

reg [17:0] temp1Q17;
reg [17:0] temp1Q17_reg1;
always@(posedge clk or posedge reset)begin
    if(reset)begin
        tempQ14<=0;
        temp1Q28<=0;
    end
    else begin
        tempQ14<=x_2Q7* x_2Q7;
        temp1Q28<=x_2Q7_reg1*tempQ14;
    end
end
always@(posedge clk or posedge reset)begin
    if(reset)begin       
        tempQ18<=0;
        temp1Q23<=0;
    end
    else begin
        tempQ18<=x_2Q9*x_2Q9;
        temp1Q23 <= 2'd3 *tempQ18;
    end
end

always@(posedge clk or posedge reset)begin
    if(reset)begin
        temp1Q17<=0;     
        temp1Q17_reg1<=0;
    end
    else begin
        temp1Q17<=8'd163*x_2Q9;
        temp1Q17_reg1 <= temp1Q17;
    end
end

always@(posedge clk or posedge reset)begin
    if(reset)begin
        y_1Q10<=0; 
    end
    else begin
        y_1Q10 <= temp1Q28[28:18] - temp1Q23[23:13]+  temp1Q17_reg1[17:7] + {1'b0,7'd58,3'b0};
    end
end

endmodule
