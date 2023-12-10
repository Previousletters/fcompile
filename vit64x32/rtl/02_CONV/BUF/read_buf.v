`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module read_buf
(
	input clk,
	input rst_n,
	//from CSR
	input [`log2BRAM_NUM-1:0]dat_buf_num,
	input [`log2_W-1:0]Win,
	input [`log2_H-1:0]Hin,
	input [(`log2_W+`log2_H-1):0]Hin_x_Win,					//Win*Hin
    input conv_mode,
    
	//conv feature addr from FSM
	input feature_data_vld,
	input feature_data_rdy,
	input [`log2_H:0]feature_hin,
	input [`log2_W:0]feature_win,
	input [`log2_CH-1:0]feature_chin,
    
	//wt_addr from FSM
	input wt_addr_vld,
	input wt_addr_rdy,
	input [`log2BUF_DEP-1:0]wt_addr,

	//signal to/from logic memorys
	output reg [`BRAM_NUM-1:0]logic_mem_rd_en,
	output reg [`BRAM_NUM*`log2BRAM_DEPTH-1:0]logic_mem_rd_addr,
	input [`BRAM_NUM-1:0]logic_mem_rd_dat_vld,
	input [`BRAM_NUM*`base_Tin*`MAX_DW-1:0]logic_mem_rd_dat,

	//dat to FSM
	output reg BUF2MAC_dat_vld,
    output reg [`base_Tin-1:0]dat_vld_copy,
    output reg [`base_Tin*`MAX_DW-1:0]BUF2MAC_dat,

	//wt to FSM
	output reg BUF2MAC_wt_vld,
	output reg [`base_Tin*`MAX_DW-1:0]BUF2MAC_wt
);


wire fsm2buf_dat_rd_en=feature_data_vld&feature_data_rdy;

wire fsm2buf_dat_out_of_bound=fsm2buf_dat_rd_en&
                                (feature_hin[`log2_H]|feature_win[`log2_W]|(feature_hin[`log2_H-1:0]>=Hin)|(feature_win[`log2_W-1:0]>=Win)); 

wire fsm2buf_wt_rd_en=wt_addr_vld&wt_addr_rdy;
wire [`log2BUF_DEP-1:0]fsm2buf_wt_rd_addr=wt_addr+{dat_buf_num,{`log2BRAM_DEPTH{1'b0}}};

reg [`log2BUF_DEP-1:0] rd_addr1;
reg [`log2BUF_DEP-1:0] rd_addr2;
reg [`log2BUF_DEP-1:0] rd_addr3;
reg [`log2BUF_DEP-1:0]fsm2buf_dat_rd_addr_d;

always @(posedge clk)
begin
	rd_addr1<=Hin_x_Win*feature_chin;
    rd_addr2<=feature_hin[`log2_H-1:0]*Win;
    rd_addr3<=feature_win[`log2_W-1:0];
end
always @(posedge clk)
    fsm2buf_dat_rd_addr_d<=rd_addr1+rd_addr2+rd_addr3;
    
    
reg [`log2BUF_DEP-1:0]fsm2buf_wt_rd_addr_d[`BUF_ADDR_LATENCY-1:0];
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	fsm2buf_wt_rd_addr_d[0]<=0;
    fsm2buf_wt_rd_addr_d[1]<=0;
end
else
begin
	fsm2buf_wt_rd_addr_d[0]<=fsm2buf_wt_rd_addr;
    fsm2buf_wt_rd_addr_d[1]<=fsm2buf_wt_rd_addr_d[0];
end

reg [`BUF_ADDR_LATENCY-1:0]fsm2buf_dat_rd_en_d;
reg [`BUF_ADDR_LATENCY-1:0]fsm2buf_wt_rd_en_d;
reg [`BUF_ADDR_LATENCY-1:0]fsm2buf_dat_out_of_bound_d;

generate
	begin
		always @(posedge clk or negedge rst_n)
		if(~rst_n)
		begin
			fsm2buf_dat_out_of_bound_d<=0;
			fsm2buf_dat_rd_en_d<=0;
			fsm2buf_wt_rd_en_d<=0;
		end
		else
		begin
            fsm2buf_dat_rd_en_d<={fsm2buf_dat_rd_en_d[`BUF_ADDR_LATENCY-2:0],fsm2buf_dat_rd_en};
            fsm2buf_wt_rd_en_d<={fsm2buf_wt_rd_en_d[`BUF_ADDR_LATENCY-2:0],fsm2buf_wt_rd_en};
			fsm2buf_dat_out_of_bound_d<={fsm2buf_dat_out_of_bound_d[`BUF_ADDR_LATENCY-2:0],fsm2buf_dat_out_of_bound};
	    end
	end
endgenerate

integer i;
reg [`BRAM_NUM-1:0]dat_mem_sel;
reg [`BRAM_NUM-1:0]wt_mem_sel;

always @(*)
begin
	for(i=0;i<`BRAM_NUM;i=i+1)
	begin
		dat_mem_sel[i]= (fsm2buf_dat_rd_en_d[`BUF_ADDR_LATENCY-2] & (fsm2buf_dat_rd_addr_d[`log2BUF_DEP-1:`log2BRAM_DEPTH]== i)  
		                                      & (~fsm2buf_dat_out_of_bound_d[`BUF_ADDR_LATENCY-2]));
		wt_mem_sel[i]=  fsm2buf_wt_rd_en_d[`BUF_ADDR_LATENCY-2] &  (fsm2buf_wt_rd_addr_d[`BUF_ADDR_LATENCY-2] [`log2BUF_DEP-1:`log2BRAM_DEPTH]== i);
	end
end

always @(*)
begin
	for(i=0;i<`BRAM_NUM;i=i+1)
	begin
		logic_mem_rd_en[i]=dat_mem_sel[i]|wt_mem_sel[i];
		logic_mem_rd_addr[i*`log2BRAM_DEPTH+:`log2BRAM_DEPTH]= dat_mem_sel[i]?  fsm2buf_dat_rd_addr_d[`log2BRAM_DEPTH-1:0]:
		                           (wt_mem_sel[i]?fsm2buf_wt_rd_addr_d[`BUF_ADDR_LATENCY-2][`log2BRAM_DEPTH-1:0]:0);
	end
end

(* MAX_FANOUT = 128 *)reg [`BRAM_NUM-1:0]dat_mem_region;
(* MAX_FANOUT = 128 *)reg [`BRAM_NUM-1:0]wt_mem_region;

always @(posedge clk)
begin
	for(i=0;i<`BRAM_NUM;i=i+1)
	begin
		if(i<dat_buf_num)
			dat_mem_region[i]<=1'b1;
		else
			dat_mem_region[i]<=1'b0;
	end
end

always @(posedge clk)
begin
	for(i=0;i<`BRAM_NUM;i=i+1)
	begin
		if(i<dat_buf_num)
			wt_mem_region[i]<=1'b0;
		else
			wt_mem_region[i]<=1'b1;
	end
end


(* MAX_FANOUT = 128 *) reg wt_vld_copy;
always@(*) BUF2MAC_wt_vld<=wt_vld_copy;

always@(posedge clk)
begin
    BUF2MAC_dat_vld<=(conv_mode) & ((|(dat_mem_region & logic_mem_rd_dat_vld)) 
                                                | fsm2buf_dat_out_of_bound_d[`BUF_ADDR_LATENCY-1]);
    wt_vld_copy<=(conv_mode) & (|(wt_mem_region & logic_mem_rd_dat_vld)); // In each cycel, there is ONLY one channel will be selected.
end

genvar j;
generate
    for(j=0;j<`base_Tin;j=j+1)
    begin:dat_vld_dep
        always@(posedge clk)
        begin
            dat_vld_copy[j]<=(conv_mode) & ((|(dat_mem_region & logic_mem_rd_dat_vld)) 
                              | fsm2buf_dat_out_of_bound_d[`BUF_ADDR_LATENCY-1]);
        end
    end
endgenerate

reg [`base_Tin*`MAX_DW-1:0]tp_feature; // (dat_mem_region[i]&logic_mem_rd_dat_vld[i]) means the vld i
always @(*)
begin
	tp_feature=0;
	for(i=0;i<`BRAM_NUM-1;i=i+1) // i ends at num-1 because the last one is always for weight.
	begin
		tp_feature = tp_feature | ( (dat_mem_region[i]&logic_mem_rd_dat_vld[i])?logic_mem_rd_dat[`base_Tin*`MAX_DW*i+:`base_Tin*`MAX_DW]:{(`base_Tin*`MAX_DW){1'b0}} );
	end              // the tp_feature|xxx is to transfer the selected value to the output(BUF2MAC_dat).
	                 // In each cycel, there is ONLY one channel of feature_dat will be selected.
//	BUF2MAC_dat=tp_feature;//(conv_mode)?tp_feature:'b0;
end

always@(posedge clk) BUF2MAC_dat<=tp_feature;//(conv_mode)?tp_feature:'b0;

reg [`base_Tin*`MAX_DW-1:0]tp_wt; // (wt_mem_region[i]&logic_mem_rd_dat_vld[i]) means the vld i
always @(*)
begin
	tp_wt=0;
	for(i=1;i<`BRAM_NUM;i=i+1) // i starts from 1 because the first one is always for feature.
	begin
		tp_wt = tp_wt | ( (wt_mem_region[i]&logic_mem_rd_dat_vld[i])?logic_mem_rd_dat[`base_Tin*`MAX_DW*i+:`base_Tin*`MAX_DW]:{(`base_Tin*`MAX_DW){1'b0}} );
	end
//	BUF2MAC_wt=tp_wt;
end
always@(posedge clk) BUF2MAC_wt<=tp_wt;

endmodule


