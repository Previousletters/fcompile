///////////////////     Parallelism    ////////////////////
`define HBM
`define HBM_Port 32
`define HBM_AXI_DATA_WIDTH 256
`define log2_HBM_Port ($clog2(`HBM_Port))
`define log2_HBM_AXI_DATA_WIDTH ($clog2(`HBM_AXI_DATA_WIDTH))
`define HBM_ADDR_WIDTH 37

//`define ASYN_MODE

`ifdef ASYN_MODE
    `define ASYN_FACTOR 2
`else
    `define ASYN_FACTOR 1
`endif

`define base_Tin 128
`define log2_base_Tin ($clog2(`base_Tin))
`define Tout 32
`define log2_Tout ($clog2(`Tout))
`define Tb 1
`define log2_Tb ($clog2(`Tb)) 


`define ACC_LATENCY 2

`define MAX_DW        16
`define log2_MAX_DW    ($clog2(`MAX_DW))

///////////////////   AXI interface    ////////////////////
`define DDR_WIDTH 32
`define DDR_BYTES (`DDR_WIDTH/8)
`define MAX_AXI_DW 512  //important Max_AXI_DW >= Tout*MAX_DW
`define log2_MAX_AXI_DW ($clog2(`MAX_AXI_DW))
`define CSR_REG_NUM 256
`define log2_CSR_REG_NUM ($clog2(`CSR_REG_NUM))

`define AXI_BURST_LEN `Tout// `Tout should be `AXI_BURST_LEN in Matrix Transpose 
`define log2_AXI_BURST_LEN `log2_Tout
`define MCIF_RD_CREDIT_NUM `AXI_BURST_LEN 
`define log2_MCIF_CREDIT_NUM `log2_AXI_BURST_LEN

/////////////////// vector systolic /////////////////////
`define Column_Systolic_MAC         // Please check the "Parallel_MAC_Calculation.v" 
`define Tn  (`Tout/2)//the num of vector size
`define log2_Tn ($clog2(`Tn))

///////////////////     DATA WIDTH    ////////////////////
`define MAX_DAT_DW     `MAX_DW
`define log2_MAX_DAT_DW `log2_MAX_DW
`define MAX_DW_Ratio (`MAX_DW)
`define AXI_DAT_WIDTH (`MAX_DAT_DW*`Tout*`Tb)
`define log2_AXI_DAT_WIDTH ($clog2(`AXI_DAT_WIDTH))

`define MAX_WT_DW 4
`define log2_MAX_WT_DW ($clog2(`MAX_WT_DW))
`define MULTI_WT_BUF  //((`MAX_WT_DW*`base_Tin) < (`MAX_DAT_DW*`Tout*`Tb)? 1:0)
`define WT_AXI_BURST_LEN (64)
`define log2_WT_AXI_BURST_LEN ($clog2(`WT_AXI_BURST_LEN))

//`define FP16INT4_MAC_BUF_ADD_OUT "1111110"
//`define FP16INT4_MAC_EXP_MAX 19       //max input is 31.98    according to FP16: 2^(19-15)*(1+1023/1024)
//`define FP16INT4_MAC_ADD_R_DW 15
//`define FP16INT4_MAC_EXP_MAX_SCALE 15 
//`define FP16INT4_MAC_ADD_R_DW_SCALE 9 //
//`define FP16INT4_MAC_FIXPOINT_DW 24

////`define FP16INT4_MAC_Out_Width_fixpoint
//`define FP16INT4_MAC_Out_Width_FP16
////`define FP16INT4_MAC_Out_Width_FP20

//`ifdef FP16INT4_MAC_Out_Width_FP16
//    `define MAX_DW2   16//(`MAX_DAT_DW+`MAX_WT_DW)     // for example: multiply=datawidth*2, 
//`endif
//`ifdef FP16INT4_MAC_Out_Width_FP20
//    `define MAX_DW2   20//(`MAX_DAT_DW+`MAX_WT_DW)     // for example: multiply=datawidth*2, 
//`endif
//`ifdef FP16INT4_MAC_Out_Width_fixpoint
//    `define MAX_DW2   `FP16INT4_MAC_FIXPOINT_DW//(`MAX_DAT_DW+`MAX_WT_DW)     // for example: multiply=datawidth*2, 
//`endif

`define FP16INT4_MAC_BUF_ADD_OUT "1111110"
`define FP16INT4_MAC_EXP_MAX 19
`define FP16INT4_MAC_FIXPOINT_DW 24
`define FP16INT4_MAC_Out_Width_FP16
`define Mode_C_delay2
//`define Mode_B_delay1
//`define Mode_A_delay0
`define FP16INT4_mac_auto_expcompare

`ifdef FP16INT4_mac_auto_expcompare
    `define FP16INT4_MAC_ADD_R_DW 5
`else
    `define FP16INT4_MAC_ADD_R_DW 15
`endif
`define MAX_DW2   16//(`MAX_DAT_DW+`MAX_WT_DW)     // for example: multiply=datawidth*2, 


`define MAX_FP_DW  (16+4)// for Float point adder 
`define MAX_Matrix_FP_DW  (16+4)// for Float point adder

`define T_quant_block    128 //`base_Tin //equal to the base_Tin
`define log2_T_quant_block ($clog2(`T_quant_block)) //(`log2_base_Tin)
`define WT_quant_scale_DW 16
`define WT_CH_Tgroup (`T_quant_block*`HBM_AXI_DATA_WIDTH/`WT_quant_scale_DW)
`define log2_WT_CH_Tgroup ($clog2(`WT_CH_Tgroup))

`define MAX_BN_DW     16  //`MAX_DAT_DW
`define log2_MAX_BN_DW ($clog2(`MAX_BN_DW))
`define AXI_BN_WIDTH (`MAX_BN_DW*`Tout*`Tb)
`define log2_AXI_BN_WIDTH ($clog2(`AXI_BN_WIDTH))
`define SINGLE_BN_FIFO_DEP ((`AXI_BURST_LEN*`MAX_DAT_DW*`Tb)/(`MAX_BN_DW*2)) // It means max chout_slice =512 for each burst
`define BN_FIFO_DEP (4*`SINGLE_BN_FIFO_DEP)                                  // It means max chout_slice =4*512 for each burst
`define log2_BN_FIFO_DEP ($clog2(`BN_FIFO_DEP))
`define BN_FIFO_NUM ((`MAX_BN_DW*2)/(`MAX_DAT_DW*`Tb))

`define Pixel_Data_Width (`AXI_DAT_WIDTH)
`define Pixel_Data_Bytes ((`AXI_DAT_WIDTH)>>3)        
`define Pixel_BN_Data_Bytes ((`Tout*`MAX_BN_DW)>>3)


///////////////////   Conv and Matmul  ////////////////////
`define log2_CH 16
`define log2_H 16
`define log2_W `log2_H
`define log2_KyKx 12
`define log2_other (`log2_CH-`log2_base_Tin-4)
`define log2_P 5
`define log2_S 6
`define log2_K 6
`define log2_scale 6      //[5:0], [5]for sign, [4:0]for shift value, always positive 
//`define Need_Left_Shift   //[5]for sign, "0" means right shift, "1" means left shift

`define base_Tin_div_Tout (`base_Tin/`Tout)
`define Max_Tin (`base_Tin*`MAX_DAT_DW)        //for the 1bit base
`define log2_Max_Tin ($clog2(`Max_Tin))
`define Max_Tin_div_Tout (`Max_Tin/`Tout)
`define log2_Max_Tin_minus_log2_Tout ($clog2(`Max_Tin_div_Tout))

///////////////////   ON-CHIP BUF  ////////////////////
`define DAT_BRAM_NUM 1
`define log2_DAT_BRAM_NUM ($clog2(`DAT_BRAM_NUM))
`define DAT_BRAM_DEPTH ((1<<22)/`base_Tin/`MAX_DAT_DW/`DAT_BRAM_NUM)  //18: 256Kb for ASIC.
                                                              //23: 8Mb for ZCU104, single BRAM buf is 512(depth)*72(width)= 36864 bit
`define log2_DAT_BRAM_DEPTH ($clog2(`DAT_BRAM_DEPTH))
`define DAT_BRAM_WIDTH (`base_Tin*`MAX_DAT_DW)

////////////////////Define the on chip WT BUF
`define WT_BRAM_NUM `HBM_Port
`define WT_BRAM_WIDTH (`HBM_AXI_DATA_WIDTH) //(64*`MAX_WT_DW)
`define log2_WT_BRAM_NUM ($clog2(`WT_BRAM_NUM))
`define TRUE_WT_BRAM_DEPTH ((1<<22)/`HBM_AXI_DATA_WIDTH/`WT_BRAM_NUM)  //18: 256Kb for ASIC.
                              //23: 8Mb for ZCU104, single BRAM buf is 512(depth)*72(width)= 36864 bit
`define WT_BRAM_DEPTH (`TRUE_WT_BRAM_DEPTH/`ASYN_FACTOR)
`define log2_WT_BRAM_DEPTH ($clog2(`WT_BRAM_DEPTH))

`define BUF_ADDR_LATENCY 3
`define TOTAL_BUF_RD_LATENCY (`BUF_ADDR_LATENCY+1)
`define log2_DAT_BUF_DEPTH  (`log2_DAT_BRAM_DEPTH+`log2_DAT_BRAM_NUM)
`define log2_WT_BUF_DEPTH  (`log2_WT_BRAM_DEPTH+`log2_WT_BRAM_NUM)

`define WT_SCALE_BUF_WIDTH (`WT_quant_scale_DW*`HBM_Port)
`define WT_SCALE_BUF_DEPTH ((`WT_BRAM_NUM*`TRUE_WT_BRAM_DEPTH*`WT_BRAM_WIDTH/(`T_quant_block*`MAX_WT_DW/`WT_quant_scale_DW))/`WT_SCALE_BUF_WIDTH)
`define log2_WT_SCALE_BUF_DEPTH ($clog2(`WT_SCALE_BUF_DEPTH))

`define AUX_BRAM_WIDTH (`AXI_DAT_WIDTH) //the auxiliary on-chip BRAM
`define log2_AUX_BRAM_DEPTH (22-`log2_MAX_DAT_DW-`log2_Tout-`log2_Tb)  //22: 4Mb

///////////////////   Conv and Matmul delay  ////////////////////
`ifdef Column_Systolic_MAC
    `define Systolic_Delay (`Tout/`Tn)
`else
    `define Systolic_Delay 0
`endif

`define DSP_Delay 2
`ifdef Mode_C_delay2
    `define Add_Tree_Delay (`log2_base_Tin+4)
`elsif Mode_B_delay1
    `define Add_Tree_Delay (`log2_base_Tin+3)
`elsif Mode_A_delay0
    `define Add_Tree_Delay (`log2_base_Tin+2)
`else
    `define Add_Tree_Delay (`log2_base_Tin+2)
`endif

`define MAC_LATENCY (`Add_Tree_Delay + `DSP_Delay +`Systolic_Delay)
`define CACC_LATENCY `ACC_LATENCY

`ifdef  Column_Systolic_MAC    
    `define FSM_CREDIT_NUM    (`Tout+1)      //it means how many feature pixels there are in the Conv_stripe 
    `define log2FSM_CREDIT_NUM ($clog2(`FSM_CREDIT_NUM))
    `define CACC_OUT_DEP (`FSM_CREDIT_NUM+`MAC_LATENCY+`CACC_LATENCY) // (`Tout)+(`log2_base_Tin+`Tout)+1
    `define log2_CACC_OUT_DEP ($clog2(`CACC_OUT_DEP))
`else
    `define FSM_CREDIT_NUM    `Tout            //it means how many feature pixels there are in the Conv_stripe 
    `define log2FSM_CREDIT_NUM `log2_Tout  
    `define CACC_OUT_DEP (`FSM_CREDIT_NUM+`MAC_LATENCY+`CACC_LATENCY)  //bn_fifo_dep
    `define log2_CACC_OUT_DEP ($clog2(`CACC_OUT_DEP))
`endif

///////////////////   Pool  ////////////////////
`define AXI_BURST_LEN_POOL `AXI_BURST_LEN
`define log2_AXI_BURST_LEN_POOL `log2_AXI_BURST_LEN

`define POOL_BRAM_DEPTH ((1<<18)/`Tout/`MAX_DAT_DW/`Tb)  //18: 256Kb for ASIC.
`define log2_POOL_BRAM_DEPTH ($clog2(`POOL_BRAM_DEPTH))  //20: 1Mb
`define Pool_credit_cnt 4
`define log2_Pool_credit_cnt ($clog2(`Pool_credit_cnt))
`define Pool_avg_dw 16

///////////////////   Elementwise  ////////////////////
`define AXI_BURST_LEN_ElementWise `AXI_BURST_LEN
`define log2_AXI_BURST_LEN_ElementWise `log2_AXI_BURST_LEN

`define Elementwise_credit_cnt 3
`define log2_Elementwise_credit_cnt ($clog2(`Elementwise_credit_cnt))


`define AXI_BURST_LEN_EMB `AXI_BURST_LEN
`define log2_AXI_BURST_LEN_EMB `log2_AXI_BURST_LEN

///////////////////   Transformer   ////////////////////
`define Emb_credit_cnt 5
`define log2_Emb_credit_cnt ($clog2(`Emb_credit_cnt))
`define log2_Matrix_pixel 16
`define log2_Matrix_ch 16

`define AXI_BURST_LEN_SOFTMAX 2
`define log2AXI_BURST_LEN_SOFTMAX ($clog2(`AXI_BURST_LEN_SOFTMAX))
`define fp16_divider_latency 21
`define fp20_divider_latency 27
`define Softmax_delay 3

`define LN_div_delay 16
`define Sqrt_delay 5
`define LN_stage1_delay (`Sqrt_delay+2)
`define Tout_ACC_Out_Width_FP20
`define Tout_ACC_BUF_ADD_OUT "111110"
`define Tout_ACC_tp_DW 24
`define Tout_SQUARE_ACC_Out_Width_FP20
`define Tout_SQUARE_ACC_BUF_ADD_OUT "111110"
`define Tout_SQUARE_ACC_tp_DW 33

`define LN_cal_delay 5
`define LN_credit_cnt (`LN_cal_delay+3)//3 is for backup


`define ACT_credit_cnt (4+1)
`define Linear_DW 16

`define F2WTrans_scale_credit_cnt 2
`define F2WTrans_dat_credit_cnt 2

`define Transpose_credit_cnt 3
`define log2_Transpose_credit_cnt ($clog2(`Transpose_credit_cnt)+2)
`define COMP_DELAY1 "1111111" //`log2_Tout
`define COMP_DELAY2 "1111"  //`log2_T_quant_block-`log2_Tout

`define CFG_ID_WIDTH    (8)
`define CFG_ARAAY_DEPTH (2*`Tout)
`define CFG_ARAAY_WIDTH (`AXI_DAT_WIDTH)

`define MAX_TOKEN 128
`define MIN_WT_HEAD 2
`define MAX_CH_per_HEAD    128
`define TRP_WT_BUF_DEP (`MAX_TOKEN*`MIN_WT_HEAD*`MAX_CH_per_HEAD/`Tout)
`define TRP_DAT_BUF_DEP (`AXI_BURST_LEN*`MAX_CH_per_HEAD/`Tout)
`define TRP_CREDIT_NUM 2

`define FP16FP16_MAC_BUF_ADD_OUT "111110"
`define FP16FP16_MAC_EXP_MAX 33
`define FP16FP16_MAC_ADD_R_DW 0
`define FP16FP16_MAC_Out_Width_FP16