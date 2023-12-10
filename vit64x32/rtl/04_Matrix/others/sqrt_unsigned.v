`timescale 1ns / 1ps

module sqrt_unsigned
(
    input           rst_n,
    input           clk,
    input           valid_in,
    input [19:0]    x,
    output reg [9:0]   y_10Q10
 );

wire [9:0] temp1;
wire [4:0] temp2;
wire flag_x_0; //x=0置位1
wire [10:0] y_1Q10;
wire count_1; //10
wire count_2;   //5

reg count_1_reg1; //10
reg count_2_reg1;   //5
reg [2:0] count_3; //0-5
reg [10:0] x_2Q9;
reg [4:0] temp2_reg1;
reg [19:0] x_reg1;
reg [19:0] x_reg2;
reg [4:0] M; //最高有效位 0-19,用二分法找
reg [4:0] M_reg1;
reg [4:0] M_reg2;
reg [4:0] M_reg3;
reg [4:0] M_reg4;
reg flag_x_0_reg1;
reg flag_x_0_reg2;
reg flag_x_0_reg3;
reg flag_x_0_reg4;
reg flag_x_0_reg5;
reg valid_in_reg1;
reg valid_in_reg2;
reg valid_in_reg3;
reg valid_in_reg4;
reg valid_in_reg5;

assign flag_x_0 = (x==0)? 1:0;

assign count_1 = | x[19:10];
assign temp1 = count_1?x[19:10]:x[9:0];
assign count_2 = | temp1[9:5];
assign temp2 = count_2?temp1[9:5]:temp1[4:0];


always@(*)begin
case({count_1_reg1,count_2_reg1})
    2'b11: M=15+count_3;
    2'b10: M=10+count_3;
    2'b01: M=5+count_3;
    2'b00: M=count_3;
endcase
end


always @ (*)begin
    case (M_reg1)
        0 :begin x_2Q9={1'b0,x_reg2[0],9'b0};end
        1 :begin x_2Q9={x_reg2[1:0],9'b0};end
        2 :begin x_2Q9={1'b0,x_reg2[2:0],7'b0};end
        3 :begin x_2Q9={x_reg2[3:0],7'b0};end
        4 :begin x_2Q9={1'b0,x_reg2[4:0],5'b0};end
        5 :begin x_2Q9={x_reg2[5:0],5'b0};end
        6 :begin x_2Q9={1'b0,x_reg2[6:0],3'b0};end
        7 :begin x_2Q9={x_reg2[7:0],3'b0};end
        8 :begin x_2Q9={1'b0,x_reg2[8:0],1'b0};end
        9 :begin x_2Q9= {x_reg2[9:0],1'b0};end
        10:begin x_2Q9={1'b0,x_reg2[10:1]    };end
        11:begin x_2Q9={x_reg2[11:1]    };end
        12:begin x_2Q9={1'b0,x_reg2[12:3]    };end
        13:begin x_2Q9={x_reg2[13:3]    };end
        14:begin x_2Q9={1'b0,x_reg2[14:5]    };end
        15:begin x_2Q9={x_reg2[15:5]    };end
        16:begin x_2Q9={1'b0,x_reg2[16:7]    };end
        17:begin x_2Q9={x_reg2[17:7]    };end
        18:begin x_2Q9={1'b0,x_reg2[18:9]    };end
        19:begin x_2Q9={x_reg2[19:9]   };end
        default:x_2Q9=0;
    endcase
end


always@(posedge clk) temp2_reg1<=temp2;


always@(posedge clk)
begin
    count_1_reg1<=count_1;
    count_2_reg1<=count_2;
end

reg [19:0] tp_y_10Q10;
always@(posedge clk) tp_y_10Q10<=(flag_x_0_reg5==1)?0:y_1Q10<<M_reg4[4:1];



always@(*)
begin
    if (temp2_reg1[4]==1)
        count_3 = 4;
    else if (temp2_reg1[3]==1)
        count_3 = 3;
    else if (temp2_reg1[2]==1)
        count_3 = 2;
    else if (temp2_reg1[1]==1)
        count_3 = 1;
    else 
        count_3 = 0;
end


always@(posedge clk)
begin
    M_reg1<=M;
    M_reg2<=M_reg1;
    M_reg3<=M_reg2;
    M_reg4<=M_reg3;
end


always@(posedge clk)
begin
    x_reg1<=x;
    x_reg2<=x_reg1;
end

always @(posedge clk)
begin
    flag_x_0_reg1<=flag_x_0;
    flag_x_0_reg2<=flag_x_0_reg1;
    flag_x_0_reg3<=flag_x_0_reg2;
    flag_x_0_reg4<=flag_x_0_reg3;
    flag_x_0_reg5<=flag_x_0_reg4;
end


sqrt_4_unsigned u_sqrt_4_unsigned(
    .clk(clk),
    .x_2Q9(x_2Q9), // 11bit无符号数，整数位2，小数位9
    .y_1Q10(y_1Q10) 
);

always@(*) y_10Q10=tp_y_10Q10[19:10]+tp_y_10Q10[9];

endmodule
