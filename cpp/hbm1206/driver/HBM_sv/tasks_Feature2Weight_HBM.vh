
task Feature2Weight_Map_Feature_Data(input bit[31:0]Height, input bit[31:0]Width, input bit[31:0]CH, 
                               input bit [(`DAT_DW_L0-1):0] in[`Hin][`Win][`CHout],
                              output bit[`MAX_DAT_DW*`Tout-1:0]mem[`Win*`Hin*`CHout_div_Tout]);
begin
    bit [31:0] cnt;
//    $display("Height=%0d, Width=%0d, CH=%0d",Height,Width,CH);
    cnt=0;
    for(int i=0;i<Height;i++)
        for(int j=0;j<Width;j++)
        begin
            for(int k=0;k<CH;k=k+`Tout)
            begin
                bit [`MAX_DAT_DW *`Tout-1:0]tp;
                tp=0;
                for(int kk=k;kk<k+`Tout;kk++)
                begin
                    if(kk<CH) 
                       tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[i][j][kk]);
                    else
                       tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=0;
                end
                mem[Height*Width*(k/`Tout)+i*Width+j]=tp;
//                $display("mem[%0d]=%h",cnt,mem[cnt]);
//                cnt += 1; 
            end
        end
end
endtask


task Run_Feature2Weight(bit[`log2_W-1:0]pixel_in, bit [`log2_CH-1:0]CHout_div_Tout,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride, bit [31:0]wt_out_base_addr);
begin
	int reg_bias;      
	bit [31:0]rdata,in_base_addr;
    bit [`HBM_ADDR_WIDTH-32-1:0]Left_WT_Base_Addr;
    
    Left_WT_Base_Addr=0;
    reg_bias=192;
	$display("dat_in_base_addr=%h",dat_in_base_addr);
	
    AXI_Lite_Write(reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
	AXI_Lite_Write(reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in    
	AXI_Lite_Write(reg_bias+6 ,wt_out_base_addr);//wt_out_base_addrwire 
    AXI_Lite_Write(reg_bias+7 ,`log2_WT_base_addr_Bank_Step);//log2_HBM_bank_addr_step
    AXI_Lite_Write(reg_bias+8 ,Left_WT_Base_Addr);//Left_WT_Base_Addr  
	AXI_Lite_Write(reg_bias+19,pixel_in);//Total CHin
    AXI_Lite_Write(reg_bias+20,CHout_div_Tout);//CHout_div_Tout
    AXI_Lite_Write(reg_bias+26,{1'b0,1'b0});//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
    
    //Kick of the run
    AXI_Lite_Write(reg_bias+0,32'b1000_0000);//Feature2Weight_start
    $display("Run_Feature2Weight start!");
        
    //Waiting for Ln_wdma done
    AXI_Lite_Read(reg_bias+1,rdata);
    while(rdata!==1) AXI_Lite_Read(reg_bias+1,rdata);
    $display("Feature2Weight_wdma done!");
end
endtask

