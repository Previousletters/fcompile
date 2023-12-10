`include "CNN_defines.vh"

module buf_wt_ack
(
	input clk,
	input rst_n,

	//from CSR
	input fsm_logic_init,
	input dma_wt_reuse,
	
	//wt information from DMA
	input wt_addr_updt,
	input [`log2BUF_DEP-1:0]wt_addr_dma,

	//'go' from buf_stripe_seq
	input wt_ch_go,
	
	//wt_addr from FSM
	input wt_addr_vld,
	input [`log2BUF_DEP-1:0]wt_addr,
	output wt_addr_rdy
);

reg existing_data;
reg [`log2BUF_DEP-1:0]wt_addr_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	wt_addr_now<='d0;
	existing_data<=1'b0;
end
else
	if(fsm_logic_init&(~dma_wt_reuse))
	begin
		wt_addr_now<='d0;
		existing_data<=1'b0;
	end
	else
		if(wt_addr_updt)
		begin
			wt_addr_now<=wt_addr_dma;
			existing_data<=1'b1;
		end

assign wt_addr_rdy=wt_addr_vld&existing_data&wt_ch_go&(wt_addr<=wt_addr_now);

endmodule
