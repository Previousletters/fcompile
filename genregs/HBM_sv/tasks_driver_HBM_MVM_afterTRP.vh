
task General_Map_Wt_TRP_Data(input bit [(`DAT_DW_L0-1):0] wt_in[`CHout][`CHin],
                                output bit [`MAX_DAT_DW*`Tout-1:0]mem[`CHout*`CHin_div_Tout]);
begin
    bit [31:0] cnt;
    
    cnt=0;
    for(int j=0;j<`CHout;j++)
    begin
        for(int k=0;k<`CHin;k=k+`Tout)
        begin
            bit [`MAX_DAT_DW*`Tout-1:0]tp;
            tp=0;
            for(int kk=k;kk<k+`Tout;kk++)
            begin
                if(kk<`CHin) 
                   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(wt_in[j][kk]);
                else
                   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=0;
            end
            mem[`CHout*(k/`Tout)+j]=tp;
          //$display("mem[%0d]=%h",`CHout*(k/`Tout)+j,tp);
          cnt += 1; 
        end
    end
end
endtask


task TRP_RunMVMFP16_soft(input real real_feature_in[`Feature_Head][`Hin][`Win][`CHin],
                     input bit[`DAT_DW_L0-1:0] feature_in[`Feature_Head][`Hin][`Win][`CHin],
                     input real Wt[`Weight_Head][`CHout][`CHin],
                     output bit[`MAX_BN_DW-1:0] feature_out[`Feature_Head][`Hout][`Wout][`CHout],
                     output real real_fp16_feature_out[`Feature_Head][`Hout][`Wout][`CHout],
                     output real real_fp32_feature_out[`Feature_Head][`Hout][`Wout][`CHout]);
begin

    for(int i=0;i<`Weight_Head;i++)
        for(int b=0;b<`Feature_Head/`Weight_Head;b++)
            for(int chout=0;chout<`CHout;chout++)
            if(chout <`Token)
                for(int hout=0;hout<`Hout;hout++)
                begin
                    for(int wout=0;wout<`Token;wout++)
                    begin
                        real real_tp_out_fp32,real_tp_out_fp16;
                        real_tp_out_fp32=0;real_tp_out_fp16=0;
                        for(int chin=0;chin<`CHin;chin++)
                        begin 
                            bit[(`DAT_DW_L0-1):0] data;
                            real real_data,real_wt;
                            int hin,win;
                            hin=hout;
                            win=wout;
                            if( (hin<0) || (hin>=`Hin) || (win<0) || (win>=`Win) )//padding 0
                                data=0;
                            else
                                data=feature_in[b*`Weight_Head+i][hin][win][chin];
                                
                            if( (`KV_cache_mode==1 & wout!=`Token-1) || (`KV_cache_mode==0 & wout>`Token) )
                                begin end
                            else
                            begin
                                FP16_to_real(data,real_data);
                                real_wt=Wt[i][chout][chin];
                                real_tp_out_fp32=real_tp_out_fp32 + real'(real_data)*real'(real_wt);
//                                $display("real_data=%f, real_wt=%f, real_tp_out_fp32=%f",real_data,real_wt,real_tp_out_fp32);
                            end
                        end
                        
                        if( (`KV_cache_mode==1 & wout!=`Token-1) || (`KV_cache_mode==0 & wout>`Token) )
                        begin
                            real_tp_out_fp32=real_feature_in[b*`Weight_Head+i][hout][wout][chout];
//                            $display("real_tp_out_fp32=%f",real_tp_out_fp32);
                        end
                        else
                            real_tp_out_fp32=real_tp_out_fp32*real'(1/$sqrt(`CHin));
                            
                        real_to_FP16(real_tp_out_fp32,feature_out[b*`Weight_Head+i][hout][wout][chout]);
                        FP16_to_real(feature_out[b*`Weight_Head+i][hout][wout][chout],real_tp_out_fp16);
                        real_fp16_feature_out[b*`Weight_Head+i][hout][wout][chout]=real_tp_out_fp16;
                        real_fp32_feature_out[b*`Weight_Head+i][hout][wout][chout]=real_tp_out_fp32;
    //                  $display("feature_out[head%0d][hout%0d][wout%0d][chout%0d]=%0b",b*`Weight_Head+i,hout,wout,chout,$signed(feature_out[b][hout][wout][chout]));
    //                  $display("fp32_out=%f, fp16_out=%f",real_tp_out_fp32,real_tp_out_fp16);
                    end
                end
end
endtask


task RunHBM_MVM_afterTRP(
        bit[31:0] feature_in_base,bit[31:0] feature_in_head_stride,bit[31:0] feature_in_line_stride,
        bit[31:0] wt_base_addr,
        bit[31:0] feature_out_base,bit[31:0] feature_out_head_stride,bit[31:0] feature_out_line_stride);
begin
    bit [31:0]rdata,feature_in_base_in_KVMode,feature_out_base_in_KVMode;    
    int Feature_Repeat_times_minus1, reg_bias;
    bit [31:0]FP32_rsqrt,FP20_rsqrt,FP16_rsqrt;
    real recip_ch;
    recip_ch=real'((1/$sqrt(`CHin)));
    real_to_FP16(recip_ch,FP16_rsqrt);
    $display("recip_ch=%f, FP16_rsqrt=%h",recip_ch, FP16_rsqrt);
    
    feature_in_base_in_KVMode=feature_in_base+(`Token-1)*`Pixel_Data_Bytes;
    feature_out_base_in_KVMode=feature_out_base+(`Token-1)*`Pixel_Data_Bytes;
    
    reg_bias=192; 
    Feature_Repeat_times_minus1=`Feature_Head/`MIN_WT_HEAD-1;
    
    if(`KV_cache_mode==1)
    begin //KV cache
        AXI_Lite_Write(reg_bias+2 , wt_base_addr               );
        AXI_Lite_Write(reg_bias+3 , feature_in_base_in_KVMode  );//feature_in_base_addr
        AXI_Lite_Write(reg_bias+4 , feature_in_head_stride     );//feature_in_head_stride_in
        AXI_Lite_Write(reg_bias+5 , feature_in_line_stride     );//feature_in_line_stride_in
        AXI_Lite_Write(reg_bias+6 , feature_out_base_in_KVMode );//feature_out_base_addr
        AXI_Lite_Write(reg_bias+7 , feature_out_head_stride    );//feature_out_head_stride
        AXI_Lite_Write(reg_bias+8 , feature_out_line_stride    );//feature_out_line_stride
        AXI_Lite_Write(reg_bias+9 , (`CHin+`Tout-1)/`Tout      );//CHin_div_Tout
        AXI_Lite_Write(reg_bias+10, `Token                     );//TRP_Weight_Win                 
        AXI_Lite_Write(reg_bias+11, `Weight_Head               );//TRP_Weight_Head                
        AXI_Lite_Write(reg_bias+12, 1                          );//TRP_Feature_Win                
        AXI_Lite_Write(reg_bias+13, `Feature_Head              );//TRP_Feature_Head               
        AXI_Lite_Write(reg_bias+14, Feature_Repeat_times_minus1);//TRP_Feature_Repeat_times_minus1
        AXI_Lite_Write(reg_bias+15, FP16_rsqrt                 );
        AXI_Lite_Write(reg_bias+16, {1'b0,1'b0}                );//reg Matrix_Out_to_BRAM, Matrix_In_from_BRAM
        AXI_Lite_Write(reg_bias+17, 6'b00_0010                 );//Transpose_start
    end
    else 
    begin
        AXI_Lite_Write(reg_bias+2 , wt_base_addr               );
        AXI_Lite_Write(reg_bias+3 , feature_in_base            );//feature_in_base_addr
        AXI_Lite_Write(reg_bias+4 , feature_in_head_stride     );//feature_in_head_stride_in
        AXI_Lite_Write(reg_bias+5 , feature_in_line_stride     );//feature_in_line_stride_in
        AXI_Lite_Write(reg_bias+6 , feature_out_base           );//feature_out_base_addr
        AXI_Lite_Write(reg_bias+7 , feature_out_head_stride    );//feature_out_head_stride
        AXI_Lite_Write(reg_bias+8 , feature_out_line_stride    );//feature_out_line_stride
        AXI_Lite_Write(reg_bias+9 , (`CHin+`Tout-1)/`Tout      );//CHin_div_Tout
        AXI_Lite_Write(reg_bias+10, `Token                     );//TRP_Weight_Win                 
        AXI_Lite_Write(reg_bias+11, `Weight_Head               );//TRP_Weight_Head                
        AXI_Lite_Write(reg_bias+12, `Token                     );//TRP_Feature_Win                
        AXI_Lite_Write(reg_bias+13, `Feature_Head              );//TRP_Feature_Head               
        AXI_Lite_Write(reg_bias+14, Feature_Repeat_times_minus1);//TRP_Feature_Repeat_times_minus1
        AXI_Lite_Write(reg_bias+15, FP16_rsqrt                 );
        AXI_Lite_Write(reg_bias+16, {1'b0,1'b0}                );//reg Matrix_Out_to_BRAM, Matrix_In_from_BRAM
        AXI_Lite_Write(reg_bias+17, 6'b00_0010                 );//Transpose_start
    end
        
        AXI_Lite_Read(reg_bias+1,rdata);
        while(rdata!==32'b1) AXI_Lite_Read(reg_bias+1,rdata);

    
end
endtask
