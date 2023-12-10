`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`define Height   32//
`define Width_in 128//

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win (((`Height+`AXI_BURST_LEN_SOFTMAX-1)/`AXI_BURST_LEN_SOFTMAX)*`AXI_BURST_LEN_SOFTMAX)
`define Hin (1)
`define CHin `Width_in
`define CHout `CHin

`define Wout (`Win)
`define Hout (`Hin)
`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define DAT_IN_BASE_ADDR 32'h100_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

///////////////// no use ////////////////////
`define RELU_EN 0

`define WT_scale_bits (`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_quant_scale_DW/`base_Tin)
`define WT_BASE_ADDR 32'h200_0000
`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define WT_SIZE_IN_BYTES (((`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_DW)>>3)+((`WT_scale_bits)>>3))
`define WT_NUM_DIV_TIN   (`CHout_Padding*`WT_CHin_div_Tin)
`define WT_scale 5

`define Conv_out_scale 0 // make sure wt_scale + in_scale >= out_scale

`define BN_BASE_ADDR 32'h400_0000
`define BN_SURFACE_STRIDE (`Tout*`MAX_BN_DW*2)>>3
`define BN_num_per_AXI_DW (`AXI_DAT_WIDTH/(2*`BN_DW)) // BN_num_per_AXI_DW <= `Tout
`define BN_ch_group_times (`CHout_Padding/`BN_num_per_AXI_DW)
`define BN_WT_scale 3//(`MAX_BN_DW-1)
`define BN_BIAS_scale 2//2

`define Res_Add_BASE_ADDR 32'h600_0000
`define Res_Add_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define Res_Add_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define Res_Add_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define Res_Add_scale 0		  

`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_ACT;

parameter M_AXI_ID_WIDTH = 4;     // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

////////////////////////////////////////
`include "basic_tasks.vh"
`include "tasks_ACT.vh"
always #5 clk=~clk;

integer flag=1;
bit [31:0]rdata;

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real real_wt[16];
real real_bias[16];
real real_x_region[16];

bit [`MAX_DAT_DW-1:0] FP16_wt[16];      
bit [`MAX_DAT_DW-1:0] FP16_bias[16];    
bit [`MAX_DAT_DW-1:0] FP16_x_region[16];
bit [`AXI_DAT_WIDTH-1:0] parameter_in_mem[3];

real          real_fp16_software_out[`Tb][`Hout][`Wout][`CHout_Padding];
real          real_fp32_software_out[`Tb][`Hout][`Wout][`CHout_Padding];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]      dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout_Padding];
real real_software_out,real_hardware_out,difference,relative_error;


initial
begin
/////////////////// generate test vector //////////////////////////////
	for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin;k++)
                begin
                    real_dat_in[b][i][j][k]=0;
                    if(i*`Win+j<`Height)
                    begin
//                        dat_in[b][i][j][k]=k+j;//$random();//16'b0_01110_11_1111_1111;//(k<32)?0:i*`Win+j+k*2+1;
                        real_dat_in[b][i][j][k]=real'($random())/real'(1<<28);//
                        if(real_dat_in[b][i][j][k]>128 || real_dat_in[b][i][j][k]<-128) real_dat_in[b][i][j][k]=$random()%128;
                        real_to_FP16(real_dat_in[b][i][j][k],dat_in[b][i][j][k]);
                        $display("real_dat_in[b%0d]h[%0d][w%0d][ch%0d]=%0f, int_dat_in=%b",b,i,j,k,real_dat_in[b][i][j][k], dat_in[b][i][j][k]);
                    end
//                    real_dat_in[b][i][j][`CHin-1]=5;
                end

    for(int i=0;i<16;i++)
    begin
        real_wt[i]      =1;//real'($random())/real'(1<<27);
        real_bias[i]    =2;//real'($random())/real'(1<<28);
        real_x_region[i]=i+1;// real'($random())/real'(1<<28);
        real_to_FP16(real_wt[i],      FP16_wt[i]);
        real_to_FP16(real_bias[i],    FP16_bias[i]);
        real_to_FP16(real_x_region[i],FP16_x_region[i]);
        $display("real_wt[%0d]=%0f, real_bias[%0d]=%0f, real_x_region[%0d]=%0f",i,real_wt[i], i,real_bias[i], i,real_x_region[i]);
        $display("FP16_wt[%0d]=%0h, FP16_bias[%0d]=%0h, FP16_x_region[%0d]=%0h",i,FP16_wt[i], i,FP16_bias[i], i,FP16_x_region[i]);
    end
    
    for(int i=0;i<16;i++)
    begin
        parameter_in_mem[0][`MAX_DAT_DW*i+:`MAX_DAT_DW]=FP16_wt[i];
        parameter_in_mem[1][`MAX_DAT_DW*i+:`MAX_DAT_DW]=FP16_bias[i];
        parameter_in_mem[2][`MAX_DAT_DW*i+:`MAX_DAT_DW]=FP16_x_region[i];
    end
    
    for(int i=0;i<3;i++)
        for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
        begin
            AXI_HP_Slave_DDR00.memory[`WT_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=parameter_in_mem[i][32*j+:32];
//            $display("addr=%h, AXI_HP_Slave_DDR_wt[%0d][%0d]=%h",`WT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j,i,j,parameter_in_mem[i][32*j+:32]);
        end
//////////////////// data preparation ////////////////////////////


    for(int b=0;b<`Tb;b++)
        General_Map_Feature_Data(`Hin,`Win,`CHin,dat_in[b],tp_dat_in_mem[b]);
        
    for(int i=0;i<`Win*`Hin*`CHin_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
        begin
            dat_in_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout]=tp_dat_in_mem[b][i];
//            $display("dat_in_mem[%0d][%0d]=%h",i,b,tp_dat_in_mem[b][i]);
        end

	for(int i=0;i<`Win*`Hin*`CHin_div_Tout;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
		begin
			AXI_HP_Slave_DDR00.memory[`DAT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=dat_in_mem[i][32*j+:32];
//            $display("addr=%h, AXI_HP_Slave_DDR_dat[%0d][%0d]=%h",`DAT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j,i,j,dat_in_mem[i][32*j+:32]);
        end


//////////////////// Run in software ////////////////////////////
	Run_Activation_Soft(real_dat_in,real_wt,real_bias, real_x_region,
	                       dat_out, real_fp16_software_out,real_fp32_software_out);
    $display("Run_Activation_Soft done!");


//////////////////// Run in hardware ////////////////////////////
    rst_n=1;
    #20 rst_n=0;
    #20 rst_n=1;
    repeat(30) @(posedge clk);
    
    Run_Activation_Hardware(`WT_BASE_ADDR,
                      `DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
                      `DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);

    $display("Run Softmax Finish! ");
	

    for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
        for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
            dat_out_mem[i][32*j+:32]=AXI_HP_Slave_DDR00.memory[`DAT_OUT_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j];
        
    for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
            tp_dat_out_mem[b][i]=dat_out_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout];

    for(int b=0;b<`Tb;b++) 
        General_DeMap_Feature_Data(`Hout,`Wout,`CHout_Padding,tp_dat_out_mem[b],dat_out[b]);


    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout_Padding;k++)
                    if(i*`Wout+j<`Height)
                    begin
                        real_software_out=real_fp16_software_out[b][i][j][k];
                        FP16_to_real(dat_out[b][i][j][k],real_hardware_out);
//                        $display("dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%f, software_out=%f,%f",b,i,j,k,real_hardware_out,real_software_out,real_fp16_software_out[b][i][j][k]);
                        difference=real_hardware_out-real_software_out;
                        relative_error=difference/(real_software_out+0.000001);
                        if ( ((relative_error>0.01)|(relative_error<-0.01)) & ((difference>0.01)|(difference<-0.01)) )
                        begin
                            flag=0;
                            $display("large error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t abs_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, difference);
                        end
                        else begin
                            $display("small error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t abs_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, difference);
                        end
                    end

    if(flag==1)
        $display("\n=============================\n\t  result small error\n=============================");
    else
        $display("\n=============================\n\t  result mismatch   \n=============================");
    
    AXI_Lite_Read(192+27,rdata);$display("dma_dat_performance_cnt_r: %0d",rdata);
    $display("Total operations: %0d",`Height*`Width_in*`Tb);
    $display("Effiency=%2f%%",(((`Height*`Width_in*`Tb))*100.0)/(`Tout*`Tb*rdata) );

    if(flag==1)
       #10 $finish;
    else
       #20 $finish; 
end

initial
begin
#100000000 $finish;
end

//////////////////////////////////////////////////
//AXI-lite to CSR
wire S_AXI_AWVALID;
wire S_AXI_AWREADY;
wire [`log2_CSR_REG_NUM+2-1:0]S_AXI_AWADDR;
wire [2:0]S_AXI_AWPROT;
wire [31:0]S_AXI_WDATA;
wire S_AXI_WVALID;
wire S_AXI_WREADY;
wire [3:0]S_AXI_WSTRB;
wire [1:0]S_AXI_BRESP;
wire S_AXI_BVALID;
wire S_AXI_BREADY;
wire S_AXI_ARVALID;
wire S_AXI_ARREADY;
wire [`log2_CSR_REG_NUM+2-1:0]S_AXI_ARADDR;
wire [2:0]S_AXI_ARPROT;
wire [31:0]S_AXI_RDATA;
wire [1:0]S_AXI_RRESP;
wire S_AXI_RVALID;
wire S_AXI_RREADY;

//AXI to Mem for 1 AXI
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_AWID;
wire [32-1 : 0]M_AXI_AWADDR;
wire [7 : 0]M_AXI_AWLEN;
wire [2 : 0]M_AXI_AWSIZE;//=clogb2((M_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M_AXI_AWBURST;//=2'b01;
wire  M_AXI_AWLOCK;//1'b0;
wire [3 : 0]M_AXI_AWCACHE;//=4'b0010
wire [2 : 0]M_AXI_AWPROT;//=3'h0;
wire [3 : 0]M_AXI_AWQOS;//=4'h0;
wire M_AXI_AWVALID;
wire M_AXI_AWREADY;
wire [M_AXI_DATA_WIDTH-1 : 0]M_AXI_WDATA;
wire [M_AXI_DATA_WIDTH/8-1 : 0]M_AXI_WSTRB;
wire M_AXI_WLAST;
wire M_AXI_WVALID;
wire M_AXI_WREADY;
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_BID;//ignore
wire [1 : 0] M_AXI_BRESP;//ignore
wire M_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_ARID;
wire [32-1 : 0]M_AXI_ARADDR;
wire [7 : 0]M_AXI_ARLEN;
wire [2 : 0]M_AXI_ARSIZE;//=clogb2((M_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M_AXI_ARBURST;//=2'b01;
wire M_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M_AXI_ARCACHE;//=4'b0010;
wire [2 : 0]M_AXI_ARPROT;//=3'h0;
wire [3 : 0]M_AXI_ARQOS;//=4'h0;
wire M_AXI_ARVALID;
wire M_AXI_ARREADY;
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_RID;
wire [M_AXI_DATA_WIDTH-1 : 0]M_AXI_RDATA;
wire [1 : 0]M_AXI_RRESP;//ignore
wire M_AXI_RLAST;
wire M_AXI_RVALID;
wire M_AXI_RREADY;	

AXI_GP_Master_CPU #
(
	.M_AXI_ADDR_WIDTH(`log2_CSR_REG_NUM+2),
	.M_AXI_DATA_WIDTH(32)
)AXI_GP_Master_CPU
(
    .M_AXI_ACLK(clk),
    .M_AXI_ARESETN(rst_n),
    
    //AW channel
    .M_AXI_AWVALID(S_AXI_AWVALID),
    .M_AXI_AWREADY(S_AXI_AWREADY),
    .M_AXI_AWADDR(S_AXI_AWADDR),
    .M_AXI_AWPROT(S_AXI_AWPROT),//=3'h0

    //Wr channel
    .M_AXI_WDATA(S_AXI_WDATA),
    .M_AXI_WVALID(S_AXI_WVALID),
    .M_AXI_WREADY(S_AXI_WREADY),
    .M_AXI_WSTRB(S_AXI_WSTRB),//={(M_AXI_DATA_WIDTH/8){1'b1}}
    .M_AXI_BRESP(S_AXI_BRESP),//ignore
    .M_AXI_BVALID(S_AXI_BVALID),
    .M_AXI_BREADY(S_AXI_BREADY),        

    //AR channel
    .M_AXI_ARVALID(S_AXI_ARVALID),
    .M_AXI_ARREADY(S_AXI_ARREADY),
    .M_AXI_ARADDR(S_AXI_ARADDR),
    .M_AXI_ARPROT(S_AXI_ARPROT),//=3'b0

    //Rd channel
    .M_AXI_RDATA(S_AXI_RDATA),
    .M_AXI_RRESP(S_AXI_RRESP),//ignore
    .M_AXI_RVALID(S_AXI_RVALID),
    .M_AXI_RREADY(S_AXI_RREADY)
);

TOP_wrapper #
(	
    .M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .M_AXI_DATA_WIDTH(`AXI_DAT_WIDTH)
)TOP_wrapper
(
    .clk(clk),
    .rst_n(rst_n),

    //AXI-lite to CSR
    .S_AXI_AWVALID(S_AXI_AWVALID),
    .S_AXI_AWREADY(S_AXI_AWREADY),
    .S_AXI_AWADDR(S_AXI_AWADDR),
    .S_AXI_AWPROT(S_AXI_AWPROT),//ignore
    .S_AXI_WDATA(S_AXI_WDATA),
    .S_AXI_WVALID(S_AXI_WVALID),
    .S_AXI_WREADY(S_AXI_WREADY),
    .S_AXI_WSTRB(S_AXI_WSTRB),//ignore
    .S_AXI_BRESP(S_AXI_BRESP),//=2'b0
    .S_AXI_BVALID(S_AXI_BVALID),
    .S_AXI_BREADY(S_AXI_BREADY),
    .S_AXI_ARVALID(S_AXI_ARVALID),
    .S_AXI_ARREADY(S_AXI_ARREADY),
    .S_AXI_ARADDR(S_AXI_ARADDR),
    .S_AXI_ARPROT(S_AXI_ARPROT),//ignore
    .S_AXI_RDATA(S_AXI_RDATA),
    .S_AXI_RRESP(S_AXI_RRESP),//=2'b0
    .S_AXI_RVALID(S_AXI_RVALID),
    .S_AXI_RREADY(S_AXI_RREADY),

    //AXI to Mem00 
    .M_AXI_AWID(M_AXI_AWID),
    .M_AXI_AWADDR(M_AXI_AWADDR),
    .M_AXI_AWLEN(M_AXI_AWLEN),
    .M_AXI_AWSIZE(M_AXI_AWSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_AWBURST(M_AXI_AWBURST),//=2'b01;
    .M_AXI_AWLOCK(M_AXI_AWLOCK),//1'b0;
    .M_AXI_AWCACHE(M_AXI_AWCACHE),//=4'b0010
    .M_AXI_AWPROT(M_AXI_AWPROT),//=3'h0;
    .M_AXI_AWQOS(M_AXI_AWQOS),//=4'h0;
    .M_AXI_AWVALID(M_AXI_AWVALID),
    .M_AXI_AWREADY(M_AXI_AWREADY),
    .M_AXI_WDATA(M_AXI_WDATA),
    .M_AXI_WSTRB(M_AXI_WSTRB),
    .M_AXI_WLAST(M_AXI_WLAST),
    .M_AXI_WVALID(M_AXI_WVALID),
    .M_AXI_WREADY(M_AXI_WREADY),
    .M_AXI_BID(M_AXI_BID),//ignore
    .M_AXI_BRESP(M_AXI_BRESP),//ignore
    .M_AXI_BVALID(M_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M_AXI_BREADY(M_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M_AXI_ARID(M_AXI_ARID),
    .M_AXI_ARADDR(M_AXI_ARADDR),
    .M_AXI_ARLEN(M_AXI_ARLEN),
    .M_AXI_ARSIZE(M_AXI_ARSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_ARBURST(M_AXI_ARBURST),//=2'b01;
    .M_AXI_ARLOCK(M_AXI_ARLOCK),//=1'b0;
    .M_AXI_ARCACHE(M_AXI_ARCACHE),//=4'b0010;
    .M_AXI_ARPROT(M_AXI_ARPROT),//=3'h0;
    .M_AXI_ARQOS(M_AXI_ARQOS),//=4'h0;
    .M_AXI_ARVALID(M_AXI_ARVALID),
    .M_AXI_ARREADY(M_AXI_ARREADY),
    .M_AXI_RID(M_AXI_RID),
    .M_AXI_RDATA(M_AXI_RDATA),
    .M_AXI_RRESP(M_AXI_RRESP),//ignore
    .M_AXI_RLAST(M_AXI_RLAST),
    .M_AXI_RVALID(M_AXI_RVALID),
    .M_AXI_RREADY(M_AXI_RREADY)
);

AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .S_AXI_DATA_WIDTH(`AXI_DAT_WIDTH),
    .S_AXI_ADDR_WIDTH(`DDR_WIDTH),
    .BW_div_FREQ_100(1300)
)AXI_HP_Slave_DDR00
(
    .S_AXI_ACLK(clk),
    .S_AXI_ARESETN(rst_n),
    .S_AXI_AWID(M_AXI_AWID),
    .S_AXI_AWADDR(M_AXI_AWADDR),
    .S_AXI_AWLEN(M_AXI_AWLEN),
    .S_AXI_AWSIZE(M_AXI_AWSIZE),
    .S_AXI_AWBURST(M_AXI_AWBURST),
    .S_AXI_AWLOCK(M_AXI_AWLOCK),
    .S_AXI_AWCACHE(M_AXI_AWCACHE),
    .S_AXI_AWPROT(M_AXI_AWPROT),
    .S_AXI_AWQOS(M_AXI_AWQOS),
    .S_AXI_AWVALID(M_AXI_AWVALID),
    .S_AXI_AWREADY(M_AXI_AWREADY),
    .S_AXI_WDATA(M_AXI_WDATA),
    .S_AXI_WSTRB(M_AXI_WSTRB),
    .S_AXI_WLAST(M_AXI_WLAST),
    .S_AXI_WVALID(M_AXI_WVALID),
    .S_AXI_WREADY(M_AXI_WREADY),
    .S_AXI_BID(M_AXI_BID),
    .S_AXI_BRESP(M_AXI_BRESP),
    .S_AXI_BVALID(M_AXI_BVALID),
    .S_AXI_BREADY(M_AXI_BREADY),
    .S_AXI_ARID(M_AXI_ARID),
    .S_AXI_ARADDR(M_AXI_ARADDR),
    .S_AXI_ARLEN(M_AXI_ARLEN),
    .S_AXI_ARSIZE(M_AXI_ARSIZE),
    .S_AXI_ARBURST(M_AXI_ARBURST),
    .S_AXI_ARLOCK(M_AXI_ARLOCK),
    .S_AXI_ARCACHE(M_AXI_ARCACHE),
    .S_AXI_ARPROT(M_AXI_ARPROT),
    .S_AXI_ARQOS(M_AXI_ARQOS),
    .S_AXI_ARVALID(M_AXI_ARVALID),
    .S_AXI_ARREADY(M_AXI_ARREADY),
    .S_AXI_RID(M_AXI_RID),
    .S_AXI_RDATA(M_AXI_RDATA),
    .S_AXI_RRESP(M_AXI_RRESP),
    .S_AXI_RLAST(M_AXI_RLAST),
    .S_AXI_RVALID(M_AXI_RVALID),
    .S_AXI_RREADY(M_AXI_RREADY)
);


endmodule
