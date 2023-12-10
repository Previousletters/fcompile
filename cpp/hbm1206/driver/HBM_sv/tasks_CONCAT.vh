
task Map_Clstoken_Data(input bit [(`DAT_DW_L0-1):0] in[1][1][`CHin_Padding_with_Tout],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[((`CHin_Padding_with_Tout+`Tout-1)/`Tout)]);
begin
    for(int k=0;k<`CHin_Padding_with_Tout;k=k+`Tout)
    begin
        bit [`MAX_DAT_DW *`Tout-1:0]tp;
        tp=0;
        for(int kk=k;kk<k+`Tout;kk++)
        begin
            if(kk<`CHin_Padding_with_Tout) 
               tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[0][0][kk]);
        end
        mem[(k/`Tout)]=tp;
    end
end
endtask


task Run_CONCAT(bit [`log2_W-1:0]Win,bit [`log2_H-1:0]Hin,bit [`log2_CH-1:0]CHin,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
			bit [31:0]clstoken_base_addr,bit [`log2_W-1:0]Wout,bit [`log2_H-1:0]Hout,bit [`log2_CH-1:0]CHout,
			bit [31:0]dat_out_base_addr,bit [31:0]dat_out_surface_stride,bit [31:0]dat_out_line_stride);
begin
	int Concat_reg_bias;      
	bit [31:0]rdata;
    Concat_reg_bias=192;

    $display("********************************");
    $display("reg=%d, data=%d", 3 ,dat_in_base_addr);//feature_in_base_addr
    $display("reg=%d, data=%d", 4 ,dat_in_surface_stride);//feature_in_surface_stride_in
    $display("reg=%d, data=%d", 5 ,dat_in_line_stride);//feature_in_line_stride_in
    $display("reg=%d, data=%d", 6 ,dat_out_base_addr);//feature_out_base_addr
    $display("reg=%d, data=%d", 7 ,dat_out_surface_stride);//feature_out_surface_stride_in
    $display("reg=%d, data=%d", 8 ,dat_out_line_stride);//feature_out_line_stride_in
    $display("reg=%d, data=%d", 9 ,(CHin+`Tout-1)/`Tout);//CHin_div_Tout
    $display("reg=%d, data=%d", 11,Win);//Win
    $display("reg=%d, data=%d", 24,clstoken_base_addr);//gama and beta
    $display("********************************");
    
	AXI_Lite_Write(Concat_reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
	AXI_Lite_Write(Concat_reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in
	AXI_Lite_Write(Concat_reg_bias+5 ,dat_in_line_stride);//feature_in_line_stride_in
	AXI_Lite_Write(Concat_reg_bias+6 ,dat_out_base_addr);//feature_out_base_addr
	AXI_Lite_Write(Concat_reg_bias+7 ,dat_out_surface_stride);//feature_out_surface_stride_in
	AXI_Lite_Write(Concat_reg_bias+8 ,dat_out_line_stride);//feature_out_line_stride_in
	AXI_Lite_Write(Concat_reg_bias+9 ,(CHin+`Tout-1)/`Tout);//CHin_div_Tout
    AXI_Lite_Write(Concat_reg_bias+11,Win);//Win
    AXI_Lite_Write(Concat_reg_bias+24,clstoken_base_addr);//gama and beta
    AXI_Lite_Write(Concat_reg_bias+26,{1'b0,1'b0});//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
    
	//Kick of the run
	AXI_Lite_Write(Concat_reg_bias+0,6'b00_0100);//Concat_start
    $display("Run Concat start!");
    
	//Waiting for Ln_wdma done
	AXI_Lite_Read(Concat_reg_bias+1,rdata);
	while(rdata!==1) AXI_Lite_Read(Concat_reg_bias+1,rdata);
	$display("Concat_wdma done!");
end
endtask