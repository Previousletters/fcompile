
task Run_LN_Soft(input real In_Feature [`Tb][`Hin][`Win][`CHin],
                input real k_factor[`CHin],input real bias[`CHin],
                output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout],
                output real real_fp16_software_out [`Tb][`Hout][`Wout][`CHout],
                output real real_fp32_software_out [`Tb][`Hout][`Wout][`CHout]);
begin
    real mean,variance,recip_std,tp_out;
    real tp_dat,tp_sum,tp_dat_square_sum,mean_square;
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
            begin
                tp_sum=0;tp_dat_square_sum=0;
                mean=0;mean_square=0;variance=0;recip_std=0;
                for(int k=0;k<`CHin;k++)
                begin
                    tp_sum=tp_sum+In_Feature[b][i][j][k];
                    tp_dat_square_sum=tp_dat_square_sum+(In_Feature[b][i][j][k])*(In_Feature[b][i][j][k]);
                end
        
                mean=tp_sum/`CHin;
                mean_square=mean*mean;
                variance=tp_dat_square_sum/`CHin-mean_square;
                recip_std=1/($sqrt(variance+0.000001));
                $display("***[batch%d][H%d][W%d],tp_sum=%f, mean=%f, mean_square=%f,tp_dat_square_sum=%f, variance=%f, recip_std=%f",b,i,j,tp_sum,mean,mean_square,tp_dat_square_sum,variance,recip_std);
                
                for(int k=0;k<`CHin;k++)
                begin
                    tp_dat=In_Feature[b][i][j][k];
                    
                    if(`Layer_Norm == 1)
                        tp_out=(tp_dat-mean)*recip_std*k_factor[k]+bias[k];
                    else
                        tp_out=(tp_dat-0)   *recip_std*k_factor[k]+bias[k];
                        
                    real_to_FP16(tp_out,feature_out[b][i][j][k]);
                    FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    real_fp32_software_out[b][i][j][k]=tp_out;
//                    $display("***[batch%d][H%d][W%d][CH%d], mean=%f,recip_std=%f, in=%f",b,i,j,k,mean,recip_std,tp_dat);
//                    $display("LN_out[%d]=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",k,tp_out,b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                    $display("fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                end
            end
end
endtask


task Run_LN(bit [31:0]Win,bit [31:0]Hin,bit [31:0]CHin,bit [31:0]wt_base_addr,
            bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
            bit [31:0]dat_out_base_addr,bit [31:0]dat_out_surface_stride,bit [31:0]dat_out_line_stride);
begin
    int Ln_reg_bias;
    bit [31:0]rdata;
    bit [31:0]pixel_in;
    bit [31:0]ch_out;    
    bit [31:0]LN_CH_burst_times_minus1;
    bit [31:0]FP32_recip_CH_r,FP20_recip_CH_r,FP16_recip_CH_r;
    real recip_ch;
    recip_ch=1/real'(CHin);
    FP32_recip_CH_r=$shortrealtobits(recip_ch);
    real_to_FP20(FP32_recip_CH_r,FP20_recip_CH_r);
//    $display("FP20_recip_CH_r=%b", FP20_recip_CH_r);
    LN_CH_burst_times_minus1=(CHin/`LN_num_per_AXI_DW)>>`log2_AXI_BURST_LEN;
    pixel_in=`Height;
    ch_out=`Width_in;
    Ln_reg_bias=192;
    
    AXI_Lite_Write(Ln_reg_bias+2 , wt_base_addr            );//gama and beta
    AXI_Lite_Write(Ln_reg_bias+3 , dat_in_base_addr        );//feature_in_base_addr
    AXI_Lite_Write(Ln_reg_bias+4 , dat_in_surface_stride   );//feature_in_surface_stride_in
    AXI_Lite_Write(Ln_reg_bias+5 , dat_in_line_stride      );//feature_in_line_stride_in
    AXI_Lite_Write(Ln_reg_bias+6 , dat_out_base_addr       );//feature_out_base_addr
    AXI_Lite_Write(Ln_reg_bias+7 , dat_out_surface_stride  );//feature_out_surface_stride_in
    AXI_Lite_Write(Ln_reg_bias+8 , dat_out_line_stride     );//feature_out_line_stride_in
    AXI_Lite_Write(Ln_reg_bias+9 , (CHin+`Tout-1)/`Tout    );//CHin_div_Tout
    AXI_Lite_Write(Ln_reg_bias+10, Hin                     );//Hin
    AXI_Lite_Write(Ln_reg_bias+11, Win                     );//Win
    AXI_Lite_Write(Ln_reg_bias+12, pixel_in                );//pixel_in
    AXI_Lite_Write(Ln_reg_bias+13, FP20_recip_CH_r         );
    AXI_Lite_Write(Ln_reg_bias+14, LN_CH_burst_times_minus1);
    AXI_Lite_Write(Ln_reg_bias+15,              `Layer_Norm);
    AXI_Lite_Write(Ln_reg_bias+16,              {1'b0,1'b0});//reg Matrix_Out_to_BRAM, Matrix_In_from_BRAM
    AXI_Lite_Write(Ln_reg_bias+17,               6'b10_0000);//LN_start
    
    AXI_Lite_Read(Ln_reg_bias+1,rdata);
    while(rdata!==1) AXI_Lite_Read(Ln_reg_bias+1,rdata);
    
    $display("LN wdma done done!");

end
endtask

