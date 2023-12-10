task AXI_Lite_Write(input bit[9:0] addr,input bit[31:0] data);
begin
    $display("csb_rtl.append([%0d, %0d])", addr, data);
end
endtask

task AXI_Lite_Read(input bit[9:0] addr,output bit[31:0] data);
begin
    data = 1;
end
endtask

task FP16_to_real(input bit [16-1:0]data_in_FP16, output real data_out_real);
begin
    data_out_real = (data_in_FP16[14:10]==5'b00000)?(((-1)**(data_in_FP16[15])) * (2.0**(-14)) * (real'(data_in_FP16[9:0])/1024)):
    ((-1)**(data_in_FP16[15])) * (2.0**signed'(data_in_FP16[14:10]-15)) * (1+real'(data_in_FP16[9:0])/1024);
end
endtask

task real_to_FP16(input real in, output bit[15:0]out);
begin
    bit [31:0]fp32;
    bit [15:0]tp;
    real fp16;
    tp=0;fp32=0;out=0;

//    $display("tp=%h",tp); 
    fp32=$shortrealtobits(in);
    tp=((fp32&32'h007f_ffff)>>13) | ((fp32&32'h8000_0000)>>16) |
    $signed($signed((fp32&32'h7f80_0000)>>13)-$signed(112<<10));
    if(fp32&16'h1000&(tp[9:0]!=10'b11_1111_1111)) tp[9:0]=tp[9:0]+1;
//    $display("tp=%h",tp); 
        
    if(in>65504) 
        out=16'b0_11110_1111111111;
    else 
    begin
        if(in<-65504)
        begin
            out=16'b1_11110_1111111111; 
        end
        else 
        begin
            if((in<0.00002) & (in>-0.00002))
                out=16'b0_00000_0000000000;
            else 
            begin
                out=tp;
//                $display("fp16=%0b\n",out);
            end
        end
    end
//    $display("tp=%h",tp);  
//    $display("fp16=%0b\n",out);
end
endtask    

task real_to_FP20
(
    input [31:0] FP32_i,
    output [19:0] FP20_o
);
begin

    bit            FP32_i_s;
    bit [8-1:0]    FP32_i_e;
    bit [23-1:0]   FP32_i_f;
    
    bit FP20_o_s;
    bit [6-1:0] FP20_o_e;
    bit [23-1:0] FP20_o_m_tmp;
    bit [13-1:0] FP20_o_m; 
    bit overflow_tmp;
    bit underflow_tmp;
    bit [3:0] r_cnt;
    bit C_tmp;
    bit [23:0] i_m_tmp0;//Used to store shifted data
    bit [23:0] i_m_tmp1;//Used to store shifted data
       
	FP32_i_s = FP32_i[31];
    FP32_i_e = FP32_i[30-:8];
    FP32_i_f = FP32_i[22:0];
	
    begin
        if(FP32_i_e<84) begin//fp32<2^(-43)
            FP20_o_s = 0;
            FP20_o_e = 0;
            FP20_o_m = 0;
            overflow_tmp = 0;
            underflow_tmp = 1;            
        end
        else if(FP32_i_e>=159) begin//fp32>=2^32
            FP20_o_s = FP32_i_s;
            FP20_o_e = 6'b111110;
            FP20_o_m = 13'b1111_1111_1111_1;
            overflow_tmp = 1;
            underflow_tmp = 0; 
        end
        else begin
            FP20_o_s = FP32_i_s;
            overflow_tmp = 0;
            underflow_tmp = 0;
            if(FP32_i_e>=97) begin
                FP20_o_e = FP32_i_e - 96;
                C_tmp = FP32_i_f[9];
                if(C_tmp) FP20_o_m_tmp = FP32_i_f + 11'b1000_0000_000;
                else FP20_o_m_tmp = FP32_i_f;
                FP20_o_m = FP20_o_m_tmp[(23-1)-:13];
            end
            else begin
                FP20_o_e = 0;
                r_cnt = 97 - FP32_i_e;
                i_m_tmp0 = {1'b1,FP32_i_f};
                C_tmp = i_m_tmp0[9+r_cnt];
                if(C_tmp) begin
                    i_m_tmp1 = (i_m_tmp0>>r_cnt) + 11'b1000_0000_000;
                    FP20_o_m = i_m_tmp1[(23-1)-:13];                 
                end
                else begin 
                    i_m_tmp1 = (i_m_tmp0>>r_cnt);
                    FP20_o_m = i_m_tmp1[(23-1)-:13];                  
                end
            end             
        end
    end
    
    FP20_o = {FP20_o_s,FP20_o_e,FP20_o_m};
end   
endtask
