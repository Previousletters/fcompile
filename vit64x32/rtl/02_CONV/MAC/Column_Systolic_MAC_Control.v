`include "CNN_defines.vh"

module Column_Systolic_MAC_Control
(
    input clk,
    input rst_n,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit    
    
    //data from BUF
    input dat_vld,
    input [`base_Tin*`MAX_DAT_DW-1:0]dat,
    input Wout_loop_start,
    input Wout_loop_end,
    
    //wt from BUF
    input wt_vld,
    input [`base_Tin*`MAX_WT_DW-1:0]wt,
    input [`log2Tout-1:0]wt_cnt,
    
    //dat to ACC
    output [`Tout*(`MAX_DW2+`base_log2Tin)-1:0]dat_o
);

//////// start feature /////////////
reg [`base_Tin*`MAX_DAT_DW-1:0]dat_to_mac;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    dat_to_mac<='d0;
else
    if(dat_vld)
        dat_to_mac<=dat;

reg dat_to_mac_vld;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    dat_to_mac_vld<=1'b0;
else
    dat_to_mac_vld<=dat_vld;


//////// start wt ///////////////
wire wt_load_new=wt_vld&(wt_cnt==0);
reg [1:0]wt_load_cnt;
always @(posedge clk or negedge rst_n)
if(~rst_n)
    wt_load_cnt<=2'd0;
else
    case({wt_load_new,Wout_loop_start})
        2'b01:wt_load_cnt<=wt_load_cnt-2'd1;
        2'b10:wt_load_cnt<=wt_load_cnt+2'd1;
    endcase

wire wt_load_reg2=(wt_load_cnt==2'd1)&(wt_load_new|Wout_loop_start);
reg [`base_Tin*`MAX_WT_DW-1:0]wt_reg1[`Tout-1:0];
genvar i;
generate
    for(i=0;i<`Tout;i=i+1)
    begin:wt_reg1_b
        always @(posedge clk)
            if(wt_vld&(wt_cnt==i))
                wt_reg1[i]<=wt;
    end
endgenerate


reg [`base_Tin*`MAX_WT_DW*`Tn-1:0]wt_reg2[`Tout/`Tn-1:0];
genvar n,m;
generate
    for(n=0;n<`Tout/`Tn;n=n+1)
    begin:wt_reg_a
        for(m=0;m<`Tn;m=m+1)
        begin:wt_reg_b
            always @(posedge clk)
            if(wt_load_reg2)
                wt_reg2[n][m*(`base_Tin*`MAX_WT_DW)+:(`base_Tin*`MAX_WT_DW)]<=wt_reg1[n*`Tn+m];
        end
    end
endgenerate


reg [`base_Tin*`MAX_WT_DW*`Tn-1:0]wt_to_mac[`Tout/`Tn-1:1];
(* MAX_FANOUT = 128 *)wire [`Tout/`Tn-1:1]wt_to_mac_vld;


generate
    for(n=1;n<`Tout/`Tn;n=n+1)
    begin:shift_reg_vld_b
         generate_vld_shift #
        (
            .DATA_WIDTH(1),
            .DEPTH(n)
        )wt_shift_reg
        (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(wt_load_reg2),
            .data_out(wt_to_mac_vld[n])
        );
    end
endgenerate

generate
    for(n=1;n<`Tout/`Tn;n=n+1)
    begin:wt_to_mac_b
        always @(posedge clk)
            if(wt_to_mac_vld[n])
                wt_to_mac[n]<=wt_reg2[n];
    end
endgenerate


//////// start calculation ////////
wire [`base_Tin*`MAX_DAT_DW-1:0]right_dat_out[`Tout/`Tn-1:0];
wire [(`base_log2Tin+`MAX_DW2)*`Tn-1:0]down_dat_out[`Tout/`Tn-1:0];
//(* dont_touch="yes" *)(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d0;
(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d0;
always@(posedge clk) Tin_factor_d0<=Tin_factor;

Systolic_PE_Column Systolic_PE_Column0
(
    .clk(clk),
    .rst_n(rst_n),
    .Tin_factor(Tin_factor_d0),
        
    .wt(wt_reg2[0]),
    .left_dat_in(dat_to_mac),    
    .right_dat_out(right_dat_out[0]),
    
    .down_dat_out(down_dat_out[0])
);

(* MAX_FANOUT=128 *) reg [3:0]Tin_factor_d[`Tout/`Tn-1:0];
generate
    for(n=1;n<`Tout/`Tn;n=n+1)
    begin:col_pe
    
        always@(posedge clk)
            Tin_factor_d[n]<=Tin_factor;
    
        Systolic_PE_Column Systolic_PE_Column
        (
            .clk(clk),
            .rst_n(rst_n),
            .Tin_factor(Tin_factor_d[n]),

            .wt(wt_to_mac[n]),
            .left_dat_in(right_dat_out[n-1]),    
            .right_dat_out(right_dat_out[n]),
            
            .down_dat_out(down_dat_out[n])
        );
    end

endgenerate

///start ouptut shift
wire [`Tout*(`base_log2Tin+`MAX_DW2)-1:0]dat_to_cacc;
wire [(`MAX_DW2+`base_log2Tin)*`Tn-1:0] dat_out_shift[`Tout/`Tn-1:0];

generate
    for(n=0;n<`Tout/`Tn;n=n+1)
    begin:shift_reg_b
        generate_dat_shift #
        (
            .DATA_WIDTH((`MAX_DW2+`base_log2Tin)*`Tn),
            .DEPTH(`Tout/`Tn-n)
        )dat_out_shift_reg
        (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(down_dat_out[n]),
            .data_out(dat_out_shift[n])
        );
        assign dat_to_cacc[(n*`Tn+`Tn)*(`MAX_DW2+`base_log2Tin)-1:(n*`Tn)*(`MAX_DW2+`base_log2Tin)]=dat_out_shift[n];
    end
endgenerate

assign dat_o=dat_to_cacc;

endmodule
