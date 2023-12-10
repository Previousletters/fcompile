`define pixel `Height
task Softmax_task(input real x[`CHin_L0-1:0], output real y[`CHin_L0-1:0] );
begin
    automatic int cnt = 0;
    automatic real in_max = 0;
    automatic real exp[`CHin_L0];
    automatic real exp_sum = 0;
    
    for(cnt = 0;cnt<`CHin_L0;cnt++)begin
        if(in_max<x[cnt])
            in_max = x[cnt];
    end
    
    for(cnt = 0;cnt<`CHin_L0;cnt++)begin
        x[cnt] = x[cnt] - in_max;
        exp[cnt] = $exp(x[cnt]);
        exp_sum = exp_sum + exp[cnt];
    end
    
    for(cnt = 0;cnt<`CHin_L0;cnt++)begin
        y[cnt] = exp[cnt]/exp_sum;
    end
end
endtask

task Run_Softmax_Software(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0], 
                    output bit [`DAT_DW_L1-1:0] Out_Feature [`Hout_L0][`Wout_L0][`CHout_L0] );
begin
    bit [`DAT_DW_L0-1:0]  In[`pixel][`CHin_L0];
    real  In_real[`pixel][`CHin_L0];
    real Out_real[`pixel][`CHout_L0];
    
    
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
            for(int k=0;k<`CHin_L0;k++)
            begin
                if(i*`Win_L0+j<`pixel)
                begin
                    In[i*`Win_L0+j][k]=In_Feature[i][j][k];
//                    $display("**In[CH%d][%d]=%d",k,i*`Win_L0+j,$signed(In_Feature[i][j][k]));
                    In_real[i*`Win_L0+j][k]=$itor($signed(In_Feature[i][j][k]))   /  $itor(1<<`in_scale);
//                    $display("In_real[CH%d][%d]=%.8f",k,i*`Win_L0+j,In_real[k][i*`Win_L0+j]);
                end
            end

    for(int k=0;k<`Hin_L0*`Win_L0;k++)
    begin
        Softmax_task(In_real[k],Out_real[k]);
    end
    
    for(int i=0;i<`Hout_L0;i++)
        for(int j=0;j<`Wout_L0;j++)
            for(int k=0;k<`CHout_L0;k++)
            begin
                if(i*`Win_L0+j<`pixel)
                begin
                    Out_Feature[i][j][k]=$itor(Out_real[i*`Win_L0+j][k]) * $itor(1<<`out_scale);
//                    $display("**Out_real[CH%d][%d]=%.5f",k,i*`Win_L0+j,Out_real[k][i*`Win_L0+j]); 
//                    $display("Out_Feature[H%d][W%d][CH%d]=%d",i,j,k,$signed(Out_Feature[i][j][k])); 
                end
            end
end
endtask


task Run_Softmax_Hardware(
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
begin
    
	int Softmax_reg_bias;      
    bit [31:0]rdata;
    bit [31:0]pixel_in;
    bit [31:0]pixel_out;
    pixel_in=`Height;
    pixel_out=`Width_in;
    Softmax_reg_bias=128;
    
    $display("********************************");
    $display("reg=%d, data=%d", 3 ,feature_in_base);//feature_in_base_addr
    $display("reg=%d, data=%d", 4 ,feature_in_surface_stride);//feature_in_surface_stride_in
    $display("reg=%d, data=%d", 5 ,feature_in_line_stride);//feature_in_line_stride_in
    $display("reg=%d, data=%d", 6 ,feature_out_base);//feature_out_base_addr
    $display("reg=%d, data=%d", 7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    $display("reg=%d, data=%d", 8 ,feature_out_line_stride);//feature_out_line_stride_in
    $display("reg=%d, data=%d", 9 ,`slice_of_CHout_L0);//ch_in_div_Tout
    $display("reg=%d, data=%d", 10,`Hin_L0);//Hin
    $display("reg=%d, data=%d", 11,`Win_L0);//Win
    $display("reg=%d, data=%d", 13,`in_scale);//in_scale
    $display("reg=%d, data=%d", 16,`out_scale);//out_scale
    $display("reg=%d, data=%d", 19,pixel_in);//pixel_in   
    $display("********************************");    
    
    CSB_Write(Softmax_reg_bias+3 ,feature_in_base);//feature_in_base_addr
    CSB_Write(Softmax_reg_bias+4 ,feature_in_surface_stride);//feature_in_surface_stride_in
    CSB_Write(Softmax_reg_bias+5 ,feature_in_line_stride);//feature_in_line_stride_in
    CSB_Write(Softmax_reg_bias+6 ,feature_out_base);//feature_out_base_addr
    CSB_Write(Softmax_reg_bias+7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    CSB_Write(Softmax_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
    CSB_Write(Softmax_reg_bias+9 ,`slice_of_CHout_L0);//ch_in_div_Tout
    CSB_Write(Softmax_reg_bias+10,`Hin_L0);//Hin
    CSB_Write(Softmax_reg_bias+11,`Win_L0);//Win

    CSB_Write(Softmax_reg_bias+13,`in_scale);//Hin
    CSB_Write(Softmax_reg_bias+16,`out_scale);//Win    
    
    CSB_Write(Softmax_reg_bias+19,pixel_in);//pixel_in
    CSB_Write(Softmax_reg_bias+23,pixel_out);//pixel_out
    
    //Kick of the run
    CSB_Write(Softmax_reg_bias+0,32'b1000);//Softmax_start
    $display("Run Softmax start!");
    
    //Waiting for Softmax_wdma done
    CSB_Read(Softmax_reg_bias+1,rdata);
    while(rdata!==1) 
        CSB_Read(Softmax_reg_bias+1,rdata);
    $display("Softmax_wdma done!");
    
end
endtask
