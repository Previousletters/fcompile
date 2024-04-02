
task Run_Activation_Soft(input real In_Feature [`Tb][`Hin][`Win][`CHin], 
                         input real wt[16], input real bias[16], input real x_region[16],
                        output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout_Padding],
                        output real real_fp16_software_out [`Tb][`Hout][`Wout][`CHout_Padding],
                        output real real_fp32_software_out [`Tb][`Hout][`Wout][`CHout_Padding]);
begin
    bit [31:0]CH;
    real x,y;
    
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                if(i*`Wout+j<`Height)
                begin
                    for(int k=0;k<`CHout_Padding;k++)
                    begin
                        x=In_Feature[b][i][j][k];
                        if     (x<x_region[ 0]) y=wt[ 0]*x+bias[ 0];
                        else if(x<x_region[ 1]) y=wt[ 1]*x+bias[ 1];
                        else if(x<x_region[ 2]) y=wt[ 2]*x+bias[ 2];
                        else if(x<x_region[ 3]) y=wt[ 3]*x+bias[ 3];  
                        else if(x<x_region[ 4]) y=wt[ 4]*x+bias[ 4];
                        else if(x<x_region[ 5]) y=wt[ 5]*x+bias[ 5];
                        else if(x<x_region[ 6]) y=wt[ 6]*x+bias[ 6];
                        else if(x<x_region[ 7]) y=wt[ 7]*x+bias[ 7];  
                        else if(x<x_region[ 8]) y=wt[ 8]*x+bias[ 8];
                        else if(x<x_region[ 9]) y=wt[ 9]*x+bias[ 9];
                        else if(x<x_region[10]) y=wt[10]*x+bias[10];
                        else if(x<x_region[11]) y=wt[11]*x+bias[11];  
                        else if(x<x_region[12]) y=wt[12]*x+bias[12];
                        else if(x<x_region[13]) y=wt[13]*x+bias[13];
                        else if(x<x_region[14]) y=wt[14]*x+bias[14];
                        else                    y=wt[15]*x+bias[15]; 

                        real_to_FP16(y,feature_out[b][i][j][k]);
                        FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                        real_fp32_software_out[b][i][j][k]=y;
//                        $display("softmax_out[%d]=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",
//                                    k,softmax_out[k],b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                        $display("fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    end
                end
end
endtask



task Run_Activation_Hardware(
        bit[31:0] parameters_base,
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
        bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
begin
    
    int activation_reg_bias;      
    bit [31:0]rdata;
    bit [31:0]pixel_in;
    bit [5:0]parameter_addr;
    pixel_in=`Height;
    activation_reg_bias=192;
    
    AXI_Lite_Write(activation_reg_bias+2 ,parameters_base           );//parameters_in_base_addr
    AXI_Lite_Write(activation_reg_bias+3 ,feature_in_base           );//feature_in_base_addr
    AXI_Lite_Write(activation_reg_bias+4 ,feature_in_surface_stride );//feature_in_surface_stride_in
    AXI_Lite_Write(activation_reg_bias+5 ,feature_in_line_stride    );//feature_in_line_stride_in
    AXI_Lite_Write(activation_reg_bias+6 ,feature_out_base          );//feature_out_base_addr
    AXI_Lite_Write(activation_reg_bias+7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    AXI_Lite_Write(activation_reg_bias+8 ,feature_out_line_stride   );//feature_out_line_stride_in
    AXI_Lite_Write(activation_reg_bias+9 ,`CHout_div_Tout           );//CHin_div_Tout
    AXI_Lite_Write(activation_reg_bias+10,`Hin                      );//Hin
    AXI_Lite_Write(activation_reg_bias+11,`Win                      );//Win
    AXI_Lite_Write(activation_reg_bias+12,pixel_in                  );//pixel_in
    AXI_Lite_Write(activation_reg_bias+13,`CHout_div_Tout           );//CHin_div_Tout
    AXI_Lite_Write(activation_reg_bias+14,`CHout                    );//CHout_div_Tout
    AXI_Lite_Write(activation_reg_bias+15,0                         );
    AXI_Lite_Write(activation_reg_bias+16,{1'b0,1'b0}               );//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
    AXI_Lite_Write(activation_reg_bias+17,6'b01_0000                );//activation_start
    $display("Run activation start!");
    
    //Waiting for activation_wdma done
    AXI_Lite_Read(activation_reg_bias+1,rdata);
    while(rdata!==1) 
        AXI_Lite_Read(activation_reg_bias+1,rdata);
    $display("activation_wdma done!");
    
end
endtask
