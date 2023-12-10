
task RunHBM_MVM(input bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,bit relu_en,
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] wt_base_addr,bit [31:0]wt_size_in_bytes,bit [31:0]wt_num_div_Tin,
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
    bit [31:0] out_ch_slice,out_ch_slice_last;
    
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
    Hout                   = `Hout/`Skip_Factor     ;
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
    
    DAT_IN_scale           = `DAT_IN_scale          ;
    WT_scale               = `WT_scale              ;
    Conv_out_scale         = `Conv_out_scale        ;
    DAT_OUT_scale          = `DAT_OUT_scale         ;
    
	mininum_bw=0;
	overlap=0;
	dat_num_per_row=Win*WT_CHin_div_Tin;
	min_dat_depth=dat_num_per_row;
	min_wt_depth=WT_CHin_div_Tin*((`Tin*`MAX_WT_DW)/`HBM_AXI_DATA_WIDTH)*(Tout/`HBM_Port);

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
    out_ch_slice=(`WT_BRAM_DEPTH/min_wt_depth)*Tout;
    if(CHout_Padding%out_ch_slice==0)
    begin
        out_ch_slice_last=out_ch_slice;
    end
    else
    begin
        out_ch_slice=Tout;
        out_ch_slice_last=Tout;
    end
    
    if(out_ch_slice>=CHout_Padding)
    begin
        out_ch_slice=CHout_Padding;
        CHout_Split_Times=1;
    end
    else 
        CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;
        
    Hout_first=`DAT_BRAM_DEPTH/dat_num_per_row;
    in_height_first=Hout_first;
    Hout_middle=`DAT_BRAM_DEPTH/dat_num_per_row;
    in_height_middle=Hout_middle;

    if(Hout_first>=Hout)
    begin
        Hout_first=Hout;
        in_height_first=Hin;
    end

    if((Hout-Hout_first)%Hout_middle == 0)
    begin
        Hout_Split_Times=(Hout-Hout_first)/Hout_middle+1;
        Hout_last=Hout_middle;
    end
    else
    begin
        Hout_Split_Times=(Hout-Hout_first)/Hout_middle+2;
        Hout_last=(Hout-Hout_first)%Hout_middle;
    end
    in_height_last=Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap);
    
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

    $display("CH_out=%0d,out_ch_slice=%0d,out_ch_slice_last=%0d,CHout_Split_Times=%0d",CHout_Padding,out_ch_slice,out_ch_slice_last,CHout_Split_Times);
    $display("in_width=%0d,Wout=%0d",Win,Wout);
    $display("in_height=%0d,Hout=%0d,Hout_Split_Times=%0d",Hin,Hout,Hout_Split_Times);
    $display("in_height_first=%0d,in_height_middle=%0d,in_height_last=%0d",in_height_first,in_height_middle,in_height_last);
    $display("Hout_first=%0d,Hout_middle=%0d,Hout_last=%0d",Hout_first,Hout_middle,Hout_last);
    $display("best_method=%0d",best_method);
    
    if(best_method==0)// reuse wt,  total_bytes_if_reuse_wt < total_bytes_if_reuse_dat
    begin
        for(integer n=0;n<CHout_Split_Times;n=n+1)
            for(integer k=0;k<Hout_Split_Times;k=k+1)
            begin
                int current_repeat_times;
                int in_height_single,Hout_single;
                int line_offset_in,line_offset_out;
                int CH_in_single;//
                int CH_out_single;
                int pad_y_single;
                bit dma_wt_reuse_single;//dma_dat_reuse_single=0
            
                current_repeat_times=n*Hout_Split_Times+k;
                CH_in_single=CHin;//                        

                if(n!=CHout_Split_Times-1)
                    CH_out_single=out_ch_slice;
                else
                    CH_out_single=out_ch_slice_last;

                if(k==0)
                    begin line_offset_in=0;line_offset_out=0;pad_y_single=0;dma_wt_reuse_single=0;end
                else
                    begin 
                        line_offset_in=(in_height_first-overlap)+(k-1)*(in_height_middle-overlap);
                        line_offset_out=Hout_first+(k-1)*Hout_middle;
                        pad_y_single=0;
                        dma_wt_reuse_single=1;
                    end

                if(k==0)
                    begin in_height_single=in_height_first;Hout_single=Hout_first;end
                else
                    if(k==Hout_Split_Times-1)
                        begin in_height_single=in_height_last;Hout_single=Hout_last;end
                    else
                        begin in_height_single=in_height_middle;Hout_single=Hout_middle;end

                //if(m==0)
                RunHBM_MVM_single_time(current_repeat_times,CH_in_single,in_height_single,Win,CH_out_single,relu_en,Tout,Tin,
                        feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,
                        wt_base_addr+wt_size_in_bytes/`HBM_Port/CHout_Padding*out_ch_slice*n,wt_size_in_bytes/CHout_Padding*CH_out_single*8,wt_num_div_Tin/CHout_Padding*CH_out_single,
                        feature_out_base+feature_out_line_stride*line_offset_out+feature_out_surface_stride*n*(out_ch_slice/Tout),feature_out_surface_stride,feature_out_line_stride,
                        Wout,Hout_single,best_single_dat_banks,0,dma_wt_reuse_single);
            end
    end
    else// reuse dat,  total_bytes_if_reuse_wt > total_bytes_if_reuse_dat
    begin
        for(integer k=0;k<Hout_Split_Times;k=k+1)
            for(integer n=0;n<CHout_Split_Times;n=n+1)
            begin
                integer in_height_single,Hout_single;
                integer line_offset_in,line_offset_out;

                integer CH_in_single;//
                integer CH_out_single;
                integer wt_size_single;
                integer pad_y_single;
                bit dma_dat_reuse_single;//dma_wt_reuse_single=0
                int current_repeat_times;
                
                current_repeat_times=n*Hout_Split_Times+k;
                CH_in_single=CHin;//

                if(n!=CHout_Split_Times-1)
                    CH_out_single=out_ch_slice;
                else
                    CH_out_single=out_ch_slice_last;

                if(n==0)
                    dma_dat_reuse_single=0;
                else
                    dma_dat_reuse_single=1;

                if(k==0)
                    begin line_offset_in=0;line_offset_out=0;pad_y_single=0;end
                else
                    begin 
                        line_offset_in=(in_height_first-overlap)+(k-1)*(in_height_middle-overlap);
                        line_offset_out=Hout_first+(k-1)*Hout_middle;
                        pad_y_single=0;
                    end

                if(k==0)
                    begin in_height_single=in_height_first;Hout_single=Hout_first;end
                else
                    if(k==Hout_Split_Times-1)
                        begin in_height_single=in_height_last;Hout_single=Hout_last;end
                    else
                        begin in_height_single=in_height_middle;Hout_single=Hout_middle;end

                // if(m==0)
                RunHBM_MVM_single_time(current_repeat_times,CH_in_single,in_height_single,Win,CH_out_single,relu_en,Tout,Tin,
                        feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,
                        wt_base_addr+wt_size_in_bytes/`HBM_Port/CHout_Padding*out_ch_slice*n,wt_size_in_bytes/CHout_Padding*CH_out_single*8,wt_num_div_Tin/CHout_Padding*CH_out_single,
                        feature_out_base+feature_out_line_stride*line_offset_out+feature_out_surface_stride*n*(out_ch_slice/Tout),feature_out_surface_stride,feature_out_line_stride,
                        Wout,Hout_single,best_single_dat_banks,dma_dat_reuse_single,0);
            end
        end
    end
endtask


task RunHBM_MVM_single_time(bit[31:0]current_repeat_times,
        bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,bit relu_en,bit [31:0]Tout,bit [31:0]Tin,
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] wt_base_addr,bit[31:0] wt_size_in_bits,bit[31:0] wt_num_div_Tin,  
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,
		bit[31:0] Wout,bit[31:0]Hout,bit[31:0]single_dat_buf_num,bit dma_dat_reuse,bit dma_wt_reuse);
begin

	bit [31:0]rdata;
    bit [31:0]CH_in_res_Tin_div_Tout_minus1;
    bit [31:0]CH_in_extend;
    bit [8-1:0]log2_WT_base_addr_Bank_Step;
    bit [`HBM_ADDR_WIDTH-32-1:0]Left_WT_Base_Addr;
    Left_WT_Base_Addr=0;
    log2_WT_base_addr_Bank_Step=`log2_WT_base_addr_Bank_Step;

    CH_in_extend=((CHin+Tin-1)/Tin)*Tin;
//    CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    if(CHin%Tin==0)
        CH_in_res_Tin_div_Tout_minus1=Tin/Tout-1;
    else
        CH_in_res_Tin_div_Tout_minus1=(((CHin%Tin)+Tout-1)/Tout-1);

    $display("CH_in_extend=%0d",CH_in_extend);
    $display("CH_in_res_Tin_div_Tout_minus1=%0d",CH_in_res_Tin_div_Tout_minus1);

	//cfg Conv_path
	AXI_Lite_Write(3,single_dat_buf_num);
	AXI_Lite_Write(4,Win);
	AXI_Lite_Write(5,Hin);
	AXI_Lite_Write(6,Win*Hin);
	AXI_Lite_Write(7,(CHin+Tout-1)/Tout);
	AXI_Lite_Write(8,(CHin+Tin-1)/Tin);
	AXI_Lite_Write(31,CH_in_res_Tin_div_Tout_minus1);
	AXI_Lite_Write(15,Wout);
	AXI_Lite_Write(16,Wout*Hout);
	AXI_Lite_Write(17,Hout);
	AXI_Lite_Write(18,CHout);
	AXI_Lite_Write(19,((CHout+Tout-1)/Tout));
	AXI_Lite_Write(33,((Wout*Hout+Tout-1)/Tout));
    
	AXI_Lite_Write(20,feature_in_base);
	AXI_Lite_Write(21,feature_in_surface_stride);
	AXI_Lite_Write(22,feature_in_line_stride);
	AXI_Lite_Write(23,wt_size_in_bits);
	AXI_Lite_Write(32,wt_num_div_Tin);
	AXI_Lite_Write(24,wt_base_addr);
    AXI_Lite_Write(49, {Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step});
	AXI_Lite_Write(25,dma_wt_reuse*2+dma_dat_reuse);
    AXI_Lite_Write(47,{1'b0,1'b0});//Conv_Out_to_BRAM_mode_r<=wdata[1], Conv_In_from_BRAM_mode_r<=wdata[0]
    AXI_Lite_Write(51,(`Skip_Factor-1));

	AXI_Lite_Write(64+4,Hout);
	AXI_Lite_Write(64+5,Wout);
    AXI_Lite_Write(64+6,((CHout+Tout-1)/Tout));
    AXI_Lite_Write(64+12,Wout*Hout);    //effect_out_pixel
    AXI_Lite_Write(64+18,{1'b0,relu_en});
    AXI_Lite_Write(64+20,feature_out_base);
    AXI_Lite_Write(64+21,feature_out_surface_stride);
    AXI_Lite_Write(64+22,feature_out_line_stride);        
    
    //Kick off the run
    //if softmax*V_mode AXI_Lite_Write(0,32'b1001_1111);//Conv Path:softmax*V_mode,QKt_mode,FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    //if       QKt_mode AXI_Lite_Write(0,32'b0101_1111);//Conv Path:softmax*V_mode,QKt_mode,FC_mode,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat    
    AXI_Lite_Write(0,32'b01_1111);//Conv Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat
    AXI_Lite_Write(64+0,32'b0001);//{ResAdd, BN, wdma}
    
	AXI_Lite_Read(64+19,rdata);
	while(rdata!==32'b1) AXI_Lite_Read(64+19,rdata);

end
endtask

