`define DDR_WIDTH 32
`define DDR_BYTES (`DDR_WIDTH/8)
//`define AXI_3ports
`define MAX_AXI_DW 512  //important Max_AXI_DW >= Tout*MAX_DW
`define log2_MAX_AXI_DW 9

`define Log2_LN_pixel 10
`define Log2_LN_ch_max 10
`define Log2_Trans_pixel 10
`define Log2_Mat_pixel 10
`define Log2_Trans_ch_max `Log2_Trans_pixel
`define Log2_Concat_pixel `Log2_Trans_pixel
`define Log2_Softmax_pixel `Log2_Trans_pixel
`define Log2_ACT_pixel  `Log2_Trans_pixel

`define log2_CH 16
`define log2_H 12
`define log2_W `log2_H
`define log2_KyKx 12
`define log2_other (`log2_CH-`base_log2Tin-4)
`define log2_P 5
`define log2_S 6
`define log2_K 6
`define log2_scale 6      //[5:0], [5]for sign, [4:0]for shift value, always positive 
//`define Need_Left_Shift   //[5]for sign, "0" means right shift, "1" means left shift

`define Column_Systolic_MAC         // Please check the "Parallel_MAC_Calculation.v" 
`define Multi_Precision
//`define INT4_Enable

`define base_Tin 64	            //Parallel factor of CH_in
`define base_log2Tin 6
`define Tout 32					//Parallel factor of CH_out
`define log2Tout 5				

`define base_Tin_div_Tout (`base_Tin/`Tout)
`define Max_Tin (`base_Tin*`MAX_DAT_DW)        // 1 is for the 1bit base
`define Max_log2Tin (`base_log2Tin+`MAX_log2DAT_DW)
`define Max_Tin_div_Tout (`Max_Tin/`Tout)
`define Max_log2Tin_minus_log2Tout (`Max_log2Tin-`log2Tout)

///////////////////////////////
`define MAX_DW    8                 // datawidth
`define MAX_log2DW   3
`define MAX_DW_Ratio (`MAX_DW)

`define MAX_DAT_DW `MAX_DW
`define MAX_log2DAT_DW `MAX_log2DW
`define AXI_DAT_WIDTH (`MAX_DAT_DW *`Tout)
`define log2AXI_DAT_WIDTH (`MAX_log2DAT_DW +`log2Tout)

`define MAX_WT_DW `MAX_DW
`define MAX_log2WT_DW `MAX_log2DW
`define AXI_WT_WIDTH (`MAX_WT_DW *`Tout)
`define log2AXI_WT_WIDTH (`MAX_log2WT_DW +`log2Tout)

`define MAX_BN_DW 8//`MAX_DAT_DW
`define MAX_log2BN_DW 3//`MAX_log2DAT_DW
`define AXI_BN_WIDTH (`MAX_BN_DW *`Tout)
`define log2AXI_BN_WIDTH (`MAX_log2BN_DW +`log2Tout)

`define MAX_DW2   (`MAX_DAT_DW+`MAX_WT_DW)     // for example: multiply=datawidth*2, 

`define Pixel_Data_Width (`Tout*`MAX_DAT_DW)
`define Pixel_Data_Bytes ((`Tout*`MAX_DAT_DW)>>3)        
`define Pixel_BN_Data_Bytes ((`Tout*`MAX_BN_DW)>>3)

////////////////////Define the on chip BUF
`define BRAM_DEPTH ((1<<24)/`base_Tin/`MAX_DAT_DW/`BRAM_NUM)  //18: 256Kb for ASIC.
                                                              //23: 8Mb for ZCU104, single BRAM buf is 512(depth)*72(width)= 36864 bit
`define log2BRAM_DEPTH (24-`base_log2Tin-`MAX_log2DAT_DW-`log2BRAM_NUM)
`define BRAM_NUM 16
`define log2BRAM_NUM 4
`define Buf_Width   `base_Tin*`MAX_DAT_DW
`define BUF_ADDR_LATENCY 3
`define TOTAL_BUF_RD_LATENCY (`BUF_ADDR_LATENCY+1)
`define log2BUF_DEP  (`log2BRAM_DEPTH+`log2BRAM_NUM)


`define AUX_BRAM_WIDTH (`MAX_DAT_DW *`Tout) //the auxiliary on-chip BRAM
`define log2_AUX_BRAM_DEPTH (22-`MAX_log2DAT_DW-`log2Tout)  //22: 4Mb


////////////////////////// vector systolic
`define Tn  (`Tout/2)//the num of vector size
`define log2Tn (`log2Tout-1)

`ifdef Column_Systolic_MAC
    `define Systolic_Delay (`Tout/`Tn)
`else
    `define Systolic_Delay 0
`endif

`define DSP_Delay 1
`define Add_Tree_Delay (`base_log2Tin+1)

`define MAC_LATENCY (`Add_Tree_Delay + `DSP_Delay +`Systolic_Delay)
`define CACC_LATENCY 4

///////////////////// AXI BUS
`define CSR_REG_NUM 256
`define log2CSR_REG_NUM 8

`define AXI_BURST_LEN `Tout// `Tout should be `AXI_BURST_LEN in Matrix Transpose 
`define log2AXI_BURST_LEN `log2Tout


///// don't change /////
`define LN_div_delay 16
`define Sqrt_delay 6
`define Softmax_delay 10
`define Gelu_delay 8

///////////////////Credit of the FSM->ACC buf vld
`ifdef  Column_Systolic_MAC    
    `define FSM_CREDIT_NUM	(`Tout+1)  	//it means how many feature pixels there are in the conv_stripe 
    `define log2FSM_CREDIT_NUM (`log2Tout+1) 
    `define CACC_OUT_DEP (`FSM_CREDIT_NUM+`MAC_LATENCY+`CACC_LATENCY+`Gelu_delay) // (`Tout)+(`base_log2Tin+`Tout)+1
    `define log2_CACC_OUT_DEP (`log2Tout+2)      //suppose  `Tout>`base_log2Tin+2
`else
    `define FSM_CREDIT_NUM	`Tout			//it means how many feature pixels there are in the conv_stripe 
    `define log2FSM_CREDIT_NUM `log2Tout  
    `define CACC_OUT_DEP (`FSM_CREDIT_NUM+`MAC_LATENCY+`CACC_LATENCY+`Gelu_delay)  //bn_fifo_dep
    `define log2_CACC_OUT_DEP (`log2FSM_CREDIT_NUM + 1) //suppose `Tout>=`base_log2Tin+1
`endif

`define BN_FIFO_DEP `AXI_BURST_LEN
`define log2_BN_FIFO_DEP `log2AXI_BURST_LEN

`define POOL_1D_DAT_IN_DEP `AXI_BURST_LEN
`define log2_POOL_1D_DAT_IN_DEP (`log2AXI_BURST_LEN)
`define POOL_2D_DAT_IN_DEP `AXI_BURST_LEN*2
`define log2_POOL_2D_DAT_IN_DEP (`log2AXI_BURST_LEN+1)

`define MCIF_RD_CREDIT_NUM `AXI_BURST_LEN 
`define log2MCIF_CREDIT_NUM   `log2AXI_BURST_LEN

