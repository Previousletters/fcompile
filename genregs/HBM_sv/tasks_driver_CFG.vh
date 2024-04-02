

task RunCFG_MVM_BN_Res(input bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,bit relu_en,
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
        bit[31:0] wt_base_addr,bit [31:0]wt_size_in_bytes,bit [31:0]wt_num_div_Tin,bit[31:0] BN_base_addr,
        bit[31:0] ResAdd_base_addr,bit[31:0] ResAdd_surface_stride,bit[31:0] ResAdd_line_stride,
        bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,
        output bit[`AXI_DAT_WIDTH-1:0]cfg_reg_array[`CFG_ARAAY_DEPTH-1:0],
        output bit [31:0]Aux_Reg_Group_Nums,
        output bit [31:0]max_tasks_num);
begin
    bit [31:0] overlap;
    bit [31:0] dat_num_per_row;
    bit [31:0] Wout,Hout;
    bit [31:0] min_dat_depth;
    bit [31:0] min_wt_depth;
    
    bit [31:0] single_dat_buf_num;
    bit [31:0] best_single_dat_banks;
    bit [31:0] best_wt_banks;
    bit [31:0] mininum_bw;
    bit [31:0] best_method;
    bit [31:0] wt_factor;
    
    bit [31:0] total_bytes_if_reuse_wt,total_bytes_if_reuse_dat;
    bit [31:0] Hout_Split_Times;
    bit [31:0] in_height_first,in_height_middle,in_height_last;    
    bit [31:0] Hout_first,Hout_middle,Hout_last;
    bit [31:0] CHout_Split_Times;
    bit [31:0] out_ch_slice,out_ch_slice_last;   
    
    bit [31:0] CHout_Split_Times_minus1; 
    bit [31:0] BN_SURFACE_STRIDE     ;
    bit [31:0] BN_FIFO_bits          ;
    bit [31:0] BN_FIFO_chout_num     ;
    
    bit [31:0] Tb                    ;
    bit [31:0] Tout                  ;
    bit [31:0] Tin                   ;
    bit [31:0] CHout_div_Tout        ;
    bit [31:0] CHin_div_Tout         ;
    bit [31:0] Tin_div_Tout          ;
    bit [31:0] CHout_Padding         ;
    bit [31:0] CHin_Padding_with_Tout;
    
    bit [31:0] RELU_EN               ;
    bit [31:0] WT_CHin_div_Tin       ;
    bit [31:0] WT_CHin_Padding_with_Tin;
    bit [31:0] WT_SIZE_IN_BYTES      ;
    bit [31:0] WT_NUM_DIV_TIN        ;
    
    bit [31:0] DAT_IN_scale          ;
    bit [31:0] WT_scale              ;
    bit [31:0] Conv_out_scale        ;
    bit [31:0] DAT_OUT_scale         ;
    
    Tb                     = `Tb                    ;
    Tout                   = `Tout                  ;
    Tin                    = `Tin                   ;
    Wout                   = `Wout                  ; 
    Hout                   = `Hout                  ;
    CHout_div_Tout         = `CHout_div_Tout        ;
    CHin_div_Tout          = `CHin_div_Tout         ;
    Tin_div_Tout           = `Tin_div_Tout          ;
    CHout_Padding          = `CHout_Padding         ;
    CHin_Padding_with_Tout = `CHin_Padding_with_Tout;
    
    RELU_EN                = `RELU_EN               ;
    WT_CHin_div_Tin        = `WT_CHin_div_Tin       ;
    WT_CHin_Padding_with_Tin=`WT_CHin_Padding_with_Tin;
    WT_SIZE_IN_BYTES       = `WT_SIZE_IN_BYTES      ;
    WT_NUM_DIV_TIN         = `WT_NUM_DIV_TIN        ;
    
    BN_SURFACE_STRIDE      = (`Tout*`MAX_BN_DW*2)>>3;
    DAT_IN_scale           = `DAT_IN_scale          ;
    WT_scale               = `WT_scale              ;
    Conv_out_scale         = `Conv_out_scale        ;
    DAT_OUT_scale          = `DAT_OUT_scale         ;
    
    mininum_bw=0;

    Wout=Win;
    Hout=Hin;
    overlap=0;
    dat_num_per_row=Win*WT_CHin_div_Tin;
    min_dat_depth=dat_num_per_row;
    min_wt_depth=WT_CHin_div_Tin*((`Tin*`MAX_WT_DW)/`HBM_AXI_DATA_WIDTH)*(Tout/`HBM_Port);

    $display("\nTotal DAT_BRAM_NUM=%0d",`DAT_BRAM_NUM);
    $display("\nTotal  WT_BRAM_NUM=%0d",`WT_BRAM_NUM);
    $display("DAT_BRAM_DEPTH=%0d, DAT_BRAM_WIDTH=%d, min_dat_depth=%0d",`DAT_BRAM_DEPTH,`DAT_BRAM_WIDTH,min_dat_depth);
    $display(" WT_BRAM_DEPTH=%0d, WT_BRAM_WIDTH =%d, min_wt_depth=%0d",`WT_BRAM_DEPTH,`WT_BRAM_WIDTH,min_wt_depth);
    $display(" WT_SCALE_BUF_DEPTH=%0d,  WT_SCALE_BUF_WIDTH=%0d",`WT_SCALE_BUF_DEPTH,`WT_SCALE_BUF_WIDTH);
    $display("\nWout=%0d, Hout=%0d, Chin=%0d",Wout,Hout,CHin);
    $display("CHout_Extend=%d,    CHin_Padding_with_Tout=%d",CHout_Padding,   CHin_Padding_with_Tout);
    $display("CHin_div_Tout=%0d, CHout_div_Tout=%0d",CHin_div_Tout,CHout_div_Tout);
    $display("WT_CHin_div_Tin=%0d, WT_CHin_Padding_with_Tin=%0d",WT_CHin_div_Tin,WT_CHin_Padding_with_Tin);
    $display("Pure_WT_SIZE_IN_BYTES=%0d",((`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_DW)>>3));
    $display("WT_Scale_SIZE_IN_BYTES=%0d",(`WT_scale_bits)>>3);
    $display("WT_SIZE_IN_BYTES=%0d",WT_SIZE_IN_BYTES);
    $display("WT_NUM_DIV_TIN=%0d",WT_NUM_DIV_TIN);
    $display("min_wt_depth=%0d",min_wt_depth);
    
    if(min_dat_depth>`DAT_BRAM_DEPTH)
    begin
        $display("=======================================================================");
        $display("=============== FPGA DAT BRAM DEPTH not enough!    ====================");
        $display("=======================================================================");
        $finish;
    end
    if(min_wt_depth>`WT_BRAM_DEPTH)
    begin
        $display("=======================================================================");
        $display("================ FPGA WT BRAM DEPTH not enough!    ====================");
        $display("=======================================================================");
        $finish;
    end
    
    total_bytes_if_reuse_wt=Tb*(dat_num_per_row*Hin)*CHout_Split_Times*(Tin*`MAX_DAT_DW/8)
                          +CHout_Padding*WT_CHin_div_Tin*(Tin*`MAX_WT_DW/8)
                          +(CHout_Split_Times * Hout_Split_Times * 100 * 32/8);
    total_bytes_if_reuse_dat=Hout_Split_Times*CHout_Padding*WT_CHin_div_Tin*(Tin*`MAX_WT_DW/8)
                          +(dat_num_per_row*Hin)*(Tin*`MAX_DAT_DW/8)*Tb
                          +(CHout_Split_Times*Hout_Split_Times * 100 * 32/8);

    $display("total_bytes_if_reuse_wt=%0d,total_bytes_if_reuse_dat=%0d\n",total_bytes_if_reuse_wt,total_bytes_if_reuse_dat);

    if (total_bytes_if_reuse_wt < total_bytes_if_reuse_dat)
        best_method = 0;
    else
        best_method = 1;

    out_ch_slice=(`WT_BRAM_DEPTH/min_wt_depth)*Tout;

    /////////////////////////////////////////////////////////
    BN_FIFO_bits=`AXI_BN_WIDTH*`BN_FIFO_DEP*`BN_FIFO_NUM;
    BN_FIFO_chout_num=BN_FIFO_bits/(`MAX_BN_DW*2);
    if(out_ch_slice>BN_FIFO_chout_num)
        out_ch_slice=BN_FIFO_chout_num;
    $display("BN_FIFO_bits=%0d,BN_FIFO_chout_num=%0d\n",BN_FIFO_bits,BN_FIFO_chout_num);
    /////////////////////////////////////////////////////////
    
    if(out_ch_slice>=CHout_Padding)
        begin
            out_ch_slice=CHout_Padding;
            CHout_Split_Times=1;
        end
    else
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    if(CHout%out_ch_slice==0)
        out_ch_slice_last=out_ch_slice;
    else
        out_ch_slice_last=CHout_Padding%out_ch_slice;
        
    CHout_Split_Times_minus1=CHout_Split_Times-1;
    
    $display("CH_out=%0d,out_ch_slice=%0d,out_ch_slice_last=%0d,CHout_Split_Times_minus1=%0d",CHout_Padding,out_ch_slice,out_ch_slice_last,CHout_Split_Times_minus1);
    $display("best_method=%0d",1);
    
    RunCFG_MVM_BN_Res_single_time(CHout_Split_Times_minus1,CHin,out_ch_slice,out_ch_slice_last,Hin,Win,Hout,Wout,relu_en,Tout,Tin,
            feature_in_base,feature_in_surface_stride,feature_in_line_stride,
            wt_base_addr,wt_size_in_bytes/CHout_Padding*8,BN_base_addr,
            ResAdd_base_addr,ResAdd_surface_stride,ResAdd_line_stride,
            feature_out_base,feature_out_surface_stride,feature_out_line_stride,
            cfg_reg_array,Aux_Reg_Group_Nums,max_tasks_num);
end
endtask

//cfg_dma_DAT_start=current_task_ID[3]==1;
//cfg_dma_WT_start =current_task_ID[3]==1;
//cfg_buf_init     =current_task_ID[3]==1;
//cfg_fsm_dat_start=current_task_ID[3]==1;
//cfg_fsm_wt_start =current_task_ID[3]==1;
//cfg_FC_start     =0;
//cfg_wdma_start   =current_task_ID[3]==1;
//cfg_QKt_start    =current_task_ID[4]==1;
//cfg_SV_start     =current_task_ID[5]==1;
//cfg_BN_start     =current_task_ID[6]==1;
//cfg_ResAdd_start =current_task_ID[7]==1;
//
//cfg_Elementwise_start=current_task_ID==8'b0000_0001;
//
//cfg_FWtrans_start    =current_task_ID==8'b0000_0010;
//cfg_Trans_start      =current_task_ID==8'b0000_0011;
//cfg_Emb_start        =current_task_ID==8'b0000_0100;
//cfg_Softmax_start    =current_task_ID==8'b0000_0101;
//cfg_Activation_start =current_task_ID==8'b0000_0110;
//cfg_LN_start         =current_task_ID==8'b0000_0111;

task RunCFG_MVM_BN_Res_single_time(bit[31:0]CHout_Split_Times_minus1,
        bit[31:0] CHin,bit[31:0] CHout,bit[31:0] CHout_last,bit[31:0] Hin,bit[31:0] Win,bit[31:0] Hout,bit[31:0] Wout,
        bit relu_en,bit [31:0]Tout,bit [31:0]Tin,
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
        bit[31:0] wt_base_addr,bit[31:0] wt_size_in_bits, bit[31:0] BN_base_addr,
        bit[31:0] ResAdd_base_addr,bit[31:0] ResAdd_surface_stride,bit[31:0] ResAdd_line_stride,
        bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,
        output bit[`AXI_DAT_WIDTH-1:0]cfg_reg_array[`CFG_ARAAY_DEPTH-1:0],
        output bit [31:0]Aux_Reg_Group_Nums,
        output bit [31:0]max_tasks_num);
begin

    bit [  `AXI_DAT_WIDTH-1:0]cfg_reg_array0;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array1;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array2;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array3;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array4;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array5;
    bit [1*`AXI_DAT_WIDTH-1:0]cfg_reg_array6;
    bit [2*`AXI_DAT_WIDTH-1:0]cfg_reg_array7;
    
    bit [ 7:0]ID_list [32-1:0];
    bit [31:0]rdata;
    bit [8-1:0]log2_WT_base_addr_Bank_Step;
    bit [`HBM_ADDR_WIDTH-32-1:0]Left_WT_Base_Addr;    
    bit [31:0]LN_CH_burst_times_minus1;
    bit [31:0]FP32_recip_CH_r,FP20_recip_CH_r,FP16_recip_CH_r;
    real recip_ch;
    recip_ch=1/real'(CHin);
    FP32_recip_CH_r=$shortrealtobits(recip_ch);
    real_to_FP20(FP32_recip_CH_r,FP20_recip_CH_r);
    
    Left_WT_Base_Addr=0;
    log2_WT_base_addr_Bank_Step=`log2_WT_base_addr_Bank_Step;

    LN_CH_burst_times_minus1=(1024/`LN_num_per_AXI_DW)>>`log2_AXI_BURST_LEN;
    ID_list[0]=8'b0000_0111;//LN
    ID_list[1]=8'b1100_1000;//MVM_BN_RES
    ID_list[2]=8'b0000_0101;//Softmax
    ID_list[3]=8'b0100_1000;//MVM_BN
    ID_list[4]=8'b1000_1000;//MVM_RES
    ID_list[5]=8'b0000_0001;//Elementwise
    ID_list[6]=8'b1100_1000;//MVM_BN_RES
    
    Aux_Reg_Group_Nums=1+6*2+1;
    max_tasks_num=7;//
    
    cfg_reg_array0[7*8-1:0]={ID_list[6],ID_list[5],ID_list[4],ID_list[3],ID_list[2],ID_list[1],ID_list[0]};

    cfg_reg_array1[ 0*32+32-1: 0*32]=wt_base_addr;                                                                 
    cfg_reg_array1[ 1*32+32-1: 1*32]=feature_in_base;                                                              
    cfg_reg_array1[ 2*32+32-1: 2*32]=feature_in_surface_stride;                                                    
    cfg_reg_array1[ 3*32+32-1: 3*32]=feature_in_line_stride;                                                       
    cfg_reg_array1[ 4*32+32-1: 4*32]=feature_out_base;                                                             
    cfg_reg_array1[ 5*32+32-1: 5*32]=feature_out_surface_stride;                                                   
    cfg_reg_array1[ 6*32+32-1: 6*32]=feature_out_line_stride;                                                      
    cfg_reg_array1[ 7*32+32-1: 7*32]=(1024+`Tout-1)/`Tout;                                                         
    cfg_reg_array1[ 8*32+32-1: 8*32]=1;                                                                            
    cfg_reg_array1[ 9*32+32-1: 9*32]=19;                                                                           
    cfg_reg_array1[10*32+32-1:10*32]=19;                                                                           
    cfg_reg_array1[11*32+32-1:11*32]={16'b0,1'b0,1'b0};//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r 
    cfg_reg_array1[12*32+32-1:12*32]=0;
    cfg_reg_array1[13*32+32-1:13*32]=FP20_recip_CH_r;                                                              
    cfg_reg_array1[14*32+32-1:14*32]=LN_CH_burst_times_minus1;                                                     
    cfg_reg_array1[15*32+32-1:15*32]=`Layer_Norm;
    cfg_reg_array1[16*32+32-1:16*32]=0;
    cfg_reg_array1[17*32+32-1:17*32]=0;                         
    cfg_reg_array1[18*32+32-1:18*32]=0;                         
    cfg_reg_array1[19*32+32-1:19*32]=0;                         
    cfg_reg_array1[20*32+32-1:20*32]=0;                         
    cfg_reg_array1[21*32+32-1:21*32]=0;                         
    cfg_reg_array1[22*32+32-1:22*32]=0;                         
    cfg_reg_array1[23*32+32-1:23*32]=0;                         
    cfg_reg_array1[24*32+32-1:24*32]=0;              
    cfg_reg_array1[25*32+32-1:25*32]=0;          
    cfg_reg_array1[26*32+32-1:26*32]=0;                         
    cfg_reg_array1[27*32+32-1:27*32]=0; 
    cfg_reg_array1[28*32+32-1:28*32]=0;    
    cfg_reg_array1[29*32+32-1:29*32]=0;
    cfg_reg_array1[30*32+32-1:30*32]=0;   
    cfg_reg_array1[31*32+32-1:31*32]=0;
    
    cfg_reg_array2[ 0*32+32-1: 0*32]=CHin;                 
    cfg_reg_array2[ 1*32+32-1: 1*32]=Win;                  
    cfg_reg_array2[ 2*32+32-1: 2*32]=Hin;                  
    cfg_reg_array2[ 3*32+32-1: 3*32]=Wout;                 
    cfg_reg_array2[ 4*32+32-1: 4*32]=Hout;                 
    cfg_reg_array2[ 5*32+32-1: 5*32]=CHout;                
    cfg_reg_array2[ 6*32+32-1: 6*32]=CHout_last;           
    cfg_reg_array2[ 7*32+32-1: 7*32]=`Height;
    cfg_reg_array2[ 8*32+32-1: 8*32]=feature_in_base;                                    
    cfg_reg_array2[ 9*32+32-1: 9*32]=wt_base_addr;                                       
    cfg_reg_array2[10*32+32-1:10*32]=wt_size_in_bits;                                    
    cfg_reg_array2[11*32+32-1:11*32]=feature_out_base;                                   
    cfg_reg_array2[12*32+32-1:12*32]=CHout_Split_Times_minus1;                           
    cfg_reg_array2[13*32+32-1:13*32]={Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step};    
    cfg_reg_array2[14*32+32-1:14*32]={1'b0,relu_en};                                     
    cfg_reg_array2[15*32+32-1:15*32]=(`Skip_Factor-1);                                   
    cfg_reg_array2[16*32+32-1:16*32]={1'b1,1'b1};//{out_to_BRAM,in_from_BRAM}
    cfg_reg_array2[17*32+32-1:17*32]=0;                         
    cfg_reg_array2[18*32+32-1:18*32]=0;                         
    cfg_reg_array2[19*32+32-1:19*32]=1;                         
    cfg_reg_array2[20*32+32-1:20*32]=1;                         
    cfg_reg_array2[21*32+32-1:21*32]=1;                         
    cfg_reg_array2[22*32+32-1:22*32]=1;                         
    cfg_reg_array2[23*32+32-1:23*32]=0;                         
    cfg_reg_array2[24*32+32-1:24*32]=BN_base_addr;              
    cfg_reg_array2[25*32+32-1:25*32]=ResAdd_base_addr;          
    cfg_reg_array2[26*32+32-1:26*32]=0;                         
    cfg_reg_array2[27*32+32-1:27*32]=feature_in_surface_stride; 
    cfg_reg_array2[28*32+32-1:28*32]=feature_in_line_stride;    
    cfg_reg_array2[29*32+32-1:29*32]=feature_out_surface_stride;
    cfg_reg_array2[30*32+32-1:30*32]=feature_out_line_stride;   
    cfg_reg_array2[31*32+32-1:31*32]=0;

    cfg_reg_array3[ 0*32+32-1: 0*32]=0;                 
    cfg_reg_array3[ 1*32+32-1: 1*32]=feature_in_base;                  
    cfg_reg_array3[ 2*32+32-1: 2*32]=1216;                  
    cfg_reg_array3[ 3*32+32-1: 3*32]=1216;                 
    cfg_reg_array3[ 4*32+32-1: 4*32]=feature_out_base;                 
    cfg_reg_array3[ 5*32+32-1: 5*32]=1216;                
    cfg_reg_array3[ 6*32+32-1: 6*32]=1216;           
    cfg_reg_array3[ 7*32+32-1: 7*32]=1;
    cfg_reg_array3[ 8*32+32-1: 8*32]=4;                                    
    cfg_reg_array3[ 9*32+32-1: 9*32]=20;                                       
    cfg_reg_array3[10*32+32-1:10*32]=19;                                    
    cfg_reg_array3[11*32+32-1:11*32]={1'b0,1'b0}; //reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r                                
    cfg_reg_array3[12*32+32-1:12*32]=0;                           
    cfg_reg_array3[13*32+32-1:13*32]=0;    
    cfg_reg_array3[14*32+32-1:14*32]=0;                                     
    cfg_reg_array3[15*32+32-1:15*32]=0;                                   
    cfg_reg_array3[16*32+32-1:16*32]=19;
    cfg_reg_array3[17*32+32-1:17*32]=0;                         
    cfg_reg_array3[18*32+32-1:18*32]=0;                         
    cfg_reg_array3[19*32+32-1:19*32]=0; 
    
    cfg_reg_array4[ 0*32+32-1: 0*32]=CHin;                 
    cfg_reg_array4[ 1*32+32-1: 1*32]=Win;                  
    cfg_reg_array4[ 2*32+32-1: 2*32]=Hin;                  
    cfg_reg_array4[ 3*32+32-1: 3*32]=Wout;                 
    cfg_reg_array4[ 4*32+32-1: 4*32]=Hout;                 
    cfg_reg_array4[ 5*32+32-1: 5*32]=CHout;                
    cfg_reg_array4[ 6*32+32-1: 6*32]=CHout_last;           
    cfg_reg_array4[ 7*32+32-1: 7*32]=`Height;
    cfg_reg_array4[ 8*32+32-1: 8*32]=feature_in_base;                                    
    cfg_reg_array4[ 9*32+32-1: 9*32]=wt_base_addr;                                       
    cfg_reg_array4[10*32+32-1:10*32]=wt_size_in_bits;                                    
    cfg_reg_array4[11*32+32-1:11*32]=feature_out_base;                                   
    cfg_reg_array4[12*32+32-1:12*32]=CHout_Split_Times_minus1;                           
    cfg_reg_array4[13*32+32-1:13*32]={Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step};    
    cfg_reg_array4[14*32+32-1:14*32]={1'b0,relu_en};                                     
    cfg_reg_array4[15*32+32-1:15*32]=(`Skip_Factor-1);                                   
    cfg_reg_array4[16*32+32-1:16*32]={1'b0,1'b1};//{out_to_BRAM,in_from_BRAM}
    cfg_reg_array4[17*32+32-1:17*32]=0;                         
    cfg_reg_array4[18*32+32-1:18*32]=0;                         
    cfg_reg_array4[19*32+32-1:19*32]=1;                         
    cfg_reg_array4[20*32+32-1:20*32]=1;                         
    cfg_reg_array4[21*32+32-1:21*32]=1;                         
    cfg_reg_array4[22*32+32-1:22*32]=1;                         
    cfg_reg_array4[23*32+32-1:23*32]=0;                         
    cfg_reg_array4[24*32+32-1:24*32]=BN_base_addr;              
    cfg_reg_array4[25*32+32-1:25*32]=ResAdd_base_addr;          
    cfg_reg_array4[26*32+32-1:26*32]=0;                         
    cfg_reg_array4[27*32+32-1:27*32]=feature_in_surface_stride; 
    cfg_reg_array4[28*32+32-1:28*32]=feature_in_line_stride;    
    cfg_reg_array4[29*32+32-1:29*32]=feature_out_surface_stride;
    cfg_reg_array4[30*32+32-1:30*32]=feature_out_line_stride;   
    cfg_reg_array4[31*32+32-1:31*32]=0;    
    
    cfg_reg_array5[ 0*32+32-1: 0*32]=CHin;                 
    cfg_reg_array5[ 1*32+32-1: 1*32]=Win;                  
    cfg_reg_array5[ 2*32+32-1: 2*32]=Hin;                  
    cfg_reg_array5[ 3*32+32-1: 3*32]=Wout;                 
    cfg_reg_array5[ 4*32+32-1: 4*32]=Hout;                 
    cfg_reg_array5[ 5*32+32-1: 5*32]=CHout;                
    cfg_reg_array5[ 6*32+32-1: 6*32]=CHout_last;           
    cfg_reg_array5[ 7*32+32-1: 7*32]=`Height;
    cfg_reg_array5[ 8*32+32-1: 8*32]=feature_in_base;                                    
    cfg_reg_array5[ 9*32+32-1: 9*32]=wt_base_addr;                                       
    cfg_reg_array5[10*32+32-1:10*32]=wt_size_in_bits;                                    
    cfg_reg_array5[11*32+32-1:11*32]=feature_out_base;                                   
    cfg_reg_array5[12*32+32-1:12*32]=CHout_Split_Times_minus1;                           
    cfg_reg_array5[13*32+32-1:13*32]={Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step};    
    cfg_reg_array5[14*32+32-1:14*32]={1'b0,relu_en};                                     
    cfg_reg_array5[15*32+32-1:15*32]=(`Skip_Factor-1);                                   
    cfg_reg_array5[16*32+32-1:16*32]={1'b0,1'b0};//{out_to_BRAM,in_from_BRAM}
    cfg_reg_array5[17*32+32-1:17*32]=0;                         
    cfg_reg_array5[18*32+32-1:18*32]=0;                         
    cfg_reg_array5[19*32+32-1:19*32]=1;                         
    cfg_reg_array5[20*32+32-1:20*32]=1;                         
    cfg_reg_array5[21*32+32-1:21*32]=1;                         
    cfg_reg_array5[22*32+32-1:22*32]=1;                         
    cfg_reg_array5[23*32+32-1:23*32]=0;                         
    cfg_reg_array5[24*32+32-1:24*32]=BN_base_addr;              
    cfg_reg_array5[25*32+32-1:25*32]=ResAdd_base_addr;          
    cfg_reg_array5[26*32+32-1:26*32]=0;                         
    cfg_reg_array5[27*32+32-1:27*32]=feature_in_surface_stride; 
    cfg_reg_array5[28*32+32-1:28*32]=feature_in_line_stride;    
    cfg_reg_array5[29*32+32-1:29*32]=feature_out_surface_stride;
    cfg_reg_array5[30*32+32-1:30*32]=feature_out_line_stride;   
    cfg_reg_array5[31*32+32-1:31*32]=0;    
    
    cfg_reg_array6[ 0*32+32-1: 0*32]=1;                 
    cfg_reg_array6[ 1*32+32-1: 1*32]=feature_in_base;                  
    cfg_reg_array6[ 2*32+32-1: 2*32]=feature_in_surface_stride;                  
    cfg_reg_array6[ 3*32+32-1: 3*32]=feature_in_line_stride;                 
    cfg_reg_array6[ 4*32+32-1: 4*32]=feature_out_base;                 
    cfg_reg_array6[ 5*32+32-1: 5*32]=feature_out_surface_stride;                
    cfg_reg_array6[ 6*32+32-1: 6*32]=feature_out_line_stride;           
    cfg_reg_array6[ 7*32+32-1: 7*32]=`CHout_div_Tout;
    cfg_reg_array6[ 8*32+32-1: 8*32]=Hin;                                    
    cfg_reg_array6[ 9*32+32-1: 9*32]=Win;                                       
    cfg_reg_array6[10*32+32-1:10*32]=feature_in_base;                              
    cfg_reg_array6[11*32+32-1:11*32]={1'b0,1'b0};   //{out_to_BRAM,in_from_BRAM}
    
    cfg_reg_array7[ 0*32+32-1: 0*32]=CHin;                 
    cfg_reg_array7[ 1*32+32-1: 1*32]=Win;                  
    cfg_reg_array7[ 2*32+32-1: 2*32]=Hin;                  
    cfg_reg_array7[ 3*32+32-1: 3*32]=Wout;                 
    cfg_reg_array7[ 4*32+32-1: 4*32]=Hout;                 
    cfg_reg_array7[ 5*32+32-1: 5*32]=CHout;                
    cfg_reg_array7[ 6*32+32-1: 6*32]=CHout_last;           
    cfg_reg_array7[ 7*32+32-1: 7*32]=`Height;
    cfg_reg_array7[ 8*32+32-1: 8*32]=feature_in_base;                                    
    cfg_reg_array7[ 9*32+32-1: 9*32]=wt_base_addr;                                       
    cfg_reg_array7[10*32+32-1:10*32]=wt_size_in_bits;                                    
    cfg_reg_array7[11*32+32-1:11*32]=feature_out_base;                                   
    cfg_reg_array7[12*32+32-1:12*32]=CHout_Split_Times_minus1;                           
    cfg_reg_array7[13*32+32-1:13*32]={Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step};    
    cfg_reg_array7[14*32+32-1:14*32]={1'b0,relu_en};                                     
    cfg_reg_array7[15*32+32-1:15*32]=(`Skip_Factor-1);                                   
    cfg_reg_array7[16*32+32-1:16*32]={1'b0,1'b0};//{out_to_BRAM,in_from_BRAM}
    cfg_reg_array7[17*32+32-1:17*32]=0;                         
    cfg_reg_array7[18*32+32-1:18*32]=0;                         
    cfg_reg_array7[19*32+32-1:19*32]=1;                         
    cfg_reg_array7[20*32+32-1:20*32]=1;                         
    cfg_reg_array7[21*32+32-1:21*32]=1;                         
    cfg_reg_array7[22*32+32-1:22*32]=1;                         
    cfg_reg_array7[23*32+32-1:23*32]=0;                         
    cfg_reg_array7[24*32+32-1:24*32]=BN_base_addr;              
    cfg_reg_array7[25*32+32-1:25*32]=ResAdd_base_addr;          
    cfg_reg_array7[26*32+32-1:26*32]=0;                         
    cfg_reg_array7[27*32+32-1:27*32]=feature_in_surface_stride; 
    cfg_reg_array7[28*32+32-1:28*32]=feature_in_line_stride;    
    cfg_reg_array7[29*32+32-1:29*32]=feature_out_surface_stride;
    cfg_reg_array7[30*32+32-1:30*32]=feature_out_line_stride;   
    cfg_reg_array7[31*32+32-1:31*32]=0;    
    ///////////////////////////////////////////////////////////////
    cfg_reg_array[ 0]=cfg_reg_array0;
    cfg_reg_array[ 1]=cfg_reg_array1[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[ 2]=cfg_reg_array1[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH];
    cfg_reg_array[ 3]=cfg_reg_array2[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[ 4]=cfg_reg_array2[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH];
    cfg_reg_array[ 5]=cfg_reg_array3[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[ 6]=cfg_reg_array3[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH];
    cfg_reg_array[ 7]=cfg_reg_array4[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[ 8]=cfg_reg_array4[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH]; 
    cfg_reg_array[ 9]=cfg_reg_array5[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[10]=cfg_reg_array5[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH]; 
    cfg_reg_array[11]=cfg_reg_array6[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[12]=cfg_reg_array7[1*`AXI_DAT_WIDTH-1:             0];
    cfg_reg_array[13]=cfg_reg_array7[2*`AXI_DAT_WIDTH-1:`AXI_DAT_WIDTH];    

end
endtask
