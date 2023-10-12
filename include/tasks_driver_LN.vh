

task LN_MEAN_Soft(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0],
                   output bit [`DAT_DW_L0-1:0] mean[`Hin_L0][`Win_L0],
                   output bit [`DAT_DW_L0*2-1:0] mean_square[`Hin_L0][`Win_L0]);
begin   
    bit [`DAT_DW_L0+`Log2_LN_ch_max-1:0] tp;
    bit [`DAT_DW_L0*2+`Log2_LN_ch_max-1:0] tp_mean;
    bit [(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:0]tp_dat_square;
    bit [(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:0]round_dat_square;
    bit [`MAX_DAT_DW*2-1:0]tp_dat_square_out;
    bit [`DAT_DW_L0*2:0] recip_CH;
    recip_CH={1'b1,{(`DAT_DW_L0*2){1'b0}}}/(`CHin_L0);
    
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
        begin
            tp=0;
            for(int k=0;k<`CHin_L0;k++)
            begin
                tp=$signed(tp)+$signed(In_Feature[i][j][k]);
//                $display("Feature=%d, tp=%d",$signed(In_Feature[i][j][k]),$signed(tp));
            end
//            $display("tp=%d",$signed(tp));
            tp_mean=$signed(tp)*$signed({1'b0,recip_CH});
//            $display("tp_mean=%d",$signed(tp_mean));


            tp_dat_square=$signed(tp_mean)*$signed(tp_mean);
            round_dat_square=tp_dat_square[(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1:`MAX_DAT_DW*4] + tp_dat_square[(`MAX_DAT_DW*4-1)];
            
            if( (!round_dat_square[(`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-1]) & (|round_dat_square[((`MAX_DAT_DW*2+`Log2_LN_ch_max)*2-2):(`MAX_DAT_DW*4-1)]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
                tp_dat_square_out={1'b0,{(`MAX_DAT_DW*2-1){1'b1}}};//`MAX_DW'h7fff;
            else
                tp_dat_square_out=round_dat_square[(`MAX_DAT_DW*2-1):0];   

            mean[i][j]=tp_mean[(`DAT_DW_L0*2+`Log2_LN_ch_max-1):`DAT_DW_L0*2]+tp_mean[(`DAT_DW_L0*2-1)];
            $display("mean[h%d][w%d]=%d",i,j,$signed(mean[i][j]));

            mean_square[i][j]=tp_dat_square_out;

        end
end
endtask


//task LN_VAR_Soft(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0],
//                  input bit [`DAT_DW_L0-1:0] MEAN[`Hin_L0][`Win_L0],
//                  output bit [`DAT_DW_L0*2-1:0] Variance[`Hin_L0][`Win_L0]);
//begin   

//    bit [`DAT_DW_L0*2:0] recip_CH;
//    recip_CH={1'b1,{(`DAT_DW_L0*2){1'b0}}}/(`CHin_L0);
////    $display("recip_CH=%d",$signed(recip_CH));
    
//    for(int i=0;i<`Hin_L0;i++)
//        for(int j=0;j<`Win_L0;j++)
//        begin
//            bit [`DAT_DW_L0-1:0] difference;
//            bit [`DAT_DW_L0*2+`Log2_LN_ch_max-1:0] tp;
//            bit [`DAT_DW_L0*3+`Log2_LN_ch_max-1:0] tp_variance;
//            tp=0;
            
//            for(int k=0;k<`CHin_L0;k++)
//            begin
//                difference=$signed(In_Feature[i][j][k]) - $signed(MEAN[i][j]);
//                tp=$signed(tp)+$signed(difference)*$signed(difference);
                                        
////                $display("Feature=%d, MEAN[i][j]=%d, difference=%d, tp=%d",
////                            $signed(In_Feature[i][j][k]),$signed(MEAN[i][j]),$signed(difference),$signed(tp));
//            end
////            $display("tp=%d",$signed(tp));
//            tp_variance=$signed(tp)*$signed({1'b0,recip_CH});
////            $display("tp_variance=%d",$signed(tp_variance));
//            Variance[i][j]=tp_variance[`DAT_DW_L0*3+`Log2_LN_ch_max-1:`DAT_DW_L0*2]+tp_variance[(`DAT_DW_L0*2-1)];
//            $display("Variance[h%d][w%d]=%d",i,j,$signed(Variance[i][j]));

//        end
//end
//endtask

task LN_VAR_Soft2(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0],
                  input bit [`DAT_DW_L0*2-1:0] mean_square[`Hin_L0][`Win_L0],
                  output bit [`DAT_DW_L0*2-1:0] Variance[`Hin_L0][`Win_L0]);
begin   

    bit [`DAT_DW_L0*2:0] recip_CH;
    recip_CH={1'b1,{(`DAT_DW_L0*2){1'b0}}}/(`CHin_L0);
//    $display("recip_CH=%d",$signed(recip_CH));
    
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
        begin
            bit [`DAT_DW_L0-1:0] difference;
            bit [`DAT_DW_L0*2+`Log2_LN_ch_max-1:0] tp;
            bit [`DAT_DW_L0*3+`Log2_LN_ch_max-1:0] tp_variance;
            bit [`DAT_DW_L0*2                -1:0] tp_variance2;
            tp=0;
 
            for(int k=0;k<`CHin_L0;k++)
            begin
                tp=$signed(tp) + $signed(In_Feature[i][j][k]) * $signed(In_Feature[i][j][k]);
            end
//            $display("**tp=%d",$signed(tp));
            tp_variance=$signed(tp)*$signed({1'b0,recip_CH});
            tp_variance2=tp_variance[`DAT_DW_L0*3+`Log2_LN_ch_max-1:`DAT_DW_L0*2]+tp_variance[(`DAT_DW_L0*2-1)];
//            $display("65536/tp_variance=%d",$signed(tp_variance));
//            $display("tp_variance=%d",$signed(tp_variance2));
            
            Variance[i][j]=$signed(tp_variance2)-$signed(mean_square[i][j]);
            $display("Variance[h%d][w%d]=%d",i,j,$signed(Variance[i][j]));

        end
end
endtask


task Run_LN_Soft(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0],
                    input bit [`DAT_DW_L0-1:0] bias[`CHin_L0], input bit [`log2_S-1:0] bias_scale,
                    input bit [`DAT_DW_L0-1:0] k_factor[`CHin_L0], input bit [`log2_S-1:0] k_factor_scale, 
                    input bit [`log2_S-1:0] out_scale,
                    output bit [`DAT_DW_L1-1:0] Out_Feature [`Hout_L0][`Wout_L0][`CHout_L0]);
begin
    bit [`DAT_DW_L0*2-1:0] mean_square[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0-1:0] mean[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0*2-1:0] variance[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0-1:0] one_div_sqrt[`Hin_L0][`Win_L0];
    
    LN_MEAN_Soft(In_Feature, mean, mean_square);
    LN_VAR_Soft2(In_Feature, mean_square, variance);
//    LN_VAR_Soft(In_Feature, mean, variance);
    
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
        begin
            real tp_variance;
            tp_variance=variance[i][j];

            for(int k=0;k<`CHin_L0;k++)
            begin
                bit [`DAT_DW_L0:0]tp_difference;
                bit [`DAT_DW_L0*2:0]tp_LN_diff_mul_k;
                bit [`DAT_DW_L0*2-1:0]LN_wt_out;
                bit [`DAT_DW_L0-1:0]shift_LN_wt_out;
                
                bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN_wt_out;
                bit [`DAT_DW_L0*2-1:0]tp_bias;
                bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN;
                bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN_round_out;
                bit [`DAT_DW_L0-1:0]  LN_out;

                tp_difference=$signed(In_Feature[i][j][k])-$signed(mean[i][j]);
                tp_LN_diff_mul_k=$signed(tp_difference)*$signed(k_factor[k]);
                LN_wt_out=$signed(tp_LN_diff_mul_k)/$sqrt(tp_variance); //scale= LN_wt_scale
                        
//                $display("tp_difference=%d, tp_LN_diff_mul_k=%d, LN_wt_out[%d][%d]=%d",
//                          $signed(tp_difference),$signed(tp_LN_diff_mul_k),i,j,$signed(LN_wt_out));
                                  
                if(LN_wt_out[(`DAT_DW_L0*2-1)]&&(!(&LN_wt_out[(`DAT_DW_L0*2-2):(`DAT_DW_L0-1)])))//tp2_mul_pc is negetive and tp2_mul_pc<16'h8000
                    shift_LN_wt_out=$signed({1'b1, {(`DAT_DW_L0-1){1'b0}} });
                else
                    if((!LN_wt_out[(`DAT_DW_L0*2-1)])&&(|LN_wt_out[(`DAT_DW_L0*2-2):(`DAT_DW_L0-1)]))//tp2_mul_pc is positive and tp2_mul_pc>16'h7fff
                        shift_LN_wt_out=$signed({1'b0, {(`DAT_DW_L0-1){1'b1}} });
                    else
                        shift_LN_wt_out=$signed(LN_wt_out[(`DAT_DW_L0-1):0]);
//                $display("shift_LN_wt_out=%d",$signed(shift_LN_wt_out));
                                  
                tp_LN_wt_out = $signed(shift_LN_wt_out)<<<bias_scale;
                tp_bias = $signed(bias[k])<<<k_factor_scale;
                tp_LN = $signed(tp_LN_wt_out) + $signed(tp_bias);
                tp_LN_round_out = $signed(tp_LN) >>> (bias_scale+k_factor_scale-out_scale);
                if(((bias_scale+k_factor_scale-out_scale)!=0)&&(tp_LN!={1'b0, {(`DAT_DW_L1*2+`log2_S-1){1'b1}} }))
                    tp_LN_round_out=tp_LN_round_out+tp_LN[(bias_scale+k_factor_scale-out_scale)-1];
                //saturate
                if(tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-1)]&&(!(&tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-2):(`DAT_DW_L1-1)])))//tp2_mul_pc is negetive and tp2_mul_pc<16'h8000
                    LN_out=$signed({1'b1, {(`DAT_DW_L1-1){1'b0}} });
                else
                    if((!tp_LN_round_out[(`MAX_DAT_DW*2-1)])&&(|tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-2):(`DAT_DW_L1-1)]))//tp2_mul_pc is positive and tp2_mul_pc>16'h7fff
                        LN_out=$signed({1'b0, {(`DAT_DW_L1-1){1'b1}} });
                    else
                        LN_out=$signed(tp_LN_round_out[(`DAT_DW_L1-1):0]);
                        
                Out_Feature[i][j][k]=LN_out;
//                $display("LN_out[H%d][w%d][CH%d]=%d",i,j,k,$signed(LN_out));
            end
        end

end
endtask

task Run_LN_Soft2(input bit [`DAT_DW_L0-1:0] In_Feature [`Hin_L0][`Win_L0][`CHin_L0],
                    input bit [`DAT_DW_L0-1:0] bias[`CHin_L0], input bit [`log2_S-1:0] bias_scale,
                    input bit [`DAT_DW_L0-1:0] k_factor[`CHin_L0], input bit [`log2_S-1:0] k_factor_scale, 
                    input bit [`log2_S-1:0] out_scale,
                    output bit [`DAT_DW_L1-1:0] Out_Feature [`Hout_L0][`Wout_L0][`CHout_L0]);
begin
    
    bit [`DAT_DW_L0*2-1:0] mean_square[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0-1:0] mean[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0*2-1:0] variance[`Hin_L0][`Win_L0];
    bit [`DAT_DW_L0-1:0] one_div_sqrt[`Hin_L0][`Win_L0];
    
    LN_MEAN_Soft(In_Feature, mean, mean_square);
    LN_VAR_Soft2(In_Feature, mean_square, variance);
//    LN_VAR_Soft(In_Feature, mean, variance);
    
    for(int i=0;i<`Hin_L0;i++)
        for(int j=0;j<`Win_L0;j++)
        begin
            if(i*`Win_L0+j>=`Height)
                for(int k=0;k<`CHin_L0;k++)
                     Out_Feature[i][j][k]=0;
            else
            begin
                real real_sqrt,real_variance,real_one_div_sqrt;
                bit [`DAT_DW_L0+2-1:0] sqrt;
                bit [`DAT_DW_L0*2+1-1:0] one_div_sqrt;
                real_variance=variance[i][j];
                real_sqrt=$sqrt(real_variance);
                sqrt=real_sqrt;
//                if (sqrt*sqrt>variance[i][j]) sqrt=sqrt-1;
                real_one_div_sqrt=65535/sqrt;
    
                one_div_sqrt=real_one_div_sqrt;
                if(sqrt==0)one_div_sqrt=65535;
                $display("sqrt[h %d][w %d]=%d, one_div_sqrt=%d",i,j,$signed(sqrt),$signed(one_div_sqrt));
                
                for(int k=0;k<`CHin_L0;k++)
                begin
                    bit [`DAT_DW_L0:0]tp_difference;
                    bit [`DAT_DW_L0*2:0]tp_LN_diff_mul_k;
                    bit [`DAT_DW_L0*4-1:0]LN_wt_out;
                    bit [`DAT_DW_L0*2-1:0]shift_LN_wt_out;
                    
                    bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN_wt_out;
                    bit [`DAT_DW_L0*2-1:0]tp_bias;
                    bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN;
                    bit [`DAT_DW_L0*2+`log2_S-1:0]tp_LN_round_out;
                    bit [`DAT_DW_L0-1:0]  LN_out;
    
                    tp_difference=$signed(In_Feature[i][j][k])-$signed(mean[i][j]);
                    tp_LN_diff_mul_k=$signed(tp_difference)*$signed(k_factor[k]);
                    LN_wt_out=$signed(tp_LN_diff_mul_k)*$signed(one_div_sqrt); //scale= LN_wt_scale
                            
    //                $display("tp_difference=%d, tp_LN_diff_mul_k=%d, LN_wt_out[%d][%d]=%d",
    //                          $signed(tp_difference),$signed(tp_LN_diff_mul_k),i,j,$signed(LN_wt_out));
                    
                    shift_LN_wt_out=LN_wt_out[`MAX_DAT_DW*4-1:`MAX_DAT_DW*2]+LN_wt_out[(`MAX_DAT_DW*2-1)]; // *65536
    //                $display("shift_LN_wt_out=%d",$signed(shift_LN_wt_out));
                                      
                    tp_LN_wt_out = $signed(shift_LN_wt_out)<<<bias_scale;
                    tp_bias = $signed(bias[k])<<<k_factor_scale;
                    tp_LN = $signed(tp_LN_wt_out) + $signed(tp_bias);
                    tp_LN_round_out = $signed(tp_LN) >>> (bias_scale+k_factor_scale-out_scale);
                    if(((bias_scale+k_factor_scale-out_scale)!=0)&&(tp_LN!={1'b0, {(`DAT_DW_L1*2+`log2_S-1){1'b1}} }))
                        tp_LN_round_out=tp_LN_round_out+tp_LN[(bias_scale+k_factor_scale-out_scale)-1];
                    //saturate
                    if(tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-1)]&&(!(&tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-2):(`DAT_DW_L1-1)])))//tp2_mul_pc is negetive and tp2_mul_pc<16'h8000
                        LN_out=$signed({1'b1, {(`DAT_DW_L1-1){1'b0}} });
                    else
                        if((!tp_LN_round_out[(`MAX_DAT_DW*2-1)])&&(|tp_LN_round_out[(`MAX_DAT_DW*2+`log2_S-2):(`DAT_DW_L1-1)]))//tp2_mul_pc is positive and tp2_mul_pc>16'h7fff
                            LN_out=$signed({1'b0, {(`DAT_DW_L1-1){1'b1}} });
                        else
                            LN_out=$signed(tp_LN_round_out[(`DAT_DW_L1-1):0]);
                            
                    Out_Feature[i][j][k]=LN_out;
    //                $display("LN_out[H%d][w%d][CH%d]=%d",i,j,k,$signed(LN_out));
                end
            end
        end

end
endtask

task Run_LN(bit [`log2_W-1:0]Win,bit [`log2_H-1:0]Hin,bit [`log2_CH-1:0]CHin,
			bit [31:0]dat_in_base_addr,bit [31:0]dat_in_surface_stride,bit [31:0]dat_in_line_stride,
			bit [31:0]wt_base_addr,
			bit [31:0]dat_out_base_addr,bit [31:0]dat_out_surface_stride,bit [31:0]dat_out_line_stride);
begin
	int Ln_reg_bias;      
	bit [31:0]rdata;
    bit [`DAT_DW_L0-1:0] LN_bias; 
    bit [`log2_S-1:0]    LN_bias_scale;
    bit [`DAT_DW_L0-1:0] LN_wt; 
    bit [`log2_S-1:0]    LN_wt_scale; 
    bit [`log2_S-1:0]    out_scale;
    bit [`DAT_DW_L0:0]   recip_CH_int8;
    bit [`DAT_DW_L0*2:0] recip_CH_int16;
    bit [`Log2_LN_pixel-1:0]pixel_in;
    
    pixel_in=`Height;
    LN_bias_scale=`bias_scale;
    LN_wt_scale=`wt_scale;
    out_scale=`out_scale;
    recip_CH_int16={1'b1,{(`DAT_DW_L0*2){1'b0}}}/(`CHin_L0);
    recip_CH_int8 ={1'b1,{(`DAT_DW_L0){1'b0}}}/(`CHin_L0);
    Ln_reg_bias=128;

    $display("********************************");    
    $display("reg=%d, data=%d", 2 ,wt_base_addr);//gama and beta
    $display("reg=%d, data=%d", 3 ,dat_in_base_addr);//feature_in_base_addr
    $display("reg=%d, data=%d", 4 ,dat_in_surface_stride);//feature_in_surface_stride_in
    $display("reg=%d, data=%d", 5 ,dat_in_line_stride);//feature_in_line_stride_in
    $display("reg=%d, data=%d", 6 ,dat_out_base_addr);//feature_out_base_addr
    $display("reg=%d, data=%d", 7 ,dat_out_surface_stride);//feature_out_surface_stride_in
    $display("reg=%d, data=%d", 8 ,dat_out_line_stride);//feature_out_line_stride_in
    $display("reg=%d, data=%d", 9 ,(CHin+`Tout-1)/`Tout);//ch_in_div_Tout
    $display("reg=%d, data=%d", 10,Hin);//Hin
    $display("reg=%d, data=%d", 11,Win);//Win
    $display("reg=%d, data=%d", 13,LN_wt_scale);//LN_k_factor_scale
    $display("reg=%d, data=%d", 15,LN_bias_scale);//LN_bias_scale
    $display("reg=%d, data=%d", 16,out_scale);//out_scale
    $display("reg=%d, data=%d", 17,recip_CH_int8);//recip_CH_int8
    $display("reg=%d, data=%d", 18,recip_CH_int16);//recip_CH_int16
    $display("reg=%d, data=%d", 19,pixel_in);//pixel_in
    $display("********************************");    
    
	CSB_Write(Ln_reg_bias+2 ,wt_base_addr);//gama and beta
	CSB_Write(Ln_reg_bias+3 ,dat_in_base_addr);//feature_in_base_addr
	CSB_Write(Ln_reg_bias+4 ,dat_in_surface_stride);//feature_in_surface_stride_in
	CSB_Write(Ln_reg_bias+5 ,dat_in_line_stride);//feature_in_line_stride_in
	CSB_Write(Ln_reg_bias+6 ,dat_out_base_addr);//feature_out_base_addr
	CSB_Write(Ln_reg_bias+7 ,dat_out_surface_stride);//feature_out_surface_stride_in
	CSB_Write(Ln_reg_bias+8 ,dat_out_line_stride);//feature_out_line_stride_in
	CSB_Write(Ln_reg_bias+9 ,(CHin+`Tout-1)/`Tout);//ch_in_div_Tout
	CSB_Write(Ln_reg_bias+10,Hin);//Hin
    CSB_Write(Ln_reg_bias+11,Win);//Win
    CSB_Write(Ln_reg_bias+13,LN_wt_scale);//LN_k_factor_scale
    CSB_Write(Ln_reg_bias+15,LN_bias_scale);//LN_bias_scale
    CSB_Write(Ln_reg_bias+16,out_scale);//out_scale
    CSB_Write(Ln_reg_bias+17,recip_CH_int8);//recip_CH_int8
    CSB_Write(Ln_reg_bias+18,recip_CH_int16);//recip_CH_int16
    CSB_Write(Ln_reg_bias+19,pixel_in);//pixel_in
    
	//Kick of the run
	CSB_Write(Ln_reg_bias+0,32'b10_0000);//Ln_stage0_start
	CSB_Read(Ln_reg_bias+1,rdata);
	while(rdata!==1) CSB_Read(Ln_reg_bias+1,rdata);

    CSB_Write(Ln_reg_bias+26,{1'b0,1'b0});//reg Matrix_Out_to_BRAM_mode_r, Matrix_In_from_BRAM_mode_r
	CSB_Write(Ln_reg_bias+0,32'b100_0000);//Ln_stage1_start
	CSB_Read(Ln_reg_bias+1,rdata);
	while(rdata!==1) CSB_Read(Ln_reg_bias+1,rdata);
	
	$display("LN wdma done done!");

end
endtask

