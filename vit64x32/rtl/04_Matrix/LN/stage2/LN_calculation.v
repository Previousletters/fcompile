`include "CNN_defines.vh"

module LN_calculation
(
	input clk,
	input rst_n,

	//from CSR reg
	input [`log2_CH-`log2Tout-1:0]CH_in_div_Tout,		//ceil(ch_in/Tout)
    input [`log2_H-1:0]h_in,
    input [`log2_W-1:0]w_in,
    input [`Log2_LN_pixel-1:0]pixel_in,
    input [`log2_S-1:0]k_factor_scale,
    input [`log2_S-1:0]bias_scale,
    input [`log2_S-1:0]out_scale,

	input [`MAX_DAT_DW*`Tout-1:0]original_feature,
	input original_feature_vld,
	input [`MAX_DAT_DW-1:0]mean,
	input [`MAX_DAT_DW*2-1:0]recip_sqrt,
    input mean_vld,
	input [`MAX_DAT_DW*`Tout-1:0]wt, //delay 1clk
    input [`MAX_DAT_DW*`Tout-1:0]bias,//delay 3clk
    input wt_vld,
    input bias_vld,
    
    output reg[`MAX_DAT_DW*`Tout-1:0]dat_out,
    output dat_out_vld

);

wire [`log2AXI_BURST_LEN-1:0]current_burst_len;

reg [`log2_W-1:0]w_cnt;
wire w_cnt_will_update_now=mean_vld;
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

reg [`Log2_LN_ch_max-`log2Tout-1:0]ch_cnt;
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

wire [`Log2_LN_pixel-1:0]current_pixel=h_cnt*w_in+w_burst_cnt*`AXI_BURST_LEN+w_cnt;
wire cnt_less_than_pixel=(current_pixel<pixel_in)?1'b1:1'b0;
wire cnt_less_than_pixel_vld;
generate_vld_shift #
(
    .DATA_WIDTH(2),
    .DEPTH(5)
)u_vld
(
    .clk(clk),
    .rst_n(rst_n),
    .data_in({original_feature_vld,cnt_less_than_pixel}),
    .data_out({dat_out_vld,cnt_less_than_pixel_vld})
);

/////////////  start calculation ///////////////

reg [`MAX_DAT_DW-1:0]feature[`Tout-1:0];
reg [`MAX_DAT_DW:0]tp_difference[`Tout-1:0];
reg [`MAX_DAT_DW*2+1:0]tp_LN_diff_mul_wt[`Tout-1:0];
reg [`MAX_DAT_DW*4-1:0]LN_wt_out[`Tout-1:0];
reg [`MAX_DAT_DW*2-1:0]shift_LN_wt_out[`Tout-1:0];

reg [`MAX_DAT_DW*2+`log2_S-1:0]tp_LN_wt_out[`Tout-1:0];
reg [`MAX_DAT_DW*2-1:0]tp_bias[`Tout-1:0];
reg [`MAX_DAT_DW*2+`log2_S-1:0]tp_LN[`Tout-1:0];
reg [`MAX_DAT_DW*2+`log2_S-1:0]tp_LN_round_out[`Tout-1:0];
reg [`MAX_DAT_DW*2+`log2_S-1:0]tp_LN_shift_out[`Tout-1:0];
reg [`MAX_DAT_DW-1:0]  LN_out[`Tout-1:0];

reg [`MAX_DAT_DW*2-1:0]recip_sqrt_d0,recip_sqrt_d1;
always@(posedge clk)
begin
    recip_sqrt_d0<=recip_sqrt;
    recip_sqrt_d1<=recip_sqrt_d0;
end

genvar i;
generate
    for(i=0;i<`Tout;i=i+1)
    begin:cal
        always@(*)
            feature[i]=original_feature[`MAX_DAT_DW*i+:`MAX_DAT_DW];
            
        always@(posedge clk)
            tp_difference[i]<=$signed(feature[i])-$signed(mean);
        always@(posedge clk)
            tp_LN_diff_mul_wt[i]<=$signed(tp_difference[i])*$signed(wt[`MAX_DAT_DW*i+:`MAX_DAT_DW]);
        always@(posedge clk)
            LN_wt_out[i]<=$signed(tp_LN_diff_mul_wt[i])*$signed(recip_sqrt_d1); //scale= LN_wt_scale

        always@(*)
        begin
            shift_LN_wt_out[i]=LN_wt_out[i][`MAX_DAT_DW*4-1:`MAX_DAT_DW*2]+LN_wt_out[i][(`MAX_DAT_DW*2-1)];
            tp_LN_wt_out[i] = $signed(shift_LN_wt_out[i])<<<bias_scale;
            tp_bias[i]      = $signed(bias[`MAX_DAT_DW*i+:`MAX_DAT_DW])<<<k_factor_scale;
        end
        always@(posedge clk)
        begin        
            tp_LN[i] = $signed(tp_LN_wt_out[i]) + $signed(tp_bias[i]);
        end  

        always@(*)
        begin
            tp_LN_shift_out[i] = $signed(tp_LN[i]) >>> (bias_scale+k_factor_scale-out_scale);
            if(((bias_scale+k_factor_scale-out_scale)!=0)&&(tp_LN[i]!={1'b0, {(`MAX_DAT_DW*2-1){1'b1}} }))
                tp_LN_round_out[i]=tp_LN_shift_out[i]+tp_LN[i][(bias_scale+k_factor_scale-out_scale)-1];
        end
        
        always@(posedge clk)
        begin   
            if(tp_LN_round_out[i][(`MAX_DAT_DW*2+`log2_S-1)]&&(!(&tp_LN_round_out[i][(`MAX_DAT_DW*2+`log2_S-2):(`MAX_DAT_DW-1)])))//tp2_mul_pc is negetive and tp2_mul_pc<16'h8000
                LN_out[i]<=$signed({1'b1, {(`MAX_DAT_DW-1){1'b0}} });
            else
                if((!tp_LN_round_out[i][(`MAX_DAT_DW*2+`log2_S-1)])&&(|tp_LN_round_out[i][(`MAX_DAT_DW*2+`log2_S-2):(`MAX_DAT_DW-1)]))//tp2_mul_pc is positive and tp2_mul_pc>16'h7fff
                    LN_out[i]<=$signed({1'b0, {(`MAX_DAT_DW-1){1'b1}} });
                else
                    LN_out[i]<=$signed(tp_LN_round_out[i][(`MAX_DAT_DW-1):0]);
        end
   
        always@(*)
        begin
            dat_out[`MAX_DAT_DW*i+:`MAX_DAT_DW]=cnt_less_than_pixel_vld?LN_out[i]:0;
        end
    end
endgenerate


endmodule
