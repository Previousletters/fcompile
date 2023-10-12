`include "CNN_defines.vh"

module LN_dat_sum_and_square
(
	input clk,
	input rst_n,

	//from CSR
	input start,
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,		//ceil(ch_in/Tout)
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
	
	//to CSR
	output done,

	//rd response from MCIF
	input rd_resp_vld,
	output rd_resp_rdy,
	input [`MAX_DAT_DW *`Tout-1:0]rd_resp_pd,
	output rd_fifo_pop,
	
	//dat2mean
	output [`MAX_DAT_DW+`log2Tout-1:0]dat_sum,
    output dat_sum_vld,

    output [`MAX_DAT_DW*2+`log2Tout-1:0]dat_square_sum,
    output dat_square_sum_vld,
    
    output Stripe_loop_end,
    output CH_max_now,
    output CH_Stripe_loop_end

);
wire [`log2AXI_BURST_LEN-1:0]current_burst_len;

reg [`log2_W-1:0]w_cnt;
wire w_cnt_will_update_now=rd_resp_vld&rd_resp_rdy;
wire w_cnt_is_max_now=(w_cnt==current_burst_len);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_cnt<=0;
	else
		if(w_cnt_will_update_now)
		begin
			if(w_cnt_is_max_now)
				w_cnt<=0;
			else
				w_cnt<=w_cnt+1;
		end
end

reg [`log2_W-1:0]ch_cnt;
wire ch_cnt_will_update_now=w_cnt_will_update_now&w_cnt_is_max_now;
wire ch_cnt_is_max_now=(ch_cnt==CH_in_div_Tout-1);
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

reg [`log2_W-1:0]w_burst_cnt;
wire w_burst_cnt_will_update_now=ch_cnt_will_update_now&ch_cnt_is_max_now;
wire w_burst_cnt_is_max_now=(w_burst_cnt==((w_in-1)>>`log2AXI_BURST_LEN));
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		w_burst_cnt<=0;
	else
		if(w_burst_cnt_will_update_now)
		begin
			if(w_burst_cnt_is_max_now)
				w_burst_cnt<=0;
			else
				w_burst_cnt<=w_burst_cnt+1;
		end
end

assign current_burst_len=(w_burst_cnt_is_max_now)?(w_in[`log2AXI_BURST_LEN-1:0]-1):(`AXI_BURST_LEN-1);


reg [`log2_W-1:0]h_cnt;
wire h_cnt_will_update_now=w_burst_cnt_will_update_now&w_burst_cnt_is_max_now;
wire h_cnt_is_max_now=(h_cnt==h_in-1);
always @(posedge clk or negedge rst_n)
begin
	if(~rst_n)
		h_cnt<=0;
	else
		if(h_cnt_will_update_now)
		begin
			if(h_cnt_is_max_now)
				h_cnt<=0;
			else
				h_cnt<=h_cnt+1;
		end
end


reg working;
assign done=h_cnt_will_update_now&h_cnt_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;
assign rd_resp_rdy=working;
assign rd_fifo_pop=rd_resp_vld&rd_resp_rdy;


generate_vld_shift #
(
    .DATA_WIDTH(4),
    .DEPTH(`log2Tout)
)shift_reg
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({rd_resp_vld&rd_resp_rdy,w_cnt_is_max_now,ch_cnt_is_max_now,CH_max_now&w_cnt_is_max_now}),
    .data_out({dat_sum_vld,Stripe_loop_end,CH_max_now,CH_Stripe_loop_end})
);

Tout_acc # 
(
  .DATA_WIDTH(`MAX_DAT_DW)
)for_mean
(
    .clk(clk),
    .i_dat(rd_resp_pd),
    .o_dat(dat_sum)
); 

reg[`MAX_DAT_DW*2*`Tout-1:0]tp_dat_square;
integer i;
always @(*)
begin
    for(i=0;i<`Tout;i=i+1)
    begin
        tp_dat_square[`MAX_DAT_DW*2*i+:`MAX_DAT_DW*2]=$signed(rd_resp_pd[`MAX_DAT_DW*i+:`MAX_DAT_DW])
                                                        * $signed(rd_resp_pd[`MAX_DAT_DW*i+:`MAX_DAT_DW]);
    end
end

Tout_acc # 
(
  .DATA_WIDTH(`MAX_DAT_DW*2)
)for_variance
(
    .clk(clk),
    .i_dat(tp_dat_square),
    .o_dat(dat_square_sum)
); 

assign dat_square_sum_vld=dat_sum_vld;

endmodule

