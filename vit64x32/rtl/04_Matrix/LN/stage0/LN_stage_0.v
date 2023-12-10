`include "CNN_defines.vh"

module LN_stage_0
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,
    input [31:0]wt_base_addr,
    
    //rd CMD to MCIF
    output LN2mcif_rd_req_vld,
    input LN2mcif_rd_req_rdy,
    output [`log2AXI_BURST_LEN+32+32-1:0]LN2mcif_rd_req_pd,
    
    //rd response from MCIF
    input mcif2LN_rd_resp_vld,
    output mcif2LN_rd_resp_rdy,
    input [`MAX_DAT_DW *`Tout-1:0]mcif2LN_rd_resp_pd,
    output LN_rd_fifo_pop,

	output stage0_working,
	output stage0_done,

    input [`Log2_LN_ch_max-`log2Tout-1:0]rd_wt_addr,
    input rd_wt_en,
    output [`MAX_DAT_DW *`Tout-1:0]wt_rd_dat,
    output rd_wt_vld,

    input [`Log2_LN_ch_max-`log2Tout-1:0]rd_bias_addr,
    input rd_bias_en,
    output [`MAX_DAT_DW *`Tout-1:0]bias_rd_dat,
    output rd_bias_vld
);

wire dma_done;
reg dma_working;
reg wr_SRAM_working;
wire wr_SRAM_done;
assign stage0_working=dma_working|wr_SRAM_working;
assign stage0_done=wr_SRAM_done;

always @(posedge clk or negedge rst_n)
if(~rst_n)
    dma_working<=1'b0;
else
    if(start)
        dma_working<=1'b1;
    else
        if(dma_done)
            dma_working<=1'b0;

always @(posedge clk or negedge rst_n)
if(~rst_n)
    wr_SRAM_working<=1'b0;
else
    if(start)
        wr_SRAM_working<=1'b1;
    else
        if(wr_SRAM_done)
            wr_SRAM_working<=1'b0;

//////// DMA  start////////
reg [`Log2_LN_ch_max-`log2Tout:0]k;
wire k_is_max_now=( k == ((CH_in_div_Tout-1)>>`log2AXI_BURST_LEN) );
wire k_will_update_now=LN2mcif_rd_req_vld&LN2mcif_rd_req_rdy;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	k<=0;
else
	if(k_will_update_now)
	begin
		if(k_is_max_now)
			k<=0;
		else
			k<=k+1;
	end

reg [1:0]cnt;
wire cnt_is_max_now=( cnt == 1 );
wire cnt_will_update_now=k_is_max_now&k_will_update_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	cnt<=0;
else
	if(cnt_will_update_now)
	begin
		if(cnt_is_max_now)
			cnt<=0;
		else
			cnt<=cnt+1;
	end

assign LN2mcif_rd_req_vld=dma_working;
assign dma_done=cnt_is_max_now&cnt_will_update_now;

wire [`log2AXI_BURST_LEN-1:0]cmd_len=k_is_max_now?(CH_in_div_Tout[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);
wire [31:0]cmd_addr=(k<<(`MAX_log2WT_DW-3+`log2Tout+`log2AXI_BURST_LEN)) + 
                    (cnt==0?0:(CH_in_div_Tout<<(`MAX_log2WT_DW-3+`log2Tout)));
                    

assign LN2mcif_rd_req_pd={cmd_len,wt_base_addr,cmd_addr};

//////// DMA  end ////////

//////// dat2SRAM ////////
wire [`log2AXI_BURST_LEN-1:0]current_burst_len;
reg [`log2_W-1:0]ch_cnt;
wire ch_cnt_will_update_now=mcif2LN_rd_resp_vld&mcif2LN_rd_resp_rdy;
wire ch_cnt_is_max_now=(ch_cnt==current_burst_len);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		ch_cnt<=0;
	else
		if(ch_cnt_will_update_now)
		begin
			if(ch_cnt_is_max_now)
				ch_cnt<=0;
			else
				ch_cnt<=ch_cnt+1;
		end
end

reg [`log2_W-1:0]burst_cnt;
wire burst_cnt_will_update_now=ch_cnt_will_update_now&ch_cnt_is_max_now;
wire burst_cnt_is_max_now=(burst_cnt==((CH_in_div_Tout-1)>>`log2AXI_BURST_LEN));
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		burst_cnt<=0;
	else
		if(burst_cnt_will_update_now)
		begin
			if(burst_cnt_is_max_now)
				burst_cnt<=0;
			else
				burst_cnt<=burst_cnt+1;
		end
end

assign current_burst_len=(burst_cnt_is_max_now)?(CH_in_div_Tout[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);

reg [1:0]wt_or_bias_cnt;
wire wt_or_bias_cnt_is_max_now=( wt_or_bias_cnt == 1 );
wire wt_or_bias_cnt_will_update_now=burst_cnt_will_update_now&burst_cnt_is_max_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	wt_or_bias_cnt<=0;
else
	if(wt_or_bias_cnt_will_update_now)
	begin
		if(wt_or_bias_cnt_is_max_now)
			wt_or_bias_cnt<=0;
		else
			wt_or_bias_cnt<=wt_or_bias_cnt+1;
	end
	
assign mcif2LN_rd_resp_rdy=wr_SRAM_working;
assign LN_rd_fifo_pop=mcif2LN_rd_resp_vld&mcif2LN_rd_resp_rdy;

wire wr_en=mcif2LN_rd_resp_vld&mcif2LN_rd_resp_rdy;
wire [`Log2_LN_ch_max-`log2Tout-1:0]wr_addr=ch_cnt+burst_cnt*`AXI_BURST_LEN;
assign wr_SRAM_done=wt_or_bias_cnt_is_max_now&wt_or_bias_cnt_will_update_now;

/////// SRAM ////////
wire wt_wr_en=wt_or_bias_cnt==0&wr_en;
wire [`Log2_LN_ch_max-`log2Tout-1:0]wt_wr_addr=wr_addr;
wire [`MAX_DAT_DW*`Tout-1:0]wt_wr_dat=mcif2LN_rd_resp_pd;

wire bias_wr_en=wt_or_bias_cnt==1&wr_en;
wire [`Log2_LN_ch_max-`log2Tout-1:0]bias_wr_addr=wr_addr;
wire [`MAX_DAT_DW*`Tout-1:0]bias_wr_dat=mcif2LN_rd_resp_pd;


center_buf #
(
   .DATA_WIDTH((`MAX_DAT_DW*`Tout)),
   .DEPTH(1<<(`Log2_LN_ch_max-`log2Tout)),
   .log2_DEPTH(`Log2_LN_ch_max-`log2Tout)
)wt_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    //Wr Port
    .wr_en(wt_wr_en),
    .wr_addr(wt_wr_addr),
    .wr_dat(wt_wr_dat),

    //Rd Port
    .rd_en(rd_wt_en),
    .rd_addr(rd_wt_addr),
    .rd_dat_vld(rd_wt_vld),
    .rd_dat(wt_rd_dat)
);  


center_buf #
(
   .DATA_WIDTH((`MAX_DAT_DW*`Tout)),
   .DEPTH(1<<(`Log2_LN_ch_max-`log2Tout)),
   .log2_DEPTH(`Log2_LN_ch_max-`log2Tout)
)bias_buf
(
    .clk(clk),
    .rst_n(rst_n),
    
    //Wr Port
    .wr_en(bias_wr_en),
    .wr_addr(bias_wr_addr),
    .wr_dat(bias_wr_dat),

    //Rd Port
    .rd_en(rd_bias_en),
    .rd_addr(rd_bias_addr),
    .rd_dat_vld(rd_bias_vld),
    .rd_dat(bias_rd_dat)
);  

endmodule