
task General_Map_Res_Add_Data(input bit[31:0]Height, input bit[31:0]Width, input bit[31:0]CH, 
                               input bit [(`DAT_DW_L0-1):0] in[`Hout][`Wout][`CHout],
                              output bit[`MAX_DAT_DW*`Tout-1:0]mem[`Wout*`Hout*`CHout_div_Tout]);
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
                       tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=0;
                end
                mem[Height*Width*(k/`Tout)+i*Width+j]=tp;
//                $display("mem=%0d",mem[cnt]);
//                cnt += 1; 
            end
        end
end
endtask


task Run_BN_soft(input real feature_in[`Tb][`Hout][`Wout][`CHout],
        real bn_weight[`CHout_Padding],real bn_bias[`CHout_Padding],
        output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout],
        output real fp16_feature_out[`Tb][`Hout][`Wout][`CHout],
        output real fp32_feature_out[`Tb][`Hout][`Wout][`CHout]);
begin

    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                begin
                    real feature;
                    real BN_wt;
                    real BN_bias;
                    feature=feature_in[b][i][j][k];
                    BN_wt=bn_weight[k];
                    BN_bias=bn_bias[k];
                    
                    fp32_feature_out[b][i][j][k]=feature*BN_wt+BN_bias;
                    real_to_FP16(fp32_feature_out[b][i][j][k],feature_out[b][i][j][k]);
                    FP16_to_real(feature_out[b][i][j][k],fp16_feature_out[b][i][j][k]);
//                    $display("feature_out[b%d][h%d][w%d][ch%d]=%d",b,i,j,k,$signed(feature_out[b][i][j][k]));
                end
end
endtask


task Run_Res_soft(input bit ew_mode, input real feature_in[`Tb][`Hout][`Wout][`CHout],
                real Res_Add[`Tb][`Hout][`Wout][`CHout], 
                output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout],
                output real fp16_feature_out[`Tb][`Hout][`Wout][`CHout],
                output real fp32_feature_out[`Tb][`Hout][`Wout][`CHout]);
begin

    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                begin
                    real main_dat, res_dat;
                    main_dat=feature_in[b][i][j][k];
                    res_dat=Res_Add[b][i][j][k];
                    
                    if(ew_mode==0)
                        fp32_feature_out[b][i][j][k]=main_dat+res_dat;
                    if(ew_mode==1)
                        fp32_feature_out[b][i][j][k]=main_dat*res_dat;
                        
                    real_to_FP16(fp32_feature_out[b][i][j][k],feature_out[b][i][j][k]);
                    FP16_to_real(feature_out[b][i][j][k],fp16_feature_out[b][i][j][k]);
//                    $display("feature_out[b%d][h%d][w%d][ch%d]=%d",b,i,j,k,$signed(feature_out[b][i][j][k]));
                end
end
endtask


task Run_Dat_Clip_soft(input real feature_in[`Tb][`Hout][`Wout][`CHout],input bit Conv_relu,
                      output real feature_out[`Tb][`Hout][`Wout][`CHout]);
begin
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                begin
                    feature_out[b][i][j][k]=(Conv_relu&(feature_in[b][i][j][k]<0))?0:feature_in[b][i][j][k];
                end
end
endtask
    