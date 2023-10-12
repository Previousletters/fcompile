`include "CNN_defines.vh"

module CONV_BUF
(
	input clk,
	input rst_n,
    input conv_mode,
	//from CSR
	input fsm_logic_init,
	input [`log2BRAM_NUM-1:0]dat_buf_num,
	input [`log2_W-1:0]Win,
	input [`log2_H-1:0]Hin,
	input [(`log2_W+`log2_H-1):0]Hin_x_Win,					//Win*Hin
	input dma_dat_reuse,
	input dma_wt_reuse,

	//for Backpressure
	input CHinKyKx_max_now_comb,
	input credit_vld,

	//dat from DMA 
	input dma2buf_DAT_wr_en,
	input [`log2BUF_DEP-1:0]dma2buf_DAT_wr_addr,
	input [`base_Tin*`MAX_DAT_DW-1:0]dma2buf_DAT_wr_data,

	//row information from DMA
	input row_num_updt,
	input [`log2_H-1:0]row_num,
    input chin_num_updt,
    input [`log2_CH-1:0]chin_num,
    
	//wt from DMA
	input dma2buf_WT_wr_en,
	input [`log2BUF_DEP-1:0]dma2buf_WT_wr_addr,
	input [`base_Tin*`MAX_WT_DW-1:0]dma2buf_WT_wr_data,

	//wt information from DMA
	input wt_addr_updt,
	input [`log2BUF_DEP-1:0]wt_addr_dma,
    
	//axis from FSM
	input feature_data_vld,
	input [`log2_H:0]feature_hin,
	input [`log2_W:0]feature_win,
	input [`log2_CH-1:0]feature_chin,
	input Wout_loop_end_comb,
	output feature_data_rdy,
	
	//wt_addr from FSM
	input wt_addr_vld,
	output wt_addr_rdy,
	input wt_last_comb,
	input [`log2BUF_DEP-1:0]wt_addr,

	//dat to FSM
	output BUF2MAC_dat_vld,
	output [`base_Tin-1:0]dat_vld_copy,
	output [`base_Tin*`MAX_DAT_DW-1:0]BUF2MAC_dat,

	//wt to FSM
	output BUF2MAC_wt_vld,
	output [`base_Tin*`MAX_WT_DW-1:0]BUF2MAC_wt
);

wire [`BRAM_NUM-1:0]logic_mem_wr_en;
wire [`log2BRAM_DEPTH-1:0]logic_mem_wr_addr;
wire [`base_Tin*`MAX_DW-1:0]logic_mem_wr_dat;

wire [`BRAM_NUM-1:0]logic_mem_rd_en;
wire [`BRAM_NUM*`log2BRAM_DEPTH-1:0]logic_mem_rd_addr;
wire [`BRAM_NUM-1:0]logic_mem_rd_dat_vld;
wire [`BRAM_NUM*`base_Tin*`MAX_DW-1:0]logic_mem_rd_dat;

write_buf write_buf
(
    .dma2buf_DAT_wr_en(dma2buf_DAT_wr_en),
    .dma2buf_DAT_wr_addr(dma2buf_DAT_wr_addr),
    .dma2buf_DAT_wr_data(dma2buf_DAT_wr_data),

    .dma2buf_WT_wr_en(dma2buf_WT_wr_en),
    .dma2buf_WT_wr_addr(dma2buf_WT_wr_addr),
    .dma2buf_WT_wr_data(dma2buf_WT_wr_data),

    .logic_mem_wr_en(logic_mem_wr_en),
    .logic_mem_wr_addr(logic_mem_wr_addr),
    .logic_mem_wr_dat(logic_mem_wr_dat)
);

genvar i;
generate
    for(i=0;i<`BRAM_NUM;i=i+1)
    begin:conv_buf_loop_i
        center_buf  #
        (
           .DATA_WIDTH(`base_Tin*`MAX_DW),
           .DEPTH(`BRAM_DEPTH),
           .log2_DEPTH(`log2BRAM_DEPTH)
        )u_center_buf
        (
            .clk(clk),
            .rst_n(rst_n),
            
            //Wr Port
            .wr_en(logic_mem_wr_en[i]),
            .wr_addr(logic_mem_wr_addr),
            .wr_dat(logic_mem_wr_dat),
        
            //Rd Port
            .rd_en(logic_mem_rd_en[i]),
            .rd_addr(logic_mem_rd_addr[`log2BRAM_DEPTH*i+:`log2BRAM_DEPTH]),
            .rd_dat_vld(logic_mem_rd_dat_vld[i]),
            .rd_dat(logic_mem_rd_dat[`base_Tin*`MAX_DW*i+:`base_Tin*`MAX_DW])                
        );    
    end
endgenerate
    
wire [`base_Tin*`MAX_DAT_DW-1:0]tp_BUF2MAC_dat;
assign BUF2MAC_dat=tp_BUF2MAC_dat;

(* MAX_FANOUT=128 *)reg [`log2BRAM_NUM-1:0]dat_buf_num_copy;
always@(posedge clk)
    dat_buf_num_copy<=dat_buf_num;

read_buf read_buf
(
	.clk(clk),
	.rst_n(rst_n),
    .conv_mode(conv_mode),
    
	//from CSR
	.dat_buf_num(dat_buf_num),
	.Win(Win),
	.Hin(Hin),
	.Hin_x_Win(Hin_x_Win),					//Win*Hin

	
	//axis from FSM
	.feature_data_vld(feature_data_vld),
	.feature_data_rdy(feature_data_rdy),
	.feature_hin(feature_hin),
	.feature_win(feature_win),
	.feature_chin(feature_chin),
    
	//wt_addr from FSM
	.wt_addr_vld(wt_addr_vld),
	.wt_addr_rdy(wt_addr_rdy),
	.wt_addr(wt_addr),

	//signal to/from logic memorys
	.logic_mem_rd_en(logic_mem_rd_en),
	.logic_mem_rd_addr(logic_mem_rd_addr),
	.logic_mem_rd_dat_vld(logic_mem_rd_dat_vld),
	.logic_mem_rd_dat(logic_mem_rd_dat),
    
	//dat to FSM
	.BUF2MAC_dat_vld(BUF2MAC_dat_vld),
	.dat_vld_copy(dat_vld_copy),
	.BUF2MAC_dat(tp_BUF2MAC_dat),

	//wt to FSM
	.BUF2MAC_wt_vld(BUF2MAC_wt_vld),
	.BUF2MAC_wt(BUF2MAC_wt)
);


wire dat_ch_go;
wire wt_ch_go;
buf_stripe_control buf_stripe_control
(
	.clk(clk),
	.rst_n(rst_n),

	//for Backpressure
	.feature_data_vld(feature_data_vld),
	.feature_data_rdy(feature_data_rdy),
	.CHinKyKx_max_now_comb(CHinKyKx_max_now_comb),
	.credit_vld(credit_vld),

	.Wout_loop_end_comb(Wout_loop_end_comb),
	.wt_last_comb(wt_last_comb),
	
	.dat_ch_go(dat_ch_go),
	.wt_ch_go(wt_ch_go)
);

buf_dat_ack buf_dat_ack
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.fsm_logic_init(fsm_logic_init),
	.Hin(Hin),
	.dma_dat_reuse(dma_dat_reuse),
	
	//row information from DMA
	.row_num_updt(row_num_updt),
	.row_num(row_num),

	//'go' from buf_stripe_seq
	.dat_ch_go(dat_ch_go),
	
	//axis from FSM
	.feature_data_vld(feature_data_vld),
	.feature_hin(feature_hin),
	.feature_data_rdy(feature_data_rdy)
);

buf_wt_ack buf_wt_ack
(
	.clk(clk),
	.rst_n(rst_n),

	//from CSR
	.fsm_logic_init(fsm_logic_init),
	.dma_wt_reuse(dma_wt_reuse),
	
	//wt information from DMA
	.wt_addr_updt(wt_addr_updt),
	.wt_addr_dma(wt_addr_dma),

	//'go' from buf_stripe_seq
	.wt_ch_go(wt_ch_go),
	
	//wt_addr from FSM
	.wt_addr_vld(wt_addr_vld),
	.wt_addr(wt_addr),
	.wt_addr_rdy(wt_addr_rdy)
);

endmodule

