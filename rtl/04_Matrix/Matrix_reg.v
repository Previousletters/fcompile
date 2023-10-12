`include "CNN_defines.vh"

module Matrix_reg
(
	input clk,
	input rst_n,

	//from CSB Master
	input csb2csr_req_vld,
	output csb2csr_req_rdy,
	input [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd,
	output reg csr2csb_resp_vld,
	output reg [31:0]csr2csb_resp_pd,

	//to datapath
	output LN_stage0_start,                        //reg 0
	input  LN_stage0_done,
	output LN_stage1and2_start,                    //reg 0
	output Trans_start,
	output Concat_start,
	output Softmax_start,
	output Activation_start,
	
    output [31:0]wt_base_addr,                     //reg 2  
	output [31:0]feature_in_base_addr,             //reg 3
	output [25:0]feature_in_surface_stride,        //reg 4
	output [15:0]feature_in_line_stride,           //reg 5
	output [31:0]feature_out_base_addr,            //reg 6
	output [25:0]feature_out_surface_stride,       //reg 7
	output [15:0]feature_out_line_stride,          //reg 8
	output [`log2_CH-`log2Tout-1:0]ch_in_div_Tout, //reg 9
	output [`log2_H-1:0]h_in,                      //reg 10
	output [`log2_W-1:0]w_in,                      //reg 11  
	output [`log2_S-1:0]k_factor_scale,            //reg 13
	output [`log2_S-1:0]bias_scale,                //reg 15
	output [`log2_S-1:0]out_scale,                 //reg 16
    output [`MAX_DW:0]recip_CH_int8,               //reg 17
    output [`MAX_DW*2:0]recip_CH_int16,            //reg 18
    output [`Log2_LN_pixel-1:0]pixel_in,           //reg 19
    
    output [`log2_CH-`log2Tout-1:0]CH_out_div_Tout, //reg 20
    output [`log2_H-1:0]h_out,                      //reg 21
    output [`log2_W-1:0]w_out,                      //reg 22  
    output [`Log2_Trans_pixel-1:0]pixel_out,        //reg 23
    output [31:0]clstoken_base_addr,                //reg 24
    output [31:0]wt_num_div_Tout,                   //reg 25
    
	//from datapath
	input wdma_done,
    output Matrix_In_from_BRAM_mode,
    output Matrix_Out_to_BRAM_mode
);

wire [`log2CSR_REG_NUM:0]req_addr=csb2csr_req_pd[`log2CSR_REG_NUM:0];
wire [31:0]wdata=csb2csr_req_pd[(`log2CSR_REG_NUM+32):(`log2CSR_REG_NUM+1)];
wire wr_rd=csb2csr_req_pd[`log2CSR_REG_NUM+32+1];

assign csb2csr_req_rdy=1'b1;

always @(posedge clk or negedge rst_n)
if(~rst_n)
	csr2csb_resp_vld<=1'b0;
else
	csr2csb_resp_vld<=csb2csr_req_vld&csb2csr_req_rdy&(~wr_rd);
	
reg [31:0] performance_cnt_r;
reg wdma_done_r;//reg1
reg LN_stage0_done_r,LN_stage1_done_r,LN_stage2_done_r;

reg [31:0]wt_base_addr_r;                     //reg 2
reg [31:0]feature_in_base_addr_r;             //reg 3
reg [25:0]feature_in_surface_stride_r;        //reg 4
reg [15:0]feature_in_line_stride_r;           //reg 5
reg [31:0]feature_out_base_addr_r;            //reg 6
reg [25:0]feature_out_surface_stride_r;       //reg 7
reg [15:0]feature_out_line_stride_r;          //reg 8
reg [`log2_CH-`log2Tout-1:0]ch_in_div_Tout_r; //reg 9
reg [`log2_H-1:0]h_in_r;                      //reg 10
reg [`log2_W-1:0]w_in_r;                      //reg 11  
//reg [`MAX_DW-1:0]k_factor_r;                //reg 12
reg [`log2_S-1:0]k_factor_scale_r;            //reg 13
//reg [`MAX_DW-1:0]bias_r;                    //reg 14
reg [`log2_S-1:0]bias_scale_r;                 //reg 15
reg [`log2_S-1:0]out_scale_r;                  //reg 16
reg [`MAX_DW  :0]recip_CH_int8_r;              //reg 17
reg [`MAX_DW*2:0]recip_CH_int16_r;             //reg 18
reg [`Log2_LN_pixel-1:0]pixel_in_r;            //reg 19
reg [`log2_CH-`log2Tout-1:0]CH_out_div_Tout_r; //reg 20
reg [`log2_H-1:0]h_out_r;                      //reg 21
reg [`log2_W-1:0]w_out_r;                      //reg 22  
reg [`Log2_Trans_pixel-1:0]pixel_out_r;        //reg 23
reg [31:0]clstoken_base_addr_r;                //reg 24
reg [31:0]wt_num_div_Tout_r;                   //reg 25
reg Matrix_In_from_BRAM_mode_r;                //reg 26
reg Matrix_Out_to_BRAM_mode_r;                 //reg 26

always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    wt_base_addr_r<=0;              //reg 2
    feature_in_base_addr_r<=0;      //reg 3
    feature_in_surface_stride_r<=0; //reg 4
    feature_in_line_stride_r<=0;    //reg 5
    feature_out_base_addr_r<=0;     //reg 6
    feature_out_surface_stride_r<=0;//reg 7
    feature_out_line_stride_r<=0;   //reg 8
    ch_in_div_Tout_r<=0;            //reg 9
    h_in_r<=0;                      //reg 10
    w_in_r<=0;                      //reg 11  

    k_factor_scale_r<=0;            //reg 13

    bias_scale_r<=0;                //reg 15
    out_scale_r<=0;                 //reg 16
    recip_CH_int8_r<=0;             //reg 17
    recip_CH_int16_r<=0;            //reg 18
    pixel_in_r<=0;                  //reg 19
    
    CH_out_div_Tout_r<=0;           //reg 20
    h_out_r<=0;                     //reg 21
    w_out_r<=0;                     //reg 22  
    pixel_out_r<=0;                 //reg 23
    clstoken_base_addr_r<=0;        //reg 24
    wt_num_div_Tout_r<=0;           //reg 25
    Matrix_In_from_BRAM_mode_r<=0;  //reg 26
    Matrix_Out_to_BRAM_mode_r<=0;   //reg 26
end
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd)
	begin
		case(req_addr)
			2:wt_base_addr_r<=wdata[31:0];
			3:feature_in_base_addr_r<=wdata[31:0];
			4:feature_in_surface_stride_r<=wdata[31:0];
			5:feature_in_line_stride_r<=wdata[31:0];
			6:feature_out_base_addr_r<=wdata[31:0];
			7:feature_out_surface_stride_r<=wdata[31:0];
			8:feature_out_line_stride_r<=wdata[31:0];
			9:ch_in_div_Tout_r<=wdata[`log2_CH-`log2Tout-1:0];
            10:h_in_r<=wdata[`log2_H-1:0];              
            11:w_in_r<=wdata[`log2_H-1:0];               
    
            13:k_factor_scale_r<=wdata[`log2_S-1:0]; 
       
            15:bias_scale_r<=wdata[`log2_S-1:0];     
            16:out_scale_r<=wdata[`log2_S-1:0];         
            17:recip_CH_int8_r<=wdata[`MAX_DW:0];     
            18:recip_CH_int16_r<=wdata[`MAX_DW*2:0];  
            19:pixel_in_r<=wdata[`Log2_LN_pixel-1:0];
            
            20:CH_out_div_Tout_r<=wdata[`log2_CH-`log2Tout-1:0];
            21:h_out_r<=wdata[`log2_H-1:0];
            22:w_out_r<=wdata[`log2_W-1:0];
            23:pixel_out_r<=wdata[`Log2_Trans_pixel-1:0];
            
            24:clstoken_base_addr_r<=wdata[31:0];
            25:wt_num_div_Tout_r<=wdata[31:0];
            26:begin
                   Matrix_Out_to_BRAM_mode_r<=wdata[1];  //reg 26
                   Matrix_In_from_BRAM_mode_r<=wdata[0];   //reg 26
               end
		endcase
	end

always @(posedge clk or negedge rst_n)
if(~rst_n)
	csr2csb_resp_pd<=32'b0;
else
	if(csb2csr_req_vld&csb2csr_req_rdy&(~wr_rd))
	begin
		case(req_addr)
			1:csr2csb_resp_pd<={31'b0,wdma_done_r};
            2:csr2csb_resp_pd<=wt_base_addr_r;
			3:csr2csb_resp_pd<=feature_in_base_addr_r;
			4:csr2csb_resp_pd<=feature_in_surface_stride_r;
			5:csr2csb_resp_pd<=feature_in_line_stride_r;
			6:csr2csb_resp_pd<=feature_out_base_addr_r;
			7:csr2csb_resp_pd<=feature_out_surface_stride_r;
			8:csr2csb_resp_pd<=feature_out_line_stride_r;
			9:csr2csb_resp_pd<={{(32-(`log2_CH-`log2Tout)){1'b0}},ch_in_div_Tout_r};
			10:csr2csb_resp_pd<={{(32-`log2_H){1'b0}},h_in_r};
            11:csr2csb_resp_pd<={{(32-`log2_W){1'b0}},w_in_r};

            13:csr2csb_resp_pd<={{(32-`log2_S){1'b0}},k_factor_scale_r};

            15:csr2csb_resp_pd<={{(32-`log2_S){1'b0}},bias_scale_r};
            16:csr2csb_resp_pd<={{(32-`log2_S){1'b0}},out_scale_r};
            17:csr2csb_resp_pd<={{(32-`MAX_DW){1'b0}},recip_CH_int8_r};
            18:csr2csb_resp_pd<={{(32-`MAX_DW*2){1'b0}},recip_CH_int16_r};
            19:csr2csb_resp_pd<={{(32-`Log2_LN_pixel){1'b0}},pixel_in_r};
            
            20:csr2csb_resp_pd<={{(32-(`log2_CH-`log2Tout)){1'b0}},CH_out_div_Tout_r};
            21:csr2csb_resp_pd<={{(32-`log2_H){1'b0}},h_out_r};
            22:csr2csb_resp_pd<={{(32-`log2_W){1'b0}},w_out_r};
            23:csr2csb_resp_pd<={{(32-`Log2_Trans_pixel){1'b0}},pixel_out_r};
            
            24:csr2csb_resp_pd<=clstoken_base_addr_r;
            25:csr2csb_resp_pd<=wt_num_div_Tout_r;
            26:csr2csb_resp_pd<={30'b0,Matrix_Out_to_BRAM_mode_r,Matrix_In_from_BRAM_mode_r};
            27:csr2csb_resp_pd<=performance_cnt_r;
			default:csr2csb_resp_pd<=32'b0;
		endcase
    end
	
always @(posedge clk or negedge rst_n)
if(~rst_n)
	wdma_done_r<=1'b0;
else
	if(LN_stage0_start|LN_stage1and2_start|Trans_start|Concat_start|Softmax_start|Activation_start)
		wdma_done_r<=1'b0;
	else
		if(wdma_done|LN_stage0_done)
			wdma_done_r<=1'b1;
			
reg wdma_working;
always @(posedge clk or negedge rst_n)
if(~rst_n)
	wdma_working<=1'b0;
else
	if(LN_stage0_start|LN_stage1and2_start|Trans_start|Concat_start|Softmax_start|Activation_start)
		wdma_working<=1'b1;
	else
		if(wdma_done|LN_stage0_done)
			wdma_working<=1'b0;			

always @(posedge clk or negedge rst_n)
if(~rst_n)
	performance_cnt_r<=32'd0;
else
	if(csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==27))
		performance_cnt_r<=wdata[31:0];
	else
		if(wdma_working)
			performance_cnt_r<=performance_cnt_r+1;

//assign LN_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[0];//reg 0	
assign LN_stage0_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[5];//reg 0	
assign LN_stage1and2_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[6];//reg 0	
assign Trans_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[1];//reg 0	
assign Concat_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[2];//reg 0	
assign Softmax_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[3];//reg 0	
assign Activation_start=csb2csr_req_vld&csb2csr_req_rdy&wr_rd&(req_addr==0)&wdata[4];//reg 0	


reg [31:0]wt_base_addr_d;                     //reg 2
reg [31:0]feature_in_base_addr_d;             //reg 3
reg [25:0]feature_in_surface_stride_d;        //reg 4
reg [15:0]feature_in_line_stride_d;           //reg 5
reg [31:0]feature_out_base_addr_d;            //reg 6
reg [25:0]feature_out_surface_stride_d;       //reg 7
reg [15:0]feature_out_line_stride_d;          //reg 8
reg [`log2_CH-`log2Tout-1:0]ch_in_div_Tout_d; //reg 9
reg [`log2_H-1:0]h_in_d;                      //reg 10
reg [`log2_W-1:0]w_in_d;                      //reg 11  
reg [`log2_S-1:0]k_factor_scale_d;            //reg 13
reg [`log2_S-1:0]bias_scale_d;                 //reg 15
reg [`log2_S-1:0]out_scale_d;                  //reg 16
reg [`MAX_DW  :0]recip_CH_int8_d;              //reg 17
reg [`MAX_DW*2:0]recip_CH_int16_d;             //reg 18
reg [`Log2_LN_pixel-1:0]pixel_in_d;            //reg 19
reg [`log2_CH-`log2Tout-1:0]CH_out_div_Tout_d; //reg 20
reg [`log2_H-1:0]h_out_d;                      //reg 21
reg [`log2_W-1:0]w_out_d;                      //reg 22  
reg [`Log2_Trans_pixel-1:0]pixel_out_d;        //reg 23
reg [31:0]clstoken_base_addr_d;                //reg 24
reg [31:0]wt_num_div_Tout_d;                   //reg 25
reg Matrix_In_from_BRAM_mode_d;                //reg 26
reg Matrix_Out_to_BRAM_mode_d;                 //reg 26

always @(posedge clk or negedge rst_n)
if(~rst_n)
begin
    wt_base_addr_d<=0;              //reg 2
    feature_in_base_addr_d<=0;      //reg 3
    feature_in_surface_stride_d<=0; //reg 4
    feature_in_line_stride_d<=0;    //reg 5
    feature_out_base_addr_d<=0;     //reg 6
    feature_out_surface_stride_d<=0;//reg 7
    feature_out_line_stride_d<=0;   //reg 8
    ch_in_div_Tout_d<=0;            //reg 9
    h_in_d<=0;                      //reg 10
    w_in_d<=0;                      //reg 11  
    k_factor_scale_d<=0;            //reg 13
    bias_scale_d<=0;                //reg 15
    out_scale_d<=0;                 //reg 16
    recip_CH_int8_d<=0;             //reg 17
    recip_CH_int16_d<=0;            //reg 18
    pixel_in_d<=0;                  //reg 19
    
    CH_out_div_Tout_d<=0;           //reg 20
    h_out_d<=0;                     //reg 21
    w_out_d<=0;                     //reg 22  
    pixel_out_d<=0;                 //reg 23
    clstoken_base_addr_d<=0;        //reg 24
    wt_num_div_Tout_d<=0;           //reg 25
    Matrix_In_from_BRAM_mode_d<=0;  //reg 26
    Matrix_Out_to_BRAM_mode_d<=0;   //reg 26
end
else
begin
    wt_base_addr_d<=wt_base_addr_r;                                   //reg 2
    feature_in_base_addr_d<=feature_in_base_addr_r;                   //reg 3
    feature_in_surface_stride_d<=feature_in_surface_stride_r;         //reg 4
    feature_in_line_stride_d<=feature_in_line_stride_r;               //reg 5
    feature_out_base_addr_d<=feature_out_base_addr_r;                 //reg 6
    feature_out_surface_stride_d<=feature_out_surface_stride_r;       //reg 7
    feature_out_line_stride_d<=feature_out_line_stride_r;             //reg 8
    ch_in_div_Tout_d<=ch_in_div_Tout_r;                               //reg 9
    h_in_d<=h_in_r;                                                   //reg 10
    w_in_d<=w_in_r;                                                   //reg 11  
    k_factor_scale_d<=k_factor_scale_r;                               //reg 13
    bias_scale_d<=bias_scale_r;                                       //reg 15
    out_scale_d<=out_scale_r;                                         //reg 16
    recip_CH_int8_d<=recip_CH_int8_r;                                 //reg 17
    recip_CH_int16_d<=recip_CH_int16_r;                               //reg 18
    pixel_in_d<=pixel_in_r;                                           //reg 19
    
    CH_out_div_Tout_d<=CH_out_div_Tout_r;                             //reg 20
    h_out_d<=h_out_r;                                                 //reg 21
    w_out_d<=w_out_r;                                                 //reg 22  
    pixel_out_d<=pixel_out_r;                                         //reg 23
    clstoken_base_addr_d<=clstoken_base_addr_r;                       //reg 24
    wt_num_div_Tout_d<=wt_num_div_Tout_r;                             //reg 25
    Matrix_In_from_BRAM_mode_d<=Matrix_In_from_BRAM_mode_r;           //reg 26
    Matrix_Out_to_BRAM_mode_d<=Matrix_Out_to_BRAM_mode_r;             //reg 26
end

assign wt_base_addr=wt_base_addr_d;
assign feature_in_base_addr=feature_in_base_addr_d;
assign feature_in_surface_stride=feature_in_surface_stride_d;
assign feature_in_line_stride=feature_in_line_stride_d;
assign feature_out_base_addr=feature_out_base_addr_d;
assign feature_out_surface_stride=feature_out_surface_stride_d;
assign feature_out_line_stride=feature_out_line_stride_d;
assign ch_in_div_Tout=ch_in_div_Tout_d;
assign h_in=h_in_d;
assign w_in=w_in_d;

assign k_factor_scale= k_factor_scale_d;
assign bias_scale= bias_scale_d;

assign out_scale= out_scale_d;
assign recip_CH_int8= recip_CH_int8_d;
assign recip_CH_int16=recip_CH_int16_d; 
assign pixel_in=pixel_in_d;
assign CH_out_div_Tout=CH_out_div_Tout_d;
assign h_out=h_out_d;
assign w_out=w_out_d;
assign pixel_out=pixel_out_d;

assign clstoken_base_addr=clstoken_base_addr_d;
assign wt_num_div_Tout=wt_num_div_Tout_d;
assign Matrix_In_from_BRAM_mode=Matrix_In_from_BRAM_mode_d;
assign Matrix_Out_to_BRAM_mode=Matrix_Out_to_BRAM_mode_d;

endmodule
