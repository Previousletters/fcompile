
task Run_CONCAT(bit [`log2_W-1:0]Win,bit [`log2_H-1:0]Hin,bit [`log2_CH-1:0]CHin,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
			bit [31:0]clstoken_base_addr,bit [`log2_W-1:0]Wout,bit [`log2_H-1:0]Hout,bit [`log2_CH-1:0]CHout,
			bit [31:0]dat_out_base_addr,bit [31:0]dat_out_surface_stride,bit [31:0]dat_out_line_stride);
begin
	int Concat_reg_bias;      
	bit [31:0]rdata;
    Concat_reg_bias=128;

    $display("********************************");
    $display("reg=%d, data=%d", 3 ,dat_in_base_addr);//feature_in_base_addr
    $display("reg=%d, data=%d", 4 ,dat_in_surface_stride);//feature_in_surface_stride_in
    $display("reg=%d, data=%d", 5 ,dat_in_line_stride);//feature_in_line_stride_in
    $display("reg=%d, data=%d", 6 ,dat_out_base_addr);//feature_out_base_addr
    $display("reg=%d, data=%d", 7 ,dat_out_surface_stride);//feature_out_surface_stride_in
    $display("reg=%d, data=%d", 8 ,dat_out_line_stride);//feature_out_line_stride_in
    $display("reg=%d, data=%d", 9 ,(CHin+`Tout-1)/`Tout);//ch_in_div_Tout
    $display("reg=%d, data=%d", 11,Win);//Win
    $display("reg=%d, data=%d", 24,clstoken_base_addr);//gama and beta
    $display("********************************");
    
	CSB_Write(Concat_reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
	CSB_Write(Concat_reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in
	CSB_Write(Concat_reg_bias+5 ,dat_in_line_stride);//feature_in_line_stride_in
	CSB_Write(Concat_reg_bias+6 ,dat_out_base_addr);//feature_out_base_addr
	CSB_Write(Concat_reg_bias+7 ,dat_out_surface_stride);//feature_out_surface_stride_in
	CSB_Write(Concat_reg_bias+8 ,dat_out_line_stride);//feature_out_line_stride_in
	CSB_Write(Concat_reg_bias+9 ,(CHin+`Tout-1)/`Tout);//ch_in_div_Tout
    CSB_Write(Concat_reg_bias+11,Win);//Win
    CSB_Write(Concat_reg_bias+24,clstoken_base_addr);//gama and beta
    
	//Kick of the run
	CSB_Write(Concat_reg_bias+0,3'b100);//Ln_start
    $display("Run Concat start!");
    
	//Waiting for Ln_wdma done
	CSB_Read(Concat_reg_bias+1,rdata);
	while(rdata!==1) CSB_Read(Concat_reg_bias+1,rdata);
	$display("Concat_wdma done!");
end
endtask