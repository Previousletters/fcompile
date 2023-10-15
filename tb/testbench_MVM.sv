`include "CNN_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps
`define Height 197//5//
`define Width_in  192//4//
`define Width_out 64//3//


`define DAT_DW_L0 8
`define DAT_DW_L1 8

`define WT_DW_L0 `DAT_DW_L0
`define BN_DW_L0 8

`define Tin_L0 (`base_Tin*(`MAX_DAT_DW/`DAT_DW_L0))

`define Win_L0 `Height
`define CHin_L0 `Width_in
`define CHout_L0 `Width_out
`define Hin_L0 1

`define Kx_L0 1
`define Ky_L0 1
`define Sx_L0 1
`define Sy_L0 1
`define Px_L0 0
`define Py_L0 0



`define Wout_L0 ((`Win_L0+2*`Px_L0-`Kx_L0)/`Sx_L0+1)
`define Hout_L0 ((`Hin_L0+2*`Py_L0-`Ky_L0)/`Sy_L0+1)

`define slice_of_CHout_L0  ((`CHout_L0+`Tout-1)/`Tout)
`define slice_of_CHin_L0  ((`CHin_L0+`Tin_L0-1)/`Tin_L0)
`define CHin_Exp_L0 (`slice_of_CHin_L0*`Tin_L0)
`define slice_of_Tin_div_Tout_L0  ((`Tin_L0+`Tout-1)/`Tout)

`define RELU_EN_L0 0

`define DAT_IN_BASE_ADDR_L0 32'h000_0000
`define DAT_IN_SURFACE_STRIDE_L0 (`Pixel_Data_Bytes*`Win_L0*`Hin_L0)
`define DAT_IN_LINE_STRIDE_L0 (`Pixel_Data_Bytes*`Win_L0)
`define DAT_IN_scale_L0 4

`define WT_BASE_ADDR_L0 32'h200_0000
`define WT_SIZE_IN_BYTES_L0 (((`Tin_L0*`WT_DW_L0)>>3)*`Kx_L0*`Ky_L0*`CHout_L0*((`CHin_L0+`Tin_L0-1)/`Tin_L0))
`define WT_NUM_DIV_TIN_L0 (`Kx_L0*`Ky_L0*`CHout_L0*((`CHin_L0+`Tin_L0-1)/`Tin_L0))
`define WT_scale_L0 3
`define Conv_out_scale_L0 2 // make sure wt_scale + in_scale >= out_scale

`define DAT_OUT_BASE_ADDR_L0 32'h800_0000
`define DAT_OUT_SURFACE_STRIDE_L0 (`Pixel_Data_Bytes*`Wout_L0*`Hout_L0)
`define DAT_OUT_LINE_STRIDE_L0 (`Pixel_Data_Bytes*`Wout_L0)
`define DAT_OUT_scale_L0 `Conv_out_scale_L0
////////////////////////////////////////////////////////////////////////////////

module testbench_MVM;

parameter M_AXI_ID_WIDTH = 4;     // 1AXI
parameter M_AXI_DATA_WIDTH = `MAX_DAT_DW *`Tout;

bit clk;
bit rst_n;

//////////////////////////////////////////////////
//AXI-lite to CSR
wire S_AXI_AWVALID;
wire S_AXI_AWREADY;
wire [`log2CSR_REG_NUM+2-1:0]S_AXI_AWADDR;
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
wire [`log2CSR_REG_NUM+2-1:0]S_AXI_ARADDR;
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

`include "tasks_driver_MVM.vh"
`include "basic_tasks.vh"
always #5 clk=~clk;

integer flag=1;
bit [31:0]rdata;

bit [`WT_DW_L0-1:0] weight_reorg[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout][`Tin_L0];
bit [`WT_DW_L0*`Tin_L0-1:0] weight_Tin[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout];
bit [`WT_DW_L0*`Tout-1:0] weight_Tout[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout][`slice_of_Tin_div_Tout_L0];	

bit [`DAT_DW_L0-1:0] dat_in[`Hin_L0][`Win_L0][`CHin_L0];
bit [`MAX_DAT_DW *`Tout-1:0]dat_in_mem[]=new[`Win_L0*`Hin_L0*((`CHin_Exp_L0+`Tout-1)/`Tout)];
bit [`DAT_DW_L0-1:0] dat_in_test[`Hin_L0][`Win_L0][`CHin_L0];

bit [`WT_DW_L0-1:0] wt[`Ky_L0][`Kx_L0][`CHin_L0][`CHout_L0];
bit [`WT_DW_L0 *`Tout-1:0]wt_mem[]=new[`slice_of_CHout_L0*`slice_of_CHin_L0*`Ky_L0*`Kx_L0*`Tout*`slice_of_Tin_div_Tout_L0];

bit [`DAT_DW_L1-1:0] dat_out[`Hout_L0][`Wout_L0][`CHout_L0];
bit [`MAX_DAT_DW *`Tout-1:0]dat_out_mem[]=new[`Wout_L0*`Hout_L0*((`CHout_L0+`Tout-1)/`Tout)];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Hout_L0][`Wout_L0][`CHout_L0];								  
bit [(`DAT_DW_L1-1):0]before_relu;
bit [(`DAT_DW_L1-1):0]expected_result;

bit [`MAX_DAT_DW-1:0] input_dt [`Hin_L0*`Win_L0*`CHin_L0-1:0];
bit [`MAX_DAT_DW-1:0] input_wt [`Ky_L0*`Kx_L0*`CHin_L0*`CHout_L0-1:0];
bit [`MAX_DAT_DW-1:0] output_dt[`Hout_L0*`Wout_L0*`CHout_L0-1:0];

initial
begin
    $readmemh("input_dt", input_dt);
    $readmemh("input_wt", input_wt);
	
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
            for(int k=0;k<`CHin_L0;k++)
            begin
                dat_in[i][j][k] = input_dt[i*`Win_L0*`CHin_L0+j*`CHin_L0+k][`DAT_DW_L0-1:0];
                //if(i*`Win_L0+j>`Height)
                //    dat_in[i][j][k]=0;
                //else
                //    dat_in[i][j][k]=$random()%46;//(k==0)? (k-0)*4+i*`Win_L0+j+1 : 0;//(k<=2)?j+1:0;//(k%2==0)? $random()%6 : 0;//(k<1)?$random()%6:0; //0*(`Hin_L0*`Win_L0)+i*`Win_L0+j+1+k;//
                
                //if(`DAT_DW_L0==2 | `DAT_DW_L0==4) dat_in[i][j][k]=($signed(dat_in[i][j][k])>=0)?dat_in[i][j][k]:0;
                //                dat_in[i][j][k] = ($random%2 < 0)? 0 : 1;
                //                $display("dat_in     [%0d][%0d][%0d]=%0d",i,j,k,$signed(dat_in[i][j][k]));
            end

    for(int i=0;i<`Ky_L0;i++)
        for(int j=0;j<`Kx_L0;j++)
            for(int k=0;k<`CHin_L0;k++)
                for(int l=0;l<`CHout_L0;l++)
                begin
                    //wt[i][j][k][l]=$random()%54; //(k==0 & l==0)?1:0; //(k==0)?2*l+j*3+i*5+1:0;//2*l+j*3+i*5+1;//k==0?j+1:0;//1+$random()%2;//
                    wt[i][j][k][l] = input_wt[i*`Kx_L0*`CHin_L0*`CHout_L0+j*`CHin_L0*`CHout_L0+k*`CHout_L0+l][`DAT_DW_L0-1:0];
//                    if(k==0) wt[i][j][k][l]= 1+l+i*`Ky_L0+j;
//                    else wt[i][j][k][l]= 0;//$random()%3;//1+$random()%2;//
//                    $display("[Ky][Kx][CHin][CHout]   wt[%0d][%0d][%0d][%0d]=%0d",i,j,k,l,$unsigned(wt[i][j][k][l]));
                end

    RunMVM_soft(`CHin_L0,`Hin_L0,`Win_L0,`CHout_L0,
                `Kx_L0,`Ky_L0,`Sx_L0,`Sy_L0,`Px_L0,`Py_L0,
                `DAT_IN_scale_L0,`WT_scale_L0,`Conv_out_scale_L0,`DAT_DW_L0,`DAT_DW_L1, 1'b0,
                dat_in,wt,dat_out_soft);

	Map_Feature_Data(dat_in,dat_in_mem);
	Map_Weight_Tout(wt,wt_mem);
	
	for(int i=0;i<`Win_L0*`Hin_L0*((`CHin_Exp_L0+`Tout-1)/`Tout);i++)
		for(int j=0;j<`MAX_DAT_DW *`Tout/32;j++)
			AXI_HP_Slave_DDR00.memory[`DAT_IN_BASE_ADDR_L0/4+i*`MAX_DAT_DW *`Tout/32+j]=dat_in_mem[i][32*j+:32];
    
//    for(int i=0;i<`Hin_L0;i++)
//        for(int j=0;j<`Win_L0;j++)
//            for(int k=0;k<`CHin_Exp_L0;k=k+`Tout)
//            begin
//                for(int kk=k;kk<k+`Tout;kk++)
//                begin
//                    if(kk<`CHin_L0) dat_in_test[i][j][kk]=dat_in_mem[`Win_L0*`Hin_L0*(k/`Tout)+i*`Win_L0+j][(kk-k)*16+:16];
//                    $display("dat_in_test[%0d][%0d][%0d]=%0d",i,j,k,$signed(dat_in_test[i][j][k]));
//                    $display("dat_in     [%0d][%0d][%0d]=%0d",i,j,k,$signed(dat_in[i][j][k]));
//                end
//            end
            
    for(int i=0;i<`slice_of_CHout_L0*`slice_of_CHin_L0*`Ky_L0*`Kx_L0*`Tout*`slice_of_Tin_div_Tout_L0;i++)
	begin
        if (`WT_DW_L0*`Tout>=32)
        begin
            for(int j=0;j<`WT_DW_L0*`Tout/32;j++)
                AXI_HP_Slave_DDR00.memory[`WT_BASE_ADDR_L0/4+i*`WT_DW_L0 *`Tout/32+j]=wt_mem[i][32*j+:32];
        end
        else
        begin
            $display("Error!!   WT_DW_L0*Tout < 32  !");
            $finish;
        end
    end

	rst_n=1;
	#20 rst_n=0;
	#20 rst_n=1;
	repeat(30) @(posedge clk);
	
	RunMVM(`CHin_L0,`Hin_L0,`Win_L0,`CHout_L0,`Kx_L0,`Ky_L0,`Sx_L0,`Sy_L0,`Px_L0,`Py_L0,`RELU_EN_L0,`DAT_DW_L0,`DAT_DW_L1,
			`DAT_IN_BASE_ADDR_L0,`DAT_IN_SURFACE_STRIDE_L0,`DAT_IN_LINE_STRIDE_L0,`DAT_IN_scale_L0,
			`WT_BASE_ADDR_L0,`WT_SIZE_IN_BYTES_L0,`WT_NUM_DIV_TIN_L0,`WT_scale_L0,`Conv_out_scale_L0,
			`DAT_OUT_BASE_ADDR_L0,`DAT_OUT_SURFACE_STRIDE_L0,`DAT_OUT_LINE_STRIDE_L0,`DAT_OUT_scale_L0);
	
	$display("Run Finish");
	
	for(int i=0;i<`Wout_L0*`Hout_L0*((`CHout_L0+`Tout-1)/`Tout);i++)
		for(int j=0;j<`MAX_DAT_DW *`Tout/32;j++)
			dat_out_mem[i][32*j+:32]=AXI_HP_Slave_DDR00.memory[`DAT_OUT_BASE_ADDR_L0/4+i*`MAX_DAT_DW *`Tout/32+j];
	
	DeMap_Feature_Data(dat_out_mem,dat_out);
	
	for(int i=0;i<`Hout_L0;i++)
		for(int j=0;j<`Wout_L0;j++)
			for(int k=0;k<`CHout_L0;k++)
			begin
			    if(`DAT_DW_L1==1)
				    expected_result=dat_out_soft[i][j][k]; 
				else
				    expected_result=(`RELU_EN_L0&dat_out_soft[i][j][k][(`DAT_DW_L1-1)])?`DAT_DW_L1'b0:dat_out_soft[i][j][k]; 
				          
				$display("dat_out_software[%0d][%0d][%0d]=%0d",i,j,k,$signed(expected_result));
				
				if(dat_out[i][j][k][(`DAT_DW_L1-1):0]!=expected_result)
				begin
					flag=0;
					$display("error! dat_out_hardware[%0d][%0d][%0d]=%0d, dat_out_software=%0d",i,j,k,$signed(dat_out[i][j][k]), $signed(expected_result));
				end
                output_dt[i*`Wout_L0*`CHout_L0+j*`CHout_L0+k] = $signed(dat_out[i][j][k]);
			end

	if(flag==1)
		$display("\n==================================\n\t result match\n==================================");
	else
		$display("\n==================================\n\t result mismatch\n==================================");
	
	CSB_Read(27,rdata);$display("dma_dat_performance_cnt_r: %0d",rdata);
	CSB_Read(28,rdata);$display("dma_wt_performance_cnt_r : %0d",rdata);
	CSB_Read(29,rdata);$display("fsm_dat_performance_cnt_r : %0d",rdata);
	CSB_Read(30,rdata);$display("fsm_wt_performance_cnt_r  : %0d",rdata);
	$display("Total MAC: %0d",`Kx_L0*`Ky_L0*`CHout_L0*`CHin_L0*`Wout_L0*`Hout_L0);
    $display("MAC Array Effiency=%f%%%%",(((`Kx_L0*`Ky_L0*`CHout_L0*`CHin_L0*`Wout_L0*`Hout_L0))*100.0)/(`Tin_L0*`Tout*rdata) );
    
    $writememh("output_dt", output_dt);
    if(flag==1)
	   #100 $finish;
    else
       #200 $finish; 
end

initial
begin
#100000000 $finish;
end


AXI_GP_Master_CPU #
(
	.M_AXI_ADDR_WIDTH(`log2CSR_REG_NUM+2),
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

Vit_wrapper #
(	
    .M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .M_AXI_DATA_WIDTH(`Tout*`MAX_DAT_DW)
)Vit_wrapper
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
    .S_AXI_DATA_WIDTH(`MAX_DAT_DW *`Tout),
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
