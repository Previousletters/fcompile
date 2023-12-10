`define length197 197


task gelu_task(input real x, output real y);
begin
    automatic real x_t;
    automatic real tanh_o;
    automatic real tanh_i;
    automatic real pi = 3.1415926535897932;
    automatic real cst_1;
    automatic real cst_2 = 0.044715;
    cst_1 = $sqrt(2/pi);
    
    x_t = x;
    tanh_i = cst_1 * (x_t + cst_2 * $pow(x_t,3));
    tanh_o= $tanh(tanh_i);
    y  = 0.5 * x_t * (1 + tanh_o);
end
endtask


task Run_Activation_Software(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0], 
                    output bit [`DAT_DW_L1-1:0] Out_Feature [`Hout_L0][`Wout_L0][`CHout_L0] );
begin
    bit [`DAT_DW_L0-1:0]  In[`CHin_L0][`length197];
    real In_real[`CHin_L0][`length197];
    bit [`DAT_DW_L1-1:0] Out[`CHin_L0][`length197];
    real Out_real[`CHout_L0][`length197];

    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
            for(int k=0;k<`CHin_L0;k++)
            begin
                if(i*`Win_L0+j<`length197)
                begin
                    In[k][i*`Win_L0+j]=In_Feature[i][j][k];
//                    $display("**In[CH%d][%d]=%d",k,i*`Win_L0+j,$signed(In_Feature[i][j][k]));
                    In_real[k][i*`Win_L0+j]=$itor($signed(In_Feature[i][j][k]))   /  $itor(1<<`in_scale);
//                    $display("In_real[CH%d][%d]=%.8f",k,i*`Win_L0+j,In_real[k][i*`Win_L0+j]);

                    gelu_task(In_real[k][i*`Win_L0+j], Out_real[k][i*`Win_L0+j]);
                    
                    Out_Feature[i][j][k]=$itor(Out_real[k][i*`Win_L0+j]) * $itor(1<<`out_scale);
//                    $display("**Out_real[CH%d][%d]=%.5f",k,i*`Win_L0+j,Out_real[k][i*`Win_L0+j]); 
//                    $display("Out_Feature[H%d][W%d][CH%d]=%d",i,j,k,$signed(Out_Feature[i][j][k])); 

                end
            end
            
end
endtask


task Run_Activation_Hardware(
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
begin
    
	int activation_reg_bias;      
    bit [31:0]rdata;
    bit [31:0]pixel_in;
    pixel_in=`Height;
    activation_reg_bias=128;
    
    CSB_Write(activation_reg_bias+3 ,feature_in_base);//feature_in_base_addr
    CSB_Write(activation_reg_bias+4 ,feature_in_surface_stride);//feature_in_surface_stride_in
    CSB_Write(activation_reg_bias+5 ,feature_in_line_stride);//feature_in_line_stride_in
    CSB_Write(activation_reg_bias+6 ,feature_out_base);//feature_out_base_addr
    CSB_Write(activation_reg_bias+7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    CSB_Write(activation_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
    CSB_Write(activation_reg_bias+9 ,`slice_of_CHout_L0);//ch_in_div_Tout
    
    CSB_Write(activation_reg_bias+11,`Win_L0);//Win
    CSB_Write(activation_reg_bias+13,`in_scale);//Hin
    CSB_Write(activation_reg_bias+16,`out_scale);//Win    
    
    CSB_Write(activation_reg_bias+19,pixel_in);//pixel_in
    //Kick of the run
    CSB_Write(activation_reg_bias+0,32'b10000);//activation_start
    $display("Run activation start!");
    
    //Waiting for activation_wdma done
    CSB_Read(activation_reg_bias+1,rdata);
    while(rdata!==1) 
        CSB_Read(activation_reg_bias+1,rdata);
    $display("activation_wdma done!");
    
end
endtask
