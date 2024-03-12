///////////////////     Parallelism    ////////////////////
#define HBM
#define HBM_Port 32
#define HBM_AXI_DATA_WIDTH 256
#define log2_HBM_Port (log2(HBM_Port))
#define log2_HBM_AXI_DATA_WIDTH (log2(HBM_AXI_DATA_WIDTH))
#define HBM_ADDR_WIDTH 37

#define base_Tin 128
#define log2_base_Tin (log2(base_Tin))
#define Tout 32
#define log2_Tout (log2(Tout))
#define Tb 1
#define log2_Tb (log2(Tb)) 


//#define FP16INT4_MAC_Out_Width_FP16
#define FP16INT4_MAC_Out_Width_FP20
#define ACC_LATENCY 2

#define MAX_DW        16
#define log2_MAX_DW    (log2(MAX_DW))

///////////////////   AXI interface    ////////////////////
#define DDR_WIDTH 32
#define DDR_BYTES (DDR_WIDTH/8)
#define MAX_AXI_DW 512  //important Max_AXI_DW >= Tout*MAX_DW
#define log2_MAX_AXI_DW (log2(MAX_AXI_DW))
#define CSR_REG_NUM 256
#define log2_CSR_REG_NUM (log2(CSR_REG_NUM))

#define AXI_BURST_LEN Tout// Tout should be AXI_BURST_LEN in Matrix Transpose 
#define log2_AXI_BURST_LEN log2_Tout
#define MCIF_RD_CREDIT_NUM AXI_BURST_LEN 
#define log2_MCIF_CREDIT_NUM log2_AXI_BURST_LEN

/////////////////// vector systolic /////////////////////
#define Column_Systolic_MAC         // Please check the "Parallel_MAC_Calculation.v" 
#define Tn  (Tout/2)//the num of vector size
#define log2_Tn (log2(Tn))

///////////////////     DATA WIDTH    ////////////////////
#define MAX_DAT_DW     MAX_DW
#define log2_MAX_DAT_DW log2_MAX_DW
#define MAX_DW_Ratio (MAX_DW)
#define AXI_DAT_WIDTH (MAX_DAT_DW*Tout*Tb)
#define log2_AXI_DAT_WIDTH (log2_MAX_DAT_DW +log2_Tout+log2_Tb)

#define MAX_WT_DW 4
#define log2_MAX_WT_DW 2
#define MULTI_WT_BUF  //((MAX_WT_DW*base_Tin) < (MAX_DAT_DW*Tout*Tb)? 1:0)
#define AXI_BURST_LEN_WT AXI_BURST_LEN
#define log2_AXI_BURST_LEN_WT log2_AXI_BURST_LEN

#ifdef FP16INT4_MAC_Out_Width_FP16
    #define MAX_DW2   16//(MAX_DAT_DW+MAX_WT_DW)     // for example: multiply=datawidth*2, 
#endif
#ifdef FP16INT4_MAC_Out_Width_FP20
    #define MAX_DW2   20//(MAX_DAT_DW+MAX_WT_DW)     // for example: multiply=datawidth*2, 
#endif
#define MAX_FP_DW  (16+4)// for Float point adder 
#define MAX_Matrix_FP_DW  (16+4)// for Float point adder

#define T_quant_block    128 //base_Tin //equal to the base_Tin
#define log2_T_quant_block 7 //(log2_base_Tin)
#define WT_quant_scale_DW 16
#define WT_CH_Tgroup (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW)
#define log2_WT_CH_Tgroup (log2(WT_CH_Tgroup))

#define MAX_BN_DW     16  //MAX_DAT_DW
#define log2_MAX_BN_DW 4//log2_MAX_DAT_DW
#define AXI_BN_WIDTH (MAX_BN_DW*Tout*Tb)
#define log2_AXI_BN_WIDTH (log2_MAX_BN_DW +log2_Tout+log2_Tb)
#define SIGNLE_BN_FIFO_DEP ((AXI_BURST_LEN*MAX_DAT_DW*Tb)/(MAX_BN_DW*2))
#define BN_FIFO_DEP (1*SIGNLE_BN_FIFO_DEP)
#define log2_BN_FIFO_DEP (log2_AXI_BURST_LEN+log2_MAX_DAT_DW+log2_Tb-log2_MAX_BN_DW-1)
#define BN_FIFO_NUM ((MAX_BN_DW*2)/(MAX_DAT_DW*Tb))
#define BN_SURFACE_STRIDE ((Tout*MAX_BN_DW*2) >> 3)

#define Pixel_Data_Width (AXI_DAT_WIDTH)
#define Pixel_Data_Bytes ((AXI_DAT_WIDTH)>>3)        
#define Pixel_BN_Data_Bytes ((Tout*MAX_BN_DW)>>3)


///////////////////   Conv and Matmul  ////////////////////
#define log2_CH 16
#define log2_H 16
#define log2_W log2_H
#define log2_KyKx 12
#define log2_other (log2_CH-log2_base_Tin-4)
#define log2_P 5
#define log2_S 6
#define log2_K 6
#define log2_scale 6      //[5:0], [5]for sign, [4:0]for shift value, always positive 
//#define Need_Left_Shift   //[5]for sign, "0" means right shift, "1" means left shift

#define base_Tin_div_Tout (base_Tin/Tout)
#define Max_Tin (base_Tin*MAX_DAT_DW)        //for the 1bit base
#define log2_Max_Tin (log2_base_Tin+log2_MAX_DAT_DW)
#define Max_Tin_div_Tout (Max_Tin/Tout)
#define log2_Max_Tin_minus_log2_Tout (log2_Max_Tin-log2_Tout)

///////////////////   ON-CHIP BUF  ////////////////////
#define DAT_BRAM_NUM 1
#define log2_DAT_BRAM_NUM 0
#define DAT_BRAM_DEPTH ((1<<22)/base_Tin/MAX_DAT_DW/DAT_BRAM_NUM)  //18: 256Kb for ASIC.
                                                              //23: 8Mb for ZCU104, single BRAM buf is 512(depth)*72(width)= 36864 bit
#define log2_DAT_BRAM_DEPTH (log2(DAT_BRAM_DEPTH))
#define DAT_BRAM_WIDTH (base_Tin*MAX_DAT_DW)

////////////////////Define the on chip WT BUF
#define WT_BRAM_NUM HBM_Port
#define WT_BRAM_WIDTH (HBM_AXI_DATA_WIDTH) //(64*MAX_WT_DW)
#define log2_WT_BRAM_NUM (log2(WT_BRAM_NUM))
#define WT_BRAM_DEPTH ((1<<24)/HBM_AXI_DATA_WIDTH/WT_BRAM_NUM)  //18: 256Kb for ASIC.
                                                              //23: 8Mb for ZCU104, single BRAM buf is 512(depth)*72(width)= 36864 bit
#define log2_WT_BRAM_DEPTH (log2(WT_BRAM_DEPTH))

#define BUF_ADDR_LATENCY 3
#define TOTAL_BUF_RD_LATENCY (BUF_ADDR_LATENCY+1)
#define log2_DAT_BUF_DEPTH  (log2_DAT_BRAM_DEPTH+log2_DAT_BRAM_NUM)
#define log2_WT_BUF_DEPTH  (log2_WT_BRAM_DEPTH+log2_WT_BRAM_NUM)

#define WT_SCALE_BUF_WIDTH (WT_quant_scale_DW*HBM_Port)
#define WT_SCALE_BUF_DEPTH ((WT_BRAM_NUM*WT_BRAM_DEPTH*WT_BRAM_WIDTH/(T_quant_block*MAX_WT_DW/WT_quant_scale_DW))/WT_SCALE_BUF_WIDTH)
#define log2_WT_SCALE_BUF_DEPTH (log2(WT_SCALE_BUF_DEPTH))

#define AUX_BRAM_WIDTH (AXI_DAT_WIDTH) //the auxiliary on-chip BRAM
#define log2_AUX_BRAM_DEPTH (23-log2_MAX_DAT_DW-log2_Tout-log2_Tb)  //22: 4Mb

///////////////////   Conv and Matmul delay  ////////////////////
#ifdef Column_Systolic_MAC
    #define Systolic_Delay (Tout/Tn)
#else
    #define Systolic_Delay 0
#endif

#define FP16INT4_MAC_BUF_ADD_OUT "1111110"
#define FP16INT4_MAC_EXP_MAX 23
#define FP16INT4_MAC_ADD_R_DW 0

#define DSP_Delay 2
#define Add_Tree_Delay (log2_base_Tin+1)

#define MAC_LATENCY (Add_Tree_Delay + DSP_Delay +Systolic_Delay)
#define CACC_LATENCY ACC_LATENCY

#ifdef  Column_Systolic_MAC    
    #define FSM_CREDIT_NUM	(Tout+1)  	//it means how many feature pixels there are in the Conv_stripe 
    #define log2FSM_CREDIT_NUM (log2_Tout+1) 
    #define CACC_OUT_DEP (FSM_CREDIT_NUM+MAC_LATENCY+CACC_LATENCY) // (Tout)+(log2_base_Tin+Tout)+1
    #define log2_CACC_OUT_DEP (log2(CACC_OUT_DEP))
#else
    #define FSM_CREDIT_NUM	Tout			//it means how many feature pixels there are in the Conv_stripe 
    #define log2FSM_CREDIT_NUM log2_Tout  
    #define CACC_OUT_DEP (FSM_CREDIT_NUM+MAC_LATENCY+CACC_LATENCY)  //bn_fifo_dep
    #define log2_CACC_OUT_DEP (log2(CACC_OUT_DEP))
#endif

///////////////////   Pool  ////////////////////
#define AXI_BURST_LEN_POOL AXI_BURST_LEN
#define log2_AXI_BURST_LEN_POOL log2_AXI_BURST_LEN

#define POOL_BRAM_DEPTH ((1<<18)/Tout/MAX_DAT_DW/Tb)  //18: 256Kb for ASIC.
#define log2_POOL_BRAM_DEPTH (18-log2_MAX_DAT_DW-log2_Tout-log2_Tb)  //20: 1Mb
#define Pool_delay 4
#define log2_Pool_delay 2
#define Pool_avg_dw 16

///////////////////   Transformer   ////////////////////
#define log2_Matrix_pixel 12
#define log2_Matrix_ch 16

#define AXI_BURST_LEN_SOFTMAX 4
#define log2AXI_BURST_LEN_SOFTMAX 2
#define fp16_divider_latency 21
#define fp20_divider_latency 27
#define Softmax_delay 3

#define LN_div_delay 16
#define Sqrt_delay 5
#define LN_stage1_delay (Sqrt_delay+2)
#define Tout_ACC_Out_Width_FP20
#define Tout_ACC_BUF_ADD_OUT "111110"
#define Tout_ACC_tp_DW 24
#define Tout_SQUARE_ACC_Out_Width_FP20
#define Tout_SQUARE_ACC_BUF_ADD_OUT "111110"
#define Tout_SQUARE_ACC_tp_DW 23

#define LN_cal_delay 5
#define LN_credit_cnt (LN_cal_delay)


#define ACT_delay 3
#define Linear_DW 16

#define Transpose_credit_cnt 3
#define log2_Transpose_credit_cnt (log2(Transpose_credit_cnt)+2)
#define COMP_DELAY1 "1111111" //log2_Tout
#define COMP_DELAY2 "1111"  //log2_T_quant_block-log2_Tout
