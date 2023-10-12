`include "CNN_defines.vh"

module buf_dat_ack
(
	input clk,
	input rst_n,

	//from CSR
	input fsm_logic_init,
	input [`log2_H-1:0]Hin,
	input dma_dat_reuse,
	
	//row information from DMA
	input row_num_updt,
	input [`log2_H-1:0]row_num,

	//'go' from buf_stripe_seq
	input dat_ch_go,
	
	//axis from FSM
	input feature_data_vld,
	input [`log2_H:0]feature_hin,
	output feature_data_rdy
);

reg existing_data;
reg [`log2_H-1:0]row_num_now;
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
	row_num_now<='d0;
	existing_data<=1'b0;
end
else
	if(fsm_logic_init&(~dma_dat_reuse))
	begin
		row_num_now<='d0;
		existing_data<=1'b0;
	end
	else
		if(row_num_updt)
		begin
			row_num_now<=row_num;
			existing_data<=1'b1;
		end

//assign feature_data_rdy=feature_data_vld&existing_data&dat_ch_go&
//                        ((feature_hin[`log2_H-1:0]<=row_num_now)|feature_hin[`log2_H]|(feature_hin[`log2_H-1:0]>=Hin));

assign feature_data_rdy=feature_data_vld&existing_data&dat_ch_go&((feature_hin[`log2_H-1:0]<=row_num_now)|feature_hin[`log2_H]|(feature_hin[`log2_H-1:0]>=Hin));

endmodule
