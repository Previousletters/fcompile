


task Run_TRANSPOSE(bit [`log2_W-1:0]Win,bit [`log2_W-1:0]Hin,bit [`log2_CH-1:0]CHin,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
			bit [`log2_CH-1:0]CHout, bit [31:0]dat_out_base_addr);
begin
	int reg_bias;      
	bit [31:0]rdata;
    bit [`Log2_Trans_pixel-1:0]pixel_in;
    bit [`Log2_Trans_pixel-1:0]pixel_out;
    bit [31:0]wt_num_div_Tout;
    
    pixel_in=Win;//197
    pixel_out=CHin;//192 or 64
    reg_bias=128;
    wt_num_div_Tout=pixel_in*((CHin+`Tout-1)/`Tout);
    
    $display("********************************");
	$display("reg=%d, data=%d", 3 , dat_in_base_addr);//feature_in_base_addr
	$display("reg=%d, data=%d", 4 , dat_in_surface_stride);//feature_in_surface_stride_in
	$display("reg=%d, data=%d", 6 , dat_out_base_addr);//feature_out_base_addr
	$display("reg=%d, data=%d", 9 , (CHin+`Tout-1)/`Tout);//ch_in_div_Tout
    $display("reg=%d, data=%d", 11, Win);//Win
    $display("reg=%d, data=%d", 19, pixel_in);//pixel_in
    $display("reg=%d, data=%d", 23, pixel_out);//pixel_out
    $display("reg=%d, data=%d", 25, wt_num_div_Tout);//wt_num_div_Tout
    $display("********************************");
    
	CSB_Write(reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
	CSB_Write(reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in
	CSB_Write(reg_bias+6 ,dat_out_base_addr);//feature_out_base_addr
	CSB_Write(reg_bias+9 ,(CHin+`Tout-1)/`Tout);//ch_in_div_Tout
    CSB_Write(reg_bias+11,Win);//Win
    CSB_Write(reg_bias+19,pixel_in);//pixel_in
    CSB_Write(reg_bias+23,pixel_out);//pixel_out
    CSB_Write(reg_bias+25,wt_num_div_Tout);//wt_num_div_Tout
    CSB_Write(reg_bias+26,{1'b0,1'b0});//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
    
	//Kick of the run
	CSB_Write(reg_bias+0,32'b10);//Ln_start
    $display("Run_TRANSPOSE start!");
        
	//Waiting for Ln_wdma done
	CSB_Read(reg_bias+1,rdata);
	while(rdata!==1) CSB_Read(reg_bias+1,rdata);
	$display("TRANSPOSE_wdma done!");
end
endtask

