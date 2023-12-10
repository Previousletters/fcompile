

task RunHBM_FC_BN_RES(input bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,bit relu_en,
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] wt_base_addr,bit [31:0]wt_size_in_bytes,bit [31:0]wt_num_div_Tin,bit[31:0] BN_base_addr,		
		bit[31:0] ResAdd_base,bit[31:0] ResAdd_surface_stride,bit[31:0] ResAdd_line_stride,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
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
    bit [31:0] CHout_Split_Times_minus1;
    bit [31:0] out_ch_slice,out_ch_slice_last;
    
    bit [31:0] BN_SURFACE_STRIDE     ;									  
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

    best_method = 1;
    out_ch_slice=(`WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(CHout_Padding%out_ch_slice!=0)
        out_ch_slice=Tout;
        
    if(out_ch_slice>=CHout_Padding)
    begin
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    end
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    CHout_Split_Times_minus1=CHout_Split_Times-1;
	
    $display("CH_out=%0d,out_ch_slice=%0d,out_ch_slice_last=%0d,CHout_Split_Times_minus1=%0d",CHout_Padding,out_ch_slice,out_ch_slice_last,CHout_Split_Times_minus1);
    $display("best_method=%0d",1);
    
	RunHBM_FC_BN_RES_single_time(CHout_Split_Times_minus1,CHin,out_ch_slice,relu_en,Tout,Tin,
							feature_in_base,
							wt_base_addr,wt_size_in_bytes/CHout_Padding*out_ch_slice*8,
							BN_base_addr,ResAdd_base,feature_out_base);
end
endtask


task RunHBM_FC_BN_RES_single_time(bit[31:0]CHout_Split_Times_minus1, bit[31:0]CHin,bit[31:0] CHout,bit relu_en,bit [31:0]Tout,bit [31:0]Tin,
					bit[31:0] feature_in_base,
					bit[31:0] wt_base_addr,bit[31:0] wt_size_in_bits,
					bit[31:0] BN_base_addr, bit[31:0] ResAdd_base,
					bit[31:0] feature_out_base);
begin

	bit [31:0]rdata;
    bit [31:0]CH_in_res_Tin_div_Tout_minus1;
	bit [31:0]BN_CH_burst_times_minus1;
    bit [31:0]CH_in_extend;
    bit [8-1:0]log2_WT_base_addr_Bank_Step;
    bit [`HBM_ADDR_WIDTH-32-1:0]Left_WT_Base_Addr;
    Left_WT_Base_Addr=0;
    log2_WT_base_addr_Bank_Step=`log2_WT_base_addr_Bank_Step;
    BN_CH_burst_times_minus1=(CHout+(`BN_num_per_AXI_DW*`AXI_BURST_LEN)-1)/(`BN_num_per_AXI_DW*`AXI_BURST_LEN)-1;
    CH_in_extend=((CHin+Tin-1)/Tin)*Tin;
//    CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    if(CHin%Tin==0)
        CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%Tin)+Tout-1)/Tout-1);

    $display("CHout=%0d",CHout);
    $display("BN_num_per_AXI_DW=%0d",`BN_num_per_AXI_DW);
    $display("BN_CH_burst_times_minus1=%0d",BN_CH_burst_times_minus1);
    $display("CH_in_extend=%0d",CH_in_extend);
    $display("CH_in_res_Tin_div_Tout_minus1=%0d",CH_in_res_Tin_div_Tout_minus1);

	//cfg Conv_path
	AXI_Lite_Write(7,(CHin+Tout-1)/Tout);
	AXI_Lite_Write(8,(CHin+Tin-1)/Tin);
	AXI_Lite_Write(31,CH_in_res_Tin_div_Tout_minus1);
	AXI_Lite_Write(19,((CHout+Tout-1)/Tout));
    
	AXI_Lite_Write(20,feature_in_base);
	AXI_Lite_Write(23,wt_size_in_bits);
	AXI_Lite_Write(24,wt_base_addr);
    AXI_Lite_Write(49, {Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step});
	
//	AXI_Lite_Write(25,dma_wt_reuse*2+dma_dat_reuse);//2'b00
    AXI_Lite_Write(47,{1'b0,1'b0});//Conv_Out_to_BRAM_mode_r<=wdata[1], Conv_In_from_BRAM_mode_r<=wdata[0]
    AXI_Lite_Write(50,CHout_Split_Times_minus1);
	
	AXI_Lite_Write(64+1,BN_CH_burst_times_minus1);
	AXI_Lite_Write(64+2,BN_base_addr);
    AXI_Lite_Write(64+6,((CHout+Tout-1)/Tout));
    AXI_Lite_Write(64+7,ResAdd_base);
    AXI_Lite_Write(64+18,relu_en);
    AXI_Lite_Write(64+20,feature_out_base);
    AXI_Lite_Write(64+23,CHout_Split_Times_minus1);     
    
    //Kick off the run
    
    //if softmax*V_mode AXI_Lite_Write(0,32'b1011_1111);//Conv Path:softmax*V_mode,QKt_mode,FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    //if       QKt_mode AXI_Lite_Write(0,32'b0111_1111);//Conv Path:softmax*V_mode,QKt_mode,FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    AXI_Lite_Write(0,  32'b11_1111);//Conv Path:FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    AXI_Lite_Write(64+0,32'b1111);//{FC_mode,ResAdd, BN, wdma}
    
	AXI_Lite_Read(64+19,rdata);
	while(rdata!==32'b1) AXI_Lite_Read(64+19,rdata);

end
endtask