
task RunElementwise_Software(input [31:0]ew_mode,
                 input real real_dat_in_A [`Tb][`Hin][`Win][`CHin],
                 input real real_dat_in_B [`Tb][`Hin][`Win][`CHin],
                output bit[`DAT_DW_L1-1:0] feature_out[`Tb][`Hout][`Wout][`CHout],
                output real real_fp16_software_out [`Tb][`Hout][`Wout][`CHout],
                output real real_fp32_software_out [`Tb][`Hout][`Wout][`CHout]);
begin
    real A,B,tp_out;
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin;k++)
                begin
                    A=real_dat_in_A[b][i][j][k];
                    B=real_dat_in_B[b][i][j][k];
                    
                    if(ew_mode == 0)
                        tp_out=A+B;
                    else
                        if(ew_mode == 1)
                            tp_out=A-B;
                        else
                            tp_out=A*B;
                        
                    real_to_FP16(tp_out,feature_out[b][i][j][k]);
                    FP16_to_real(feature_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                    real_fp32_software_out[b][i][j][k]=tp_out;
//                    $display("***[batch%d][H%d][W%d][CH%d], mean=%f,recip_std=%f, in=%f",b,i,j,k,mean,recip_std,tp_dat);
//                    $display("LN_out[%d]=%f,real_fp32_software_out[batch %0d][H %0d][W %0d][CH %0d]=%f",k,tp_out,b,i,j,k,real_fp32_software_out[b][i][j][k]);
//                    $display("fp32_out=%f, fp16_out=%f",real_fp32_software_out[b][i][j][k],real_fp16_software_out[b][i][j][k]);
                end
end
endtask


task RunElementwise_Hardware(input bit[31:0] Hin,input bit[31:0] Win,input bit[31:0] CHin_div_Tout,
                    input bit[31:0] ElementWise_Mode, input bit[31:0] A_dat_in_base_addr,
                    input bit[31:0] B_dat_in_base_addr,input bit[31:0] dat_in_surface_stride,input bit[31:0] dat_in_line_stride,
                    input bit[31:0] dat_out_base_addr,input bit[31:0] dat_out_surface_stride,input bit[31:0] dat_out_line_stride);
begin
    int Elementwise_reg_bias;      
    bit [31:0]rdata;
    Elementwise_reg_bias = 128;
    
    AXI_Lite_Write(Elementwise_reg_bias+2,ElementWise_Mode);//ElementWise_Mode
    AXI_Lite_Write(Elementwise_reg_bias+3,A_dat_in_base_addr);//feature_in_base_addr
    AXI_Lite_Write(Elementwise_reg_bias+4,dat_in_surface_stride);//feature_in_surface_stride_in
    AXI_Lite_Write(Elementwise_reg_bias+5,dat_in_line_stride);//feature_in_line_stride_in
    AXI_Lite_Write(Elementwise_reg_bias+6,dat_out_base_addr);//dat_out_base_addr
    AXI_Lite_Write(Elementwise_reg_bias+7,dat_out_surface_stride);//feature_out_surface_stride_in
    AXI_Lite_Write(Elementwise_reg_bias+8,dat_out_line_stride);//feature_out_line_stride_in
    AXI_Lite_Write(Elementwise_reg_bias+9,CHin_div_Tout);//CHin_div_Tout
    AXI_Lite_Write(Elementwise_reg_bias+10,Hin);
    AXI_Lite_Write(Elementwise_reg_bias+11,Win);
    AXI_Lite_Write(Elementwise_reg_bias+12,B_dat_in_base_addr);//feature_in_base_addr
    AXI_Lite_Write(Elementwise_reg_bias+13,{1'b0,1'b0});//Elementwise_Out_to_BRAM_mode_r<=wdata[1], Elementwise_In_from_BRAM_mode_r<=wdata[0]

    //Kick of the run
    AXI_Lite_Write(Elementwise_reg_bias+14,32'b01);//Elementwise_start
    $display("Elementwise_start!");

    //Waiting for Elementwise_wdma done
    AXI_Lite_Read(Elementwise_reg_bias+1,rdata);
    while(rdata!==1) AXI_Lite_Read(Elementwise_reg_bias+1,rdata);
    $display("Elementwise_wdma done!");
            
end
endtask

