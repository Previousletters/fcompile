`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module Accumulation
(
	input clk,
	input rst_n,
    input relu,
    input [`MAX_log2BN_DW-1:0]out_data_width, //'b111 means 8bit, 'b11 means 4bit, 'b1 means 2bit,'b0 means 1bit

	//from CSR
	input [`log2_scale-1:0]conv_out_shift,

	//dat from CMAC
	input dat_vld_i,
    
	input Wout_loop_end_i,
	input CHinKyKx_max_now_i,
	input CHinKyKxWout_loop_end_i,
	input [`Tout*(`MAX_DW2+`base_log2Tin)-1:0]dat_i,

	//dat to SDP
	output reg conv_out_dat_vld,
	output reg [`MAX_BN_DW*`Tout-1:0]conv_out_dat
);

wire wr_en;
wire [`log2Tout-1:0]wr_addr;
wire [`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)-1:0]wr_dat;
wire rd_en;
wire [`log2Tout-1:0]rd_addr;
wire [`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)-1:0]rd_dat;
wire rd_dat_vld;


center_buf  #
(
   .DATA_WIDTH(`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)),
   .DEPTH(`Tout),
   .log2_DEPTH(`log2Tout)
)u_center_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    //Wr Port
    .wr_en(wr_en),
    .wr_addr(wr_addr),
    .wr_dat(wr_dat),

    //Rd Port
    .rd_en(rd_en),
    .rd_addr(rd_addr),
    .rd_dat_vld(rd_dat_vld),
    .rd_dat(rd_dat)                
);   

reg [`log2Tout-1:0]ptr;
reg [`log2Tout-1:0]ptr_d1;
always @(posedge clk) ptr_d1<=ptr;

reg load_new_data, load_new_data_d1;
reg [`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)-1:0]adder_out;
reg [`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)-1:0]adder_in_0;
reg [`Tout*(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)-1:0]origin_dat_in;
reg [`Tout*(`MAX_DW2+`base_log2Tin)-1:0]adder_in_1;

always @(posedge clk) load_new_data_d1<=load_new_data;
always @(*) adder_in_0=load_new_data_d1?0:rd_dat;

always @(posedge clk)
    adder_in_1<=dat_i;
	
integer i;
always @(*)
begin
    for(i=0;i<`Tout;i=i+1)
    begin
        adder_out[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)*i+:(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)]=
        $signed(adder_in_0[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)*i+:(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)])
        +$signed(adder_in_1[(`MAX_DW2+`base_log2Tin)*i+:(`MAX_DW2+`base_log2Tin)]);
		
		origin_dat_in[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)*i+:(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)]=
		  $signed(dat_i[(`MAX_DW2+`base_log2Tin)*i+:(`MAX_DW2+`base_log2Tin)]);
    end
end

always @(posedge clk or negedge rst_n)
if(~rst_n)
    load_new_data<=1'b1;
else
    if(CHinKyKxWout_loop_end_i & dat_vld_i)
        load_new_data<=1'b1;
    else
        if(Wout_loop_end_i & dat_vld_i)
            load_new_data<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
    ptr<='d0;
else
    if(Wout_loop_end_i & dat_vld_i)
        ptr<='d0;
    else
        if(dat_vld_i)
            ptr<=ptr+'d1;

assign rd_en=1'b1;
assign rd_addr=ptr;

reg dat_vld_i_d1;
always @(posedge clk) dat_vld_i_d1<=dat_vld_i;

assign wr_en=(dat_vld_i&load_new_data)|(dat_vld_i_d1&(!load_new_data));
assign wr_addr=(dat_vld_i&load_new_data)?ptr:((dat_vld_i_d1&(!load_new_data))?ptr_d1:0);
assign wr_dat=(dat_vld_i&load_new_data)?origin_dat_in:((dat_vld_i_d1&(!load_new_data))?adder_out:0);


///////////////////////////
reg [`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1:0]tp[`Tout-1:0];
reg [`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1:0]tp1[`Tout-1:0];
reg [`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1:0]tp2[`Tout-1:0];
reg [1:0]round_dat[`Tout-1:0];
reg [0:0]tp_sat_1bit[`Tout-1:0];
reg [1:0]tp_sat_2bit[`Tout-1:0];
reg [3:0]tp_sat_4bit[`Tout-1:0];
reg [`MAX_BN_DW-1:0]tp_sat_8bit[`Tout-1:0];

always @(*)
begin
	for(i=0;i<`Tout;i=i+1)
	begin
		tp[i]=adder_out[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)*i+:(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other)];
		//right shift and round
        tp1[i]=$signed(tp[i])>>>conv_out_shift[`log2_scale-2:0];
        round_dat[i]=((conv_out_shift!=0)&&(tp1[i]!={1'b0, {(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1){1'b1}} }))?
                      {1'b0,tp[i][conv_out_shift-1]}:0;
	end
end 

always @(posedge clk)
begin
	for(i=0;i<`Tout;i=i+1)
	begin
        tp2[i]<=$signed(tp1[i])+$signed(round_dat[i]);
	end
end 

always @(posedge clk)
begin
	for(i=0;i<`Tout;i=i+1)
	begin
		case(out_data_width) //if(out_data_width==3'b111)  
		3'b111: //8bit
		begin  
			if( (tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (!(&tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):(`MAX_BN_DW-1)])) ) //tp2 is negetive and tp2[`MAX_DW2+`log2Tc-2:(`MAX_DW-1)] is not all 1, means tp2 < `MAX_DW'h8000(-2^(`MAX_DW-1))
				tp_sat_8bit[i]<={1'b1,{(`MAX_BN_DW-1){1'b0}}};// sat to -2^(`MAX_DW-1)
			else
			begin
				if( (!tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):(`MAX_BN_DW-1)]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
					tp_sat_8bit[i]<={1'b0,{(`MAX_BN_DW-1){1'b1}}};//`MAX_DW'h7fff;
				else
					tp_sat_8bit[i]<=tp2[i][(`MAX_BN_DW-1):0];    
			end
		end
		
        3'b011: //4bit
        begin  
            if( (tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (!(&tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):3])) ) //tp2 is negetive and tp2[`MAX_DW2+`log2Tc-2:(`MAX_DW-1)] is not all 1, means tp2 < `MAX_DW'h8000(-2^(`MAX_DW-1))
                tp_sat_4bit[i]<={1'b1,{3{1'b0}}};// sat to -2^(`MAX_DW-1)
            else
            begin
                if( (!tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):3]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
                    tp_sat_4bit[i]<={1'b0,{3{1'b1}}};//`MAX_DW'h7fff;
                else
                    tp_sat_4bit[i]<=tp2[i][3:0];    
            end
        end
		
        3'b001: //2bit 
        begin       
            if( (tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (!(&tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):1])) ) //tp2 is negetive and tp2[`MAX_DW2+`log2Tc-2:(`MAX_DW-1)] is not all 1, means tp2 < `MAX_DW'h8000(-2^(`MAX_DW-1))
                tp_sat_2bit[i]<={1'b1,{(1){1'b0}}};// sat to -2^(`MAX_DW-1)
            else
            begin
                if( (!tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):1]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
                    tp_sat_2bit[i]<={1'b0,{(1){1'b1}}};//`MAX_DW'h7fff;
                else
                    tp_sat_2bit[i]<=tp2[i][1:0];    
            end
        end
        
        default: //1bit
        begin       
            tp_sat_1bit[i]=($signed(tp2[i])<0)?1:0;  //for 1bit: out=1 means -1 
//          if(tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (!(&tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):1])) ) //tp2 is negetive and tp2[`MAX_DW2+`log2Tc-2:(`MAX_DW-1)] is not all 1, means tp2 < `MAX_DW'h8000(-2^(`MAX_DW-1))
//                tp_sat_1bit[i]<={1'b1,{(0){1'b0}}};// sat to -2^(`MAX_DW-1)
//            else
//            begin
//                if( (!tp2[i][`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[i][(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):1]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
//                    tp_sat_1bit[i]<={1'b0,{(0){1'b1}}};//`MAX_DW'h7fff;
//                else
//                    tp_sat_1bit[i]<=tp2[i][0:0];    
//            end
        end

        endcase    
	end
end


always @(posedge clk)
begin
	for(i=0;i<`Tout;i=i+1)
	begin
		case(out_data_width)
			3'b111:conv_out_dat[`MAX_BN_DW*i+:`MAX_BN_DW]<=(relu&(tp_sat_8bit[i][`MAX_BN_DW-1]))?0:$signed(tp_sat_8bit[i]);
			3'b011:conv_out_dat[`MAX_BN_DW*i+:`MAX_BN_DW]<=(relu&(tp_sat_4bit[i][4-1]))?0:$signed(tp_sat_4bit[i]);
			3'b001:conv_out_dat[`MAX_BN_DW*i+:`MAX_BN_DW]<=(relu&(tp_sat_2bit[i][2-1]))?0:$signed(tp_sat_2bit[i]);
			default:conv_out_dat[`MAX_BN_DW*i+:`MAX_BN_DW]<=tp_sat_1bit[i]; //for 1bit: out=1 means -1 
		endcase
	end
end      



reg conv_out_dat_vld_d0,conv_out_dat_vld_d1,conv_out_dat_vld_d2;      
always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_out_dat_vld_d0<=1'b0;
else
	conv_out_dat_vld_d0<=dat_vld_i&CHinKyKx_max_now_i;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_out_dat_vld_d1<=1'b0;
else
	conv_out_dat_vld_d1<=conv_out_dat_vld_d0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_out_dat_vld_d2<=1'b0;
else
	conv_out_dat_vld_d2<=conv_out_dat_vld_d1;
		
always @(posedge clk or negedge rst_n)
if(~rst_n)
	conv_out_dat_vld<=1'b0;
else
	conv_out_dat_vld<=conv_out_dat_vld_d2;
	

endmodule
