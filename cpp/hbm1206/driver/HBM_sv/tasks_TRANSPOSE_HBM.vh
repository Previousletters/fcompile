
task TRANS_Map_Weight_Tout(input bit [(`WT_DW-1):0] weight_in[1][1][`CHout][`CHin],
                    output bit [`WT_DW *`Tout-1:0]output_wt[`CHout_div_Tout*`WT_CHin_div_Tin*`Tout*`Tin_div_Tout]);
begin
  
    bit [`WT_DW-1:0]       weight_reorg[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin];
    bit [`WT_DW*`Tin-1:0] weight_Tin[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout];
    bit [`WT_DW*`Tout-1:0]  weight_Tout[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin_div_Tout];    

    integer addr;
    integer M;

    addr=0;
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
                for (int tin=0; tin<`Tin;tin=tin+1)
                begin
                    bit [`WT_DW-1:0] tp1;
                    tp1=0;
                    if ((chout*`Tout+tout < `CHout) && (chin*`Tin+tin<`CHin))
                        tp1 = weight_in[0][0][chout*`Tout+tout][chin*`Tin+tin];
                    else
                        tp1 = 0;
                    weight_reorg[chout][chin][tout][tin] = tp1;
//					$display("weight_reorg[chout%0d][chin%0d][tout%0d][tin%0d]=%0d",chout,chin,tout,tin,$signed(tp1));
                end
    
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
            begin
                bit [`WT_DW*`Tin-1:0] tp2;
                tp2 = 0;
                for (int tin=0; tin<`Tin;tin=tin+1)
                    tp2[`WT_DW*tin+:`WT_DW]=weight_reorg[chout][chin][tout][tin];
                weight_Tin[chout][chin][tout] = tp2;
//                $display("output_wt_Tin[chout%0d][chin%0d][tout%0d]=%0h",chout,chin,tout,tp2);
            end
    
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
            begin
                 bit [`WT_DW*`Tin-1:0]tp3;
                 bit [`WT_DW*`Tout-1:0] tp4;
                 tp3 = weight_Tin[chout][chin][tout];
                 tp4 = 0;
//               $display("output_wt_Tin[chout%0d][chin%0d][tout%0d]=%0d",chout,chin,tout,tp3);
                 for (int tin_out=0; tin_out<`Tin_div_Tout;tin_out=tin_out+1)
                 begin
                     weight_Tout[chout][chin][tout][tin_out] = tp3[`WT_DW *`Tout*tin_out+:`WT_DW *`Tout];
                     tp4 = weight_Tout[chout][chin][tout][tin_out];
                     output_wt[addr] = tp4;
//                   $display("\t output_wt_Tout[%0d]=%0h",addr,weight_Tout[chout][chin][tout][tin_out]);
//                   $display("\t tp4[%0d]=%0h",addr,tp4);
//                   $display("\t output_wt[%0d]=%0h",addr,output_wt[addr]);
                     addr = addr + 1;
                 end
            end	
//	$display("addr=%0d",addr);
end	
endtask	


task Run_Transpose(bit [`log2_W-1:0]Win,bit [`log2_W-1:0]Hin,bit [`log2_CH-1:0]CH_padding_with_Tin,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
			bit [`log2_CH-1:0]CHout, bit [31:0]dat_out_base_addr);
begin
	int reg_bias;      
	bit [31:0]rdata;
    bit [`log2_Matrix_pixel-1:0]pixel_in;
    bit [31:0]wt_burst_times;
    bit [`HBM_ADDR_WIDTH-32-1:0]Left_WT_Base_Addr;
    
    Left_WT_Base_Addr=0;
    pixel_in=`Height;//197
    reg_bias=192;
    wt_burst_times=`CHout_div_Tout*`WT_CHin_div_Tblock*(`MAX_DAT_DW+`T_quant_block*4)/`MAX_DAT_DW;
    $display("********************************");
	$display("reg=%d, data=%d", 3 , dat_in_base_addr);//feature_in_base_addr
	$display("reg=%d, data=%d", 4 , dat_in_surface_stride);//feature_in_surface_stride_in
	$display("reg=%d, data=%d", 6 , dat_out_base_addr);//feature_out_base_addr
	$display("reg=%d, data=%d", 9 , (CH_padding_with_Tin+`Tout-1)/`Tout);//CHin_div_Tout
    $display("reg=%d, data=%d", 11, Win);//Win
    $display("reg=%d, data=%d", 19, pixel_in);//pixel_in
    $display("reg=%d, data=%d", 23, `Width_in);//ch_out
    $display("reg=%d, data=%d", 25, wt_burst_times);//wt_burst_times
    $display("********************************");
    
	
	AXI_Lite_Write(reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in
	AXI_Lite_Write(reg_bias+9 ,(CH_padding_with_Tin+`Tout-1)/`Tout);//CHin_div_Tout
    AXI_Lite_Write(reg_bias+10,Hin);//Hin
    AXI_Lite_Write(reg_bias+11,Win);//Win
    AXI_Lite_Write(reg_bias+19,`Height);//pixel_in w dimension
    AXI_Lite_Write(reg_bias+23,`Width_in);//ch_out
    AXI_Lite_Write(reg_bias+25,wt_burst_times);//wt_burst_times
    AXI_Lite_Write(reg_bias+26,{1'b0,1'b0});//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
    
    AXI_Lite_Write(reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
    AXI_Lite_Write(reg_bias+6 ,dat_out_base_addr);//feature_out_base_addrwire 
    AXI_Lite_Write(reg_bias+7 ,`log2_WT_base_addr_Bank_Step);//log2_HBM_bank_addr_step
    AXI_Lite_Write(reg_bias+8 ,Left_WT_Base_Addr);//Left_WT_Base_Addr  
    
    //Kick of the run
    AXI_Lite_Write(reg_bias+0,32'b10);//Transpose_start
    $display("Run_TRANSPOSE start!");
        
    //Waiting for Ln_wdma done
    AXI_Lite_Read(reg_bias+1,rdata);
    while(rdata!==1) AXI_Lite_Read(reg_bias+1,rdata);
    $display("TRANSPOSE_wdma done!");
end
endtask

