
task Softmax_Map_Feature_Data(input bit[31:0]Height, input bit[31:0]Width, input bit[31:0]CH, 
                               input bit [(`DAT_DW_L0-1):0] in[`Hin][`Win][`CHin],
                              output bit[`MAX_DAT_DW*`Tout-1:0]mem[`Win*`Hin*`CHin_div_Tout]);
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
                       tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=16'hfbff;
                end
                mem[Height*Width*(k/`Tout)+i*Width+j]=tp;
//                $display("mem[%0d]=%h",cnt,mem[cnt]);
//                cnt += 1; 
            end
        end
end
endtask


task Softmax_task(input bit[31:0] no_mask_len, input real x[`Token], input bit[31:0]CH, output real y[`Token] );
begin
    automatic real in_max;
    automatic real exp[`Token];
    automatic real exp_sum = 0;

    if (CH<`MAX_TOKEN)
        for(int cnt = no_mask_len;cnt<`MAX_TOKEN;cnt++)begin
            x[cnt]=-65504;//16'b1_11110_1111111111;
        end
        
    for(int cnt = 0;cnt<`Token;cnt++)begin
        if(cnt==0)
            in_max = x[0];
        else
            if(in_max<x[cnt])
                in_max = x[cnt];
    end
//    $display("in_max=%f",in_max);
    
    for(int cnt = 0;cnt<`Token;cnt++)begin
        x[cnt] = x[cnt] - in_max;
        exp[cnt] = $exp(x[cnt]);
        exp_sum = exp_sum + exp[cnt];
    end
    
    for(int cnt = 0;cnt<`Token;cnt++)begin
        y[cnt] = exp[cnt]/exp_sum;
//        $display("cnt=%d,x=%f, exp(x)[%d]=%f, exp_sum=%f, out[%d]=%f",cnt,x[cnt],cnt,exp[cnt],exp_sum,cnt,y[cnt]);
    end
end
endtask


task Run_Softmax_Software(
                    input real real_feature_in [`Feature_Head][`Hin][`Win][`CHin], 
                    output bit[`MAX_BN_DW-1:0] feature_out[`Feature_Head][`Hout][`Wout][`CHout_Padding],
                    output real real_fp16_software_out [`Feature_Head][`Hout][`Wout][`CHout_Padding],
                    output real real_fp32_software_out [`Feature_Head][`Hout][`Wout][`CHout_Padding]);
begin
    bit [31:0]CH,no_mask_len;
    real softmax_in[`Token],softmax_out[`Token];
    
//    for(int b=0;b<`Feature_Head;b++)
//        for(int i=0;i<`Hout;i++)
//            for(int j=0;j<`Wout;j++)
//                for(int k=`Token;k<`CHout_Padding;k++)
//                begin
//                    real_fp32_software_out[b][i][j][k]=real_feature_in[b][i][j][k];
//                    real_fp16_software_out[b][i][j][k]=real_feature_in[b][i][j][k];
//                end
                
    for(int b=0;b<`Feature_Head;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                if(j<`Height)
                begin
                    for(int k=0;k<`Token;k++)
                    begin
                        softmax_in[k]=real_feature_in[b][i][j][k];
                    end
                    no_mask_len=`Need_Mask==1?j+1:`Token;
                    Softmax_task(no_mask_len, softmax_in,`Token,softmax_out);
                    
                    for(int k=0;k<`Token;k++)
                    begin
                        real_to_FP16(softmax_out[k],feature_out[b][i][j][k]);
                        FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                        real_fp32_software_out[b][i][j][k]=softmax_out[k];
//                        $display("softmax_in[%d]=%f,softmax_out[%d]=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",
//                                    k,softmax_in[k],k,softmax_out[k],b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                        $display("fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    end
                end

end
endtask


task Run_Softmax_Hardware(
        bit[31:0] feature_in_base,bit[31:0] feature_in_head_stride,bit[31:0] feature_in_line_stride,
        bit[31:0] feature_out_base,bit[31:0] feature_out_head_stride,bit[31:0] feature_out_line_stride);
begin
    
    int Softmax_reg_bias;
    bit [31:0]rdata,w_in,in_base_addr_in_KVMode,out_base_addr_in_KVMode;    
    
    Softmax_reg_bias=192;
    in_base_addr_in_KVMode=feature_in_base+(`Token-1)*`Pixel_Data_Bytes;
    out_base_addr_in_KVMode=feature_out_base+(`Token-1)*`Pixel_Data_Bytes;
    //w_in=(`Token+`AXI_BURST_LEN_SOFTMAX-1)/`AXI_BURST_LEN_SOFTMAX*`AXI_BURST_LEN_SOFTMAX;
    w_in=`Token;

    if(`KV_cache_mode==1)
    begin //KV cache
        AXI_Lite_Write(Softmax_reg_bias+2 ,`Need_Mask             );
        AXI_Lite_Write(Softmax_reg_bias+3 ,in_base_addr_in_KVMode );//feature_in_base_addr
        AXI_Lite_Write(Softmax_reg_bias+4 ,feature_in_head_stride );//feature_in_surface_stride_in
        AXI_Lite_Write(Softmax_reg_bias+5 ,feature_in_line_stride );//feature_in_line_stride_in
        AXI_Lite_Write(Softmax_reg_bias+6 ,out_base_addr_in_KVMode);//feature_out_base_addr
        AXI_Lite_Write(Softmax_reg_bias+7 ,feature_out_head_stride);//feature_out_surface_stride_in
        AXI_Lite_Write(Softmax_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
        AXI_Lite_Write(Softmax_reg_bias+9 ,(`Token+`Tout-1)/`Tout );//CHin_div_Tout
        AXI_Lite_Write(Softmax_reg_bias+10,`Feature_Head          );//Feature_Head
        AXI_Lite_Write(Softmax_reg_bias+11,`Token                 );//Token
        AXI_Lite_Write(Softmax_reg_bias+12,`AXI_BURST_LEN_SOFTMAX );//w_in
        AXI_Lite_Write(Softmax_reg_bias+13,1                      );//effect_pixel
        AXI_Lite_Write(Softmax_reg_bias+14,0                      );
        AXI_Lite_Write(Softmax_reg_bias+15,0                      );
        AXI_Lite_Write(Softmax_reg_bias+16,0                      );//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
        AXI_Lite_Write(Softmax_reg_bias+17,6'b00_1000             );//Softmax_start
    end
    else
    begin
        AXI_Lite_Write(Softmax_reg_bias+2 ,`Need_Mask             );
        AXI_Lite_Write(Softmax_reg_bias+3 ,feature_in_base        );//feature_in_base_addr
        AXI_Lite_Write(Softmax_reg_bias+4 ,feature_in_head_stride );//feature_in_surface_stride_in
        AXI_Lite_Write(Softmax_reg_bias+5 ,feature_in_line_stride );//feature_in_line_stride_in
        AXI_Lite_Write(Softmax_reg_bias+6 ,feature_out_base       );//feature_out_base_addr
        AXI_Lite_Write(Softmax_reg_bias+7 ,feature_out_head_stride);//feature_out_surface_stride_in
        AXI_Lite_Write(Softmax_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
        AXI_Lite_Write(Softmax_reg_bias+9 ,(`Token+`Tout-1)/`Tout );//CHin_div_Tout
        AXI_Lite_Write(Softmax_reg_bias+10,`Feature_Head          );//Feature_Head
        AXI_Lite_Write(Softmax_reg_bias+11,`Token                 );//Token
        AXI_Lite_Write(Softmax_reg_bias+12,w_in                   );//w_in
        AXI_Lite_Write(Softmax_reg_bias+13,`Token                 );//effect_pixel
        AXI_Lite_Write(Softmax_reg_bias+14,0                      );
        AXI_Lite_Write(Softmax_reg_bias+15,0                      );
        AXI_Lite_Write(Softmax_reg_bias+16,0                      );//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
        AXI_Lite_Write(Softmax_reg_bias+17,6'b00_1000             );//Softmax_start
    end
    $display("Run Softmax start!");
    
    //Waiting for Softmax_wdma done
    AXI_Lite_Read(Softmax_reg_bias+1,rdata);
    while(rdata!==1) 
        AXI_Lite_Read(Softmax_reg_bias+1,rdata);
    $display("Softmax_wdma done!");
    
end
endtask
