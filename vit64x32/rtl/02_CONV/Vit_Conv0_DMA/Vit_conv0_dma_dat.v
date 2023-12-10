`include "CNN_defines.vh"

//(* use_dsp = "yes" *)
module Vit_conv0_dma_dat
(
	input clk,
	input rst_n,
    input conv_mode,
    input [3:0]Tin_factor,// 1 meams 8bit, 2 means 4bit, 4 means 2bit, 8 means 1bit    
    
	//from CSR
	input start,
    
    input [`log2_K-1:0]Kx,
	input [`log2_W-1:0]Win,
	input [`log2_H-1:0]Hin,
	input [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1,		//ceil(CH_in%Tin /Tout)
	input [`log2_CH-`base_log2Tin-1:0]CH_in_div_Tin,				//ceil(CH_in/Tin)
	input [31:0]dat_base_addr,
	input [31:0]surface_stride_in,
	input [15:0]line_stride_in,

	//rd CMD to MCIF
	output rd_req_vld,
	input rd_req_rdy,
	output [`log2AXI_BURST_LEN+32+32-1:0]rd_req_pd
);

reg [`log2_W-`log2AXI_BURST_LEN-1:0]k;
reg [31:0]k_bias;
reg [`log2_W-1:0]k_max_minus1;
always@(*)
    case(Kx)
        8:k_max_minus1=((Win*`base_Tin/8)>>`log2Tout)-1;
       16:k_max_minus1=((Win*`base_Tin/16)>>`log2Tout)-1;
        default: k_max_minus1=0;
    endcase
    
wire k_will_update_now=rd_req_vld&rd_req_rdy;
wire k_is_max_now=(k==k_max_minus1>>`log2AXI_BURST_LEN);// burst transfer times per Win
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    k<=0;
    k_bias<=0;
end
else
begin
    if(k_will_update_now)
    begin
        if(k_is_max_now)
        begin
            k<=0;
            k_bias<=0;
        end
        else
        begin
            k<=k+1;
            k_bias<=k_bias+(`AXI_BURST_LEN)*(`Pixel_Data_Bytes);
        end
    end
end

reg [`log2_H-1:0]hin;
reg [31:0]hin_bias;
wire hin_will_update_now;
wire hin_is_max_now;
assign hin_will_update_now=k_will_update_now&k_is_max_now;
assign hin_is_max_now=(hin==Hin-1);
always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    hin<=0;
    hin_bias<=0;
end
else
begin
    if(hin_will_update_now)
    begin
        if(hin_is_max_now)
        begin
            hin<=0;
            hin_bias<=0;
        end
        else
        begin
            hin<=hin+1;
            hin_bias<=hin_bias+line_stride_in;
        end
    end
end

reg working;
wire done;
assign done=hin_will_update_now&hin_is_max_now;  
always @(posedge clk or negedge rst_n)
if(~rst_n)
    working<=1'b0;
else
    if(start)
        working<=1'b1;
    else
        if(done)
            working<=1'b0;

assign rd_req_vld=working;

wire [`log2AXI_BURST_LEN-1:0]cmd_len=k_is_max_now?(k_max_minus1[`log2AXI_BURST_LEN-1:0]):(`AXI_BURST_LEN-1);	
wire [31:0]cmd_addr=(hin_bias+k_bias);

assign rd_req_pd={cmd_len,dat_base_addr,cmd_addr};

endmodule

