
task General_Map_Pos_Data(input bit [(`DAT_DW_L0-1):0] in[`Pos_Num][`CHin/2],
                          output bit[`MAX_DAT_DW*`Tout-1:0]mem[`Pos_Num*`CHin_div_Tout/2]);
begin
    bit [31:0] cnt;
    
    cnt=0;
    
    for(int k=0;k<`CHin_div_Tout/2;k=k+1)
    begin
        for(int j=0;j<`Pos_Num;j++)
        begin
            bit [`MAX_DAT_DW *`Tout-1:0]tp;
            tp=0;
            for(int kk=0;kk<`Tout;kk++)
            begin
                if(k*`Tout+kk<`CHin/2) 
                   tp[kk*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[j][k*`Tout+kk]);
                else
                   tp[kk*`MAX_DAT_DW+:`MAX_DAT_DW]=0;
            end
            mem[`Pos_Num*k+j]=tp;
//            $display("mem[%d]=%0h",`Pos_Num*k+j,mem[cnt]);
            cnt += 1; 
        end
    end
end
endtask



task Run_PosEmb_Software(input real In_Feature [`Tb][`Hin][`Win][`CHin], 
                         input real In_Pos[`Pos_Num][`CHin/2],
                    output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout_Padding],
                    output real real_fp16_software_out [`Tb][`Hout][`Wout][`CHout_Padding],
                    output real real_fp32_software_out [`Tb][`Hout][`Wout][`CHout_Padding]);
begin

    real query_split0 [`Tb][`Hin][`Win][`CHin/2];
    real query_split1 [`Tb][`Hin][`Win][`CHin/2];
    real query_split0_0 [`Tb][`Hin][`Win][`CHin/4];
    real query_split0_1 [`Tb][`Hin][`Win][`CHin/4];
    real   pos_split0_0            [`Win][`CHin/4];
    real   pos_split0_1            [`Win][`CHin/4];
    
    real out_A[`Tb][`Hin][`Win][`CHin/4];
    real out_B[`Tb][`Hin][`Win][`CHin/4];
    real real_feature_out;
    
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin/2;k++)
                begin
                    query_split0[b][i][j][k]=In_Feature[b][i][j][k];
                    query_split1[b][i][j][k]=In_Feature[b][i][j][`CHin/2+k];
                end
                
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin/4;k++)
                begin
                    query_split0_0[b][i][j][k]=query_split0[b][i][j][k*2];
                    query_split0_1[b][i][j][k]=query_split0[b][i][j][k*2+1];
//                    $display("query_split0_0[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,query_split0_0[b][i][j][k]);
//                    $display("query_split0_1[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,query_split0_1[b][i][j][k]);   
                end
                        
    for(int j=0;j<`Win;j++)
         for(int k=0;k<`CHin/4;k++)
         begin
             pos_split0_0[j][k]=In_Pos[j][k*2];
             pos_split0_1[j][k]=In_Pos[j][k*2+1];
//             $display("pos_split0_0[W %0d][CH %0d]=%f",j,k,pos_split0_0[j][k]);
//             $display("pos_split0_1[W %0d][CH %0d]=%f",j,k,pos_split0_1[j][k]);   
         end          

    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin/4;k++)
                begin
                    out_A[b][i][j][k]=query_split0_0[b][i][j][k]*pos_split0_0[j][k] - query_split0_1[b][i][j][k]*pos_split0_1[j][k];
                    out_B[b][i][j][k]=query_split0_1[b][i][j][k]*pos_split0_0[j][k] + query_split0_0[b][i][j][k]*pos_split0_1[j][k];
                                      
//                    $display("out_A[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,out_A[b][i][j][k]);
//                    $display("out_B[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,out_B[b][i][j][k]);                                  
                end

    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin;k++)
                begin
                    if(k>=`CHin/2)
                        real_feature_out=query_split1[b][i][j][k-`CHin/2];
                    else
                        if(k%2==0)
                            real_feature_out=out_A[b][i][j][k/2];
                        else
                            real_feature_out=out_B[b][i][j][k/2];
                
                    real_to_FP16(real_feature_out,feature_out[b][i][j][k]);
                    FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    real_fp32_software_out[b][i][j][k]=real_feature_out;
                    
//                    $display("real_feature_out=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",real_feature_out,b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                    $display("********fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);            
                end
end
endtask



task Run_PosEmb_Hardware(bit [31:0]Head,bit [31:0]Win,bit [31:0]CHout_div_Tout,
        bit [31:0]PosEmb_in_base,bit [31:0]PosEmb_in_line_stride,
        bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride);
begin
    
	int PosEmb_reg_bias;      
    bit [31:0]rdata;
    PosEmb_reg_bias=192;
    $display("PosEmb_in_line_stride=%d",PosEmb_in_line_stride);
    
    AXI_Lite_Write(PosEmb_reg_bias+3 ,feature_in_base);//feature_in_base_addr
    AXI_Lite_Write(PosEmb_reg_bias+4 ,feature_in_surface_stride);//feature_in_surface_stride_in
    AXI_Lite_Write(PosEmb_reg_bias+5 ,feature_in_line_stride);//feature_in_line_stride_in
    AXI_Lite_Write(PosEmb_reg_bias+6 ,feature_out_base);//feature_out_base_addr
    AXI_Lite_Write(PosEmb_reg_bias+7 ,feature_out_surface_stride);//feature_out_surface_stride_in
    AXI_Lite_Write(PosEmb_reg_bias+8 ,feature_out_line_stride);//feature_out_line_stride_in
    AXI_Lite_Write(PosEmb_reg_bias+9 ,CHout_div_Tout);//CHout_div_Tout
    AXI_Lite_Write(PosEmb_reg_bias+10,Head);//Hin
    AXI_Lite_Write(PosEmb_reg_bias+11,Win);//Win
    AXI_Lite_Write(PosEmb_reg_bias+24,PosEmb_in_base);//PosEmb_in_base
    AXI_Lite_Write(PosEmb_reg_bias+31,PosEmb_in_line_stride);//line_stride
    
    //Kick of the run
    AXI_Lite_Write(PosEmb_reg_bias+0,32'b0100);//PosEmb_start
    $display("Run PosEmb start!");
    
    //Waiting for PosEmb_wdma done
    AXI_Lite_Read(PosEmb_reg_bias+1,rdata);
    while(rdata!==1) 
        AXI_Lite_Read(PosEmb_reg_bias+1,rdata);
    $display("PosEmb_wdma done!");
    
end
endtask