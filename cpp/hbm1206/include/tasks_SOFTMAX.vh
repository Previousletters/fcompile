
task Softmax_task(input real x[`CHout_Padding], input bit[31:0]CH, output real y[`CHout_Padding] );
begin
    automatic real in_max;
    automatic real exp[`CHout_Padding];
    automatic real exp_sum = 0;

    if (CH<`CHout_Padding)
        for(int cnt = CH;cnt<`CHout_Padding;cnt++)begin
            x[cnt]=-65504;//16'b1_11110_1111111111;
        end
        
    for(int cnt = 0;cnt<`CHout_Padding;cnt++)begin
        if(cnt==0)
            in_max = x[0];
        else
            if(in_max<x[cnt])
                in_max = x[cnt];
    end
//    $display("in_max=%f",in_max);
    
    for(int cnt = 0;cnt<`CHout_Padding;cnt++)begin
        x[cnt] = x[cnt] - in_max;
        exp[cnt] = $exp(x[cnt]);
        exp_sum = exp_sum + exp[cnt];
    end
    
    for(int cnt = 0;cnt<`CHout_Padding;cnt++)begin
        y[cnt] = exp[cnt]/exp_sum;
//        $display("cnt=%d,x=%f, exp(x)[%d]=%f, exp_sum=%f, out[%d]=%f",cnt,x[cnt],cnt,exp[cnt],exp_sum,cnt,y[cnt]);
    end
end
endtask


task Run_Softmax_Software(input real In_Feature [`Tb][`Hin][`Win][`CHin], 
                    output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout_Padding],
                    output real real_fp16_software_out [`Tb][`Hout][`Wout][`CHout_Padding],
                    output real real_fp32_software_out [`Tb][`Hout][`Wout][`CHout_Padding]);
begin
	bit [31:0]CH;
    real softmax_in[`CHout_Padding],softmax_out[`CHout_Padding];
	CH = `CHin;
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                if(j<`Height)
                begin
                    for(int k=0;k<`CHout_Padding;k++)
                    begin
                        softmax_in[k]=In_Feature[b][i][j][k];
                    end
                    
                    Softmax_task(softmax_in,CH,softmax_out);
                    
                    for(int k=0;k<`CHout_Padding;k++)
                    begin
                        real_to_FP16(softmax_out[k],feature_out[b][i][j][k]);
                        FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                        real_fp32_software_out[b][i][j][k]=softmax_out[k];
//                        $display("softmax_out[%d]=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",
//                                    k,softmax_out[k],b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                        $display("fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    end
                end
end
endtask


task Run_Softmax_Hardware(bit [31:0]Head,bit [31:0]Win,bit [31:0]CHout_div_Tout,bit [31:0]effect_w,bit [31:0]effect_ch,
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
begin
    
	int Softmax_reg_bias;      
    bit [31:0]rdata;
    Softmax_reg_bias=192;

    AXI_Lite_Write(Softmax_reg_bias+3 ,feature_in_base);//feature_in_base_addr
    AXI_Lite_Write(Softmax_reg_bias+4 ,feature_in_surface_stride);//feature_in_surface_stride_in
    AXI_Lite_Write(Softmax_reg_bias+5 ,feature_in_line_stride);//feature_in_line_stride_in
    AXI_Lite_Write(Softmax_reg_bias+6 ,feature_out_base);//feature_out_base_addr
    AXI_Lite_Write(Softmax_reg_bias+7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    AXI_Lite_Write(Softmax_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
    AXI_Lite_Write(Softmax_reg_bias+9 ,CHout_div_Tout);//CHin_div_Tout
    AXI_Lite_Write(Softmax_reg_bias+10,Head);//Hin
    AXI_Lite_Write(Softmax_reg_bias+11,Win);//Win
    
    AXI_Lite_Write(Softmax_reg_bias+19,effect_w);//pixel_in
    AXI_Lite_Write(Softmax_reg_bias+23,effect_ch);//ch_out
    
    //Kick of the run
    AXI_Lite_Write(Softmax_reg_bias+0,32'b1000);//Softmax_start
    $display("Run Softmax start!");
    
    //Waiting for Softmax_wdma done
    AXI_Lite_Read(Softmax_reg_bias+1,rdata);
    while(rdata!==1) 
        AXI_Lite_Read(Softmax_reg_bias+1,rdata);
    $display("Softmax_wdma done!");
    
end
endtask
