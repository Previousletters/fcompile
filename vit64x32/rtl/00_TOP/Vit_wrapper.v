`include "CNN_defines.vh"

/*
This is the TOP module.
AXI-lite for the register
AXI-full channels for the data flow.

***********       ***********       **********
*         *  AXI  *  CNN    *==dat==*        *
*   ARM   *-------* module  *===wt==* 	     *
*         * lite  *         *===bn==*  DDR   *
*         *       *         *       *        *
*         *       *         *       *        *
***********       ***********       **********
*/
 module Vit_wrapper #
(	
    parameter M_AXI_ID_WIDTH = 4,    // 4, Datawith= Feature; for Input image or bias or resadder
    parameter M_AXI_DATA_WIDTH = `Tout*`MAX_DAT_DW
)
(
    input clk,
    input rst_n,

    //AXI-lite to CSR
    input S_AXI_AWVALID,
    output S_AXI_AWREADY,
    input [`log2CSR_REG_NUM+2-1:0]S_AXI_AWADDR,
    input [2:0]S_AXI_AWPROT,//ignore
    input [31:0]S_AXI_WDATA,
    input S_AXI_WVALID,
    output S_AXI_WREADY,
    input [3:0]S_AXI_WSTRB,//ignore
    output [1:0]S_AXI_BRESP,//=2'b0
    output S_AXI_BVALID,
    input S_AXI_BREADY,
    input S_AXI_ARVALID,
    output S_AXI_ARREADY,
    input [`log2CSR_REG_NUM+2-1:0]S_AXI_ARADDR,
    input [2:0]S_AXI_ARPROT,//ignore
    output [31:0]S_AXI_RDATA,
    output [1:0]S_AXI_RRESP,//=2'b0
    output S_AXI_RVALID,
    input S_AXI_RREADY,

    //AXI to Mem for Input image or bias or resadder
    output [M_AXI_ID_WIDTH-1 : 0]M_AXI_AWID,
    output [32-1 : 0]M_AXI_AWADDR,
    output [7 : 0]M_AXI_AWLEN,
    output [2 : 0]M_AXI_AWSIZE,//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    output [1 : 0]M_AXI_AWBURST,//=2'b01;
    output  M_AXI_AWLOCK,//1'b0;
    output [3 : 0]M_AXI_AWCACHE,//=4'b10
    output [2 : 0]M_AXI_AWPROT,//=3'h0;
    output [3 : 0]M_AXI_AWQOS,//=4'h0;
    output M_AXI_AWVALID,
    input M_AXI_AWREADY,
    output [M_AXI_DATA_WIDTH-1 : 0]M_AXI_WDATA,
    output [M_AXI_DATA_WIDTH/8-1 : 0]M_AXI_WSTRB,
    output M_AXI_WLAST,
    output M_AXI_WVALID,
    input M_AXI_WREADY,
    input [M_AXI_ID_WIDTH-1 : 0]M_AXI_BID,//ignore
    input [1 : 0] M_AXI_BRESP,//ignore
    input M_AXI_BVALID,//Bvalid and Bread means a a write response.
    output M_AXI_BREADY,//Bvalid and Bread means a a write response.
    output [M_AXI_ID_WIDTH-1 : 0]M_AXI_ARID,
    output [32-1 : 0]M_AXI_ARADDR,
    output [7 : 0]M_AXI_ARLEN,
    output [2 : 0]M_AXI_ARSIZE,//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    output [1 : 0]M_AXI_ARBURST,//=2'b01;
    output M_AXI_ARLOCK,//=1'b0;
    output [3 : 0]M_AXI_ARCACHE,//=4'b10;
    output [2 : 0]M_AXI_ARPROT,//=3'h0;
    output [3 : 0]M_AXI_ARQOS,//=4'h0;
    output M_AXI_ARVALID,
    input M_AXI_ARREADY,
    input [M_AXI_ID_WIDTH-1 : 0]M_AXI_RID,
    input [M_AXI_DATA_WIDTH-1 : 0]M_AXI_RDATA,
    input [1 : 0]M_AXI_RRESP,//ignore
    input M_AXI_RLAST,
    input M_AXI_RVALID,
    output M_AXI_RREADY
);

//CSB to Conv Path: 0~63
wire csb2cp_csr_req_vld;
wire csb2cp_csr_req_rdy;
wire [(`log2CSR_REG_NUM+32+1):0]csb2cp_csr_req_pd;
wire cp_csr2csb_resp_vld;
wire [31:0]cp_csr2csb_resp_pd;

//CSB to SDP: 64~255
wire csb2csr_req_vld;
wire csb2csr_req_rdy;
wire [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd;
wire csr2csb_resp_vld;
wire [31:0]csr2csb_resp_pd;

//CSB to PDP: 256~319
wire csb2Matrix_csr_req_vld;
wire csb2Matrix_csr_req_rdy;
wire [(`log2CSR_REG_NUM+32+1):0]csb2Matrix_csr_req_pd;
wire Matrix_csr2csb_resp_vld;
wire [31:0]Matrix_csr2csb_resp_pd;

//rd CMD to MCIF (dma dat)
wire dma2mcif_dat_rd_req_vld;
wire dma2mcif_dat_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_dat_rd_req_pd;

//rd response from MCIF (dma dat)
wire mcif2dma_dat_rd_resp_vld;
wire mcif2dma_dat_rd_resp_rdy;
wire [M_AXI_DATA_WIDTH-1:0]mcif2dma_dat_rd_resp_pd;
wire dma_dat_rd_fifo_pop;

//rd CMD to MCIF (dma wt)
wire dma2mcif_wt_rd_req_vld;
wire dma2mcif_wt_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_wt_rd_req_pd;

//rd response from MCIF (dma wt)
wire mcif2dma_wt_rd_resp_vld;
wire mcif2dma_wt_rd_resp_rdy;
wire [M_AXI_DATA_WIDTH-1:0]mcif2dma_wt_rd_resp_pd;
wire dma_wt_rd_fifo_pop;

//rd CMD to MCIF (dma res)
wire dma2mcif_res_rd_req_vld;
wire dma2mcif_res_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]dma2mcif_res_rd_req_pd;

//rd response from MCIF (dma res)
wire mcif2dma_res_rd_resp_vld;
wire mcif2dma_res_rd_resp_rdy;
wire [M_AXI_DATA_WIDTH-1:0]mcif2dma_res_rd_resp_pd;
wire dma_res_rd_fifo_pop;
    
//write path to MCIF (CONV)
wire conv2mcif_wr_req_vld;
wire conv2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+M_AXI_DATA_WIDTH-1:0]conv2mcif_wr_req_pd;
wire conv2mcif_wr_rsp_complete;

//rd CMD to MCIF (Matrix)
wire Matrix2mcif_rd_req_vld;
wire Matrix2mcif_rd_req_rdy;
wire [`log2AXI_BURST_LEN+32+32-1:0]Matrix2mcif_rd_req_pd;

//rd response from MCIF (Matrix)
wire mcif2Matrix_rd_resp_vld;
wire mcif2Matrix_rd_resp_rdy;
wire [M_AXI_DATA_WIDTH-1:0]mcif2Matrix_rd_resp_pd;
wire Matrix_rd_fifo_pop;

//write path to MCIF (Matrix)
wire Matrix2mcif_wr_req_vld;
wire Matrix2mcif_wr_req_rdy;
wire [2+`log2AXI_BURST_LEN+32+M_AXI_DATA_WIDTH-1:0]Matrix2mcif_wr_req_pd;
wire Matrix2mcif_wr_rsp_complete;

wire sync_async_rst_n;
sync_async_reset u_sync_async_reset
(
    .clk(clk),
    .rst_n(rst_n),
    .sync_async_rst_n(sync_async_rst_n)
);

wire [32-1:0]conv_read_base_addr;
wire [32-1:0]conv_write_base_addr;
wire [32-1:0]matrix_read_base_addr;
wire [32-1:0]matrix_write_base_addr;
wire Conv_In_from_BRAM_mode;
wire Conv_Out_to_BRAM_mode;
wire Matrix_In_from_BRAM_mode;
wire Matrix_Out_to_BRAM_mode;


reg aux_BRAM_wr_req_vld;
reg [2+`log2AXI_BURST_LEN+32+`MAX_DAT_DW *`Tout-1:0]aux_BRAM_wr_req_pd;
wire aux_BRAM_wr_rsp_complete;
reg [31:0]aux_BRAM_write_base_addr;
always@(*)
    case({Conv_Out_to_BRAM_mode,Matrix_Out_to_BRAM_mode})
        2'b01: aux_BRAM_wr_req_vld=Matrix2mcif_wr_req_vld;
        2'b10: aux_BRAM_wr_req_vld=conv2mcif_wr_req_vld;
        default:aux_BRAM_wr_req_vld=0;
    endcase
always@(*)
    case({Conv_Out_to_BRAM_mode,Matrix_Out_to_BRAM_mode})
        2'b01: aux_BRAM_wr_req_pd=Matrix2mcif_wr_req_pd;
        2'b10: aux_BRAM_wr_req_pd=conv2mcif_wr_req_pd;
        default:aux_BRAM_wr_req_pd=0;
    endcase
always@(*)
    case({Conv_Out_to_BRAM_mode,Matrix_Out_to_BRAM_mode})
        2'b01: aux_BRAM_write_base_addr=matrix_write_base_addr;
        2'b10: aux_BRAM_write_base_addr=conv_write_base_addr;
        default:aux_BRAM_write_base_addr=0;
    endcase

wire AXI_Conv_wr_req_vld  =  Conv_Out_to_BRAM_mode?0:conv2mcif_wr_req_vld;
wire AXI_Matrix_wr_req_vld=Matrix_Out_to_BRAM_mode?0:Matrix2mcif_wr_req_vld;
wire Conv_wr_done    =  Conv_Out_to_BRAM_mode?aux_BRAM_wr_rsp_complete:conv2mcif_wr_rsp_complete;
wire Matrix_wr_done  =Matrix_Out_to_BRAM_mode?aux_BRAM_wr_rsp_complete:Matrix2mcif_wr_rsp_complete;

    
reg [`log2AXI_BURST_LEN+32+32-1:0]aux_BRAM_rd_req_pd;
reg aux_BRAM_rd_req_vld;
wire aux_BRAM_rd_req_rdy;
reg [32-1:0]aux_BRAM_read_base_addr;
wire aux_BRAM_rd_dat_vld;
reg aux_BRAM_rd_dat_rdy;
wire [`MAX_DAT_DW *`Tout-1:0]aux_BRAM_rd_dat;

wire AXI_conv_dat_rd_req_vld=Conv_In_from_BRAM_mode?0:dma2mcif_dat_rd_req_vld;
wire AXI_Matrix_dat_rd_req_vld=Matrix_In_from_BRAM_mode?0:Matrix2mcif_rd_req_vld;

always@(*)
    case({Conv_In_from_BRAM_mode,Matrix_In_from_BRAM_mode})
        2'b01: aux_BRAM_rd_req_pd=Matrix2mcif_rd_req_pd;
        2'b10: aux_BRAM_rd_req_pd=dma2mcif_dat_rd_req_pd;
        default:aux_BRAM_rd_req_pd=0;
    endcase
always@(*)
    case({Conv_In_from_BRAM_mode,Matrix_In_from_BRAM_mode})
        2'b01: aux_BRAM_rd_req_vld=Matrix2mcif_rd_req_vld;
        2'b10: aux_BRAM_rd_req_vld=dma2mcif_dat_rd_req_vld;
        default:aux_BRAM_rd_req_vld=0;
    endcase
always@(*)
    case({Conv_In_from_BRAM_mode,Matrix_In_from_BRAM_mode})
        2'b01: aux_BRAM_rd_dat_rdy=mcif2Matrix_rd_resp_rdy;
        2'b10: aux_BRAM_rd_dat_rdy=mcif2dma_dat_rd_resp_rdy;
        default:aux_BRAM_rd_dat_rdy=0;
    endcase
always@(*)
    case({Conv_In_from_BRAM_mode,Matrix_In_from_BRAM_mode})
        2'b01: aux_BRAM_read_base_addr=matrix_read_base_addr;
        2'b10: aux_BRAM_read_base_addr=conv_read_base_addr;
        default:aux_BRAM_read_base_addr=0;
    endcase
wire conv_dat_rd_req_rdy=Conv_In_from_BRAM_mode?aux_BRAM_rd_req_rdy:dma2mcif_dat_rd_req_rdy;
wire conv_dat_rd_resp_vld=Conv_In_from_BRAM_mode?aux_BRAM_rd_dat_vld:mcif2dma_dat_rd_resp_vld;
wire[`MAX_DAT_DW *`Tout-1:0] conv_dat_rd_resp_pd=Conv_In_from_BRAM_mode?aux_BRAM_rd_dat:mcif2dma_dat_rd_resp_pd;

wire matrix_dat_rd_req_rdy=Matrix_In_from_BRAM_mode?aux_BRAM_rd_req_rdy:Matrix2mcif_rd_req_rdy;
wire matrix_dat_rd_resp_vld=Matrix_In_from_BRAM_mode?aux_BRAM_rd_dat_vld:mcif2Matrix_rd_resp_vld;
wire[`MAX_DAT_DW *`Tout-1:0] matrix_dat_rd_resp_pd=Matrix_In_from_BRAM_mode?aux_BRAM_rd_dat:mcif2Matrix_rd_resp_pd;

AXI_BRAM_Slave u_AXI_BRAM_Slave
(
    .clk(clk),
    .rst_n(rst_n),
    .wr_req_vld(aux_BRAM_wr_req_vld),
    .wr_req_pd(aux_BRAM_wr_req_pd),
    .wr_complete(aux_BRAM_wr_rsp_complete),
    .onchip_write_base_addr(aux_BRAM_write_base_addr),
    
    .rd_req_pd(aux_BRAM_rd_req_pd),
    .rd_req_vld(aux_BRAM_rd_req_vld),
    .rd_req_rdy(aux_BRAM_rd_req_rdy),
    .onchip_read_base_addr(aux_BRAM_read_base_addr),
    .rd_dat_vld(aux_BRAM_rd_dat_vld),
    .rd_dat_rdy(aux_BRAM_rd_dat_rdy),
    .rd_dat(aux_BRAM_rd_dat)
);


AXI_lite_CSB AXI_lite_CSB
(
    .clk(clk),
    .rst_n(sync_async_rst_n),

    //AW channel
    .S_AXI_AWVALID(S_AXI_AWVALID),
    .S_AXI_AWREADY(S_AXI_AWREADY),
    .S_AXI_AWADDR(S_AXI_AWADDR),
    .S_AXI_AWPROT(S_AXI_AWPROT),//ignore
        
    //Wr channel
    .S_AXI_WDATA(S_AXI_WDATA),
    .S_AXI_WVALID(S_AXI_WVALID),
    .S_AXI_WREADY(S_AXI_WREADY),
    .S_AXI_WSTRB(S_AXI_WSTRB),//ignore
    .S_AXI_BRESP(S_AXI_BRESP),//=2'b0
    .S_AXI_BVALID(S_AXI_BVALID),
    .S_AXI_BREADY(S_AXI_BREADY),
    
    //AR channel
    .S_AXI_ARVALID(S_AXI_ARVALID),
    .S_AXI_ARREADY(S_AXI_ARREADY),
    .S_AXI_ARADDR(S_AXI_ARADDR),
    .S_AXI_ARPROT(S_AXI_ARPROT),//ignore

    //Rd channel
    .S_AXI_RDATA(S_AXI_RDATA),
    .S_AXI_RRESP(S_AXI_RRESP),//=2'b0
    .S_AXI_RVALID(S_AXI_RVALID),
    .S_AXI_RREADY(S_AXI_RREADY),

    //CSB to Conv Path: 0~63
    .csb2cp_csr_req_vld(csb2cp_csr_req_vld),
    .csb2cp_csr_req_rdy(csb2cp_csr_req_rdy),
    .csb2cp_csr_req_pd(csb2cp_csr_req_pd),
    .cp_csr2csb_resp_vld(cp_csr2csb_resp_vld),
    .cp_csr2csb_resp_pd(cp_csr2csb_resp_pd),

    //CSB to SDP: 64~127
    .csb2csr_req_vld(csb2csr_req_vld),
    .csb2csr_req_rdy(csb2csr_req_rdy),
    .csb2csr_req_pd(csb2csr_req_pd),
    .csr2csb_resp_vld(csr2csb_resp_vld),
    .csr2csb_resp_pd(csr2csb_resp_pd),

    //CSB to PDP: 128~255
    .csb2Matrix_csr_req_vld(csb2Matrix_csr_req_vld),
    .csb2Matrix_csr_req_rdy(csb2Matrix_csr_req_rdy),
    .csb2Matrix_csr_req_pd(csb2Matrix_csr_req_pd),
    .Matrix_csr2csb_resp_vld(Matrix_csr2csb_resp_vld),
    .Matrix_csr2csb_resp_pd(Matrix_csr2csb_resp_pd)
);

CONV_TOP CONV_TOP
(
    .clk(clk),
    .rst_n(sync_async_rst_n),

    .Conv_In_from_BRAM_mode(Conv_In_from_BRAM_mode),
    .Conv_Out_to_BRAM_mode(Conv_Out_to_BRAM_mode),

    .dat_base_addr(conv_read_base_addr),    
    .dat_out_base_addr(conv_write_base_addr),

    
    //from CSB Master
    .csb2csr_req_vld(csb2cp_csr_req_vld),
    .csb2csr_req_rdy(csb2cp_csr_req_rdy),
    .csb2csr_req_pd(csb2cp_csr_req_pd),
    .csr2csb_resp_vld(cp_csr2csb_resp_vld),
    .csr2csb_resp_pd(cp_csr2csb_resp_pd),

    //rd CMD to MCIF (dat)
    .dma2mcif_dat_rd_req_vld(dma2mcif_dat_rd_req_vld),
    .dma2mcif_dat_rd_req_rdy(conv_dat_rd_req_rdy),
    .dma2mcif_dat_rd_req_pd(dma2mcif_dat_rd_req_pd),

    //rd response from MCIF (dat)
    .mcif2dma_dat_rd_resp_vld(conv_dat_rd_resp_vld),
    .mcif2dma_dat_rd_resp_rdy(mcif2dma_dat_rd_resp_rdy),
    .mcif2dma_dat_rd_resp_pd(conv_dat_rd_resp_pd),
    .dma_dat_rd_fifo_pop(dma_dat_rd_fifo_pop),

    //rd CMD to MCIF (wt)
    .dma2mcif_wt_rd_req_vld(dma2mcif_wt_rd_req_vld),
    .dma2mcif_wt_rd_req_rdy(dma2mcif_wt_rd_req_rdy),
    .dma2mcif_wt_rd_req_pd(dma2mcif_wt_rd_req_pd),

    //rd response from MCIF (wt)
    .mcif2dma_wt_rd_resp_vld(mcif2dma_wt_rd_resp_vld),
    .mcif2dma_wt_rd_resp_rdy(mcif2dma_wt_rd_resp_rdy),
    .mcif2dma_wt_rd_resp_pd(mcif2dma_wt_rd_resp_pd),
    .dma_wt_rd_fifo_pop(dma_wt_rd_fifo_pop),

    //rd CMD to MCIF (res)
    .dma2mcif_res_rd_req_vld(dma2mcif_res_rd_req_vld),
    .dma2mcif_res_rd_req_rdy(dma2mcif_res_rd_req_rdy),
    .dma2mcif_res_rd_req_pd(dma2mcif_res_rd_req_pd),

    //rd response from MCIF (res)
    .mcif2dma_res_rd_resp_vld(mcif2dma_res_rd_resp_vld),
    .mcif2dma_res_rd_resp_rdy(mcif2dma_res_rd_resp_rdy),
    .mcif2dma_res_rd_resp_pd(mcif2dma_res_rd_resp_pd),
    .dma_res_rd_fifo_pop(dma_res_rd_fifo_pop),

    //////////////////////////////////////////////////      
    
    //wdma2mcif
    .conv2mcif_wr_req_vld(conv2mcif_wr_req_vld),
    .conv2mcif_wr_req_rdy(conv2mcif_wr_req_rdy),
    .conv2mcif_wr_req_pd(conv2mcif_wr_req_pd),
    .conv2mcif_wr_rsp_complete(Conv_wr_done)
);


//////////////////////////////////////////////////
MATRIX_TOP MATRIX_TOP
(
    .clk(clk),
    .rst_n(sync_async_rst_n),
    .Matrix_In_from_BRAM_mode(Matrix_In_from_BRAM_mode),
    .Matrix_Out_to_BRAM_mode(Matrix_Out_to_BRAM_mode),
    .matrix_read_base_addr(matrix_read_base_addr),
    .matrix_write_base_addr(matrix_write_base_addr),
    
    //from CSB Master
    .csb2csr_req_vld(csb2Matrix_csr_req_vld),
    .csb2csr_req_rdy(csb2Matrix_csr_req_rdy),
    .csb2csr_req_pd(csb2Matrix_csr_req_pd),
    .csr2csb_resp_vld(Matrix_csr2csb_resp_vld),
    .csr2csb_resp_pd(Matrix_csr2csb_resp_pd),

    //rd CMD to MCIF
    .Matrix2mcif_rd_req_vld(Matrix2mcif_rd_req_vld),
    .Matrix2mcif_rd_req_rdy(matrix_dat_rd_req_rdy),
    .Matrix2mcif_rd_req_pd(Matrix2mcif_rd_req_pd),

    //rd response from MCIF
    .mcif2Matrix_rd_resp_vld(matrix_dat_rd_resp_vld),
    .mcif2Matrix_rd_resp_rdy(mcif2Matrix_rd_resp_rdy),
    .mcif2Matrix_rd_resp_pd(matrix_dat_rd_resp_pd),
    .Matrix_rd_fifo_pop(Matrix_rd_fifo_pop),

    //write path to MCIF
    .Matrix2mcif_wr_req_vld(Matrix2mcif_wr_req_vld),
    .Matrix2mcif_wr_req_rdy(Matrix2mcif_wr_req_rdy),
    .Matrix2mcif_wr_req_pd(Matrix2mcif_wr_req_pd),
    .Matrix2mcif_wr_rsp_complete(Matrix_wr_done)
);


mcif_feature_4r_2w #
(
    .M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .M_AXI_DATA_WIDTH(M_AXI_DATA_WIDTH)
)MCIF_4r_2w
(
    .clk(clk),
    .rst_n(sync_async_rst_n),

    //////////////////////////////////////////////////
    .rd_req_vld0(AXI_conv_dat_rd_req_vld),
    .rd_req_rdy0(dma2mcif_dat_rd_req_rdy),
    .rd_req_pd0(dma2mcif_dat_rd_req_pd),
    
    .rd_resp_vld0(mcif2dma_dat_rd_resp_vld),
    .rd_resp_rdy0(mcif2dma_dat_rd_resp_rdy),
    .rd_resp_pd0(mcif2dma_dat_rd_resp_pd),
    .rd_fifo_pop0(dma_dat_rd_fifo_pop),

    //////////////////////////////////////////////////
    .rd_req_vld1(AXI_Matrix_dat_rd_req_vld),
    .rd_req_rdy1(Matrix2mcif_rd_req_rdy),
    .rd_req_pd1(Matrix2mcif_rd_req_pd),

    .rd_resp_vld1(mcif2Matrix_rd_resp_vld),
    .rd_resp_rdy1(mcif2Matrix_rd_resp_rdy),
    .rd_resp_pd1(mcif2Matrix_rd_resp_pd),
    .rd_fifo_pop1(Matrix_rd_fifo_pop),
    //////////////////////////////////////////////////
    
    .rd_req_vld2(dma2mcif_wt_rd_req_vld),
    .rd_req_rdy2(dma2mcif_wt_rd_req_rdy),
    .rd_req_pd2(dma2mcif_wt_rd_req_pd),

    .rd_resp_vld2(mcif2dma_wt_rd_resp_vld),
    .rd_resp_rdy2(mcif2dma_wt_rd_resp_rdy),
    .rd_resp_pd2(mcif2dma_wt_rd_resp_pd),
    .rd_fifo_pop2(dma_wt_rd_fifo_pop),
    //////////////////////////////////////////////////
    
    .rd_req_vld3(dma2mcif_res_rd_req_vld),
    .rd_req_rdy3(dma2mcif_res_rd_req_rdy),
    .rd_req_pd3(dma2mcif_res_rd_req_pd),

    .rd_resp_vld3(mcif2dma_res_rd_resp_vld),
    .rd_resp_rdy3(mcif2dma_res_rd_resp_rdy),
    .rd_resp_pd3(mcif2dma_res_rd_resp_pd),
    .rd_fifo_pop3(dma_res_rd_fifo_pop),
    //////////////////////////////////////////////////         
          
    .wr_req_vld0(AXI_Conv_wr_req_vld),
    .wr_req_rdy0(conv2mcif_wr_req_rdy),
    .wr_req_pd0(conv2mcif_wr_req_pd),
    .wr_rsp_complete0(conv2mcif_wr_rsp_complete),
    //////////////////////////////////////////////////
    .wr_req_vld1(AXI_Matrix_wr_req_vld),
    .wr_req_rdy1(Matrix2mcif_wr_req_rdy),
    .wr_req_pd1(Matrix2mcif_wr_req_pd),
    .wr_rsp_complete1(Matrix2mcif_wr_rsp_complete),
    //////////////////////////////////////////////////
    
    //AW channel
    .M_AXI_AWID(M_AXI_AWID),
    .M_AXI_AWADDR(M_AXI_AWADDR),
    .M_AXI_AWLEN(M_AXI_AWLEN[`log2AXI_BURST_LEN-1 : 0]),
    .M_AXI_AWSIZE(M_AXI_AWSIZE),//=clogb2((`AXI_DATA_WIDTH/8)-1);
    .M_AXI_AWBURST(M_AXI_AWBURST),//=2'b01;
    .M_AXI_AWLOCK(M_AXI_AWLOCK),//1'b0;
    .M_AXI_AWCACHE(M_AXI_AWCACHE),//=4'b10
    .M_AXI_AWPROT(M_AXI_AWPROT),//=3'h0;
    .M_AXI_AWQOS(M_AXI_AWQOS),//=4'h0;
    .M_AXI_AWVALID(M_AXI_AWVALID),
    .M_AXI_AWREADY(M_AXI_AWREADY),
    
    //Wr channel
    .M_AXI_WDATA(M_AXI_WDATA),
    .M_AXI_WSTRB(M_AXI_WSTRB),
    .M_AXI_WLAST(M_AXI_WLAST),
    .M_AXI_WVALID(M_AXI_WVALID),
    .M_AXI_WREADY(M_AXI_WREADY),
    .M_AXI_BID(M_AXI_BID),//ignore
    .M_AXI_BRESP(M_AXI_BRESP),//ignore
    .M_AXI_BVALID(M_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M_AXI_BREADY(M_AXI_BREADY),//Bvalid and Bread means a a write response.
    
    //AR channel
    .M_AXI_ARID(M_AXI_ARID),
    .M_AXI_ARADDR(M_AXI_ARADDR),
    .M_AXI_ARLEN(M_AXI_ARLEN[`log2AXI_BURST_LEN-1 : 0]),
    .M_AXI_ARSIZE(M_AXI_ARSIZE),//=clogb2((`AXI_DATA_WIDTH/8)-1);
    .M_AXI_ARBURST(M_AXI_ARBURST),//=2'b01;
    .M_AXI_ARLOCK(M_AXI_ARLOCK),//=1'b0;
    .M_AXI_ARCACHE(M_AXI_ARCACHE),//=4'b10;
    .M_AXI_ARPROT(M_AXI_ARPROT),//=3'h0;
    .M_AXI_ARQOS(M_AXI_ARQOS),//=4'h0;
    .M_AXI_ARVALID(M_AXI_ARVALID),
    .M_AXI_ARREADY(M_AXI_ARREADY),
    
    //Rd channel
    .M_AXI_RID(M_AXI_RID),
    .M_AXI_RDATA(M_AXI_RDATA),
    .M_AXI_RRESP(M_AXI_RRESP),//ignore
    .M_AXI_RLAST(M_AXI_RLAST),
    .M_AXI_RVALID(M_AXI_RVALID),
    .M_AXI_RREADY(M_AXI_RREADY)
);

assign M_AXI_AWLEN[7:`log2AXI_BURST_LEN]={(8-`log2AXI_BURST_LEN){1'b0}};
assign M_AXI_ARLEN[7:`log2AXI_BURST_LEN]={(8-`log2AXI_BURST_LEN){1'b0}};

endmodule

module sync_async_reset
(
    input clk,
    input rst_n,
    output sync_async_rst_n
);
reg rst_s1_n;
reg rst_s2_n;

always@(posedge clk or negedge rst_n)begin
    if(!rst_n)begin
        rst_s1_n <= 1'b0;
        rst_s2_n <= 1'b0;
    end
    else begin
        rst_s1_n <= 1'b1;
        rst_s2_n <= rst_s1_n;
    end
end

assign sync_async_rst_n = rst_s2_n;
//assign sync_async_rst_n = rst_n;

endmodule
