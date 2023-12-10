`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`define Height   37//
`define Width_in 1024//

`define RMS_Norm 0
`define Layer_Norm (1-`RMS_Norm)

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define LN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win `Height//(((`Height+`AXI_BURST_LEN-1)/`AXI_BURST_LEN)*`AXI_BURST_LEN)
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

`define DAT_IN_BASE_ADDR 32'h000_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define LN_WT_BASE_ADDR 32'h100_0000
`define LN_SURFACE_STRIDE (`Tout*`MAX_BN_DW*2)>>3
`define LN_num_per_AXI_DW (`AXI_DAT_WIDTH/(2*`LN_DW)) // LN_num_per_AXI_DW <= `Tout
`define LN_ch_group_times (`CHout_Padding/`LN_num_per_AXI_DW)

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

///////////////// no use ////////////////////
`define RELU_EN 0


`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_LN;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;

`include "tasks_LN.vh"
`include "basic_tasks.vh"
`include "activation_tasks.vh"
always #(`half_clk_period) clk=~clk;

integer flag=1;
bit [31:0]rdata;

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

real real_LN_weight[`CHout];
real real_LN_bias[`CHout];
bit [`MAX_BN_DW-1:0] LN_weight[`CHout];
bit [`MAX_BN_DW-1:0] LN_bias[`CHout];
bit [`AXI_DAT_WIDTH-1:0] LN_wt_in_mem[2*((`CHout_Padding+`Tout-1)/`Tout)];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]     dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

real real_fp16_software_LN_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_LN_out[`Tb][`Hout][`Wout][`CHout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout];
bit [(`DAT_DW_L1-1):0]before_relu[`Tb][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;

initial
begin
    $display("Total Operations: %0d",`CHout_Padding*`Wout*`Hout*`Tb);
    $display("theoretical stop time: %0d ns",`CHout_Padding*`Wout*`Hout*`Tb/`Tout*`half_clk_period*2);

/////////////////// generate test vector //////////////////////////////
for(int b=0;b<`Tb;b++)
    for(int i=0;i<`Hin;i++)
        for(int j=0;j<`Win;j++)
            for(int k=0;k<`CHin;k++)
            begin
                dat_in[b][i][j][k]=0;
                if(i*`Win+j<`Height)
                begin
//                    dat_in[b][i][j][k]=(k<32)?0:i*`Win+j+k+1;//$random();//16'b0_01110_11_1111_1111;//
                    real_dat_in[b][i][j][k]=real'($random())/real'(1<<24);//j*2+k;//
                    if(real_dat_in[b][i][j][k]>128 || real_dat_in[b][i][j][k]<-128) real_dat_in[b][i][j][k]=$random()%128;
                    real_to_FP16(real_dat_in[b][i][j][k],dat_in[b][i][j][k]);
//                    $display("real_dat_in[b%0d]h[%0d][w%0d][ch%0d]=%0f, int_dat_in=%b",b,i,j,k,real_dat_in[b][i][j][k], dat_in[b][i][j][k]);
                end
            end

    for(int k=0;k<`CHout;k++)
    begin
//        LN_weight[k]=k*3+1;//$random();
//        LN_bias[k]=k*2+1;//$random();
//        $display("int_LN_weight[%d]=%b",k,LN_weight[k]);
//        $display(" int_LN_biast[%d]=%b",k,LN_bias[k]);     

        real_LN_weight[k]=real'($random())/real'(1<<28);
        real_LN_bias[k]  =real'($random())/real'(1<<26);
        real_to_FP16(real_LN_weight[k],LN_weight[k]);
        real_to_FP16(real_LN_bias[k],  LN_bias[k]);
//        $display("real_LN_weight[ch%0d]=%0f, int_LN_weight=%b",k,real_LN_weight[k], LN_weight[k]);
//        $display("  real_LN_bias[ch%0d]=%0f,  int_LN_biast=%b",k,real_LN_bias[k], LN_bias[k]);                    
    end


//////////////////// data and LN preparation ////////////////////////////				
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
            $display("addr=%h, AXI_HP_Slave_DDR_dat[%0d][%0d]=%h",`DAT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j,i,j,dat_in_mem[i][32*j+:32]);
        end


    for(int i=0;i<2;i++)
        for(int j=0;j<`CHout_div_Tout;j++)
            for(int k=0;k<`Tout;k++)
            begin
                if(j*`Tout+k<`CHout)
                begin
                    if (i==0) LN_wt_in_mem[i*`CHout_div_Tout+j][k*`MAX_DAT_DW+:`MAX_DAT_DW] = LN_weight[j*`Tout+k];
                    if (i==1) LN_wt_in_mem[i*`CHout_div_Tout+j][k*`MAX_DAT_DW+:`MAX_DAT_DW] = LN_bias[j*`Tout+k];
                end
                else
                    LN_wt_in_mem[i*`CHout_div_Tout+j][k*`MAX_DAT_DW+:`MAX_DAT_DW]=0;
            end

	for(int i=0;i<`LN_ch_group_times;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
		begin
			AXI_HP_Slave_DDR00.memory[`LN_WT_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=LN_wt_in_mem[i][32*j+:32];
            $display("AXI_HP_Slave_DDR_LN[%0d][%0d]=%h",i,j,LN_wt_in_mem[i][32*j+:32]);
        end

//////////////////// Run in software ////////////////////////////
	Run_LN_Soft(real_dat_in,real_LN_weight,real_LN_bias,dat_out_soft,real_fp16_software_LN_out,real_fp32_software_LN_out);

//////////////////// Run in hardware ////////////////////////////
//    print_parameters();
	rst_n=1;
	#20 rst_n=0;
	#20 rst_n=1;
	repeat(30) @(posedge clk);
	
    Run_LN(`Win,`Hin,`CHin_Padding_with_Tout,`LN_WT_BASE_ADDR,`DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
           `DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);
    $display("Run LN Finish! ");

	for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
			dat_out_mem[i][32*j+:32]=AXI_HP_Slave_DDR00.memory[`DAT_OUT_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j];
        
    for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
            tp_dat_out_mem[b][i]=dat_out_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout];

    for(int b=0;b<`Tb;b++) 
        General_DeMap_Feature_Data(`Hout,`Wout,`CHout,tp_dat_out_mem[b],dat_out[b]);


	for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                    if(i*`Wout+j<`Height)
                    begin
                        real_software_out=real_fp16_software_LN_out[b][i][j][k];
                        FP16_to_real(dat_out[b][i][j][k],real_hardware_out);
    //                    $display("real_fp16_software_LN_out[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,real_fp16_software_LN_out);
                        difference=real_hardware_out-real_software_out;
                        relative_error=difference/(real_software_out+0.000001);
                        if ( ((relative_error>0.01)|(relative_error<-0.01)) & ((difference>1)|(difference<-1)) )
                        begin
                            flag=0;
                            $display("large error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t relative_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, relative_error);
                        end
                        else begin
                            $display("small error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t relative_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, relative_error);
                        end
                    end

	if(flag==1)
		$display("\n=============================\n\t  result small error\n=============================");
	else
		$display("\n=============================\n\t  result mismatch   \n=============================");
	
	AXI_Lite_Read(192+27,rdata);$display("dma_dat_performance_cnt_r: %0d",rdata);
	$display("Total OPs: %0d",2*`CHout_Padding*`Wout*`Hout*`Tb);
    $display("Effiency=%2f%%",(((2*`CHout_Padding*`Wout*`Hout*`Tb))*100.0)/(`Tout*`Tb*rdata) );
    
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
////////////////////////////////////////

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
    .M_AXI_DATA_WIDTH(`Tout*`MAX_DAT_DW*`Tb)
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
