`include "CNN_defines.vh"


module Two_Col_MAC_Calculation
(
    input clk,
    input rst_n,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit
    input [`base_Tin*`MAX_DAT_DW-1:0]dat,
    input [`base_Tin*`MAX_WT_DW*2-1:0]wt,
    output reg[(`MAX_DW2+`base_log2Tin)*2-1:0]dat_o
);

reg [`MAX_DAT_DW-1 :0] conv_dat    [(`base_Tin/8)-1:0][8-1 : 0];
reg [ `MAX_WT_DW-1 :0] conv_wt[1:0][(`base_Tin/8)-1:0][8-1 : 0];

reg [`MAX_DAT_DW-1 :0] INT8_conv_dat[(`base_Tin/8)-1:0][8-1 : 0];
reg [`MAX_DAT_DW-1 :0] INT4_conv_dat[(`base_Tin/8)-1:0][8-1 : 0];
reg [          4-1 :0] INT2_conv_dat[(`base_Tin/8)-1:0][8-1 : 0];
reg [`MAX_DAT_DW-1 :0] INT1_conv_dat[(`base_Tin/8)-1:0][8-1 : 0];

wire [   5*`base_Tin-1:0] acc_in_1bit  [1:0];
wire [   4*`base_Tin-1:0] acc_in_2bit  [1:0];
wire [             8-1:0] acc_in_4bit  [1:0][(`base_Tin/8)-1:0][8-1 : 0];
wire [      `MAX_DW2-1:0] acc_in_8bit  [1:0][(`base_Tin/8)-1:0][8-1 : 0];

wire [(4+`base_log2Tin)-1:0]        acc_out_2bit [1:0];
wire [(5+`base_log2Tin)-1:0]        acc_out_1bit [1:0];

reg  [      `MAX_DW2*`base_Tin-1:0] acc_in       [1:0];
wire [(`MAX_DW2+`base_log2Tin)-1:0] acc_out      [1:0];


genvar n,i,j;
(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d[7:0];
generate
    for(j=0;j<8;j=j+1)
    begin:j2
        always@(posedge clk)
            Tin_factor_d[j]<=Tin_factor;
    end    
endgenerate


generate
    for(i=0;i<(`base_Tin/8);i=i+1)
    begin:i1
        for(j=0;j<8;j=j+1)
        begin:i2
        
            always@(*)
            begin
                conv_dat[i][j]   = dat[(i*8+j)*`MAX_DAT_DW+`MAX_DAT_DW-1 : (i*8+j)*`MAX_DAT_DW];
                conv_wt[0][i][j] = wt[(i*8+j)*`MAX_WT_DW+`MAX_WT_DW-1 : (i*8+j)*`MAX_WT_DW];
                conv_wt[1][i][j] = wt[(`base_Tin+i*8+j)*`MAX_WT_DW+`MAX_WT_DW-1 : (`base_Tin+i*8+j)*`MAX_WT_DW];   
                
                INT8_conv_dat[i][j] = conv_dat[i][j];
                INT4_conv_dat[i][j] = conv_dat[i][j];
                INT2_conv_dat[i][j] = {conv_dat[i][j][6],conv_dat[i][j][4],conv_dat[i][j][2],conv_dat[i][j][0]};
                INT1_conv_dat[i][j] = conv_dat[i][j];
            end


            Two_Col_INT248_MAC u_MAC
            (
                .clk(clk),
                .Tin_factor   (Tin_factor_d[j]),
                .INT8_dat_in  (INT8_conv_dat   [i][j]),
                .INT8_wt1     (conv_wt      [1][i][j]),
                .INT8_wt0     (conv_wt      [0][i][j]),
                .INT8_sum_out1(acc_in_8bit  [1][i][j]),
                .INT8_sum_out0(acc_in_8bit  [0][i][j]),
                
                .INT4_dat_in  (INT4_conv_dat   [i][j]),
                .INT4_wt1     (conv_wt      [1][i][j]),
                .INT4_wt0     (conv_wt      [0][i][j]),
                .INT4_sum_out1(acc_in_4bit  [1][i][j]),
                .INT4_sum_out0(acc_in_4bit  [0][i][j]),
                                
                .INT2_dat_in  (INT2_conv_dat   [i][j]),
                .INT2_wt1     (conv_wt      [1][i][j]),
                .INT2_wt0     (conv_wt      [0][i][j]),
                .INT2_sum_out1(acc_in_2bit  [1][(i*8+j)*4+:4]),
                .INT2_sum_out0(acc_in_2bit  [0][(i*8+j)*4+:4]),
                
                .INT1_dat_in  (INT1_conv_dat   [i][j]),
                .INT1_wt1     (conv_wt      [1][i][j]),
                .INT1_wt0     (conv_wt      [0][i][j]),
                .INT1_sum_out1(acc_in_1bit  [1][(i*8+j)*5+:5]),
                .INT1_sum_out0(acc_in_1bit  [0][(i*8+j)*5+:5])
            );
        end
    end

endgenerate  

generate
	for(n=0;n<2;n=n+1)
    begin:in_n
        for(i=0;i<(`base_Tin/8);i=i+1)
        begin:in_i
            for(j=0;j<8;j=j+1)
            begin:in_j

                always@(*)
                    case(Tin_factor_d[j])// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit

`ifdef INT4_Enable
                        2:begin acc_in[n][(i*8+j)*`MAX_DW2+:`MAX_DW2]<=$signed(acc_in_4bit[n][i][j]); end
`endif

                        1:begin acc_in[n][(i*8+j)*`MAX_DW2+:`MAX_DW2]<=$signed(acc_in_8bit[n][i][j]); end
                    endcase 

            end
        end
    end
endgenerate

generate
	for(n=0;n<2;n=n+1)
	begin:col_

		Tin_acc # 
		(
		  .DATA_WIDTH(`MAX_DW2)
		)Acc48
		(
			.clk(clk),
			.i_dat(acc_in[n]),
			.o_dat(acc_out[n])
		);   

		Tin_acc # 
		(
		  .DATA_WIDTH(4)
		)Acc2
		(
			.clk(clk),
			.i_dat(acc_in_2bit[n]),
			.o_dat(acc_out_2bit[n])
		);

		Tin_acc # 
		(
		  .DATA_WIDTH(5)
		)Acc1
		(
			.clk(clk),
			.i_dat(acc_in_1bit[n]),
			.o_dat(acc_out_1bit[n])
		);
				    
    always@(*)
        case(Tin_factor)// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit
//                  8:dat_o[(`MAX_DW2+`base_log2Tin)*n+:(`MAX_DW2+`base_log2Tin)]=$signed(acc_out_1bit[n]);
//                  4:dat_o[(`MAX_DW2+`base_log2Tin)*n+:(`MAX_DW2+`base_log2Tin)]=$signed(acc_out_2bit[n]);
            default:dat_o[(`MAX_DW2+`base_log2Tin)*n+:(`MAX_DW2+`base_log2Tin)]=acc_out[n];
            endcase 
        
	end
endgenerate

endmodule       
     
