`include "TOP_defines.vh"
////////////////////////////////////////////////////////////////////////////////
`define Height 19*32//5//
`define Width_in  (128)//4//
`define Width_out (19)//3//
`define Skip_Factor 2

`define DAT_DW_L0 `MAX_DAT_DW
`define DAT_DW_L1 `MAX_DAT_DW
`define WT_DW `MAX_WT_DW
`define BN_DW `MAX_BN_DW
`define Tin (`base_Tin)

`define Win 19//`Height//`Tout
`define Hin ((`Height+`Win-1)/`Win)
`define CHin `Width_in
`define CHout `Width_out

`define Wout (`Win)
`define Hout (`Hin)
`define CHout_div_Tout  ((`CHout+`Tout-1)/`Tout)
`define CHin_div_Tout  ((`CHin+`Tout-1)/`Tout)
`define CHin_Padding_with_Tout (`CHin_div_Tout*`Tout)
`define Tin_div_Tout  ((`Tin+`Tout-1)/`Tout)
`define CHout_Padding (`CHout_div_Tout*`Tout)

`define RELU_EN 0

`define DAT_IN_BASE_ADDR 32'h100_0000
`define DAT_IN_BATCH_STRIDE (`Pixel_Data_Bytes*`Win*`Hin*`CHin_div_Tout)
`define DAT_IN_SURFACE_STRIDE (`Pixel_Data_Bytes*`Win*`Hin)
`define DAT_IN_LINE_STRIDE (`Pixel_Data_Bytes*`Win)
`define DAT_IN_scale 0

`define WT_CHin_Padding_with_Tin (`WT_CHin_div_Tin*`Tin)
`define WT_CHin_div_Tblock ((`WT_CHin_Padding_with_Tin+`T_quant_block-1)/`T_quant_block)
`define Tblock_div_Tin (`T_quant_block/`Tin)

//`define WT_CH_Tgroup (`T_quant_block*`HBM_AXI_DATA_WIDTH/`WT_quant_scale_DW) // =2048 CHins
`define WT_scale_group_nums ((`WT_CHin_Padding_with_Tin+`WT_CH_Tgroup-1)/`WT_CH_Tgroup)
`define WT_CH_Tgroup_div_Tblock (`WT_CH_Tgroup/`T_quant_block) //2048/128=16

`define Group_WT_Bytes     (`WT_CH_Tgroup*`WT_DW/8)
`define Group_Scale_Bytes (`HBM_AXI_DATA_WIDTH/8)
`define Group_WT_and_Scale_Bytes (`Group_WT_Bytes+`Group_Scale_Bytes)
`define Last_Group_CHin        (`WT_CHin_Padding_with_Tin%`WT_CH_Tgroup)
`define Last_Group_WT_Bytes    (`Last_Group_CHin*`WT_DW/8)
`define Last_Group_Scale_Bytes (`HBM_AXI_DATA_WIDTH/8)
`define Last_Group_WT_and_Scale_Bytes (`Last_Group_WT_Bytes+`Last_Group_Scale_Bytes)

`define CHin_WT_Bytes     (`WT_CHin_Padding_with_Tin*`WT_DW/8)
`define CHin_Scale_Bytes (`HBM_AXI_DATA_WIDTH*`WT_scale_group_nums/8)
`define CHin_WT_and_Scale_Bytes (`CHin_WT_Bytes+`CHin_Scale_Bytes)
`define log2_WT_base_addr_Bank_Step 8
`define WT_base_addr_Bank_Step (1<<`log2_WT_base_addr_Bank_Step)

//`define WT_scale_bits (`CHout_Padding*`WT_CHin_div_Tblock*`WT_quant_scale_DW)
`define WT_scale_bits (`CHout_Padding*`HBM_AXI_DATA_WIDTH*`WT_scale_group_nums)
`define WT_CHin_div_Tin  ((`CHin+`Tin-1)/`Tin)
`define WT_SIZE_IN_BYTES (((`CHout_Padding*`WT_CHin_Padding_with_Tin*`WT_DW)>>3)+((`WT_scale_bits)>>3))
`define WT_NUM_DIV_TIN   (`CHout_Padding*`WT_CHin_div_Tin)
`define WT_scale 5
`define HBM00_WT_BASE_ADDR 32'h00_0000

`define Conv_out_scale 0 // make sure wt_scale + in_scale >= out_scale

`define BN_BASE_ADDR 32'h400_0000
`define BN_SURFACE_STRIDE (`Tout*`MAX_BN_DW*2)>>3
`define BN_num_per_AXI_DW (`AXI_DAT_WIDTH/(2*`BN_DW)) // BN_num_per_AXI_DW <= `Tout
`define BN_ch_group_times (`CHout_Padding/`BN_num_per_AXI_DW)
`define BN_WT_scale 3//(`MAX_BN_DW-1)
`define BN_BIAS_scale 2//2

`define Res_Add_BASE_ADDR 32'h600_0000
`define Res_Add_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define Res_Add_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define Res_Add_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define Res_Add_scale 0		  

`define DAT_OUT_BASE_ADDR 32'h800_0000
`define DAT_OUT_BATCH_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout*`CHout_div_Tout)
`define DAT_OUT_SURFACE_STRIDE (`Pixel_Data_Bytes*`Wout*`Hout)
`define DAT_OUT_LINE_STRIDE (`Pixel_Data_Bytes*`Wout)
`define DAT_OUT_scale `Conv_out_scale

`define half_clk_period 5
////////////////////////////////////////////////////////////////////////////////

module testbench_HBM_MVM_bn;

parameter M_AXI_ID_WIDTH = 4;    // 1AXI
parameter M_AXI_DATA_WIDTH = `AXI_DAT_WIDTH;

bit clk;
bit rst_n;


`include "tasks_driver_HBM_MVM_bn.vh"
`include "basic_tasks.vh"
`include "MVM_tasks.vh"
`include "activation_tasks.vh"
always #(`half_clk_period) clk=~clk;

integer flag=1;
int cnt_i;
bit [31:0]rdata;

bit [`WT_DW-1:0]       weight_reorg[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin];
bit [`WT_DW*`Tin-1:0] weight_Tin[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout];
bit [`WT_DW*`Tout-1:0]  weight_Tout[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin_div_Tout];	

real                           real_dat_in[`Tb][`Hin][`Win][`CHin];
bit [`DAT_DW_L0-1:0]                dat_in[`Tb][`Hin][`Win][`CHin];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]dat_in_mem[`Hin*`Win*`CHin_div_Tout];
bit [`DAT_DW_L0-1:0]           dat_in_test[`Tb][`Hin][`Win][`CHin_Padding_with_Tout];

bit [`WT_DW-1:0] wt[`CHout][`CHin];
bit [`WT_DW-1:0] wt_soft[`CHout][`CHin];
bit [`WT_DW*`Tout-1:0]wt_mem[`CHout_div_Tout*`WT_CHin_div_Tin*`Tout*`Tin_div_Tout];
bit [`WT_DW*`WT_CHin_Padding_with_Tin-1:0]HBM_wt_mem[`CHout_div_Tout][`Tout/`HBM_Port][`HBM_Port];
int wt_start_ch_in,wt_end_ch_in;

real                              real_wt_FP_scale[`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
bit [`WT_quant_scale_DW-1:0]           wt_FP_scale[`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
bit [`WT_quant_scale_DW*`Tout-1:0] wt_FP_scale_men[`CHout_div_Tout *`WT_CHin_div_Tblock];
bit [`HBM_AXI_DATA_WIDTH-1:0]      HBM_wt_FP_scale[`CHout_div_Tout][`WT_scale_group_nums][`Tout/`HBM_Port][`HBM_Port];
bit [31:0] wt_addr_bias;
bit [31:0] scale_addr_bias;

real real_bn_weight[`CHout_Padding];
real real_bn_bias[`CHout_Padding];
bit [`MAX_BN_DW-1:0] bn_weight[`CHout_Padding];
bit [`MAX_BN_DW-1:0] bn_bias[`CHout_Padding];
bit [`MAX_BN_DW*2-1:0] bn_wt_and_bias[`CHout_div_Tout*`Tout];
bit [`AXI_DAT_WIDTH-1:0]bn_wt_and_bias_mem[`BN_ch_group_times];

real real_Res_Add_in[`Tb][`Hout][`Wout][`CHout];
bit [`MAX_DAT_DW-1:0] Res_Add_dat[`Tb][`Hout][`Wout][`CHout];
bit [`MAX_DAT_DW*`Tout-1:0]  tp_Res_Add_mem[`Tb][`Wout*`Hout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout*`Tb-1:0]Res_Add_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`AXI_DAT_WIDTH-1:0]Res_Add_dat_mem[`Wout*`Hout*((`CHout_Padding+`Tout-1)/`Tout)];

real          real_fp16_software_conv_out[`Tb][`Hout][`Wout][`CHout];
real          real_fp32_software_conv_out[`Tb][`Hout][`Wout][`CHout];
real          real_fp16_software_conv_relu_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_conv_bn_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_res_out[`Tb][`Hout][`Wout][`CHout];
real real_fp32_software_conv_bn_res_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_relu_out[`Tb][`Hout][`Wout][`CHout];
real real_fp16_software_conv_bn_res_relu_out[`Tb][`Hout][`Wout][`CHout];

bit [`DAT_DW_L1-1:0]              dat_out[`Tb][`Hout][`Wout][`CHout_Padding];
bit [`AXI_DAT_WIDTH-1:0]     dat_out_mem[`Hout*`Wout*`CHout_div_Tout];
bit [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];

bit [`MAX_BN_DW-1:0] dat_out_soft[`Tb][`Hout][`Wout][`CHout];
bit [(`DAT_DW_L1-1):0]before_relu[`Tb][`Hout][`Wout][`CHout];
real real_software_out,real_hardware_out,difference,relative_error;

initial
begin
    $display("Tout=%d, col Tn=%d",`Tout, `Tn);
    $display("HBM_width=%d, Tin*Tout*WT_DW=%d",`HBM_Port*`HBM_AXI_DATA_WIDTH,`base_Tin*`Tout*`MAX_WT_DW);
    if ( (`Win*`Hin==1)&((`Tout/`Tn)<(`base_Tin*`Tout*`MAX_WT_DW/`HBM_Port/`HBM_AXI_DATA_WIDTH)) )
    begin
        $display("ERROR! col systolic wt will be replaced by the new wt, please enlarge Tn");
        #10 $finish;
    end
end

initial
begin
    $display("Total Operations: %0d",`CHout_Padding*`CHin_Padding_with_Tout*`Wout*`Hout*`Tb);
    $display("theoretical stop time: %0d ns",`CHout_Padding*`CHin_Padding_with_Tout*`Wout*`Hout/`Tin/`Tout*`half_clk_period*2);
    
	for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hin;i++)
            for(int j=0;j<`Win;j++)
                for(int k=0;k<`CHin;k++)
                begin
                    dat_in[b][i][j][k]=0;
                    if(i*`Win+j<`Height)
                    begin
//                        dat_in[b][i][j][k]=(k/`Tin)*5+j+1;//$random();//16'b0_01110_11_1111_1111;//(k<32)?0:i*`Win+j+k*2+1;//(k%2==0)? $random()%6 : 0;//1;//k*4+i*`Win+j+1;//
                        real_dat_in[b][i][j][k]=real'($random())/real'(1<<24);//k%128==0?k/128+1:0;//
                        if(real_dat_in[b][i][j][k]>128 || real_dat_in[b][i][j][k]<-128) real_dat_in[b][i][j][k]=$random()%128;
                        real_to_FP16(real_dat_in[b][i][j][k],dat_in[b][i][j][k]);
//                        $display("real_dat_in[b%0d]h[%0d][w%0d][ch%0d]=%0f, int_dat_in=%b",b,i,j,k,real_dat_in[b][i][j][k], dat_in[b][i][j][k]);
                    end
                end

    for(int i=0;i<`CHout;i++)
        for(int j=0;j<`CHin;j++)
        begin
            wt_soft[i][j]=$random()%8;//i%8;//i/32+j/64;//i*(1+`CHin)+j*3+1;//(j<32)?0:i*`CHin+j+1;//j==0?j+1:0;
            if($signed(wt_soft[i][j])<=-8) begin $finish; end
            if($signed(wt_soft[i][j])<0)
                begin
                    wt[i][j]=-1*$signed(wt_soft[i][j]);
                    wt[i][j]={1'b1,wt[i][j][2:0]};
                end
            else
                wt[i][j]=wt_soft[i][j];
            
//            $display("wt_soft[CHout%0d][CHin%0d]=%0d",i,j,$signed(wt_soft[i][j]));
        end


    for(int i=0;i<`CHout_div_Tout;i++)
        for(int j=0;j<`WT_CHin_div_Tblock;j++)
            for(int tout=0;tout<`Tout;tout++)
            begin
//                wt_FP_scale[i][j][tout]=i*5+j*3+tout/32+1;

                real_wt_FP_scale[i][j][tout]=real'($random())/real'(1<<30)*0.01;//tout<`HBM_Port?1:10;//j*`Tout+tout;//
                if(real_wt_FP_scale[i][j][tout]<0)     real_wt_FP_scale[i][j][tout]=-real_wt_FP_scale[i][j][tout];
                if(real_wt_FP_scale[i][j][tout]<0.0025) real_wt_FP_scale[i][j][tout]=0.00025;
                real_to_FP16(real_wt_FP_scale[i][j][tout],wt_FP_scale[i][j][tout]);
                wt_FP_scale_men[i*`WT_CHin_div_Tblock+j][tout*`WT_quant_scale_DW+:`WT_quant_scale_DW]=wt_FP_scale[i][j][tout];
//                $display("real_wt_FP_scale[chout%0d][chin%0d][t%0d]=%0f, int_wt_scale=%b",i,j,tout,real_wt_FP_scale[i][j][tout], wt_FP_scale[i][j][tout]);
//                $display("wt_FP_scale_men[chout%0d][block%0d][tout%0d]=%0f",i,j,tout, wt_FP_scale[i][j][tout]);
            
            end


    for(int k=0;k<`CHout_div_Tout*`Tout;k++)
    begin
        if(k<`CHout) begin
//            bn_weight[k]=$random();
//            bn_bias[k]=$random();
            real_bn_weight[k]=1;//real'($random())/real'(1<<28);
            real_bn_bias[k]  =0;//real'($random())/real'(1<<26);
            real_to_FP16(real_bn_weight[k],bn_weight[k]);
            real_to_FP16(real_bn_bias[k],  bn_bias[k]);
            $display("real_bn_weight[ch%0d]=%0f, int_bn_weight=%b",k,real_bn_weight[k], bn_weight[k]);
            $display("  real_bn_bias[ch%0d]=%0f,  int_bn_biast=%b",k,real_bn_bias[k], bn_bias[k]);                    
        end
        bn_wt_and_bias[k][`MAX_BN_DW*2-1:`MAX_BN_DW*1]=bn_weight[k];
        bn_wt_and_bias[k][`MAX_BN_DW*1-1:`MAX_BN_DW*0]=bn_bias[k];
    end
    
    for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                begin
                    Res_Add_dat[b][i][j][k]=0;
                    if(i*`Win+j<`Height)
                    begin
//                        Res_Add_dat[b][i][j][k]=$random();//16'b0_01110_11_1111_1111;//(k<32)?0:i*`Win+j+k*2+1;
                        real_Res_Add_in[b][i][j][k]=0;//real'($random())/real'(1<<24);
                        if(real_Res_Add_in[b][i][j][k]>128 || real_Res_Add_in[b][i][j][k]<-128) real_Res_Add_in[b][i][j][k]=$random()%128;
                        real_to_FP16(real_Res_Add_in[b][i][j][k],Res_Add_dat[b][i][j][k]);
//                        $display("real_Res_Add_in[b%0d]h[%0d][w%0d][ch%0d]=%0f, int_dat_in=%b",b,i,j,k,real_Res_Add_in[b][i][j][k], Res_Add_dat[b][i][j][k]);
                    end
                end

		
//////////////////// BN and shortcut preparation ////////////////////////////		
    for(int i=0;i<`BN_ch_group_times;i++)
		for(int j=0;j<`BN_num_per_AXI_DW;j++)
		begin
			if(`AXI_DAT_WIDTH>=(2*`BN_DW))
			     bn_wt_and_bias_mem[i][j*(`MAX_BN_DW*2)+:(`MAX_BN_DW*2)]=bn_wt_and_bias[i*`BN_num_per_AXI_DW+j];
	        else
            begin
                $display("Error!! AXI_DAT_WIDTH=%d, 2*BN_DW=%d,  AXI_DAT_WIDTH too small!",`AXI_DAT_WIDTH, (2*`BN_DW));
                $finish;
            end
	    end

	for(int i=0;i<`BN_ch_group_times;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
		begin
			AXI_HP_Slave_DDR00.memory[`BN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=bn_wt_and_bias_mem[i][32*j+:32];
//            $display("AXI_HP_Slave_DDR_BN[%0d][%0d]=%h",i,j,bn_wt_and_bias_mem[i][32*j+:32]);
        end
		
	for(int b=0;b<`Tb;b++)
        General_Map_Res_Add_Data(`Hout,`Wout,`CHout,Res_Add_dat[b],tp_Res_Add_mem[b]);

    for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
        begin
            Res_Add_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout]=tp_Res_Add_mem[b][i];
//            $display("Res_Add_mem[%0d][%0d]=%h",i,b,tp_Res_Add_mem[b][i]);
        end

	for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
		begin
			AXI_HP_Slave_DDR00.memory[`Res_Add_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=Res_Add_mem[i][32*j+:32];
//            $display("AXI_HP_Slave_DDR_dat[%0d][%0d]=%h",i,j,Res_Add_mem[i][32*j+:32]);
        end


//////////////////// Run in software ////////////////////////////
	RunMVM_soft(`CHin,`Hin,`Win,`CHout,dat_in,wt_soft,wt_FP_scale,dat_out_soft,real_fp16_software_conv_out,real_fp32_software_conv_out);
    Run_BN_soft(real_fp16_software_conv_out,real_bn_weight,real_bn_bias,dat_out_soft,real_fp16_software_conv_bn_out,real_fp32_software_conv_bn_out);
    Run_Dat_Clip_soft(real_fp16_software_conv_bn_out,`RELU_EN,real_fp16_software_conv_bn_relu_out);
    $display("Run software Finish");

    for(int b=0;b<`Tb;b++)
        General_Map_Feature_Data(`Hin,`Win,`CHin,dat_in[b],tp_dat_in_mem[b]);
        
    for(int i=0;i<`Win*`Hin*`CHin_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
        begin
            dat_in_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout]=tp_dat_in_mem[b][i];
//            $display("dat_in_mem[%0d][%0d]=%h",i,b,tp_dat_in_mem[b][i]);
        end

	for(int i=0;i<`Win*`Hin*`CHin_div_Tout;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
		begin
			AXI_HP_Slave_DDR00.memory[`DAT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j]=dat_in_mem[i][32*j+:32];
//            $display("addr=%h, AXI_HP_Slave_DDR_dat[%0d][%0d]=%h",`DAT_IN_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j,i,j,dat_in_mem[i][32*j+:32]);
        end

    for(int i=0;i<`CHout_div_Tout;i++)
        for(int j=0;j<`Tout/`HBM_Port;j++)
            for(int k=0;k<`HBM_Port;k++)
                for(int m=0;m<`WT_CHin_Padding_with_Tin;m++)
                begin
                    if((i*`Tout+j*`HBM_Port+k<`CHout) && (m<`CHin))
                        HBM_wt_mem[i][j][k][m*`WT_DW+:`WT_DW]=wt[i*`Tout+j*`HBM_Port+k][m];
                    else
                        HBM_wt_mem[i][j][k][m*`WT_DW+:`WT_DW]=0;
//                    if(k==1)$display("HBM_wt_mem[CHout_div_Tout%d][Tout_div_Port%d][Port%d][CHin%d]=%d",i,j,k,m,HBM_wt_mem[i][j][k][m*`WT_DW+:`WT_DW]);
                end

    for(int i=0;i<`CHout_div_Tout;i++)
        for(int j=0;j<`WT_scale_group_nums;j++)
            for(int k=0;k<`Tout/`HBM_Port;k++)
                for(int m=0;m<`HBM_Port;m++)
                    for(int n=0;n<`WT_CH_Tgroup_div_Tblock;n++)
                    begin
                        if((i*`Tout+k*`HBM_Port+m<`CHout) && (j*`WT_CH_Tgroup_div_Tblock+n<`WT_CHin_div_Tblock))
                            HBM_wt_FP_scale[i][j][k][m][n*`WT_quant_scale_DW+:`WT_quant_scale_DW]=wt_FP_scale[i][j*`WT_CH_Tgroup_div_Tblock+n][k*`HBM_Port+m];
                        else
                            HBM_wt_FP_scale[i][j][k][m][n*`WT_quant_scale_DW+:`WT_quant_scale_DW]=0;
                        $display("HBM_wt_FP_scale[CHout_div_Tout%d][Group%d][Tou_div_Port%b][Port%b][chin_in_group%d]=%h",i,j,k,m,n,wt_FP_scale[i][j*`WT_CH_Tgroup_div_Tblock+n][k*`HBM_Port+m]);
                    end
                
    $display("***WT_CHin_Padding_with_Tin=%d",`WT_CHin_Padding_with_Tin);
    $display("***WT_CHin_div_Tblock=%d",`WT_CHin_div_Tblock);
    $display("***Tblock_div_Tin=%d",`Tblock_div_Tin);
    $display("***WT_CH_Tgroup=%d",`WT_CH_Tgroup);
    $display("***WT_scale_group_nums=%d",`WT_scale_group_nums);
    $display("***WT_CH_Tgroup_div_Tblock=%d",`WT_CH_Tgroup_div_Tblock);
    
    $display("***Group_WT_Bytes=%d",`Group_WT_Bytes);
    $display("***Group_Scale_Bytes=%d",`Group_Scale_Bytes);
    $display("***Group_WT_and_Scale_Bytes=%d",`Group_WT_and_Scale_Bytes);  
    $display("***Last_Group_CHin=%d",`Last_Group_CHin);
    $display("***Last_Group_WT_Bytes=%d",`Last_Group_WT_Bytes);
    $display("***Last_Group_Scale_Bytes=%d",`Last_Group_Scale_Bytes);
    $display("***Last_Group_WT_and_Scale_Bytes=%d",`Last_Group_WT_and_Scale_Bytes);
    
    $display("***CHin_WT_Bytes=%d",`CHin_WT_Bytes);
    $display("***CHin_Scale_Bytes=%d",`CHin_Scale_Bytes);
    $display("***CHin_WT_and_Scale_Bytes=%d",`CHin_WT_and_Scale_Bytes);


    for(int i=0;i<`CHout_div_Tout;i++)
    begin
        for(int j=0;j<`WT_scale_group_nums;j++)
        begin
            for(int k=0;k<`Tout/`HBM_Port;k++)
            begin
				for(int m=0;m<`HBM_Port;m++)
				begin
					scale_addr_bias=(i*`CHin_WT_and_Scale_Bytes*8/32+j*`Group_WT_and_Scale_Bytes*8/32)*(`Tout/`HBM_Port)
					                + ((j==`WT_scale_group_nums-1)? (k*`Last_Group_WT_and_Scale_Bytes*8/32) : (k*`Group_WT_and_Scale_Bytes*8/32))
									+`HBM00_WT_BASE_ADDR/4+`WT_base_addr_Bank_Step/4*m;
					if(m==1) $display("***scale_addr_bias=%d",scale_addr_bias);
					for(int n=0;n<`HBM_AXI_DATA_WIDTH/32;n++)
					begin
						if(m== 0)begin HBM_AXI_HP_Slave_DDR00.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 1)begin HBM_AXI_HP_Slave_DDR01.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 2)begin HBM_AXI_HP_Slave_DDR02.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 3)begin HBM_AXI_HP_Slave_DDR03.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 4)begin HBM_AXI_HP_Slave_DDR04.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 5)begin HBM_AXI_HP_Slave_DDR05.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 6)begin HBM_AXI_HP_Slave_DDR06.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 7)begin HBM_AXI_HP_Slave_DDR07.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 8)begin HBM_AXI_HP_Slave_DDR08.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m== 9)begin HBM_AXI_HP_Slave_DDR09.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==10)begin HBM_AXI_HP_Slave_DDR10.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==11)begin HBM_AXI_HP_Slave_DDR11.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==12)begin HBM_AXI_HP_Slave_DDR12.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==13)begin HBM_AXI_HP_Slave_DDR13.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==14)begin HBM_AXI_HP_Slave_DDR14.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==15)begin HBM_AXI_HP_Slave_DDR15.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==16)begin HBM_AXI_HP_Slave_DDR16.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==17)begin HBM_AXI_HP_Slave_DDR17.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==18)begin HBM_AXI_HP_Slave_DDR18.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==19)begin HBM_AXI_HP_Slave_DDR19.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==20)begin HBM_AXI_HP_Slave_DDR20.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==21)begin HBM_AXI_HP_Slave_DDR21.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==22)begin HBM_AXI_HP_Slave_DDR22.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==23)begin HBM_AXI_HP_Slave_DDR23.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==24)begin HBM_AXI_HP_Slave_DDR24.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==25)begin HBM_AXI_HP_Slave_DDR25.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==26)begin HBM_AXI_HP_Slave_DDR26.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==27)begin HBM_AXI_HP_Slave_DDR27.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==28)begin HBM_AXI_HP_Slave_DDR28.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==29)begin HBM_AXI_HP_Slave_DDR29.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==30)begin HBM_AXI_HP_Slave_DDR30.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
						if(m==31)begin HBM_AXI_HP_Slave_DDR31.memory[scale_addr_bias+n]=HBM_wt_FP_scale[i][j][k][m][32*n+:32];end
	                    if(m==25)$display("addr=%d,scale_HBM_DDR[CHout_div_Tout%0d][Group%0d][Tout_div_Port%0d][Port%0d][CHin%0d]=%h",scale_addr_bias+n,i,j,k,m,n,HBM_wt_FP_scale[i][j][k][m][32*n+:32]);
					end
				end
            end
        end
    end


    for(int i=0;i<`CHout_div_Tout;i++)
    begin
		for(int j=0;j<`WT_scale_group_nums;j++)
		begin
			for(int k=0;k<`Tout/`HBM_Port;k++)
			begin            
				for(int m=0;m<`HBM_Port;m++)
				begin      
                    wt_start_ch_in=j*`WT_CH_Tgroup;
                    wt_end_ch_in=(j==`WT_scale_group_nums-1)?`WT_CHin_Padding_with_Tin:(j+1)*`WT_CH_Tgroup;
                    wt_addr_bias=(i*`CHin_WT_and_Scale_Bytes+j*`Group_WT_and_Scale_Bytes)*8/32*(`Tout/`HBM_Port)+`Group_Scale_Bytes*8/32
                                + ((j==`WT_scale_group_nums-1)? (k*`Last_Group_WT_and_Scale_Bytes*8/32) : (k*`Group_WT_and_Scale_Bytes*8/32))
                                +`HBM00_WT_BASE_ADDR/4+`WT_base_addr_Bank_Step/4*m;
                    if(m==1)$display("***wt_addr_bias=%d,wt_start_ch_in=%d,wt_end_ch_in=%d",wt_addr_bias,wt_start_ch_in,wt_end_ch_in);
                    for(int n=`WT_DW*wt_start_ch_in/32;n<`WT_DW*wt_end_ch_in/32;n++)
                    begin
                        if(m== 0)begin HBM_AXI_HP_Slave_DDR00.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 1)begin HBM_AXI_HP_Slave_DDR01.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 2)begin HBM_AXI_HP_Slave_DDR02.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 3)begin HBM_AXI_HP_Slave_DDR03.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 4)begin HBM_AXI_HP_Slave_DDR04.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 5)begin HBM_AXI_HP_Slave_DDR05.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 6)begin HBM_AXI_HP_Slave_DDR06.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 7)begin HBM_AXI_HP_Slave_DDR07.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 8)begin HBM_AXI_HP_Slave_DDR08.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m== 9)begin HBM_AXI_HP_Slave_DDR09.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==10)begin HBM_AXI_HP_Slave_DDR10.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==11)begin HBM_AXI_HP_Slave_DDR11.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==12)begin HBM_AXI_HP_Slave_DDR12.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==13)begin HBM_AXI_HP_Slave_DDR13.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==14)begin HBM_AXI_HP_Slave_DDR14.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==15)begin HBM_AXI_HP_Slave_DDR15.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==16)begin HBM_AXI_HP_Slave_DDR16.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==17)begin HBM_AXI_HP_Slave_DDR17.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==18)begin HBM_AXI_HP_Slave_DDR18.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==19)begin HBM_AXI_HP_Slave_DDR19.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==20)begin HBM_AXI_HP_Slave_DDR20.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==21)begin HBM_AXI_HP_Slave_DDR21.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==22)begin HBM_AXI_HP_Slave_DDR22.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==23)begin HBM_AXI_HP_Slave_DDR23.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==24)begin HBM_AXI_HP_Slave_DDR24.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==25)begin HBM_AXI_HP_Slave_DDR25.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==26)begin HBM_AXI_HP_Slave_DDR26.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==27)begin HBM_AXI_HP_Slave_DDR27.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==28)begin HBM_AXI_HP_Slave_DDR28.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==29)begin HBM_AXI_HP_Slave_DDR29.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==30)begin HBM_AXI_HP_Slave_DDR30.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==31)begin HBM_AXI_HP_Slave_DDR31.memory[wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[i][k][m][32*n+:32];end
                        if(m==25)$display("addr=%0d,wt_HBM_DDR01[CHout_div_Tout%0d][Group%0d][Tout_div_Port%0d][Port%0d][CHin%0d]=%h",wt_addr_bias+n-(`WT_DW*wt_start_ch_in/32),i,j,k,m,n,HBM_wt_mem[i][k][m][32*n+:32]);
                    end
                end
            end
        end
    end
    
    $display("Map weight to HBM Finish");
    
//    print_parameters();
	rst_n=1;
	#20 rst_n=0;
	#20 rst_n=1;
	repeat(30) @(posedge clk);
	
	RunHBM_MVM_BN(`CHin_Padding_with_Tout,`Hin,`Win,`CHout_Padding,`RELU_EN,
			`DAT_IN_BASE_ADDR,`DAT_IN_SURFACE_STRIDE,`DAT_IN_LINE_STRIDE,
			`HBM00_WT_BASE_ADDR,`WT_SIZE_IN_BYTES,`WT_NUM_DIV_TIN,`BN_BASE_ADDR,
			`DAT_OUT_BASE_ADDR,`DAT_OUT_SURFACE_STRIDE,`DAT_OUT_LINE_STRIDE);
	
	$display("Run hardware Finish");

	for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
		for(int j=0;j<`AXI_DAT_WIDTH/32;j++)
			dat_out_mem[i][32*j+:32]=AXI_HP_Slave_DDR00.memory[`DAT_OUT_BASE_ADDR/4+i*`AXI_DAT_WIDTH/32+j];
        
    for(int i=0;i<`Wout*`Hout*`CHout_div_Tout;i++)
        for(int b=0;b<`Tb;b++) 
            tp_dat_out_mem[b][i]=dat_out_mem[i][b*`MAX_DAT_DW*`Tout+:`MAX_DAT_DW*`Tout];

    for(int b=0;b<`Tb;b++) 
        General_DeMap_Feature_Data(`Hout,`Wout,`CHout,tp_dat_out_mem[b],dat_out[b]);


	for(int b=0;b<`Tb;b++)
        for(int i=0;i<`Hout;i++)
            for(int j=0;j<`Wout;j++)
                for(int k=0;k<`CHout;k++)
                    if(i*`Wout+j<`Height)
                    begin
                        real_software_out=real_fp16_software_conv_bn_relu_out[b][i][j][k];
                        FP16_to_real(dat_out[b][i][j][k],real_hardware_out);
    //                    $display("real_fp16_software_conv_bn_relu_out[batch %0d][H %0d][W %0d][CH %0d]=%f",b,i,j,k,real_fp16_software_conv_bn_relu_out);
                        difference=real_hardware_out-real_software_out;
                        relative_error=difference/(real_software_out+0.000001);
                        if ( ((relative_error>0.01)|(relative_error<-0.01)) & ((difference>1)|(difference<-1)) )
                        begin
                            flag=0;
                            $display("large error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t relative_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, relative_error);
                        end
                        else begin
                            $display("small error! dat_out_hardware[batch %0d][H %0d][W %0d][CH %0d]=%10f, \t dat_out_software=%10f, \t relative_error=%10f",
                                            b,i,j,k,real_hardware_out, real_software_out, relative_error);
                        end
                    end

	if(flag==1)
		$display("\n===========================\n\t  result small error\n==================================");
	else
		$display("\n===========================\n\t  result mismatch   \n==================================");
	
	AXI_Lite_Read(27,rdata);$display("dma_dat_performance_cnt_r: %0d",rdata);
	AXI_Lite_Read(28,rdata);$display("dma_wt_performance_cnt_r : %0d",rdata);
	AXI_Lite_Read(29,rdata);$display("fsm_dat_performance_cnt_r : %0d",rdata);
	AXI_Lite_Read(30,rdata);$display("fsm_wt_performance_cnt_r  : %0d",rdata);
	$display("Total MAC: %0d",`CHout_Padding*`CHin_Padding_with_Tout*`Wout*`Hout*`Tb);
    $display("MAC Array Effiency=%2f%%",(((`CHout_Padding*`CHin_Padding_with_Tout*`Wout*`Hout*`Tb))*100.0)/(`Tin*`Tout*`Tb*rdata) );
    
    if(flag==1)
	   #1 $finish;
    else
       #1 $finish; 
end

initial
begin
#100000000 $finish;
end


//////////////////////////////////////////////////
//AXI-lite to CSR
wire S_AXI_AWVALID;
wire S_AXI_AWREADY;
wire [`log2_CSR_REG_NUM+2-1:0]S_AXI_AWADDR;
wire [2:0]S_AXI_AWPROT;
wire [31:0]S_AXI_WDATA;
wire S_AXI_WVALID;
wire S_AXI_WREADY;
wire [3:0]S_AXI_WSTRB;
wire [1:0]S_AXI_BRESP;
wire S_AXI_BVALID;
wire S_AXI_BREADY;
wire S_AXI_ARVALID;
wire S_AXI_ARREADY;
wire [`log2_CSR_REG_NUM+2-1:0]S_AXI_ARADDR;
wire [2:0]S_AXI_ARPROT;
wire [31:0]S_AXI_RDATA;
wire [1:0]S_AXI_RRESP;
wire S_AXI_RVALID;
wire S_AXI_RREADY;

//AXI to Mem for 1 AXI
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_AWID;
wire [32-1 : 0]M_AXI_AWADDR;
wire [7 : 0]M_AXI_AWLEN;
wire [2 : 0]M_AXI_AWSIZE;//=clogb2((M_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M_AXI_AWBURST;//=2'b01;
wire  M_AXI_AWLOCK;//1'b0;
wire [3 : 0]M_AXI_AWCACHE;//=4'b0010
wire [2 : 0]M_AXI_AWPROT;//=3'h0;
wire [3 : 0]M_AXI_AWQOS;//=4'h0;
wire M_AXI_AWVALID;
wire M_AXI_AWREADY;
wire [M_AXI_DATA_WIDTH-1 : 0]M_AXI_WDATA;
wire [M_AXI_DATA_WIDTH/8-1 : 0]M_AXI_WSTRB;
wire M_AXI_WLAST;
wire M_AXI_WVALID;
wire M_AXI_WREADY;
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_BID;//ignore
wire [1 : 0] M_AXI_BRESP;//ignore
wire M_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_ARID;
wire [32-1 : 0]M_AXI_ARADDR;
wire [7 : 0]M_AXI_ARLEN;
wire [2 : 0]M_AXI_ARSIZE;//=clogb2((M_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M_AXI_ARBURST;//=2'b01;
wire M_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M_AXI_ARCACHE;//=4'b0010;
wire [2 : 0]M_AXI_ARPROT;//=3'h0;
wire [3 : 0]M_AXI_ARQOS;//=4'h0;
wire M_AXI_ARVALID;
wire M_AXI_ARREADY;
wire [M_AXI_ID_WIDTH-1 : 0]M_AXI_RID;
wire [M_AXI_DATA_WIDTH-1 : 0]M_AXI_RDATA;
wire [1 : 0]M_AXI_RRESP;//ignore
wire M_AXI_RLAST;
wire M_AXI_RVALID;
wire M_AXI_RREADY;	
////////////////////////////////////////


//AXI to Mem00 for HBM wt
wire [1-1 : 0]M00_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M00_AXI_AWADDR;
wire [7 : 0]M00_AXI_AWLEN;
wire [2 : 0]M00_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M00_AXI_AWBURST;//=2'b01;
wire  M00_AXI_AWLOCK;//1'b0;
wire [3 : 0]M00_AXI_AWCACHE;//=4'b10
wire [2 : 0]M00_AXI_AWPROT;//=3'h0;
wire [3 : 0]M00_AXI_AWQOS;//=4'h0;
wire M00_AXI_AWVALID;
wire M00_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M00_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M00_AXI_WSTRB;
wire M00_AXI_WLAST;
wire M00_AXI_WVALID;
wire M00_AXI_WREADY;
wire [1-1 : 0]M00_AXI_BID;//ignore
wire [1 : 0] M00_AXI_BRESP;//ignore
wire M00_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M00_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M00_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M00_AXI_ARADDR;
wire [7 : 0]M00_AXI_ARLEN;
wire [2 : 0]M00_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M00_AXI_ARBURST;//=2'b01;
wire M00_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M00_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M00_AXI_ARPROT;//=3'h0;
wire [3 : 0]M00_AXI_ARQOS;//=4'h0;
wire M00_AXI_ARVALID;
wire M00_AXI_ARREADY;
wire [1-1 : 0]M00_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M00_AXI_RDATA;
wire [1 : 0]M00_AXI_RRESP;//ignore
wire M00_AXI_RLAST;
wire M00_AXI_RVALID;
wire M00_AXI_RREADY;


//AXI to Mem01 for HBM wt
wire [1-1 : 0]M01_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M01_AXI_AWADDR;
wire [7 : 0]M01_AXI_AWLEN;
wire [2 : 0]M01_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M01_AXI_AWBURST;//=2'b01;
wire  M01_AXI_AWLOCK;//1'b0;
wire [3 : 0]M01_AXI_AWCACHE;//=4'b10
wire [2 : 0]M01_AXI_AWPROT;//=3'h0;
wire [3 : 0]M01_AXI_AWQOS;//=4'h0;
wire M01_AXI_AWVALID;
wire M01_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M01_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M01_AXI_WSTRB;
wire M01_AXI_WLAST;
wire M01_AXI_WVALID;
wire M01_AXI_WREADY;
wire [1-1 : 0]M01_AXI_BID;//ignore
wire [1 : 0] M01_AXI_BRESP;//ignore
wire M01_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M01_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M01_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M01_AXI_ARADDR;
wire [7 : 0]M01_AXI_ARLEN;
wire [2 : 0]M01_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M01_AXI_ARBURST;//=2'b01;
wire M01_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M01_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M01_AXI_ARPROT;//=3'h0;
wire [3 : 0]M01_AXI_ARQOS;//=4'h0;
wire M01_AXI_ARVALID;
wire M01_AXI_ARREADY;
wire [1-1 : 0]M01_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M01_AXI_RDATA;
wire [1 : 0]M01_AXI_RRESP;//ignore
wire M01_AXI_RLAST;
wire M01_AXI_RVALID;
wire M01_AXI_RREADY;


//AXI to Mem02 for HBM wt
wire [1-1 : 0]M02_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M02_AXI_AWADDR;
wire [7 : 0]M02_AXI_AWLEN;
wire [2 : 0]M02_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M02_AXI_AWBURST;//=2'b01;
wire  M02_AXI_AWLOCK;//1'b0;
wire [3 : 0]M02_AXI_AWCACHE;//=4'b10
wire [2 : 0]M02_AXI_AWPROT;//=3'h0;
wire [3 : 0]M02_AXI_AWQOS;//=4'h0;
wire M02_AXI_AWVALID;
wire M02_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M02_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M02_AXI_WSTRB;
wire M02_AXI_WLAST;
wire M02_AXI_WVALID;
wire M02_AXI_WREADY;
wire [1-1 : 0]M02_AXI_BID;//ignore
wire [1 : 0] M02_AXI_BRESP;//ignore
wire M02_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M02_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M02_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M02_AXI_ARADDR;
wire [7 : 0]M02_AXI_ARLEN;
wire [2 : 0]M02_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M02_AXI_ARBURST;//=2'b01;
wire M02_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M02_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M02_AXI_ARPROT;//=3'h0;
wire [3 : 0]M02_AXI_ARQOS;//=4'h0;
wire M02_AXI_ARVALID;
wire M02_AXI_ARREADY;
wire [1-1 : 0]M02_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M02_AXI_RDATA;
wire [1 : 0]M02_AXI_RRESP;//ignore
wire M02_AXI_RLAST;
wire M02_AXI_RVALID;
wire M02_AXI_RREADY;


//AXI to Mem03 for HBM wt
wire [1-1 : 0]M03_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M03_AXI_AWADDR;
wire [7 : 0]M03_AXI_AWLEN;
wire [2 : 0]M03_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M03_AXI_AWBURST;//=2'b01;
wire  M03_AXI_AWLOCK;//1'b0;
wire [3 : 0]M03_AXI_AWCACHE;//=4'b10
wire [2 : 0]M03_AXI_AWPROT;//=3'h0;
wire [3 : 0]M03_AXI_AWQOS;//=4'h0;
wire M03_AXI_AWVALID;
wire M03_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M03_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M03_AXI_WSTRB;
wire M03_AXI_WLAST;
wire M03_AXI_WVALID;
wire M03_AXI_WREADY;
wire [1-1 : 0]M03_AXI_BID;//ignore
wire [1 : 0] M03_AXI_BRESP;//ignore
wire M03_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M03_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M03_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M03_AXI_ARADDR;
wire [7 : 0]M03_AXI_ARLEN;
wire [2 : 0]M03_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M03_AXI_ARBURST;//=2'b01;
wire M03_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M03_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M03_AXI_ARPROT;//=3'h0;
wire [3 : 0]M03_AXI_ARQOS;//=4'h0;
wire M03_AXI_ARVALID;
wire M03_AXI_ARREADY;
wire [1-1 : 0]M03_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M03_AXI_RDATA;
wire [1 : 0]M03_AXI_RRESP;//ignore
wire M03_AXI_RLAST;
wire M03_AXI_RVALID;
wire M03_AXI_RREADY;


//AXI to Mem04 for HBM wt
wire [1-1 : 0]M04_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M04_AXI_AWADDR;
wire [7 : 0]M04_AXI_AWLEN;
wire [2 : 0]M04_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M04_AXI_AWBURST;//=2'b01;
wire  M04_AXI_AWLOCK;//1'b0;
wire [3 : 0]M04_AXI_AWCACHE;//=4'b10
wire [2 : 0]M04_AXI_AWPROT;//=3'h0;
wire [3 : 0]M04_AXI_AWQOS;//=4'h0;
wire M04_AXI_AWVALID;
wire M04_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M04_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M04_AXI_WSTRB;
wire M04_AXI_WLAST;
wire M04_AXI_WVALID;
wire M04_AXI_WREADY;
wire [1-1 : 0]M04_AXI_BID;//ignore
wire [1 : 0] M04_AXI_BRESP;//ignore
wire M04_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M04_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M04_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M04_AXI_ARADDR;
wire [7 : 0]M04_AXI_ARLEN;
wire [2 : 0]M04_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M04_AXI_ARBURST;//=2'b01;
wire M04_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M04_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M04_AXI_ARPROT;//=3'h0;
wire [3 : 0]M04_AXI_ARQOS;//=4'h0;
wire M04_AXI_ARVALID;
wire M04_AXI_ARREADY;
wire [1-1 : 0]M04_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M04_AXI_RDATA;
wire [1 : 0]M04_AXI_RRESP;//ignore
wire M04_AXI_RLAST;
wire M04_AXI_RVALID;
wire M04_AXI_RREADY;


//AXI to Mem05 for HBM wt
wire [1-1 : 0]M05_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M05_AXI_AWADDR;
wire [7 : 0]M05_AXI_AWLEN;
wire [2 : 0]M05_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M05_AXI_AWBURST;//=2'b01;
wire  M05_AXI_AWLOCK;//1'b0;
wire [3 : 0]M05_AXI_AWCACHE;//=4'b10
wire [2 : 0]M05_AXI_AWPROT;//=3'h0;
wire [3 : 0]M05_AXI_AWQOS;//=4'h0;
wire M05_AXI_AWVALID;
wire M05_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M05_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M05_AXI_WSTRB;
wire M05_AXI_WLAST;
wire M05_AXI_WVALID;
wire M05_AXI_WREADY;
wire [1-1 : 0]M05_AXI_BID;//ignore
wire [1 : 0] M05_AXI_BRESP;//ignore
wire M05_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M05_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M05_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M05_AXI_ARADDR;
wire [7 : 0]M05_AXI_ARLEN;
wire [2 : 0]M05_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M05_AXI_ARBURST;//=2'b01;
wire M05_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M05_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M05_AXI_ARPROT;//=3'h0;
wire [3 : 0]M05_AXI_ARQOS;//=4'h0;
wire M05_AXI_ARVALID;
wire M05_AXI_ARREADY;
wire [1-1 : 0]M05_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M05_AXI_RDATA;
wire [1 : 0]M05_AXI_RRESP;//ignore
wire M05_AXI_RLAST;
wire M05_AXI_RVALID;
wire M05_AXI_RREADY;


//AXI to Mem06 for HBM wt
wire [1-1 : 0]M06_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M06_AXI_AWADDR;
wire [7 : 0]M06_AXI_AWLEN;
wire [2 : 0]M06_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M06_AXI_AWBURST;//=2'b01;
wire  M06_AXI_AWLOCK;//1'b0;
wire [3 : 0]M06_AXI_AWCACHE;//=4'b10
wire [2 : 0]M06_AXI_AWPROT;//=3'h0;
wire [3 : 0]M06_AXI_AWQOS;//=4'h0;
wire M06_AXI_AWVALID;
wire M06_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M06_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M06_AXI_WSTRB;
wire M06_AXI_WLAST;
wire M06_AXI_WVALID;
wire M06_AXI_WREADY;
wire [1-1 : 0]M06_AXI_BID;//ignore
wire [1 : 0] M06_AXI_BRESP;//ignore
wire M06_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M06_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M06_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M06_AXI_ARADDR;
wire [7 : 0]M06_AXI_ARLEN;
wire [2 : 0]M06_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M06_AXI_ARBURST;//=2'b01;
wire M06_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M06_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M06_AXI_ARPROT;//=3'h0;
wire [3 : 0]M06_AXI_ARQOS;//=4'h0;
wire M06_AXI_ARVALID;
wire M06_AXI_ARREADY;
wire [1-1 : 0]M06_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M06_AXI_RDATA;
wire [1 : 0]M06_AXI_RRESP;//ignore
wire M06_AXI_RLAST;
wire M06_AXI_RVALID;
wire M06_AXI_RREADY;


//AXI to Mem07 for HBM wt
wire [1-1 : 0]M07_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M07_AXI_AWADDR;
wire [7 : 0]M07_AXI_AWLEN;
wire [2 : 0]M07_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M07_AXI_AWBURST;//=2'b01;
wire  M07_AXI_AWLOCK;//1'b0;
wire [3 : 0]M07_AXI_AWCACHE;//=4'b10
wire [2 : 0]M07_AXI_AWPROT;//=3'h0;
wire [3 : 0]M07_AXI_AWQOS;//=4'h0;
wire M07_AXI_AWVALID;
wire M07_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M07_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M07_AXI_WSTRB;
wire M07_AXI_WLAST;
wire M07_AXI_WVALID;
wire M07_AXI_WREADY;
wire [1-1 : 0]M07_AXI_BID;//ignore
wire [1 : 0] M07_AXI_BRESP;//ignore
wire M07_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M07_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M07_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M07_AXI_ARADDR;
wire [7 : 0]M07_AXI_ARLEN;
wire [2 : 0]M07_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M07_AXI_ARBURST;//=2'b01;
wire M07_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M07_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M07_AXI_ARPROT;//=3'h0;
wire [3 : 0]M07_AXI_ARQOS;//=4'h0;
wire M07_AXI_ARVALID;
wire M07_AXI_ARREADY;
wire [1-1 : 0]M07_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M07_AXI_RDATA;
wire [1 : 0]M07_AXI_RRESP;//ignore
wire M07_AXI_RLAST;
wire M07_AXI_RVALID;
wire M07_AXI_RREADY;


//AXI to Mem08 for HBM wt
wire [1-1 : 0]M08_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M08_AXI_AWADDR;
wire [7 : 0]M08_AXI_AWLEN;
wire [2 : 0]M08_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M08_AXI_AWBURST;//=2'b01;
wire  M08_AXI_AWLOCK;//1'b0;
wire [3 : 0]M08_AXI_AWCACHE;//=4'b10
wire [2 : 0]M08_AXI_AWPROT;//=3'h0;
wire [3 : 0]M08_AXI_AWQOS;//=4'h0;
wire M08_AXI_AWVALID;
wire M08_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M08_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M08_AXI_WSTRB;
wire M08_AXI_WLAST;
wire M08_AXI_WVALID;
wire M08_AXI_WREADY;
wire [1-1 : 0]M08_AXI_BID;//ignore
wire [1 : 0] M08_AXI_BRESP;//ignore
wire M08_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M08_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M08_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M08_AXI_ARADDR;
wire [7 : 0]M08_AXI_ARLEN;
wire [2 : 0]M08_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M08_AXI_ARBURST;//=2'b01;
wire M08_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M08_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M08_AXI_ARPROT;//=3'h0;
wire [3 : 0]M08_AXI_ARQOS;//=4'h0;
wire M08_AXI_ARVALID;
wire M08_AXI_ARREADY;
wire [1-1 : 0]M08_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M08_AXI_RDATA;
wire [1 : 0]M08_AXI_RRESP;//ignore
wire M08_AXI_RLAST;
wire M08_AXI_RVALID;
wire M08_AXI_RREADY;


//AXI to Mem09 for HBM wt
wire [1-1 : 0]M09_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M09_AXI_AWADDR;
wire [7 : 0]M09_AXI_AWLEN;
wire [2 : 0]M09_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M09_AXI_AWBURST;//=2'b01;
wire  M09_AXI_AWLOCK;//1'b0;
wire [3 : 0]M09_AXI_AWCACHE;//=4'b10
wire [2 : 0]M09_AXI_AWPROT;//=3'h0;
wire [3 : 0]M09_AXI_AWQOS;//=4'h0;
wire M09_AXI_AWVALID;
wire M09_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M09_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M09_AXI_WSTRB;
wire M09_AXI_WLAST;
wire M09_AXI_WVALID;
wire M09_AXI_WREADY;
wire [1-1 : 0]M09_AXI_BID;//ignore
wire [1 : 0] M09_AXI_BRESP;//ignore
wire M09_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M09_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M09_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M09_AXI_ARADDR;
wire [7 : 0]M09_AXI_ARLEN;
wire [2 : 0]M09_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M09_AXI_ARBURST;//=2'b01;
wire M09_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M09_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M09_AXI_ARPROT;//=3'h0;
wire [3 : 0]M09_AXI_ARQOS;//=4'h0;
wire M09_AXI_ARVALID;
wire M09_AXI_ARREADY;
wire [1-1 : 0]M09_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M09_AXI_RDATA;
wire [1 : 0]M09_AXI_RRESP;//ignore
wire M09_AXI_RLAST;
wire M09_AXI_RVALID;
wire M09_AXI_RREADY;


//AXI to Mem10 for HBM wt
wire [1-1 : 0]M10_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M10_AXI_AWADDR;
wire [7 : 0]M10_AXI_AWLEN;
wire [2 : 0]M10_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M10_AXI_AWBURST;//=2'b10;
wire  M10_AXI_AWLOCK;//1'b0;
wire [3 : 0]M10_AXI_AWCACHE;//=4'b10
wire [2 : 0]M10_AXI_AWPROT;//=3'h0;
wire [3 : 0]M10_AXI_AWQOS;//=4'h0;
wire M10_AXI_AWVALID;
wire M10_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M10_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M10_AXI_WSTRB;
wire M10_AXI_WLAST;
wire M10_AXI_WVALID;
wire M10_AXI_WREADY;
wire [1-1 : 0]M10_AXI_BID;//ignore
wire [1 : 0] M10_AXI_BRESP;//ignore
wire M10_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M10_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M10_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M10_AXI_ARADDR;
wire [7 : 0]M10_AXI_ARLEN;
wire [2 : 0]M10_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M10_AXI_ARBURST;//=2'b10;
wire M10_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M10_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M10_AXI_ARPROT;//=3'h0;
wire [3 : 0]M10_AXI_ARQOS;//=4'h0;
wire M10_AXI_ARVALID;
wire M10_AXI_ARREADY;
wire [1-1 : 0]M10_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M10_AXI_RDATA;
wire [1 : 0]M10_AXI_RRESP;//ignore
wire M10_AXI_RLAST;
wire M10_AXI_RVALID;
wire M10_AXI_RREADY;


//AXI to Mem11 for HBM wt
wire [1-1 : 0]M11_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M11_AXI_AWADDR;
wire [7 : 0]M11_AXI_AWLEN;
wire [2 : 0]M11_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M11_AXI_AWBURST;//=2'b01;
wire  M11_AXI_AWLOCK;//1'b0;
wire [3 : 0]M11_AXI_AWCACHE;//=4'b10
wire [2 : 0]M11_AXI_AWPROT;//=3'h0;
wire [3 : 0]M11_AXI_AWQOS;//=4'h0;
wire M11_AXI_AWVALID;
wire M11_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M11_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M11_AXI_WSTRB;
wire M11_AXI_WLAST;
wire M11_AXI_WVALID;
wire M11_AXI_WREADY;
wire [1-1 : 0]M11_AXI_BID;//ignore
wire [1 : 0] M11_AXI_BRESP;//ignore
wire M11_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M11_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M11_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M11_AXI_ARADDR;
wire [7 : 0]M11_AXI_ARLEN;
wire [2 : 0]M11_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M11_AXI_ARBURST;//=2'b01;
wire M11_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M11_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M11_AXI_ARPROT;//=3'h0;
wire [3 : 0]M11_AXI_ARQOS;//=4'h0;
wire M11_AXI_ARVALID;
wire M11_AXI_ARREADY;
wire [1-1 : 0]M11_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M11_AXI_RDATA;
wire [1 : 0]M11_AXI_RRESP;//ignore
wire M11_AXI_RLAST;
wire M11_AXI_RVALID;
wire M11_AXI_RREADY;


//AXI to Mem12 for HBM wt
wire [1-1 : 0]M12_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M12_AXI_AWADDR;
wire [7 : 0]M12_AXI_AWLEN;
wire [2 : 0]M12_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M12_AXI_AWBURST;//=2'b01;
wire  M12_AXI_AWLOCK;//1'b0;
wire [3 : 0]M12_AXI_AWCACHE;//=4'b10
wire [2 : 0]M12_AXI_AWPROT;//=3'h0;
wire [3 : 0]M12_AXI_AWQOS;//=4'h0;
wire M12_AXI_AWVALID;
wire M12_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M12_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M12_AXI_WSTRB;
wire M12_AXI_WLAST;
wire M12_AXI_WVALID;
wire M12_AXI_WREADY;
wire [1-1 : 0]M12_AXI_BID;//ignore
wire [1 : 0] M12_AXI_BRESP;//ignore
wire M12_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M12_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M12_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M12_AXI_ARADDR;
wire [7 : 0]M12_AXI_ARLEN;
wire [2 : 0]M12_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M12_AXI_ARBURST;//=2'b01;
wire M12_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M12_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M12_AXI_ARPROT;//=3'h0;
wire [3 : 0]M12_AXI_ARQOS;//=4'h0;
wire M12_AXI_ARVALID;
wire M12_AXI_ARREADY;
wire [1-1 : 0]M12_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M12_AXI_RDATA;
wire [1 : 0]M12_AXI_RRESP;//ignore
wire M12_AXI_RLAST;
wire M12_AXI_RVALID;
wire M12_AXI_RREADY;


//AXI to Mem13 for HBM wt
wire [1-1 : 0]M13_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M13_AXI_AWADDR;
wire [7 : 0]M13_AXI_AWLEN;
wire [2 : 0]M13_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M13_AXI_AWBURST;//=2'b01;
wire  M13_AXI_AWLOCK;//1'b0;
wire [3 : 0]M13_AXI_AWCACHE;//=4'b10
wire [2 : 0]M13_AXI_AWPROT;//=3'h0;
wire [3 : 0]M13_AXI_AWQOS;//=4'h0;
wire M13_AXI_AWVALID;
wire M13_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M13_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M13_AXI_WSTRB;
wire M13_AXI_WLAST;
wire M13_AXI_WVALID;
wire M13_AXI_WREADY;
wire [1-1 : 0]M13_AXI_BID;//ignore
wire [1 : 0] M13_AXI_BRESP;//ignore
wire M13_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M13_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M13_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M13_AXI_ARADDR;
wire [7 : 0]M13_AXI_ARLEN;
wire [2 : 0]M13_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M13_AXI_ARBURST;//=2'b01;
wire M13_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M13_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M13_AXI_ARPROT;//=3'h0;
wire [3 : 0]M13_AXI_ARQOS;//=4'h0;
wire M13_AXI_ARVALID;
wire M13_AXI_ARREADY;
wire [1-1 : 0]M13_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M13_AXI_RDATA;
wire [1 : 0]M13_AXI_RRESP;//ignore
wire M13_AXI_RLAST;
wire M13_AXI_RVALID;
wire M13_AXI_RREADY;


//AXI to Mem14 for HBM wt
wire [1-1 : 0]M14_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M14_AXI_AWADDR;
wire [7 : 0]M14_AXI_AWLEN;
wire [2 : 0]M14_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M14_AXI_AWBURST;//=2'b01;
wire  M14_AXI_AWLOCK;//1'b0;
wire [3 : 0]M14_AXI_AWCACHE;//=4'b10
wire [2 : 0]M14_AXI_AWPROT;//=3'h0;
wire [3 : 0]M14_AXI_AWQOS;//=4'h0;
wire M14_AXI_AWVALID;
wire M14_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M14_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M14_AXI_WSTRB;
wire M14_AXI_WLAST;
wire M14_AXI_WVALID;
wire M14_AXI_WREADY;
wire [1-1 : 0]M14_AXI_BID;//ignore
wire [1 : 0] M14_AXI_BRESP;//ignore
wire M14_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M14_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M14_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M14_AXI_ARADDR;
wire [7 : 0]M14_AXI_ARLEN;
wire [2 : 0]M14_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M14_AXI_ARBURST;//=2'b01;
wire M14_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M14_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M14_AXI_ARPROT;//=3'h0;
wire [3 : 0]M14_AXI_ARQOS;//=4'h0;
wire M14_AXI_ARVALID;
wire M14_AXI_ARREADY;
wire [1-1 : 0]M14_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M14_AXI_RDATA;
wire [1 : 0]M14_AXI_RRESP;//ignore
wire M14_AXI_RLAST;
wire M14_AXI_RVALID;
wire M14_AXI_RREADY;


//AXI to Mem15 for HBM wt
wire [1-1 : 0]M15_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M15_AXI_AWADDR;
wire [7 : 0]M15_AXI_AWLEN;
wire [2 : 0]M15_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M15_AXI_AWBURST;//=2'b01;
wire  M15_AXI_AWLOCK;//1'b0;
wire [3 : 0]M15_AXI_AWCACHE;//=4'b10
wire [2 : 0]M15_AXI_AWPROT;//=3'h0;
wire [3 : 0]M15_AXI_AWQOS;//=4'h0;
wire M15_AXI_AWVALID;
wire M15_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M15_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M15_AXI_WSTRB;
wire M15_AXI_WLAST;
wire M15_AXI_WVALID;
wire M15_AXI_WREADY;
wire [1-1 : 0]M15_AXI_BID;//ignore
wire [1 : 0] M15_AXI_BRESP;//ignore
wire M15_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M15_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M15_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M15_AXI_ARADDR;
wire [7 : 0]M15_AXI_ARLEN;
wire [2 : 0]M15_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M15_AXI_ARBURST;//=2'b01;
wire M15_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M15_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M15_AXI_ARPROT;//=3'h0;
wire [3 : 0]M15_AXI_ARQOS;//=4'h0;
wire M15_AXI_ARVALID;
wire M15_AXI_ARREADY;
wire [1-1 : 0]M15_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M15_AXI_RDATA;
wire [1 : 0]M15_AXI_RRESP;//ignore
wire M15_AXI_RLAST;
wire M15_AXI_RVALID;
wire M15_AXI_RREADY;


//AXI to Mem16 for HBM wt
wire [1-1 : 0]M16_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M16_AXI_AWADDR;
wire [7 : 0]M16_AXI_AWLEN;
wire [2 : 0]M16_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M16_AXI_AWBURST;//=2'b01;
wire  M16_AXI_AWLOCK;//1'b0;
wire [3 : 0]M16_AXI_AWCACHE;//=4'b10
wire [2 : 0]M16_AXI_AWPROT;//=3'h0;
wire [3 : 0]M16_AXI_AWQOS;//=4'h0;
wire M16_AXI_AWVALID;
wire M16_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M16_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M16_AXI_WSTRB;
wire M16_AXI_WLAST;
wire M16_AXI_WVALID;
wire M16_AXI_WREADY;
wire [1-1 : 0]M16_AXI_BID;//ignore
wire [1 : 0] M16_AXI_BRESP;//ignore
wire M16_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M16_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M16_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M16_AXI_ARADDR;
wire [7 : 0]M16_AXI_ARLEN;
wire [2 : 0]M16_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M16_AXI_ARBURST;//=2'b01;
wire M16_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M16_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M16_AXI_ARPROT;//=3'h0;
wire [3 : 0]M16_AXI_ARQOS;//=4'h0;
wire M16_AXI_ARVALID;
wire M16_AXI_ARREADY;
wire [1-1 : 0]M16_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M16_AXI_RDATA;
wire [1 : 0]M16_AXI_RRESP;//ignore
wire M16_AXI_RLAST;
wire M16_AXI_RVALID;
wire M16_AXI_RREADY;


//AXI to Mem17 for HBM wt
wire [1-1 : 0]M17_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M17_AXI_AWADDR;
wire [7 : 0]M17_AXI_AWLEN;
wire [2 : 0]M17_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M17_AXI_AWBURST;//=2'b01;
wire  M17_AXI_AWLOCK;//1'b0;
wire [3 : 0]M17_AXI_AWCACHE;//=4'b10
wire [2 : 0]M17_AXI_AWPROT;//=3'h0;
wire [3 : 0]M17_AXI_AWQOS;//=4'h0;
wire M17_AXI_AWVALID;
wire M17_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M17_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M17_AXI_WSTRB;
wire M17_AXI_WLAST;
wire M17_AXI_WVALID;
wire M17_AXI_WREADY;
wire [1-1 : 0]M17_AXI_BID;//ignore
wire [1 : 0] M17_AXI_BRESP;//ignore
wire M17_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M17_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M17_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M17_AXI_ARADDR;
wire [7 : 0]M17_AXI_ARLEN;
wire [2 : 0]M17_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M17_AXI_ARBURST;//=2'b01;
wire M17_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M17_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M17_AXI_ARPROT;//=3'h0;
wire [3 : 0]M17_AXI_ARQOS;//=4'h0;
wire M17_AXI_ARVALID;
wire M17_AXI_ARREADY;
wire [1-1 : 0]M17_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M17_AXI_RDATA;
wire [1 : 0]M17_AXI_RRESP;//ignore
wire M17_AXI_RLAST;
wire M17_AXI_RVALID;
wire M17_AXI_RREADY;


//AXI to Mem18 for HBM wt
wire [1-1 : 0]M18_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M18_AXI_AWADDR;
wire [7 : 0]M18_AXI_AWLEN;
wire [2 : 0]M18_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M18_AXI_AWBURST;//=2'b01;
wire  M18_AXI_AWLOCK;//1'b0;
wire [3 : 0]M18_AXI_AWCACHE;//=4'b10
wire [2 : 0]M18_AXI_AWPROT;//=3'h0;
wire [3 : 0]M18_AXI_AWQOS;//=4'h0;
wire M18_AXI_AWVALID;
wire M18_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M18_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M18_AXI_WSTRB;
wire M18_AXI_WLAST;
wire M18_AXI_WVALID;
wire M18_AXI_WREADY;
wire [1-1 : 0]M18_AXI_BID;//ignore
wire [1 : 0] M18_AXI_BRESP;//ignore
wire M18_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M18_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M18_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M18_AXI_ARADDR;
wire [7 : 0]M18_AXI_ARLEN;
wire [2 : 0]M18_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M18_AXI_ARBURST;//=2'b01;
wire M18_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M18_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M18_AXI_ARPROT;//=3'h0;
wire [3 : 0]M18_AXI_ARQOS;//=4'h0;
wire M18_AXI_ARVALID;
wire M18_AXI_ARREADY;
wire [1-1 : 0]M18_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M18_AXI_RDATA;
wire [1 : 0]M18_AXI_RRESP;//ignore
wire M18_AXI_RLAST;
wire M18_AXI_RVALID;
wire M18_AXI_RREADY;


//AXI to Mem19 for HBM wt
wire [1-1 : 0]M19_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M19_AXI_AWADDR;
wire [7 : 0]M19_AXI_AWLEN;
wire [2 : 0]M19_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M19_AXI_AWBURST;//=2'b01;
wire  M19_AXI_AWLOCK;//1'b0;
wire [3 : 0]M19_AXI_AWCACHE;//=4'b10
wire [2 : 0]M19_AXI_AWPROT;//=3'h0;
wire [3 : 0]M19_AXI_AWQOS;//=4'h0;
wire M19_AXI_AWVALID;
wire M19_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M19_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M19_AXI_WSTRB;
wire M19_AXI_WLAST;
wire M19_AXI_WVALID;
wire M19_AXI_WREADY;
wire [1-1 : 0]M19_AXI_BID;//ignore
wire [1 : 0] M19_AXI_BRESP;//ignore
wire M19_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M19_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M19_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M19_AXI_ARADDR;
wire [7 : 0]M19_AXI_ARLEN;
wire [2 : 0]M19_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M19_AXI_ARBURST;//=2'b01;
wire M19_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M19_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M19_AXI_ARPROT;//=3'h0;
wire [3 : 0]M19_AXI_ARQOS;//=4'h0;
wire M19_AXI_ARVALID;
wire M19_AXI_ARREADY;
wire [1-1 : 0]M19_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M19_AXI_RDATA;
wire [1 : 0]M19_AXI_RRESP;//ignore
wire M19_AXI_RLAST;
wire M19_AXI_RVALID;
wire M19_AXI_RREADY;


//AXI to Mem20 for HBM wt
wire [1-1 : 0]M20_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M20_AXI_AWADDR;
wire [7 : 0]M20_AXI_AWLEN;
wire [2 : 0]M20_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M20_AXI_AWBURST;//=2'b01;
wire  M20_AXI_AWLOCK;//1'b0;
wire [3 : 0]M20_AXI_AWCACHE;//=4'b10
wire [2 : 0]M20_AXI_AWPROT;//=3'h0;
wire [3 : 0]M20_AXI_AWQOS;//=4'h0;
wire M20_AXI_AWVALID;
wire M20_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M20_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M20_AXI_WSTRB;
wire M20_AXI_WLAST;
wire M20_AXI_WVALID;
wire M20_AXI_WREADY;
wire [1-1 : 0]M20_AXI_BID;//ignore
wire [1 : 0] M20_AXI_BRESP;//ignore
wire M20_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M20_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M20_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M20_AXI_ARADDR;
wire [7 : 0]M20_AXI_ARLEN;
wire [2 : 0]M20_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M20_AXI_ARBURST;//=2'b01;
wire M20_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M20_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M20_AXI_ARPROT;//=3'h0;
wire [3 : 0]M20_AXI_ARQOS;//=4'h0;
wire M20_AXI_ARVALID;
wire M20_AXI_ARREADY;
wire [1-1 : 0]M20_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M20_AXI_RDATA;
wire [1 : 0]M20_AXI_RRESP;//ignore
wire M20_AXI_RLAST;
wire M20_AXI_RVALID;
wire M20_AXI_RREADY;


//AXI to Mem21 for HBM wt
wire [1-1 : 0]M21_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M21_AXI_AWADDR;
wire [7 : 0]M21_AXI_AWLEN;
wire [2 : 0]M21_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M21_AXI_AWBURST;//=2'b01;
wire  M21_AXI_AWLOCK;//1'b0;
wire [3 : 0]M21_AXI_AWCACHE;//=4'b10
wire [2 : 0]M21_AXI_AWPROT;//=3'h0;
wire [3 : 0]M21_AXI_AWQOS;//=4'h0;
wire M21_AXI_AWVALID;
wire M21_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M21_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M21_AXI_WSTRB;
wire M21_AXI_WLAST;
wire M21_AXI_WVALID;
wire M21_AXI_WREADY;
wire [1-1 : 0]M21_AXI_BID;//ignore
wire [1 : 0] M21_AXI_BRESP;//ignore
wire M21_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M21_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M21_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M21_AXI_ARADDR;
wire [7 : 0]M21_AXI_ARLEN;
wire [2 : 0]M21_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M21_AXI_ARBURST;//=2'b01;
wire M21_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M21_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M21_AXI_ARPROT;//=3'h0;
wire [3 : 0]M21_AXI_ARQOS;//=4'h0;
wire M21_AXI_ARVALID;
wire M21_AXI_ARREADY;
wire [1-1 : 0]M21_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M21_AXI_RDATA;
wire [1 : 0]M21_AXI_RRESP;//ignore
wire M21_AXI_RLAST;
wire M21_AXI_RVALID;
wire M21_AXI_RREADY;


//AXI to Mem22 for HBM wt
wire [1-1 : 0]M22_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M22_AXI_AWADDR;
wire [7 : 0]M22_AXI_AWLEN;
wire [2 : 0]M22_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M22_AXI_AWBURST;//=2'b01;
wire  M22_AXI_AWLOCK;//1'b0;
wire [3 : 0]M22_AXI_AWCACHE;//=4'b10
wire [2 : 0]M22_AXI_AWPROT;//=3'h0;
wire [3 : 0]M22_AXI_AWQOS;//=4'h0;
wire M22_AXI_AWVALID;
wire M22_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M22_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M22_AXI_WSTRB;
wire M22_AXI_WLAST;
wire M22_AXI_WVALID;
wire M22_AXI_WREADY;
wire [1-1 : 0]M22_AXI_BID;//ignore
wire [1 : 0] M22_AXI_BRESP;//ignore
wire M22_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M22_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M22_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M22_AXI_ARADDR;
wire [7 : 0]M22_AXI_ARLEN;
wire [2 : 0]M22_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M22_AXI_ARBURST;//=2'b01;
wire M22_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M22_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M22_AXI_ARPROT;//=3'h0;
wire [3 : 0]M22_AXI_ARQOS;//=4'h0;
wire M22_AXI_ARVALID;
wire M22_AXI_ARREADY;
wire [1-1 : 0]M22_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M22_AXI_RDATA;
wire [1 : 0]M22_AXI_RRESP;//ignore
wire M22_AXI_RLAST;
wire M22_AXI_RVALID;
wire M22_AXI_RREADY;


//AXI to Mem23 for HBM wt
wire [1-1 : 0]M23_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M23_AXI_AWADDR;
wire [7 : 0]M23_AXI_AWLEN;
wire [2 : 0]M23_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M23_AXI_AWBURST;//=2'b01;
wire  M23_AXI_AWLOCK;//1'b0;
wire [3 : 0]M23_AXI_AWCACHE;//=4'b10
wire [2 : 0]M23_AXI_AWPROT;//=3'h0;
wire [3 : 0]M23_AXI_AWQOS;//=4'h0;
wire M23_AXI_AWVALID;
wire M23_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M23_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M23_AXI_WSTRB;
wire M23_AXI_WLAST;
wire M23_AXI_WVALID;
wire M23_AXI_WREADY;
wire [1-1 : 0]M23_AXI_BID;//ignore
wire [1 : 0] M23_AXI_BRESP;//ignore
wire M23_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M23_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M23_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M23_AXI_ARADDR;
wire [7 : 0]M23_AXI_ARLEN;
wire [2 : 0]M23_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M23_AXI_ARBURST;//=2'b01;
wire M23_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M23_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M23_AXI_ARPROT;//=3'h0;
wire [3 : 0]M23_AXI_ARQOS;//=4'h0;
wire M23_AXI_ARVALID;
wire M23_AXI_ARREADY;
wire [1-1 : 0]M23_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M23_AXI_RDATA;
wire [1 : 0]M23_AXI_RRESP;//ignore
wire M23_AXI_RLAST;
wire M23_AXI_RVALID;
wire M23_AXI_RREADY;


//AXI to Mem24 for HBM wt
wire [1-1 : 0]M24_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M24_AXI_AWADDR;
wire [7 : 0]M24_AXI_AWLEN;
wire [2 : 0]M24_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M24_AXI_AWBURST;//=2'b01;
wire  M24_AXI_AWLOCK;//1'b0;
wire [3 : 0]M24_AXI_AWCACHE;//=4'b10
wire [2 : 0]M24_AXI_AWPROT;//=3'h0;
wire [3 : 0]M24_AXI_AWQOS;//=4'h0;
wire M24_AXI_AWVALID;
wire M24_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M24_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M24_AXI_WSTRB;
wire M24_AXI_WLAST;
wire M24_AXI_WVALID;
wire M24_AXI_WREADY;
wire [1-1 : 0]M24_AXI_BID;//ignore
wire [1 : 0] M24_AXI_BRESP;//ignore
wire M24_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M24_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M24_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M24_AXI_ARADDR;
wire [7 : 0]M24_AXI_ARLEN;
wire [2 : 0]M24_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M24_AXI_ARBURST;//=2'b01;
wire M24_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M24_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M24_AXI_ARPROT;//=3'h0;
wire [3 : 0]M24_AXI_ARQOS;//=4'h0;
wire M24_AXI_ARVALID;
wire M24_AXI_ARREADY;
wire [1-1 : 0]M24_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M24_AXI_RDATA;
wire [1 : 0]M24_AXI_RRESP;//ignore
wire M24_AXI_RLAST;
wire M24_AXI_RVALID;
wire M24_AXI_RREADY;


//AXI to Mem25 for HBM wt
wire [1-1 : 0]M25_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M25_AXI_AWADDR;
wire [7 : 0]M25_AXI_AWLEN;
wire [2 : 0]M25_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M25_AXI_AWBURST;//=2'b01;
wire  M25_AXI_AWLOCK;//1'b0;
wire [3 : 0]M25_AXI_AWCACHE;//=4'b10
wire [2 : 0]M25_AXI_AWPROT;//=3'h0;
wire [3 : 0]M25_AXI_AWQOS;//=4'h0;
wire M25_AXI_AWVALID;
wire M25_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M25_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M25_AXI_WSTRB;
wire M25_AXI_WLAST;
wire M25_AXI_WVALID;
wire M25_AXI_WREADY;
wire [1-1 : 0]M25_AXI_BID;//ignore
wire [1 : 0] M25_AXI_BRESP;//ignore
wire M25_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M25_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M25_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M25_AXI_ARADDR;
wire [7 : 0]M25_AXI_ARLEN;
wire [2 : 0]M25_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M25_AXI_ARBURST;//=2'b01;
wire M25_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M25_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M25_AXI_ARPROT;//=3'h0;
wire [3 : 0]M25_AXI_ARQOS;//=4'h0;
wire M25_AXI_ARVALID;
wire M25_AXI_ARREADY;
wire [1-1 : 0]M25_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M25_AXI_RDATA;
wire [1 : 0]M25_AXI_RRESP;//ignore
wire M25_AXI_RLAST;
wire M25_AXI_RVALID;
wire M25_AXI_RREADY;


//AXI to Mem26 for HBM wt
wire [1-1 : 0]M26_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M26_AXI_AWADDR;
wire [7 : 0]M26_AXI_AWLEN;
wire [2 : 0]M26_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M26_AXI_AWBURST;//=2'b01;
wire  M26_AXI_AWLOCK;//1'b0;
wire [3 : 0]M26_AXI_AWCACHE;//=4'b10
wire [2 : 0]M26_AXI_AWPROT;//=3'h0;
wire [3 : 0]M26_AXI_AWQOS;//=4'h0;
wire M26_AXI_AWVALID;
wire M26_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M26_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M26_AXI_WSTRB;
wire M26_AXI_WLAST;
wire M26_AXI_WVALID;
wire M26_AXI_WREADY;
wire [1-1 : 0]M26_AXI_BID;//ignore
wire [1 : 0] M26_AXI_BRESP;//ignore
wire M26_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M26_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M26_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M26_AXI_ARADDR;
wire [7 : 0]M26_AXI_ARLEN;
wire [2 : 0]M26_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M26_AXI_ARBURST;//=2'b01;
wire M26_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M26_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M26_AXI_ARPROT;//=3'h0;
wire [3 : 0]M26_AXI_ARQOS;//=4'h0;
wire M26_AXI_ARVALID;
wire M26_AXI_ARREADY;
wire [1-1 : 0]M26_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M26_AXI_RDATA;
wire [1 : 0]M26_AXI_RRESP;//ignore
wire M26_AXI_RLAST;
wire M26_AXI_RVALID;
wire M26_AXI_RREADY;


//AXI to Mem27 for HBM wt
wire [1-1 : 0]M27_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M27_AXI_AWADDR;
wire [7 : 0]M27_AXI_AWLEN;
wire [2 : 0]M27_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M27_AXI_AWBURST;//=2'b01;
wire  M27_AXI_AWLOCK;//1'b0;
wire [3 : 0]M27_AXI_AWCACHE;//=4'b10
wire [2 : 0]M27_AXI_AWPROT;//=3'h0;
wire [3 : 0]M27_AXI_AWQOS;//=4'h0;
wire M27_AXI_AWVALID;
wire M27_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M27_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M27_AXI_WSTRB;
wire M27_AXI_WLAST;
wire M27_AXI_WVALID;
wire M27_AXI_WREADY;
wire [1-1 : 0]M27_AXI_BID;//ignore
wire [1 : 0] M27_AXI_BRESP;//ignore
wire M27_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M27_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M27_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M27_AXI_ARADDR;
wire [7 : 0]M27_AXI_ARLEN;
wire [2 : 0]M27_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M27_AXI_ARBURST;//=2'b01;
wire M27_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M27_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M27_AXI_ARPROT;//=3'h0;
wire [3 : 0]M27_AXI_ARQOS;//=4'h0;
wire M27_AXI_ARVALID;
wire M27_AXI_ARREADY;
wire [1-1 : 0]M27_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M27_AXI_RDATA;
wire [1 : 0]M27_AXI_RRESP;//ignore
wire M27_AXI_RLAST;
wire M27_AXI_RVALID;
wire M27_AXI_RREADY;


//AXI to Mem28 for HBM wt
wire [1-1 : 0]M28_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M28_AXI_AWADDR;
wire [7 : 0]M28_AXI_AWLEN;
wire [2 : 0]M28_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M28_AXI_AWBURST;//=2'b01;
wire  M28_AXI_AWLOCK;//1'b0;
wire [3 : 0]M28_AXI_AWCACHE;//=4'b10
wire [2 : 0]M28_AXI_AWPROT;//=3'h0;
wire [3 : 0]M28_AXI_AWQOS;//=4'h0;
wire M28_AXI_AWVALID;
wire M28_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M28_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M28_AXI_WSTRB;
wire M28_AXI_WLAST;
wire M28_AXI_WVALID;
wire M28_AXI_WREADY;
wire [1-1 : 0]M28_AXI_BID;//ignore
wire [1 : 0] M28_AXI_BRESP;//ignore
wire M28_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M28_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M28_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M28_AXI_ARADDR;
wire [7 : 0]M28_AXI_ARLEN;
wire [2 : 0]M28_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M28_AXI_ARBURST;//=2'b01;
wire M28_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M28_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M28_AXI_ARPROT;//=3'h0;
wire [3 : 0]M28_AXI_ARQOS;//=4'h0;
wire M28_AXI_ARVALID;
wire M28_AXI_ARREADY;
wire [1-1 : 0]M28_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M28_AXI_RDATA;
wire [1 : 0]M28_AXI_RRESP;//ignore
wire M28_AXI_RLAST;
wire M28_AXI_RVALID;
wire M28_AXI_RREADY;


//AXI to Mem29 for HBM wt
wire [1-1 : 0]M29_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M29_AXI_AWADDR;
wire [7 : 0]M29_AXI_AWLEN;
wire [2 : 0]M29_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M29_AXI_AWBURST;//=2'b01;
wire  M29_AXI_AWLOCK;//1'b0;
wire [3 : 0]M29_AXI_AWCACHE;//=4'b10
wire [2 : 0]M29_AXI_AWPROT;//=3'h0;
wire [3 : 0]M29_AXI_AWQOS;//=4'h0;
wire M29_AXI_AWVALID;
wire M29_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M29_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M29_AXI_WSTRB;
wire M29_AXI_WLAST;
wire M29_AXI_WVALID;
wire M29_AXI_WREADY;
wire [1-1 : 0]M29_AXI_BID;//ignore
wire [1 : 0] M29_AXI_BRESP;//ignore
wire M29_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M29_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M29_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M29_AXI_ARADDR;
wire [7 : 0]M29_AXI_ARLEN;
wire [2 : 0]M29_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M29_AXI_ARBURST;//=2'b01;
wire M29_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M29_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M29_AXI_ARPROT;//=3'h0;
wire [3 : 0]M29_AXI_ARQOS;//=4'h0;
wire M29_AXI_ARVALID;
wire M29_AXI_ARREADY;
wire [1-1 : 0]M29_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M29_AXI_RDATA;
wire [1 : 0]M29_AXI_RRESP;//ignore
wire M29_AXI_RLAST;
wire M29_AXI_RVALID;
wire M29_AXI_RREADY;


//AXI to Mem30 for HBM wt
wire [1-1 : 0]M30_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M30_AXI_AWADDR;
wire [7 : 0]M30_AXI_AWLEN;
wire [2 : 0]M30_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M30_AXI_AWBURST;//=2'b01;
wire  M30_AXI_AWLOCK;//1'b0;
wire [3 : 0]M30_AXI_AWCACHE;//=4'b10
wire [2 : 0]M30_AXI_AWPROT;//=3'h0;
wire [3 : 0]M30_AXI_AWQOS;//=4'h0;
wire M30_AXI_AWVALID;
wire M30_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M30_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M30_AXI_WSTRB;
wire M30_AXI_WLAST;
wire M30_AXI_WVALID;
wire M30_AXI_WREADY;
wire [1-1 : 0]M30_AXI_BID;//ignore
wire [1 : 0] M30_AXI_BRESP;//ignore
wire M30_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M30_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M30_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M30_AXI_ARADDR;
wire [7 : 0]M30_AXI_ARLEN;
wire [2 : 0]M30_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M30_AXI_ARBURST;//=2'b01;
wire M30_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M30_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M30_AXI_ARPROT;//=3'h0;
wire [3 : 0]M30_AXI_ARQOS;//=4'h0;
wire M30_AXI_ARVALID;
wire M30_AXI_ARREADY;
wire [1-1 : 0]M30_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M30_AXI_RDATA;
wire [1 : 0]M30_AXI_RRESP;//ignore
wire M30_AXI_RLAST;
wire M30_AXI_RVALID;
wire M30_AXI_RREADY;


//AXI to Mem31 for HBM wt
wire [1-1 : 0]M31_AXI_AWID;
wire [`HBM_ADDR_WIDTH-1 : 0]M31_AXI_AWADDR;
wire [7 : 0]M31_AXI_AWLEN;
wire [2 : 0]M31_AXI_AWSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M31_AXI_AWBURST;//=2'b01;
wire  M31_AXI_AWLOCK;//1'b0;
wire [3 : 0]M31_AXI_AWCACHE;//=4'b10
wire [2 : 0]M31_AXI_AWPROT;//=3'h0;
wire [3 : 0]M31_AXI_AWQOS;//=4'h0;
wire M31_AXI_AWVALID;
wire M31_AXI_AWREADY;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M31_AXI_WDATA;
wire [`HBM_AXI_DATA_WIDTH/8-1 : 0]M31_AXI_WSTRB;
wire M31_AXI_WLAST;
wire M31_AXI_WVALID;
wire M31_AXI_WREADY;
wire [1-1 : 0]M31_AXI_BID;//ignore
wire [1 : 0] M31_AXI_BRESP;//ignore
wire M31_AXI_BVALID;//Bvalid and Bread means a a write response.
wire M31_AXI_BREADY;//Bvalid and Bread means a a write response.
wire [1-1 : 0]M31_AXI_ARID;
wire [`HBM_ADDR_WIDTH-1 : 0]M31_AXI_ARADDR;
wire [7 : 0]M31_AXI_ARLEN;
wire [2 : 0]M31_AXI_ARSIZE;//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1);
wire [1 : 0]M31_AXI_ARBURST;//=2'b01;
wire M31_AXI_ARLOCK;//=1'b0;
wire [3 : 0]M31_AXI_ARCACHE;//=4'b10;
wire [2 : 0]M31_AXI_ARPROT;//=3'h0;
wire [3 : 0]M31_AXI_ARQOS;//=4'h0;
wire M31_AXI_ARVALID;
wire M31_AXI_ARREADY;
wire [1-1 : 0]M31_AXI_RID;
wire [`HBM_AXI_DATA_WIDTH-1 : 0]M31_AXI_RDATA;
wire [1 : 0]M31_AXI_RRESP;//ignore
wire M31_AXI_RLAST;
wire M31_AXI_RVALID;
wire M31_AXI_RREADY;
///////////////////////////////


AXI_GP_Master_CPU #
(
	.M_AXI_ADDR_WIDTH(`log2_CSR_REG_NUM+2),
	.M_AXI_DATA_WIDTH(32)
)AXI_GP_Master_CPU
(
    .M_AXI_ACLK(clk),
    .M_AXI_ARESETN(rst_n),
    
    //AW channel
    .M_AXI_AWVALID(S_AXI_AWVALID),
    .M_AXI_AWREADY(S_AXI_AWREADY),
    .M_AXI_AWADDR(S_AXI_AWADDR),
    .M_AXI_AWPROT(S_AXI_AWPROT),//=3'h0

    //Wr channel
    .M_AXI_WDATA(S_AXI_WDATA),
    .M_AXI_WVALID(S_AXI_WVALID),
    .M_AXI_WREADY(S_AXI_WREADY),
    .M_AXI_WSTRB(S_AXI_WSTRB),//={(M_AXI_DATA_WIDTH/8){1'b1}}
    .M_AXI_BRESP(S_AXI_BRESP),//ignore
    .M_AXI_BVALID(S_AXI_BVALID),
    .M_AXI_BREADY(S_AXI_BREADY),        

    //AR channel
    .M_AXI_ARVALID(S_AXI_ARVALID),
    .M_AXI_ARREADY(S_AXI_ARREADY),
    .M_AXI_ARADDR(S_AXI_ARADDR),
    .M_AXI_ARPROT(S_AXI_ARPROT),//=3'b0

    //Rd channel
    .M_AXI_RDATA(S_AXI_RDATA),
    .M_AXI_RRESP(S_AXI_RRESP),//ignore
    .M_AXI_RVALID(S_AXI_RVALID),
    .M_AXI_RREADY(S_AXI_RREADY)
);


TOP_wrapper #
(	
    .M_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .M_AXI_DATA_WIDTH(`AXI_DAT_WIDTH)
)TOP_wrapper
(
    .clk(clk),
    .rst_n(rst_n),

    //AXI-lite to CSR
    .S_AXI_AWVALID(S_AXI_AWVALID),
    .S_AXI_AWREADY(S_AXI_AWREADY),
    .S_AXI_AWADDR(S_AXI_AWADDR),
    .S_AXI_AWPROT(S_AXI_AWPROT),//ignore
    .S_AXI_WDATA(S_AXI_WDATA),
    .S_AXI_WVALID(S_AXI_WVALID),
    .S_AXI_WREADY(S_AXI_WREADY),
    .S_AXI_WSTRB(S_AXI_WSTRB),//ignore
    .S_AXI_BRESP(S_AXI_BRESP),//=2'b0
    .S_AXI_BVALID(S_AXI_BVALID),
    .S_AXI_BREADY(S_AXI_BREADY),
    .S_AXI_ARVALID(S_AXI_ARVALID),
    .S_AXI_ARREADY(S_AXI_ARREADY),
    .S_AXI_ARADDR(S_AXI_ARADDR),
    .S_AXI_ARPROT(S_AXI_ARPROT),//ignore
    .S_AXI_RDATA(S_AXI_RDATA),
    .S_AXI_RRESP(S_AXI_RRESP),//=2'b0
    .S_AXI_RVALID(S_AXI_RVALID),
    .S_AXI_RREADY(S_AXI_RREADY),

    //AXI to Mem00 
    .M_AXI_AWID(M_AXI_AWID),
    .M_AXI_AWADDR(M_AXI_AWADDR),
    .M_AXI_AWLEN(M_AXI_AWLEN),
    .M_AXI_AWSIZE(M_AXI_AWSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_AWBURST(M_AXI_AWBURST),//=2'b01;
    .M_AXI_AWLOCK(M_AXI_AWLOCK),//1'b0;
    .M_AXI_AWCACHE(M_AXI_AWCACHE),//=4'b0010
    .M_AXI_AWPROT(M_AXI_AWPROT),//=3'h0;
    .M_AXI_AWQOS(M_AXI_AWQOS),//=4'h0;
    .M_AXI_AWVALID(M_AXI_AWVALID),
    .M_AXI_AWREADY(M_AXI_AWREADY),
    .M_AXI_WDATA(M_AXI_WDATA),
    .M_AXI_WSTRB(M_AXI_WSTRB),
    .M_AXI_WLAST(M_AXI_WLAST),
    .M_AXI_WVALID(M_AXI_WVALID),
    .M_AXI_WREADY(M_AXI_WREADY),
    .M_AXI_BID(M_AXI_BID),//ignore
    .M_AXI_BRESP(M_AXI_BRESP),//ignore
    .M_AXI_BVALID(M_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M_AXI_BREADY(M_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M_AXI_ARID(M_AXI_ARID),
    .M_AXI_ARADDR(M_AXI_ARADDR),
    .M_AXI_ARLEN(M_AXI_ARLEN),
    .M_AXI_ARSIZE(M_AXI_ARSIZE),//=clogb2((M_AXI_DATA_WIDTH/8)-1);
    .M_AXI_ARBURST(M_AXI_ARBURST),//=2'b01;
    .M_AXI_ARLOCK(M_AXI_ARLOCK),//=1'b0;
    .M_AXI_ARCACHE(M_AXI_ARCACHE),//=4'b0010;
    .M_AXI_ARPROT(M_AXI_ARPROT),//=3'h0;
    .M_AXI_ARQOS(M_AXI_ARQOS),//=4'h0;
    .M_AXI_ARVALID(M_AXI_ARVALID),
    .M_AXI_ARREADY(M_AXI_ARREADY),
    .M_AXI_RID(M_AXI_RID),
    .M_AXI_RDATA(M_AXI_RDATA),
    .M_AXI_RRESP(M_AXI_RRESP),//ignore
    .M_AXI_RLAST(M_AXI_RLAST),
    .M_AXI_RVALID(M_AXI_RVALID),
    .M_AXI_RREADY(M_AXI_RREADY),

	//AXI to Mem00 for HBM wt
    .M00_AXI_AWID(M00_AXI_AWID),
    .M00_AXI_AWADDR(M00_AXI_AWADDR),
    .M00_AXI_AWLEN(M00_AXI_AWLEN),
    .M00_AXI_AWSIZE(M00_AXI_AWSIZE),//=clogb2((M00_AXI_DATA_WIDTH/8)-1);
    .M00_AXI_AWBURST(M00_AXI_AWBURST),//=2'b01;
    .M00_AXI_AWLOCK(M00_AXI_AWLOCK),//1'b0;
    .M00_AXI_AWCACHE(M00_AXI_AWCACHE),//=4'b0010
    .M00_AXI_AWPROT(M00_AXI_AWPROT),//=3'h0;
    .M00_AXI_AWQOS(M00_AXI_AWQOS),//=4'h0;
    .M00_AXI_AWVALID(M00_AXI_AWVALID),
    .M00_AXI_AWREADY(M00_AXI_AWREADY),
    .M00_AXI_WDATA(M00_AXI_WDATA),
    .M00_AXI_WSTRB(M00_AXI_WSTRB),
    .M00_AXI_WLAST(M00_AXI_WLAST),
    .M00_AXI_WVALID(M00_AXI_WVALID),
    .M00_AXI_WREADY(M00_AXI_WREADY),
    .M00_AXI_BID(M00_AXI_BID),//ignore
    .M00_AXI_BRESP(M00_AXI_BRESP),//ignore
    .M00_AXI_BVALID(M00_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M00_AXI_BREADY(M00_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M00_AXI_ARID(M00_AXI_ARID),
    .M00_AXI_ARADDR(M00_AXI_ARADDR),
    .M00_AXI_ARLEN(M00_AXI_ARLEN),
    .M00_AXI_ARSIZE(M00_AXI_ARSIZE),//=clogb2((M00_AXI_DATA_WIDTH/8)-1);
    .M00_AXI_ARBURST(M00_AXI_ARBURST),//=2'b01;
    .M00_AXI_ARLOCK(M00_AXI_ARLOCK),//=1'b0;
    .M00_AXI_ARCACHE(M00_AXI_ARCACHE),//=4'b0010;
    .M00_AXI_ARPROT(M00_AXI_ARPROT),//=3'h0;
    .M00_AXI_ARQOS(M00_AXI_ARQOS),//=4'h0;
    .M00_AXI_ARVALID(M00_AXI_ARVALID),
    .M00_AXI_ARREADY(M00_AXI_ARREADY),
    .M00_AXI_RID(M00_AXI_RID),
    .M00_AXI_RDATA(M00_AXI_RDATA),
    .M00_AXI_RRESP(M00_AXI_RRESP),//ignore
    .M00_AXI_RLAST(M00_AXI_RLAST),
    .M00_AXI_RVALID(M00_AXI_RVALID),
    .M00_AXI_RREADY(M00_AXI_RREADY),


	//AXI to Mem01 for HBM wt
    .M01_AXI_AWID(M01_AXI_AWID),
    .M01_AXI_AWADDR(M01_AXI_AWADDR),
    .M01_AXI_AWLEN(M01_AXI_AWLEN),
    .M01_AXI_AWSIZE(M01_AXI_AWSIZE),//=clogb2((M01_AXI_DATA_WIDTH/8)-1);
    .M01_AXI_AWBURST(M01_AXI_AWBURST),//=2'b01;
    .M01_AXI_AWLOCK(M01_AXI_AWLOCK),//1'b0;
    .M01_AXI_AWCACHE(M01_AXI_AWCACHE),//=4'b0010
    .M01_AXI_AWPROT(M01_AXI_AWPROT),//=3'h0;
    .M01_AXI_AWQOS(M01_AXI_AWQOS),//=4'h0;
    .M01_AXI_AWVALID(M01_AXI_AWVALID),
    .M01_AXI_AWREADY(M01_AXI_AWREADY),
    .M01_AXI_WDATA(M01_AXI_WDATA),
    .M01_AXI_WSTRB(M01_AXI_WSTRB),
    .M01_AXI_WLAST(M01_AXI_WLAST),
    .M01_AXI_WVALID(M01_AXI_WVALID),
    .M01_AXI_WREADY(M01_AXI_WREADY),
    .M01_AXI_BID(M01_AXI_BID),//ignore
    .M01_AXI_BRESP(M01_AXI_BRESP),//ignore
    .M01_AXI_BVALID(M01_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M01_AXI_BREADY(M01_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M01_AXI_ARID(M01_AXI_ARID),
    .M01_AXI_ARADDR(M01_AXI_ARADDR),
    .M01_AXI_ARLEN(M01_AXI_ARLEN),
    .M01_AXI_ARSIZE(M01_AXI_ARSIZE),//=clogb2((M01_AXI_DATA_WIDTH/8)-1);
    .M01_AXI_ARBURST(M01_AXI_ARBURST),//=2'b01;
    .M01_AXI_ARLOCK(M01_AXI_ARLOCK),//=1'b0;
    .M01_AXI_ARCACHE(M01_AXI_ARCACHE),//=4'b0010;
    .M01_AXI_ARPROT(M01_AXI_ARPROT),//=3'h0;
    .M01_AXI_ARQOS(M01_AXI_ARQOS),//=4'h0;
    .M01_AXI_ARVALID(M01_AXI_ARVALID),
    .M01_AXI_ARREADY(M01_AXI_ARREADY),
    .M01_AXI_RID(M01_AXI_RID),
    .M01_AXI_RDATA(M01_AXI_RDATA),
    .M01_AXI_RRESP(M01_AXI_RRESP),//ignore
    .M01_AXI_RLAST(M01_AXI_RLAST),
    .M01_AXI_RVALID(M01_AXI_RVALID),
    .M01_AXI_RREADY(M01_AXI_RREADY),


	//AXI to Mem02 for HBM wt
    .M02_AXI_AWID(M02_AXI_AWID),
    .M02_AXI_AWADDR(M02_AXI_AWADDR),
    .M02_AXI_AWLEN(M02_AXI_AWLEN),
    .M02_AXI_AWSIZE(M02_AXI_AWSIZE),//=clogb2((M02_AXI_DATA_WIDTH/8)-1);
    .M02_AXI_AWBURST(M02_AXI_AWBURST),//=2'b01;
    .M02_AXI_AWLOCK(M02_AXI_AWLOCK),//1'b0;
    .M02_AXI_AWCACHE(M02_AXI_AWCACHE),//=4'b0010
    .M02_AXI_AWPROT(M02_AXI_AWPROT),//=3'h0;
    .M02_AXI_AWQOS(M02_AXI_AWQOS),//=4'h0;
    .M02_AXI_AWVALID(M02_AXI_AWVALID),
    .M02_AXI_AWREADY(M02_AXI_AWREADY),
    .M02_AXI_WDATA(M02_AXI_WDATA),
    .M02_AXI_WSTRB(M02_AXI_WSTRB),
    .M02_AXI_WLAST(M02_AXI_WLAST),
    .M02_AXI_WVALID(M02_AXI_WVALID),
    .M02_AXI_WREADY(M02_AXI_WREADY),
    .M02_AXI_BID(M02_AXI_BID),//ignore
    .M02_AXI_BRESP(M02_AXI_BRESP),//ignore
    .M02_AXI_BVALID(M02_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M02_AXI_BREADY(M02_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M02_AXI_ARID(M02_AXI_ARID),
    .M02_AXI_ARADDR(M02_AXI_ARADDR),
    .M02_AXI_ARLEN(M02_AXI_ARLEN),
    .M02_AXI_ARSIZE(M02_AXI_ARSIZE),//=clogb2((M02_AXI_DATA_WIDTH/8)-1);
    .M02_AXI_ARBURST(M02_AXI_ARBURST),//=2'b01;
    .M02_AXI_ARLOCK(M02_AXI_ARLOCK),//=1'b0;
    .M02_AXI_ARCACHE(M02_AXI_ARCACHE),//=4'b0010;
    .M02_AXI_ARPROT(M02_AXI_ARPROT),//=3'h0;
    .M02_AXI_ARQOS(M02_AXI_ARQOS),//=4'h0;
    .M02_AXI_ARVALID(M02_AXI_ARVALID),
    .M02_AXI_ARREADY(M02_AXI_ARREADY),
    .M02_AXI_RID(M02_AXI_RID),
    .M02_AXI_RDATA(M02_AXI_RDATA),
    .M02_AXI_RRESP(M02_AXI_RRESP),//ignore
    .M02_AXI_RLAST(M02_AXI_RLAST),
    .M02_AXI_RVALID(M02_AXI_RVALID),
    .M02_AXI_RREADY(M02_AXI_RREADY),


	//AXI to Mem03 for HBM wt
    .M03_AXI_AWID(M03_AXI_AWID),
    .M03_AXI_AWADDR(M03_AXI_AWADDR),
    .M03_AXI_AWLEN(M03_AXI_AWLEN),
    .M03_AXI_AWSIZE(M03_AXI_AWSIZE),//=clogb2((M03_AXI_DATA_WIDTH/8)-1);
    .M03_AXI_AWBURST(M03_AXI_AWBURST),//=2'b01;
    .M03_AXI_AWLOCK(M03_AXI_AWLOCK),//1'b0;
    .M03_AXI_AWCACHE(M03_AXI_AWCACHE),//=4'b0010
    .M03_AXI_AWPROT(M03_AXI_AWPROT),//=3'h0;
    .M03_AXI_AWQOS(M03_AXI_AWQOS),//=4'h0;
    .M03_AXI_AWVALID(M03_AXI_AWVALID),
    .M03_AXI_AWREADY(M03_AXI_AWREADY),
    .M03_AXI_WDATA(M03_AXI_WDATA),
    .M03_AXI_WSTRB(M03_AXI_WSTRB),
    .M03_AXI_WLAST(M03_AXI_WLAST),
    .M03_AXI_WVALID(M03_AXI_WVALID),
    .M03_AXI_WREADY(M03_AXI_WREADY),
    .M03_AXI_BID(M03_AXI_BID),//ignore
    .M03_AXI_BRESP(M03_AXI_BRESP),//ignore
    .M03_AXI_BVALID(M03_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M03_AXI_BREADY(M03_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M03_AXI_ARID(M03_AXI_ARID),
    .M03_AXI_ARADDR(M03_AXI_ARADDR),
    .M03_AXI_ARLEN(M03_AXI_ARLEN),
    .M03_AXI_ARSIZE(M03_AXI_ARSIZE),//=clogb2((M03_AXI_DATA_WIDTH/8)-1);
    .M03_AXI_ARBURST(M03_AXI_ARBURST),//=2'b01;
    .M03_AXI_ARLOCK(M03_AXI_ARLOCK),//=1'b0;
    .M03_AXI_ARCACHE(M03_AXI_ARCACHE),//=4'b0010;
    .M03_AXI_ARPROT(M03_AXI_ARPROT),//=3'h0;
    .M03_AXI_ARQOS(M03_AXI_ARQOS),//=4'h0;
    .M03_AXI_ARVALID(M03_AXI_ARVALID),
    .M03_AXI_ARREADY(M03_AXI_ARREADY),
    .M03_AXI_RID(M03_AXI_RID),
    .M03_AXI_RDATA(M03_AXI_RDATA),
    .M03_AXI_RRESP(M03_AXI_RRESP),//ignore
    .M03_AXI_RLAST(M03_AXI_RLAST),
    .M03_AXI_RVALID(M03_AXI_RVALID),
    .M03_AXI_RREADY(M03_AXI_RREADY),


	//AXI to Mem04 for HBM wt
    .M04_AXI_AWID(M04_AXI_AWID),
    .M04_AXI_AWADDR(M04_AXI_AWADDR),
    .M04_AXI_AWLEN(M04_AXI_AWLEN),
    .M04_AXI_AWSIZE(M04_AXI_AWSIZE),//=clogb2((M04_AXI_DATA_WIDTH/8)-1);
    .M04_AXI_AWBURST(M04_AXI_AWBURST),//=2'b01;
    .M04_AXI_AWLOCK(M04_AXI_AWLOCK),//1'b0;
    .M04_AXI_AWCACHE(M04_AXI_AWCACHE),//=4'b0010
    .M04_AXI_AWPROT(M04_AXI_AWPROT),//=3'h0;
    .M04_AXI_AWQOS(M04_AXI_AWQOS),//=4'h0;
    .M04_AXI_AWVALID(M04_AXI_AWVALID),
    .M04_AXI_AWREADY(M04_AXI_AWREADY),
    .M04_AXI_WDATA(M04_AXI_WDATA),
    .M04_AXI_WSTRB(M04_AXI_WSTRB),
    .M04_AXI_WLAST(M04_AXI_WLAST),
    .M04_AXI_WVALID(M04_AXI_WVALID),
    .M04_AXI_WREADY(M04_AXI_WREADY),
    .M04_AXI_BID(M04_AXI_BID),//ignore
    .M04_AXI_BRESP(M04_AXI_BRESP),//ignore
    .M04_AXI_BVALID(M04_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M04_AXI_BREADY(M04_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M04_AXI_ARID(M04_AXI_ARID),
    .M04_AXI_ARADDR(M04_AXI_ARADDR),
    .M04_AXI_ARLEN(M04_AXI_ARLEN),
    .M04_AXI_ARSIZE(M04_AXI_ARSIZE),//=clogb2((M04_AXI_DATA_WIDTH/8)-1);
    .M04_AXI_ARBURST(M04_AXI_ARBURST),//=2'b01;
    .M04_AXI_ARLOCK(M04_AXI_ARLOCK),//=1'b0;
    .M04_AXI_ARCACHE(M04_AXI_ARCACHE),//=4'b0010;
    .M04_AXI_ARPROT(M04_AXI_ARPROT),//=3'h0;
    .M04_AXI_ARQOS(M04_AXI_ARQOS),//=4'h0;
    .M04_AXI_ARVALID(M04_AXI_ARVALID),
    .M04_AXI_ARREADY(M04_AXI_ARREADY),
    .M04_AXI_RID(M04_AXI_RID),
    .M04_AXI_RDATA(M04_AXI_RDATA),
    .M04_AXI_RRESP(M04_AXI_RRESP),//ignore
    .M04_AXI_RLAST(M04_AXI_RLAST),
    .M04_AXI_RVALID(M04_AXI_RVALID),
    .M04_AXI_RREADY(M04_AXI_RREADY),


	//AXI to Mem05 for HBM wt
    .M05_AXI_AWID(M05_AXI_AWID),
    .M05_AXI_AWADDR(M05_AXI_AWADDR),
    .M05_AXI_AWLEN(M05_AXI_AWLEN),
    .M05_AXI_AWSIZE(M05_AXI_AWSIZE),//=clogb2((M05_AXI_DATA_WIDTH/8)-1);
    .M05_AXI_AWBURST(M05_AXI_AWBURST),//=2'b01;
    .M05_AXI_AWLOCK(M05_AXI_AWLOCK),//1'b0;
    .M05_AXI_AWCACHE(M05_AXI_AWCACHE),//=4'b0010
    .M05_AXI_AWPROT(M05_AXI_AWPROT),//=3'h0;
    .M05_AXI_AWQOS(M05_AXI_AWQOS),//=4'h0;
    .M05_AXI_AWVALID(M05_AXI_AWVALID),
    .M05_AXI_AWREADY(M05_AXI_AWREADY),
    .M05_AXI_WDATA(M05_AXI_WDATA),
    .M05_AXI_WSTRB(M05_AXI_WSTRB),
    .M05_AXI_WLAST(M05_AXI_WLAST),
    .M05_AXI_WVALID(M05_AXI_WVALID),
    .M05_AXI_WREADY(M05_AXI_WREADY),
    .M05_AXI_BID(M05_AXI_BID),//ignore
    .M05_AXI_BRESP(M05_AXI_BRESP),//ignore
    .M05_AXI_BVALID(M05_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M05_AXI_BREADY(M05_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M05_AXI_ARID(M05_AXI_ARID),
    .M05_AXI_ARADDR(M05_AXI_ARADDR),
    .M05_AXI_ARLEN(M05_AXI_ARLEN),
    .M05_AXI_ARSIZE(M05_AXI_ARSIZE),//=clogb2((M05_AXI_DATA_WIDTH/8)-1);
    .M05_AXI_ARBURST(M05_AXI_ARBURST),//=2'b01;
    .M05_AXI_ARLOCK(M05_AXI_ARLOCK),//=1'b0;
    .M05_AXI_ARCACHE(M05_AXI_ARCACHE),//=4'b0010;
    .M05_AXI_ARPROT(M05_AXI_ARPROT),//=3'h0;
    .M05_AXI_ARQOS(M05_AXI_ARQOS),//=4'h0;
    .M05_AXI_ARVALID(M05_AXI_ARVALID),
    .M05_AXI_ARREADY(M05_AXI_ARREADY),
    .M05_AXI_RID(M05_AXI_RID),
    .M05_AXI_RDATA(M05_AXI_RDATA),
    .M05_AXI_RRESP(M05_AXI_RRESP),//ignore
    .M05_AXI_RLAST(M05_AXI_RLAST),
    .M05_AXI_RVALID(M05_AXI_RVALID),
    .M05_AXI_RREADY(M05_AXI_RREADY),


	//AXI to Mem06 for HBM wt
    .M06_AXI_AWID(M06_AXI_AWID),
    .M06_AXI_AWADDR(M06_AXI_AWADDR),
    .M06_AXI_AWLEN(M06_AXI_AWLEN),
    .M06_AXI_AWSIZE(M06_AXI_AWSIZE),//=clogb2((M06_AXI_DATA_WIDTH/8)-1);
    .M06_AXI_AWBURST(M06_AXI_AWBURST),//=2'b01;
    .M06_AXI_AWLOCK(M06_AXI_AWLOCK),//1'b0;
    .M06_AXI_AWCACHE(M06_AXI_AWCACHE),//=4'b0010
    .M06_AXI_AWPROT(M06_AXI_AWPROT),//=3'h0;
    .M06_AXI_AWQOS(M06_AXI_AWQOS),//=4'h0;
    .M06_AXI_AWVALID(M06_AXI_AWVALID),
    .M06_AXI_AWREADY(M06_AXI_AWREADY),
    .M06_AXI_WDATA(M06_AXI_WDATA),
    .M06_AXI_WSTRB(M06_AXI_WSTRB),
    .M06_AXI_WLAST(M06_AXI_WLAST),
    .M06_AXI_WVALID(M06_AXI_WVALID),
    .M06_AXI_WREADY(M06_AXI_WREADY),
    .M06_AXI_BID(M06_AXI_BID),//ignore
    .M06_AXI_BRESP(M06_AXI_BRESP),//ignore
    .M06_AXI_BVALID(M06_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M06_AXI_BREADY(M06_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M06_AXI_ARID(M06_AXI_ARID),
    .M06_AXI_ARADDR(M06_AXI_ARADDR),
    .M06_AXI_ARLEN(M06_AXI_ARLEN),
    .M06_AXI_ARSIZE(M06_AXI_ARSIZE),//=clogb2((M06_AXI_DATA_WIDTH/8)-1);
    .M06_AXI_ARBURST(M06_AXI_ARBURST),//=2'b01;
    .M06_AXI_ARLOCK(M06_AXI_ARLOCK),//=1'b0;
    .M06_AXI_ARCACHE(M06_AXI_ARCACHE),//=4'b0010;
    .M06_AXI_ARPROT(M06_AXI_ARPROT),//=3'h0;
    .M06_AXI_ARQOS(M06_AXI_ARQOS),//=4'h0;
    .M06_AXI_ARVALID(M06_AXI_ARVALID),
    .M06_AXI_ARREADY(M06_AXI_ARREADY),
    .M06_AXI_RID(M06_AXI_RID),
    .M06_AXI_RDATA(M06_AXI_RDATA),
    .M06_AXI_RRESP(M06_AXI_RRESP),//ignore
    .M06_AXI_RLAST(M06_AXI_RLAST),
    .M06_AXI_RVALID(M06_AXI_RVALID),
    .M06_AXI_RREADY(M06_AXI_RREADY),


	//AXI to Mem07 for HBM wt
    .M07_AXI_AWID(M07_AXI_AWID),
    .M07_AXI_AWADDR(M07_AXI_AWADDR),
    .M07_AXI_AWLEN(M07_AXI_AWLEN),
    .M07_AXI_AWSIZE(M07_AXI_AWSIZE),//=clogb2((M07_AXI_DATA_WIDTH/8)-1);
    .M07_AXI_AWBURST(M07_AXI_AWBURST),//=2'b01;
    .M07_AXI_AWLOCK(M07_AXI_AWLOCK),//1'b0;
    .M07_AXI_AWCACHE(M07_AXI_AWCACHE),//=4'b0010
    .M07_AXI_AWPROT(M07_AXI_AWPROT),//=3'h0;
    .M07_AXI_AWQOS(M07_AXI_AWQOS),//=4'h0;
    .M07_AXI_AWVALID(M07_AXI_AWVALID),
    .M07_AXI_AWREADY(M07_AXI_AWREADY),
    .M07_AXI_WDATA(M07_AXI_WDATA),
    .M07_AXI_WSTRB(M07_AXI_WSTRB),
    .M07_AXI_WLAST(M07_AXI_WLAST),
    .M07_AXI_WVALID(M07_AXI_WVALID),
    .M07_AXI_WREADY(M07_AXI_WREADY),
    .M07_AXI_BID(M07_AXI_BID),//ignore
    .M07_AXI_BRESP(M07_AXI_BRESP),//ignore
    .M07_AXI_BVALID(M07_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M07_AXI_BREADY(M07_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M07_AXI_ARID(M07_AXI_ARID),
    .M07_AXI_ARADDR(M07_AXI_ARADDR),
    .M07_AXI_ARLEN(M07_AXI_ARLEN),
    .M07_AXI_ARSIZE(M07_AXI_ARSIZE),//=clogb2((M07_AXI_DATA_WIDTH/8)-1);
    .M07_AXI_ARBURST(M07_AXI_ARBURST),//=2'b01;
    .M07_AXI_ARLOCK(M07_AXI_ARLOCK),//=1'b0;
    .M07_AXI_ARCACHE(M07_AXI_ARCACHE),//=4'b0010;
    .M07_AXI_ARPROT(M07_AXI_ARPROT),//=3'h0;
    .M07_AXI_ARQOS(M07_AXI_ARQOS),//=4'h0;
    .M07_AXI_ARVALID(M07_AXI_ARVALID),
    .M07_AXI_ARREADY(M07_AXI_ARREADY),
    .M07_AXI_RID(M07_AXI_RID),
    .M07_AXI_RDATA(M07_AXI_RDATA),
    .M07_AXI_RRESP(M07_AXI_RRESP),//ignore
    .M07_AXI_RLAST(M07_AXI_RLAST),
    .M07_AXI_RVALID(M07_AXI_RVALID),
    .M07_AXI_RREADY(M07_AXI_RREADY),


	//AXI to Mem08 for HBM wt
    .M08_AXI_AWID(M08_AXI_AWID),
    .M08_AXI_AWADDR(M08_AXI_AWADDR),
    .M08_AXI_AWLEN(M08_AXI_AWLEN),
    .M08_AXI_AWSIZE(M08_AXI_AWSIZE),//=clogb2((M08_AXI_DATA_WIDTH/8)-1);
    .M08_AXI_AWBURST(M08_AXI_AWBURST),//=2'b01;
    .M08_AXI_AWLOCK(M08_AXI_AWLOCK),//1'b0;
    .M08_AXI_AWCACHE(M08_AXI_AWCACHE),//=4'b0010
    .M08_AXI_AWPROT(M08_AXI_AWPROT),//=3'h0;
    .M08_AXI_AWQOS(M08_AXI_AWQOS),//=4'h0;
    .M08_AXI_AWVALID(M08_AXI_AWVALID),
    .M08_AXI_AWREADY(M08_AXI_AWREADY),
    .M08_AXI_WDATA(M08_AXI_WDATA),
    .M08_AXI_WSTRB(M08_AXI_WSTRB),
    .M08_AXI_WLAST(M08_AXI_WLAST),
    .M08_AXI_WVALID(M08_AXI_WVALID),
    .M08_AXI_WREADY(M08_AXI_WREADY),
    .M08_AXI_BID(M08_AXI_BID),//ignore
    .M08_AXI_BRESP(M08_AXI_BRESP),//ignore
    .M08_AXI_BVALID(M08_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M08_AXI_BREADY(M08_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M08_AXI_ARID(M08_AXI_ARID),
    .M08_AXI_ARADDR(M08_AXI_ARADDR),
    .M08_AXI_ARLEN(M08_AXI_ARLEN),
    .M08_AXI_ARSIZE(M08_AXI_ARSIZE),//=clogb2((M08_AXI_DATA_WIDTH/8)-1);
    .M08_AXI_ARBURST(M08_AXI_ARBURST),//=2'b01;
    .M08_AXI_ARLOCK(M08_AXI_ARLOCK),//=1'b0;
    .M08_AXI_ARCACHE(M08_AXI_ARCACHE),//=4'b0010;
    .M08_AXI_ARPROT(M08_AXI_ARPROT),//=3'h0;
    .M08_AXI_ARQOS(M08_AXI_ARQOS),//=4'h0;
    .M08_AXI_ARVALID(M08_AXI_ARVALID),
    .M08_AXI_ARREADY(M08_AXI_ARREADY),
    .M08_AXI_RID(M08_AXI_RID),
    .M08_AXI_RDATA(M08_AXI_RDATA),
    .M08_AXI_RRESP(M08_AXI_RRESP),//ignore
    .M08_AXI_RLAST(M08_AXI_RLAST),
    .M08_AXI_RVALID(M08_AXI_RVALID),
    .M08_AXI_RREADY(M08_AXI_RREADY),


	//AXI to Mem09 for HBM wt
    .M09_AXI_AWID(M09_AXI_AWID),
    .M09_AXI_AWADDR(M09_AXI_AWADDR),
    .M09_AXI_AWLEN(M09_AXI_AWLEN),
    .M09_AXI_AWSIZE(M09_AXI_AWSIZE),//=clogb2((M09_AXI_DATA_WIDTH/8)-1);
    .M09_AXI_AWBURST(M09_AXI_AWBURST),//=2'b01;
    .M09_AXI_AWLOCK(M09_AXI_AWLOCK),//1'b0;
    .M09_AXI_AWCACHE(M09_AXI_AWCACHE),//=4'b0010
    .M09_AXI_AWPROT(M09_AXI_AWPROT),//=3'h0;
    .M09_AXI_AWQOS(M09_AXI_AWQOS),//=4'h0;
    .M09_AXI_AWVALID(M09_AXI_AWVALID),
    .M09_AXI_AWREADY(M09_AXI_AWREADY),
    .M09_AXI_WDATA(M09_AXI_WDATA),
    .M09_AXI_WSTRB(M09_AXI_WSTRB),
    .M09_AXI_WLAST(M09_AXI_WLAST),
    .M09_AXI_WVALID(M09_AXI_WVALID),
    .M09_AXI_WREADY(M09_AXI_WREADY),
    .M09_AXI_BID(M09_AXI_BID),//ignore
    .M09_AXI_BRESP(M09_AXI_BRESP),//ignore
    .M09_AXI_BVALID(M09_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M09_AXI_BREADY(M09_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M09_AXI_ARID(M09_AXI_ARID),
    .M09_AXI_ARADDR(M09_AXI_ARADDR),
    .M09_AXI_ARLEN(M09_AXI_ARLEN),
    .M09_AXI_ARSIZE(M09_AXI_ARSIZE),//=clogb2((M09_AXI_DATA_WIDTH/8)-1);
    .M09_AXI_ARBURST(M09_AXI_ARBURST),//=2'b01;
    .M09_AXI_ARLOCK(M09_AXI_ARLOCK),//=1'b0;
    .M09_AXI_ARCACHE(M09_AXI_ARCACHE),//=4'b0010;
    .M09_AXI_ARPROT(M09_AXI_ARPROT),//=3'h0;
    .M09_AXI_ARQOS(M09_AXI_ARQOS),//=4'h0;
    .M09_AXI_ARVALID(M09_AXI_ARVALID),
    .M09_AXI_ARREADY(M09_AXI_ARREADY),
    .M09_AXI_RID(M09_AXI_RID),
    .M09_AXI_RDATA(M09_AXI_RDATA),
    .M09_AXI_RRESP(M09_AXI_RRESP),//ignore
    .M09_AXI_RLAST(M09_AXI_RLAST),
    .M09_AXI_RVALID(M09_AXI_RVALID),
    .M09_AXI_RREADY(M09_AXI_RREADY),


	//AXI to Mem10 for HBM wt
    .M10_AXI_AWID(M10_AXI_AWID),
    .M10_AXI_AWADDR(M10_AXI_AWADDR),
    .M10_AXI_AWLEN(M10_AXI_AWLEN),
    .M10_AXI_AWSIZE(M10_AXI_AWSIZE),//=clogb2((M10_AXI_DATA_WIDTH/8)-1);
    .M10_AXI_AWBURST(M10_AXI_AWBURST),//=2'b01;
    .M10_AXI_AWLOCK(M10_AXI_AWLOCK),//1'b0;
    .M10_AXI_AWCACHE(M10_AXI_AWCACHE),//=4'b0010
    .M10_AXI_AWPROT(M10_AXI_AWPROT),//=3'h0;
    .M10_AXI_AWQOS(M10_AXI_AWQOS),//=4'h0;
    .M10_AXI_AWVALID(M10_AXI_AWVALID),
    .M10_AXI_AWREADY(M10_AXI_AWREADY),
    .M10_AXI_WDATA(M10_AXI_WDATA),
    .M10_AXI_WSTRB(M10_AXI_WSTRB),
    .M10_AXI_WLAST(M10_AXI_WLAST),
    .M10_AXI_WVALID(M10_AXI_WVALID),
    .M10_AXI_WREADY(M10_AXI_WREADY),
    .M10_AXI_BID(M10_AXI_BID),//ignore
    .M10_AXI_BRESP(M10_AXI_BRESP),//ignore
    .M10_AXI_BVALID(M10_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M10_AXI_BREADY(M10_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M10_AXI_ARID(M10_AXI_ARID),
    .M10_AXI_ARADDR(M10_AXI_ARADDR),
    .M10_AXI_ARLEN(M10_AXI_ARLEN),
    .M10_AXI_ARSIZE(M10_AXI_ARSIZE),//=clogb2((M10_AXI_DATA_WIDTH/8)-1);
    .M10_AXI_ARBURST(M10_AXI_ARBURST),//=2'b01;
    .M10_AXI_ARLOCK(M10_AXI_ARLOCK),//=1'b0;
    .M10_AXI_ARCACHE(M10_AXI_ARCACHE),//=4'b0010;
    .M10_AXI_ARPROT(M10_AXI_ARPROT),//=3'h0;
    .M10_AXI_ARQOS(M10_AXI_ARQOS),//=4'h0;
    .M10_AXI_ARVALID(M10_AXI_ARVALID),
    .M10_AXI_ARREADY(M10_AXI_ARREADY),
    .M10_AXI_RID(M10_AXI_RID),
    .M10_AXI_RDATA(M10_AXI_RDATA),
    .M10_AXI_RRESP(M10_AXI_RRESP),//ignore
    .M10_AXI_RLAST(M10_AXI_RLAST),
    .M10_AXI_RVALID(M10_AXI_RVALID),
    .M10_AXI_RREADY(M10_AXI_RREADY),
	

	//AXI to Mem11 for HBM wt
    .M11_AXI_AWID(M11_AXI_AWID),
    .M11_AXI_AWADDR(M11_AXI_AWADDR),
    .M11_AXI_AWLEN(M11_AXI_AWLEN),
    .M11_AXI_AWSIZE(M11_AXI_AWSIZE),//=clogb2((M11_AXI_DATA_WIDTH/8)-1);
    .M11_AXI_AWBURST(M11_AXI_AWBURST),//=2'b01;
    .M11_AXI_AWLOCK(M11_AXI_AWLOCK),//1'b0;
    .M11_AXI_AWCACHE(M11_AXI_AWCACHE),//=4'b0010
    .M11_AXI_AWPROT(M11_AXI_AWPROT),//=3'h0;
    .M11_AXI_AWQOS(M11_AXI_AWQOS),//=4'h0;
    .M11_AXI_AWVALID(M11_AXI_AWVALID),
    .M11_AXI_AWREADY(M11_AXI_AWREADY),
    .M11_AXI_WDATA(M11_AXI_WDATA),
    .M11_AXI_WSTRB(M11_AXI_WSTRB),
    .M11_AXI_WLAST(M11_AXI_WLAST),
    .M11_AXI_WVALID(M11_AXI_WVALID),
    .M11_AXI_WREADY(M11_AXI_WREADY),
    .M11_AXI_BID(M11_AXI_BID),//ignore
    .M11_AXI_BRESP(M11_AXI_BRESP),//ignore
    .M11_AXI_BVALID(M11_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M11_AXI_BREADY(M11_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M11_AXI_ARID(M11_AXI_ARID),
    .M11_AXI_ARADDR(M11_AXI_ARADDR),
    .M11_AXI_ARLEN(M11_AXI_ARLEN),
    .M11_AXI_ARSIZE(M11_AXI_ARSIZE),//=clogb2((M11_AXI_DATA_WIDTH/8)-1);
    .M11_AXI_ARBURST(M11_AXI_ARBURST),//=2'b01;
    .M11_AXI_ARLOCK(M11_AXI_ARLOCK),//=1'b0;
    .M11_AXI_ARCACHE(M11_AXI_ARCACHE),//=4'b0010;
    .M11_AXI_ARPROT(M11_AXI_ARPROT),//=3'h0;
    .M11_AXI_ARQOS(M11_AXI_ARQOS),//=4'h0;
    .M11_AXI_ARVALID(M11_AXI_ARVALID),
    .M11_AXI_ARREADY(M11_AXI_ARREADY),
    .M11_AXI_RID(M11_AXI_RID),
    .M11_AXI_RDATA(M11_AXI_RDATA),
    .M11_AXI_RRESP(M11_AXI_RRESP),//ignore
    .M11_AXI_RLAST(M11_AXI_RLAST),
    .M11_AXI_RVALID(M11_AXI_RVALID),
    .M11_AXI_RREADY(M11_AXI_RREADY),


	//AXI to Mem12 for HBM wt
    .M12_AXI_AWID(M12_AXI_AWID),
    .M12_AXI_AWADDR(M12_AXI_AWADDR),
    .M12_AXI_AWLEN(M12_AXI_AWLEN),
    .M12_AXI_AWSIZE(M12_AXI_AWSIZE),//=clogb2((M12_AXI_DATA_WIDTH/8)-1);
    .M12_AXI_AWBURST(M12_AXI_AWBURST),//=2'b01;
    .M12_AXI_AWLOCK(M12_AXI_AWLOCK),//1'b0;
    .M12_AXI_AWCACHE(M12_AXI_AWCACHE),//=4'b0010
    .M12_AXI_AWPROT(M12_AXI_AWPROT),//=3'h0;
    .M12_AXI_AWQOS(M12_AXI_AWQOS),//=4'h0;
    .M12_AXI_AWVALID(M12_AXI_AWVALID),
    .M12_AXI_AWREADY(M12_AXI_AWREADY),
    .M12_AXI_WDATA(M12_AXI_WDATA),
    .M12_AXI_WSTRB(M12_AXI_WSTRB),
    .M12_AXI_WLAST(M12_AXI_WLAST),
    .M12_AXI_WVALID(M12_AXI_WVALID),
    .M12_AXI_WREADY(M12_AXI_WREADY),
    .M12_AXI_BID(M12_AXI_BID),//ignore
    .M12_AXI_BRESP(M12_AXI_BRESP),//ignore
    .M12_AXI_BVALID(M12_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M12_AXI_BREADY(M12_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M12_AXI_ARID(M12_AXI_ARID),
    .M12_AXI_ARADDR(M12_AXI_ARADDR),
    .M12_AXI_ARLEN(M12_AXI_ARLEN),
    .M12_AXI_ARSIZE(M12_AXI_ARSIZE),//=clogb2((M12_AXI_DATA_WIDTH/8)-1);
    .M12_AXI_ARBURST(M12_AXI_ARBURST),//=2'b01;
    .M12_AXI_ARLOCK(M12_AXI_ARLOCK),//=1'b0;
    .M12_AXI_ARCACHE(M12_AXI_ARCACHE),//=4'b0010;
    .M12_AXI_ARPROT(M12_AXI_ARPROT),//=3'h0;
    .M12_AXI_ARQOS(M12_AXI_ARQOS),//=4'h0;
    .M12_AXI_ARVALID(M12_AXI_ARVALID),
    .M12_AXI_ARREADY(M12_AXI_ARREADY),
    .M12_AXI_RID(M12_AXI_RID),
    .M12_AXI_RDATA(M12_AXI_RDATA),
    .M12_AXI_RRESP(M12_AXI_RRESP),//ignore
    .M12_AXI_RLAST(M12_AXI_RLAST),
    .M12_AXI_RVALID(M12_AXI_RVALID),
    .M12_AXI_RREADY(M12_AXI_RREADY),


	//AXI to Mem13 for HBM wt
    .M13_AXI_AWID(M13_AXI_AWID),
    .M13_AXI_AWADDR(M13_AXI_AWADDR),
    .M13_AXI_AWLEN(M13_AXI_AWLEN),
    .M13_AXI_AWSIZE(M13_AXI_AWSIZE),//=clogb2((M13_AXI_DATA_WIDTH/8)-1);
    .M13_AXI_AWBURST(M13_AXI_AWBURST),//=2'b01;
    .M13_AXI_AWLOCK(M13_AXI_AWLOCK),//1'b0;
    .M13_AXI_AWCACHE(M13_AXI_AWCACHE),//=4'b0010
    .M13_AXI_AWPROT(M13_AXI_AWPROT),//=3'h0;
    .M13_AXI_AWQOS(M13_AXI_AWQOS),//=4'h0;
    .M13_AXI_AWVALID(M13_AXI_AWVALID),
    .M13_AXI_AWREADY(M13_AXI_AWREADY),
    .M13_AXI_WDATA(M13_AXI_WDATA),
    .M13_AXI_WSTRB(M13_AXI_WSTRB),
    .M13_AXI_WLAST(M13_AXI_WLAST),
    .M13_AXI_WVALID(M13_AXI_WVALID),
    .M13_AXI_WREADY(M13_AXI_WREADY),
    .M13_AXI_BID(M13_AXI_BID),//ignore
    .M13_AXI_BRESP(M13_AXI_BRESP),//ignore
    .M13_AXI_BVALID(M13_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M13_AXI_BREADY(M13_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M13_AXI_ARID(M13_AXI_ARID),
    .M13_AXI_ARADDR(M13_AXI_ARADDR),
    .M13_AXI_ARLEN(M13_AXI_ARLEN),
    .M13_AXI_ARSIZE(M13_AXI_ARSIZE),//=clogb2((M13_AXI_DATA_WIDTH/8)-1);
    .M13_AXI_ARBURST(M13_AXI_ARBURST),//=2'b01;
    .M13_AXI_ARLOCK(M13_AXI_ARLOCK),//=1'b0;
    .M13_AXI_ARCACHE(M13_AXI_ARCACHE),//=4'b0010;
    .M13_AXI_ARPROT(M13_AXI_ARPROT),//=3'h0;
    .M13_AXI_ARQOS(M13_AXI_ARQOS),//=4'h0;
    .M13_AXI_ARVALID(M13_AXI_ARVALID),
    .M13_AXI_ARREADY(M13_AXI_ARREADY),
    .M13_AXI_RID(M13_AXI_RID),
    .M13_AXI_RDATA(M13_AXI_RDATA),
    .M13_AXI_RRESP(M13_AXI_RRESP),//ignore
    .M13_AXI_RLAST(M13_AXI_RLAST),
    .M13_AXI_RVALID(M13_AXI_RVALID),
    .M13_AXI_RREADY(M13_AXI_RREADY),


	//AXI to Mem14 for HBM wt
    .M14_AXI_AWID(M14_AXI_AWID),
    .M14_AXI_AWADDR(M14_AXI_AWADDR),
    .M14_AXI_AWLEN(M14_AXI_AWLEN),
    .M14_AXI_AWSIZE(M14_AXI_AWSIZE),//=clogb2((M14_AXI_DATA_WIDTH/8)-1);
    .M14_AXI_AWBURST(M14_AXI_AWBURST),//=2'b01;
    .M14_AXI_AWLOCK(M14_AXI_AWLOCK),//1'b0;
    .M14_AXI_AWCACHE(M14_AXI_AWCACHE),//=4'b0010
    .M14_AXI_AWPROT(M14_AXI_AWPROT),//=3'h0;
    .M14_AXI_AWQOS(M14_AXI_AWQOS),//=4'h0;
    .M14_AXI_AWVALID(M14_AXI_AWVALID),
    .M14_AXI_AWREADY(M14_AXI_AWREADY),
    .M14_AXI_WDATA(M14_AXI_WDATA),
    .M14_AXI_WSTRB(M14_AXI_WSTRB),
    .M14_AXI_WLAST(M14_AXI_WLAST),
    .M14_AXI_WVALID(M14_AXI_WVALID),
    .M14_AXI_WREADY(M14_AXI_WREADY),
    .M14_AXI_BID(M14_AXI_BID),//ignore
    .M14_AXI_BRESP(M14_AXI_BRESP),//ignore
    .M14_AXI_BVALID(M14_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M14_AXI_BREADY(M14_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M14_AXI_ARID(M14_AXI_ARID),
    .M14_AXI_ARADDR(M14_AXI_ARADDR),
    .M14_AXI_ARLEN(M14_AXI_ARLEN),
    .M14_AXI_ARSIZE(M14_AXI_ARSIZE),//=clogb2((M14_AXI_DATA_WIDTH/8)-1);
    .M14_AXI_ARBURST(M14_AXI_ARBURST),//=2'b01;
    .M14_AXI_ARLOCK(M14_AXI_ARLOCK),//=1'b0;
    .M14_AXI_ARCACHE(M14_AXI_ARCACHE),//=4'b0010;
    .M14_AXI_ARPROT(M14_AXI_ARPROT),//=3'h0;
    .M14_AXI_ARQOS(M14_AXI_ARQOS),//=4'h0;
    .M14_AXI_ARVALID(M14_AXI_ARVALID),
    .M14_AXI_ARREADY(M14_AXI_ARREADY),
    .M14_AXI_RID(M14_AXI_RID),
    .M14_AXI_RDATA(M14_AXI_RDATA),
    .M14_AXI_RRESP(M14_AXI_RRESP),//ignore
    .M14_AXI_RLAST(M14_AXI_RLAST),
    .M14_AXI_RVALID(M14_AXI_RVALID),
    .M14_AXI_RREADY(M14_AXI_RREADY),


	//AXI to Mem15 for HBM wt
    .M15_AXI_AWID(M15_AXI_AWID),
    .M15_AXI_AWADDR(M15_AXI_AWADDR),
    .M15_AXI_AWLEN(M15_AXI_AWLEN),
    .M15_AXI_AWSIZE(M15_AXI_AWSIZE),//=clogb2((M15_AXI_DATA_WIDTH/8)-1);
    .M15_AXI_AWBURST(M15_AXI_AWBURST),//=2'b01;
    .M15_AXI_AWLOCK(M15_AXI_AWLOCK),//1'b0;
    .M15_AXI_AWCACHE(M15_AXI_AWCACHE),//=4'b0010
    .M15_AXI_AWPROT(M15_AXI_AWPROT),//=3'h0;
    .M15_AXI_AWQOS(M15_AXI_AWQOS),//=4'h0;
    .M15_AXI_AWVALID(M15_AXI_AWVALID),
    .M15_AXI_AWREADY(M15_AXI_AWREADY),
    .M15_AXI_WDATA(M15_AXI_WDATA),
    .M15_AXI_WSTRB(M15_AXI_WSTRB),
    .M15_AXI_WLAST(M15_AXI_WLAST),
    .M15_AXI_WVALID(M15_AXI_WVALID),
    .M15_AXI_WREADY(M15_AXI_WREADY),
    .M15_AXI_BID(M15_AXI_BID),//ignore
    .M15_AXI_BRESP(M15_AXI_BRESP),//ignore
    .M15_AXI_BVALID(M15_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M15_AXI_BREADY(M15_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M15_AXI_ARID(M15_AXI_ARID),
    .M15_AXI_ARADDR(M15_AXI_ARADDR),
    .M15_AXI_ARLEN(M15_AXI_ARLEN),
    .M15_AXI_ARSIZE(M15_AXI_ARSIZE),//=clogb2((M15_AXI_DATA_WIDTH/8)-1);
    .M15_AXI_ARBURST(M15_AXI_ARBURST),//=2'b01;
    .M15_AXI_ARLOCK(M15_AXI_ARLOCK),//=1'b0;
    .M15_AXI_ARCACHE(M15_AXI_ARCACHE),//=4'b0010;
    .M15_AXI_ARPROT(M15_AXI_ARPROT),//=3'h0;
    .M15_AXI_ARQOS(M15_AXI_ARQOS),//=4'h0;
    .M15_AXI_ARVALID(M15_AXI_ARVALID),
    .M15_AXI_ARREADY(M15_AXI_ARREADY),
    .M15_AXI_RID(M15_AXI_RID),
    .M15_AXI_RDATA(M15_AXI_RDATA),
    .M15_AXI_RRESP(M15_AXI_RRESP),//ignore
    .M15_AXI_RLAST(M15_AXI_RLAST),
    .M15_AXI_RVALID(M15_AXI_RVALID),
    .M15_AXI_RREADY(M15_AXI_RREADY),


	//AXI to Mem16 for HBM wt
    .M16_AXI_AWID(M16_AXI_AWID),
    .M16_AXI_AWADDR(M16_AXI_AWADDR),
    .M16_AXI_AWLEN(M16_AXI_AWLEN),
    .M16_AXI_AWSIZE(M16_AXI_AWSIZE),//=clogb2((M16_AXI_DATA_WIDTH/8)-1);
    .M16_AXI_AWBURST(M16_AXI_AWBURST),//=2'b01;
    .M16_AXI_AWLOCK(M16_AXI_AWLOCK),//1'b0;
    .M16_AXI_AWCACHE(M16_AXI_AWCACHE),//=4'b0010
    .M16_AXI_AWPROT(M16_AXI_AWPROT),//=3'h0;
    .M16_AXI_AWQOS(M16_AXI_AWQOS),//=4'h0;
    .M16_AXI_AWVALID(M16_AXI_AWVALID),
    .M16_AXI_AWREADY(M16_AXI_AWREADY),
    .M16_AXI_WDATA(M16_AXI_WDATA),
    .M16_AXI_WSTRB(M16_AXI_WSTRB),
    .M16_AXI_WLAST(M16_AXI_WLAST),
    .M16_AXI_WVALID(M16_AXI_WVALID),
    .M16_AXI_WREADY(M16_AXI_WREADY),
    .M16_AXI_BID(M16_AXI_BID),//ignore
    .M16_AXI_BRESP(M16_AXI_BRESP),//ignore
    .M16_AXI_BVALID(M16_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M16_AXI_BREADY(M16_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M16_AXI_ARID(M16_AXI_ARID),
    .M16_AXI_ARADDR(M16_AXI_ARADDR),
    .M16_AXI_ARLEN(M16_AXI_ARLEN),
    .M16_AXI_ARSIZE(M16_AXI_ARSIZE),//=clogb2((M16_AXI_DATA_WIDTH/8)-1);
    .M16_AXI_ARBURST(M16_AXI_ARBURST),//=2'b01;
    .M16_AXI_ARLOCK(M16_AXI_ARLOCK),//=1'b0;
    .M16_AXI_ARCACHE(M16_AXI_ARCACHE),//=4'b0010;
    .M16_AXI_ARPROT(M16_AXI_ARPROT),//=3'h0;
    .M16_AXI_ARQOS(M16_AXI_ARQOS),//=4'h0;
    .M16_AXI_ARVALID(M16_AXI_ARVALID),
    .M16_AXI_ARREADY(M16_AXI_ARREADY),
    .M16_AXI_RID(M16_AXI_RID),
    .M16_AXI_RDATA(M16_AXI_RDATA),
    .M16_AXI_RRESP(M16_AXI_RRESP),//ignore
    .M16_AXI_RLAST(M16_AXI_RLAST),
    .M16_AXI_RVALID(M16_AXI_RVALID),
    .M16_AXI_RREADY(M16_AXI_RREADY),


	//AXI to Mem17 for HBM wt
    .M17_AXI_AWID(M17_AXI_AWID),
    .M17_AXI_AWADDR(M17_AXI_AWADDR),
    .M17_AXI_AWLEN(M17_AXI_AWLEN),
    .M17_AXI_AWSIZE(M17_AXI_AWSIZE),//=clogb2((M17_AXI_DATA_WIDTH/8)-1);
    .M17_AXI_AWBURST(M17_AXI_AWBURST),//=2'b01;
    .M17_AXI_AWLOCK(M17_AXI_AWLOCK),//1'b0;
    .M17_AXI_AWCACHE(M17_AXI_AWCACHE),//=4'b0010
    .M17_AXI_AWPROT(M17_AXI_AWPROT),//=3'h0;
    .M17_AXI_AWQOS(M17_AXI_AWQOS),//=4'h0;
    .M17_AXI_AWVALID(M17_AXI_AWVALID),
    .M17_AXI_AWREADY(M17_AXI_AWREADY),
    .M17_AXI_WDATA(M17_AXI_WDATA),
    .M17_AXI_WSTRB(M17_AXI_WSTRB),
    .M17_AXI_WLAST(M17_AXI_WLAST),
    .M17_AXI_WVALID(M17_AXI_WVALID),
    .M17_AXI_WREADY(M17_AXI_WREADY),
    .M17_AXI_BID(M17_AXI_BID),//ignore
    .M17_AXI_BRESP(M17_AXI_BRESP),//ignore
    .M17_AXI_BVALID(M17_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M17_AXI_BREADY(M17_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M17_AXI_ARID(M17_AXI_ARID),
    .M17_AXI_ARADDR(M17_AXI_ARADDR),
    .M17_AXI_ARLEN(M17_AXI_ARLEN),
    .M17_AXI_ARSIZE(M17_AXI_ARSIZE),//=clogb2((M17_AXI_DATA_WIDTH/8)-1);
    .M17_AXI_ARBURST(M17_AXI_ARBURST),//=2'b01;
    .M17_AXI_ARLOCK(M17_AXI_ARLOCK),//=1'b0;
    .M17_AXI_ARCACHE(M17_AXI_ARCACHE),//=4'b0010;
    .M17_AXI_ARPROT(M17_AXI_ARPROT),//=3'h0;
    .M17_AXI_ARQOS(M17_AXI_ARQOS),//=4'h0;
    .M17_AXI_ARVALID(M17_AXI_ARVALID),
    .M17_AXI_ARREADY(M17_AXI_ARREADY),
    .M17_AXI_RID(M17_AXI_RID),
    .M17_AXI_RDATA(M17_AXI_RDATA),
    .M17_AXI_RRESP(M17_AXI_RRESP),//ignore
    .M17_AXI_RLAST(M17_AXI_RLAST),
    .M17_AXI_RVALID(M17_AXI_RVALID),
    .M17_AXI_RREADY(M17_AXI_RREADY),


	//AXI to Mem18 for HBM wt
    .M18_AXI_AWID(M18_AXI_AWID),
    .M18_AXI_AWADDR(M18_AXI_AWADDR),
    .M18_AXI_AWLEN(M18_AXI_AWLEN),
    .M18_AXI_AWSIZE(M18_AXI_AWSIZE),//=clogb2((M18_AXI_DATA_WIDTH/8)-1);
    .M18_AXI_AWBURST(M18_AXI_AWBURST),//=2'b01;
    .M18_AXI_AWLOCK(M18_AXI_AWLOCK),//1'b0;
    .M18_AXI_AWCACHE(M18_AXI_AWCACHE),//=4'b0010
    .M18_AXI_AWPROT(M18_AXI_AWPROT),//=3'h0;
    .M18_AXI_AWQOS(M18_AXI_AWQOS),//=4'h0;
    .M18_AXI_AWVALID(M18_AXI_AWVALID),
    .M18_AXI_AWREADY(M18_AXI_AWREADY),
    .M18_AXI_WDATA(M18_AXI_WDATA),
    .M18_AXI_WSTRB(M18_AXI_WSTRB),
    .M18_AXI_WLAST(M18_AXI_WLAST),
    .M18_AXI_WVALID(M18_AXI_WVALID),
    .M18_AXI_WREADY(M18_AXI_WREADY),
    .M18_AXI_BID(M18_AXI_BID),//ignore
    .M18_AXI_BRESP(M18_AXI_BRESP),//ignore
    .M18_AXI_BVALID(M18_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M18_AXI_BREADY(M18_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M18_AXI_ARID(M18_AXI_ARID),
    .M18_AXI_ARADDR(M18_AXI_ARADDR),
    .M18_AXI_ARLEN(M18_AXI_ARLEN),
    .M18_AXI_ARSIZE(M18_AXI_ARSIZE),//=clogb2((M18_AXI_DATA_WIDTH/8)-1);
    .M18_AXI_ARBURST(M18_AXI_ARBURST),//=2'b01;
    .M18_AXI_ARLOCK(M18_AXI_ARLOCK),//=1'b0;
    .M18_AXI_ARCACHE(M18_AXI_ARCACHE),//=4'b0010;
    .M18_AXI_ARPROT(M18_AXI_ARPROT),//=3'h0;
    .M18_AXI_ARQOS(M18_AXI_ARQOS),//=4'h0;
    .M18_AXI_ARVALID(M18_AXI_ARVALID),
    .M18_AXI_ARREADY(M18_AXI_ARREADY),
    .M18_AXI_RID(M18_AXI_RID),
    .M18_AXI_RDATA(M18_AXI_RDATA),
    .M18_AXI_RRESP(M18_AXI_RRESP),//ignore
    .M18_AXI_RLAST(M18_AXI_RLAST),
    .M18_AXI_RVALID(M18_AXI_RVALID),
    .M18_AXI_RREADY(M18_AXI_RREADY),
	

	//AXI to Mem19 for HBM wt
    .M19_AXI_AWID(M19_AXI_AWID),
    .M19_AXI_AWADDR(M19_AXI_AWADDR),
    .M19_AXI_AWLEN(M19_AXI_AWLEN),
    .M19_AXI_AWSIZE(M19_AXI_AWSIZE),//=clogb2((M19_AXI_DATA_WIDTH/8)-1);
    .M19_AXI_AWBURST(M19_AXI_AWBURST),//=2'b01;
    .M19_AXI_AWLOCK(M19_AXI_AWLOCK),//1'b0;
    .M19_AXI_AWCACHE(M19_AXI_AWCACHE),//=4'b0010
    .M19_AXI_AWPROT(M19_AXI_AWPROT),//=3'h0;
    .M19_AXI_AWQOS(M19_AXI_AWQOS),//=4'h0;
    .M19_AXI_AWVALID(M19_AXI_AWVALID),
    .M19_AXI_AWREADY(M19_AXI_AWREADY),
    .M19_AXI_WDATA(M19_AXI_WDATA),
    .M19_AXI_WSTRB(M19_AXI_WSTRB),
    .M19_AXI_WLAST(M19_AXI_WLAST),
    .M19_AXI_WVALID(M19_AXI_WVALID),
    .M19_AXI_WREADY(M19_AXI_WREADY),
    .M19_AXI_BID(M19_AXI_BID),//ignore
    .M19_AXI_BRESP(M19_AXI_BRESP),//ignore
    .M19_AXI_BVALID(M19_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M19_AXI_BREADY(M19_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M19_AXI_ARID(M19_AXI_ARID),
    .M19_AXI_ARADDR(M19_AXI_ARADDR),
    .M19_AXI_ARLEN(M19_AXI_ARLEN),
    .M19_AXI_ARSIZE(M19_AXI_ARSIZE),//=clogb2((M19_AXI_DATA_WIDTH/8)-1);
    .M19_AXI_ARBURST(M19_AXI_ARBURST),//=2'b01;
    .M19_AXI_ARLOCK(M19_AXI_ARLOCK),//=1'b0;
    .M19_AXI_ARCACHE(M19_AXI_ARCACHE),//=4'b0010;
    .M19_AXI_ARPROT(M19_AXI_ARPROT),//=3'h0;
    .M19_AXI_ARQOS(M19_AXI_ARQOS),//=4'h0;
    .M19_AXI_ARVALID(M19_AXI_ARVALID),
    .M19_AXI_ARREADY(M19_AXI_ARREADY),
    .M19_AXI_RID(M19_AXI_RID),
    .M19_AXI_RDATA(M19_AXI_RDATA),
    .M19_AXI_RRESP(M19_AXI_RRESP),//ignore
    .M19_AXI_RLAST(M19_AXI_RLAST),
    .M19_AXI_RVALID(M19_AXI_RVALID),
    .M19_AXI_RREADY(M19_AXI_RREADY),

	//AXI to Mem20 for HBM wt
    .M20_AXI_AWID(M20_AXI_AWID),
    .M20_AXI_AWADDR(M20_AXI_AWADDR),
    .M20_AXI_AWLEN(M20_AXI_AWLEN),
    .M20_AXI_AWSIZE(M20_AXI_AWSIZE),//=clogb2((M20_AXI_DATA_WIDTH/8)-1);
    .M20_AXI_AWBURST(M20_AXI_AWBURST),//=2'b01;
    .M20_AXI_AWLOCK(M20_AXI_AWLOCK),//1'b0;
    .M20_AXI_AWCACHE(M20_AXI_AWCACHE),//=4'b0010
    .M20_AXI_AWPROT(M20_AXI_AWPROT),//=3'h0;
    .M20_AXI_AWQOS(M20_AXI_AWQOS),//=4'h0;
    .M20_AXI_AWVALID(M20_AXI_AWVALID),
    .M20_AXI_AWREADY(M20_AXI_AWREADY),
    .M20_AXI_WDATA(M20_AXI_WDATA),
    .M20_AXI_WSTRB(M20_AXI_WSTRB),
    .M20_AXI_WLAST(M20_AXI_WLAST),
    .M20_AXI_WVALID(M20_AXI_WVALID),
    .M20_AXI_WREADY(M20_AXI_WREADY),
    .M20_AXI_BID(M20_AXI_BID),//ignore
    .M20_AXI_BRESP(M20_AXI_BRESP),//ignore
    .M20_AXI_BVALID(M20_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M20_AXI_BREADY(M20_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M20_AXI_ARID(M20_AXI_ARID),
    .M20_AXI_ARADDR(M20_AXI_ARADDR),
    .M20_AXI_ARLEN(M20_AXI_ARLEN),
    .M20_AXI_ARSIZE(M20_AXI_ARSIZE),//=clogb2((M20_AXI_DATA_WIDTH/8)-1);
    .M20_AXI_ARBURST(M20_AXI_ARBURST),//=2'b01;
    .M20_AXI_ARLOCK(M20_AXI_ARLOCK),//=1'b0;
    .M20_AXI_ARCACHE(M20_AXI_ARCACHE),//=4'b0010;
    .M20_AXI_ARPROT(M20_AXI_ARPROT),//=3'h0;
    .M20_AXI_ARQOS(M20_AXI_ARQOS),//=4'h0;
    .M20_AXI_ARVALID(M20_AXI_ARVALID),
    .M20_AXI_ARREADY(M20_AXI_ARREADY),
    .M20_AXI_RID(M20_AXI_RID),
    .M20_AXI_RDATA(M20_AXI_RDATA),
    .M20_AXI_RRESP(M20_AXI_RRESP),//ignore
    .M20_AXI_RLAST(M20_AXI_RLAST),
    .M20_AXI_RVALID(M20_AXI_RVALID),
    .M20_AXI_RREADY(M20_AXI_RREADY),


	//AXI to Mem21 for HBM wt
    .M21_AXI_AWID(M21_AXI_AWID),
    .M21_AXI_AWADDR(M21_AXI_AWADDR),
    .M21_AXI_AWLEN(M21_AXI_AWLEN),
    .M21_AXI_AWSIZE(M21_AXI_AWSIZE),//=clogb2((M21_AXI_DATA_WIDTH/8)-1);
    .M21_AXI_AWBURST(M21_AXI_AWBURST),//=2'b01;
    .M21_AXI_AWLOCK(M21_AXI_AWLOCK),//1'b0;
    .M21_AXI_AWCACHE(M21_AXI_AWCACHE),//=4'b0010
    .M21_AXI_AWPROT(M21_AXI_AWPROT),//=3'h0;
    .M21_AXI_AWQOS(M21_AXI_AWQOS),//=4'h0;
    .M21_AXI_AWVALID(M21_AXI_AWVALID),
    .M21_AXI_AWREADY(M21_AXI_AWREADY),
    .M21_AXI_WDATA(M21_AXI_WDATA),
    .M21_AXI_WSTRB(M21_AXI_WSTRB),
    .M21_AXI_WLAST(M21_AXI_WLAST),
    .M21_AXI_WVALID(M21_AXI_WVALID),
    .M21_AXI_WREADY(M21_AXI_WREADY),
    .M21_AXI_BID(M21_AXI_BID),//ignore
    .M21_AXI_BRESP(M21_AXI_BRESP),//ignore
    .M21_AXI_BVALID(M21_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M21_AXI_BREADY(M21_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M21_AXI_ARID(M21_AXI_ARID),
    .M21_AXI_ARADDR(M21_AXI_ARADDR),
    .M21_AXI_ARLEN(M21_AXI_ARLEN),
    .M21_AXI_ARSIZE(M21_AXI_ARSIZE),//=clogb2((M21_AXI_DATA_WIDTH/8)-1);
    .M21_AXI_ARBURST(M21_AXI_ARBURST),//=2'b01;
    .M21_AXI_ARLOCK(M21_AXI_ARLOCK),//=1'b0;
    .M21_AXI_ARCACHE(M21_AXI_ARCACHE),//=4'b0010;
    .M21_AXI_ARPROT(M21_AXI_ARPROT),//=3'h0;
    .M21_AXI_ARQOS(M21_AXI_ARQOS),//=4'h0;
    .M21_AXI_ARVALID(M21_AXI_ARVALID),
    .M21_AXI_ARREADY(M21_AXI_ARREADY),
    .M21_AXI_RID(M21_AXI_RID),
    .M21_AXI_RDATA(M21_AXI_RDATA),
    .M21_AXI_RRESP(M21_AXI_RRESP),//ignore
    .M21_AXI_RLAST(M21_AXI_RLAST),
    .M21_AXI_RVALID(M21_AXI_RVALID),
    .M21_AXI_RREADY(M21_AXI_RREADY),


	//AXI to Mem22 for HBM wt
    .M22_AXI_AWID(M22_AXI_AWID),
    .M22_AXI_AWADDR(M22_AXI_AWADDR),
    .M22_AXI_AWLEN(M22_AXI_AWLEN),
    .M22_AXI_AWSIZE(M22_AXI_AWSIZE),//=clogb2((M22_AXI_DATA_WIDTH/8)-1);
    .M22_AXI_AWBURST(M22_AXI_AWBURST),//=2'b01;
    .M22_AXI_AWLOCK(M22_AXI_AWLOCK),//1'b0;
    .M22_AXI_AWCACHE(M22_AXI_AWCACHE),//=4'b0010
    .M22_AXI_AWPROT(M22_AXI_AWPROT),//=3'h0;
    .M22_AXI_AWQOS(M22_AXI_AWQOS),//=4'h0;
    .M22_AXI_AWVALID(M22_AXI_AWVALID),
    .M22_AXI_AWREADY(M22_AXI_AWREADY),
    .M22_AXI_WDATA(M22_AXI_WDATA),
    .M22_AXI_WSTRB(M22_AXI_WSTRB),
    .M22_AXI_WLAST(M22_AXI_WLAST),
    .M22_AXI_WVALID(M22_AXI_WVALID),
    .M22_AXI_WREADY(M22_AXI_WREADY),
    .M22_AXI_BID(M22_AXI_BID),//ignore
    .M22_AXI_BRESP(M22_AXI_BRESP),//ignore
    .M22_AXI_BVALID(M22_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M22_AXI_BREADY(M22_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M22_AXI_ARID(M22_AXI_ARID),
    .M22_AXI_ARADDR(M22_AXI_ARADDR),
    .M22_AXI_ARLEN(M22_AXI_ARLEN),
    .M22_AXI_ARSIZE(M22_AXI_ARSIZE),//=clogb2((M22_AXI_DATA_WIDTH/8)-1);
    .M22_AXI_ARBURST(M22_AXI_ARBURST),//=2'b01;
    .M22_AXI_ARLOCK(M22_AXI_ARLOCK),//=1'b0;
    .M22_AXI_ARCACHE(M22_AXI_ARCACHE),//=4'b0010;
    .M22_AXI_ARPROT(M22_AXI_ARPROT),//=3'h0;
    .M22_AXI_ARQOS(M22_AXI_ARQOS),//=4'h0;
    .M22_AXI_ARVALID(M22_AXI_ARVALID),
    .M22_AXI_ARREADY(M22_AXI_ARREADY),
    .M22_AXI_RID(M22_AXI_RID),
    .M22_AXI_RDATA(M22_AXI_RDATA),
    .M22_AXI_RRESP(M22_AXI_RRESP),//ignore
    .M22_AXI_RLAST(M22_AXI_RLAST),
    .M22_AXI_RVALID(M22_AXI_RVALID),
    .M22_AXI_RREADY(M22_AXI_RREADY),


	//AXI to Mem23 for HBM wt
    .M23_AXI_AWID(M23_AXI_AWID),
    .M23_AXI_AWADDR(M23_AXI_AWADDR),
    .M23_AXI_AWLEN(M23_AXI_AWLEN),
    .M23_AXI_AWSIZE(M23_AXI_AWSIZE),//=clogb2((M23_AXI_DATA_WIDTH/8)-1);
    .M23_AXI_AWBURST(M23_AXI_AWBURST),//=2'b01;
    .M23_AXI_AWLOCK(M23_AXI_AWLOCK),//1'b0;
    .M23_AXI_AWCACHE(M23_AXI_AWCACHE),//=4'b0010
    .M23_AXI_AWPROT(M23_AXI_AWPROT),//=3'h0;
    .M23_AXI_AWQOS(M23_AXI_AWQOS),//=4'h0;
    .M23_AXI_AWVALID(M23_AXI_AWVALID),
    .M23_AXI_AWREADY(M23_AXI_AWREADY),
    .M23_AXI_WDATA(M23_AXI_WDATA),
    .M23_AXI_WSTRB(M23_AXI_WSTRB),
    .M23_AXI_WLAST(M23_AXI_WLAST),
    .M23_AXI_WVALID(M23_AXI_WVALID),
    .M23_AXI_WREADY(M23_AXI_WREADY),
    .M23_AXI_BID(M23_AXI_BID),//ignore
    .M23_AXI_BRESP(M23_AXI_BRESP),//ignore
    .M23_AXI_BVALID(M23_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M23_AXI_BREADY(M23_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M23_AXI_ARID(M23_AXI_ARID),
    .M23_AXI_ARADDR(M23_AXI_ARADDR),
    .M23_AXI_ARLEN(M23_AXI_ARLEN),
    .M23_AXI_ARSIZE(M23_AXI_ARSIZE),//=clogb2((M23_AXI_DATA_WIDTH/8)-1);
    .M23_AXI_ARBURST(M23_AXI_ARBURST),//=2'b01;
    .M23_AXI_ARLOCK(M23_AXI_ARLOCK),//=1'b0;
    .M23_AXI_ARCACHE(M23_AXI_ARCACHE),//=4'b0010;
    .M23_AXI_ARPROT(M23_AXI_ARPROT),//=3'h0;
    .M23_AXI_ARQOS(M23_AXI_ARQOS),//=4'h0;
    .M23_AXI_ARVALID(M23_AXI_ARVALID),
    .M23_AXI_ARREADY(M23_AXI_ARREADY),
    .M23_AXI_RID(M23_AXI_RID),
    .M23_AXI_RDATA(M23_AXI_RDATA),
    .M23_AXI_RRESP(M23_AXI_RRESP),//ignore
    .M23_AXI_RLAST(M23_AXI_RLAST),
    .M23_AXI_RVALID(M23_AXI_RVALID),
    .M23_AXI_RREADY(M23_AXI_RREADY),


	//AXI to Mem24 for HBM wt
    .M24_AXI_AWID(M24_AXI_AWID),
    .M24_AXI_AWADDR(M24_AXI_AWADDR),
    .M24_AXI_AWLEN(M24_AXI_AWLEN),
    .M24_AXI_AWSIZE(M24_AXI_AWSIZE),//=clogb2((M24_AXI_DATA_WIDTH/8)-1);
    .M24_AXI_AWBURST(M24_AXI_AWBURST),//=2'b01;
    .M24_AXI_AWLOCK(M24_AXI_AWLOCK),//1'b0;
    .M24_AXI_AWCACHE(M24_AXI_AWCACHE),//=4'b0010
    .M24_AXI_AWPROT(M24_AXI_AWPROT),//=3'h0;
    .M24_AXI_AWQOS(M24_AXI_AWQOS),//=4'h0;
    .M24_AXI_AWVALID(M24_AXI_AWVALID),
    .M24_AXI_AWREADY(M24_AXI_AWREADY),
    .M24_AXI_WDATA(M24_AXI_WDATA),
    .M24_AXI_WSTRB(M24_AXI_WSTRB),
    .M24_AXI_WLAST(M24_AXI_WLAST),
    .M24_AXI_WVALID(M24_AXI_WVALID),
    .M24_AXI_WREADY(M24_AXI_WREADY),
    .M24_AXI_BID(M24_AXI_BID),//ignore
    .M24_AXI_BRESP(M24_AXI_BRESP),//ignore
    .M24_AXI_BVALID(M24_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M24_AXI_BREADY(M24_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M24_AXI_ARID(M24_AXI_ARID),
    .M24_AXI_ARADDR(M24_AXI_ARADDR),
    .M24_AXI_ARLEN(M24_AXI_ARLEN),
    .M24_AXI_ARSIZE(M24_AXI_ARSIZE),//=clogb2((M24_AXI_DATA_WIDTH/8)-1);
    .M24_AXI_ARBURST(M24_AXI_ARBURST),//=2'b01;
    .M24_AXI_ARLOCK(M24_AXI_ARLOCK),//=1'b0;
    .M24_AXI_ARCACHE(M24_AXI_ARCACHE),//=4'b0010;
    .M24_AXI_ARPROT(M24_AXI_ARPROT),//=3'h0;
    .M24_AXI_ARQOS(M24_AXI_ARQOS),//=4'h0;
    .M24_AXI_ARVALID(M24_AXI_ARVALID),
    .M24_AXI_ARREADY(M24_AXI_ARREADY),
    .M24_AXI_RID(M24_AXI_RID),
    .M24_AXI_RDATA(M24_AXI_RDATA),
    .M24_AXI_RRESP(M24_AXI_RRESP),//ignore
    .M24_AXI_RLAST(M24_AXI_RLAST),
    .M24_AXI_RVALID(M24_AXI_RVALID),
    .M24_AXI_RREADY(M24_AXI_RREADY),


	//AXI to Mem25 for HBM wt
    .M25_AXI_AWID(M25_AXI_AWID),
    .M25_AXI_AWADDR(M25_AXI_AWADDR),
    .M25_AXI_AWLEN(M25_AXI_AWLEN),
    .M25_AXI_AWSIZE(M25_AXI_AWSIZE),//=clogb2((M25_AXI_DATA_WIDTH/8)-1);
    .M25_AXI_AWBURST(M25_AXI_AWBURST),//=2'b01;
    .M25_AXI_AWLOCK(M25_AXI_AWLOCK),//1'b0;
    .M25_AXI_AWCACHE(M25_AXI_AWCACHE),//=4'b0010
    .M25_AXI_AWPROT(M25_AXI_AWPROT),//=3'h0;
    .M25_AXI_AWQOS(M25_AXI_AWQOS),//=4'h0;
    .M25_AXI_AWVALID(M25_AXI_AWVALID),
    .M25_AXI_AWREADY(M25_AXI_AWREADY),
    .M25_AXI_WDATA(M25_AXI_WDATA),
    .M25_AXI_WSTRB(M25_AXI_WSTRB),
    .M25_AXI_WLAST(M25_AXI_WLAST),
    .M25_AXI_WVALID(M25_AXI_WVALID),
    .M25_AXI_WREADY(M25_AXI_WREADY),
    .M25_AXI_BID(M25_AXI_BID),//ignore
    .M25_AXI_BRESP(M25_AXI_BRESP),//ignore
    .M25_AXI_BVALID(M25_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M25_AXI_BREADY(M25_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M25_AXI_ARID(M25_AXI_ARID),
    .M25_AXI_ARADDR(M25_AXI_ARADDR),
    .M25_AXI_ARLEN(M25_AXI_ARLEN),
    .M25_AXI_ARSIZE(M25_AXI_ARSIZE),//=clogb2((M25_AXI_DATA_WIDTH/8)-1);
    .M25_AXI_ARBURST(M25_AXI_ARBURST),//=2'b01;
    .M25_AXI_ARLOCK(M25_AXI_ARLOCK),//=1'b0;
    .M25_AXI_ARCACHE(M25_AXI_ARCACHE),//=4'b0010;
    .M25_AXI_ARPROT(M25_AXI_ARPROT),//=3'h0;
    .M25_AXI_ARQOS(M25_AXI_ARQOS),//=4'h0;
    .M25_AXI_ARVALID(M25_AXI_ARVALID),
    .M25_AXI_ARREADY(M25_AXI_ARREADY),
    .M25_AXI_RID(M25_AXI_RID),
    .M25_AXI_RDATA(M25_AXI_RDATA),
    .M25_AXI_RRESP(M25_AXI_RRESP),//ignore
    .M25_AXI_RLAST(M25_AXI_RLAST),
    .M25_AXI_RVALID(M25_AXI_RVALID),
    .M25_AXI_RREADY(M25_AXI_RREADY),


	//AXI to Mem26 for HBM wt
    .M26_AXI_AWID(M26_AXI_AWID),
    .M26_AXI_AWADDR(M26_AXI_AWADDR),
    .M26_AXI_AWLEN(M26_AXI_AWLEN),
    .M26_AXI_AWSIZE(M26_AXI_AWSIZE),//=clogb2((M26_AXI_DATA_WIDTH/8)-1);
    .M26_AXI_AWBURST(M26_AXI_AWBURST),//=2'b01;
    .M26_AXI_AWLOCK(M26_AXI_AWLOCK),//1'b0;
    .M26_AXI_AWCACHE(M26_AXI_AWCACHE),//=4'b0010
    .M26_AXI_AWPROT(M26_AXI_AWPROT),//=3'h0;
    .M26_AXI_AWQOS(M26_AXI_AWQOS),//=4'h0;
    .M26_AXI_AWVALID(M26_AXI_AWVALID),
    .M26_AXI_AWREADY(M26_AXI_AWREADY),
    .M26_AXI_WDATA(M26_AXI_WDATA),
    .M26_AXI_WSTRB(M26_AXI_WSTRB),
    .M26_AXI_WLAST(M26_AXI_WLAST),
    .M26_AXI_WVALID(M26_AXI_WVALID),
    .M26_AXI_WREADY(M26_AXI_WREADY),
    .M26_AXI_BID(M26_AXI_BID),//ignore
    .M26_AXI_BRESP(M26_AXI_BRESP),//ignore
    .M26_AXI_BVALID(M26_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M26_AXI_BREADY(M26_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M26_AXI_ARID(M26_AXI_ARID),
    .M26_AXI_ARADDR(M26_AXI_ARADDR),
    .M26_AXI_ARLEN(M26_AXI_ARLEN),
    .M26_AXI_ARSIZE(M26_AXI_ARSIZE),//=clogb2((M26_AXI_DATA_WIDTH/8)-1);
    .M26_AXI_ARBURST(M26_AXI_ARBURST),//=2'b01;
    .M26_AXI_ARLOCK(M26_AXI_ARLOCK),//=1'b0;
    .M26_AXI_ARCACHE(M26_AXI_ARCACHE),//=4'b0010;
    .M26_AXI_ARPROT(M26_AXI_ARPROT),//=3'h0;
    .M26_AXI_ARQOS(M26_AXI_ARQOS),//=4'h0;
    .M26_AXI_ARVALID(M26_AXI_ARVALID),
    .M26_AXI_ARREADY(M26_AXI_ARREADY),
    .M26_AXI_RID(M26_AXI_RID),
    .M26_AXI_RDATA(M26_AXI_RDATA),
    .M26_AXI_RRESP(M26_AXI_RRESP),//ignore
    .M26_AXI_RLAST(M26_AXI_RLAST),
    .M26_AXI_RVALID(M26_AXI_RVALID),
    .M26_AXI_RREADY(M26_AXI_RREADY),


	//AXI to Mem27 for HBM wt
    .M27_AXI_AWID(M27_AXI_AWID),
    .M27_AXI_AWADDR(M27_AXI_AWADDR),
    .M27_AXI_AWLEN(M27_AXI_AWLEN),
    .M27_AXI_AWSIZE(M27_AXI_AWSIZE),//=clogb2((M27_AXI_DATA_WIDTH/8)-1);
    .M27_AXI_AWBURST(M27_AXI_AWBURST),//=2'b01;
    .M27_AXI_AWLOCK(M27_AXI_AWLOCK),//1'b0;
    .M27_AXI_AWCACHE(M27_AXI_AWCACHE),//=4'b0010
    .M27_AXI_AWPROT(M27_AXI_AWPROT),//=3'h0;
    .M27_AXI_AWQOS(M27_AXI_AWQOS),//=4'h0;
    .M27_AXI_AWVALID(M27_AXI_AWVALID),
    .M27_AXI_AWREADY(M27_AXI_AWREADY),
    .M27_AXI_WDATA(M27_AXI_WDATA),
    .M27_AXI_WSTRB(M27_AXI_WSTRB),
    .M27_AXI_WLAST(M27_AXI_WLAST),
    .M27_AXI_WVALID(M27_AXI_WVALID),
    .M27_AXI_WREADY(M27_AXI_WREADY),
    .M27_AXI_BID(M27_AXI_BID),//ignore
    .M27_AXI_BRESP(M27_AXI_BRESP),//ignore
    .M27_AXI_BVALID(M27_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M27_AXI_BREADY(M27_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M27_AXI_ARID(M27_AXI_ARID),
    .M27_AXI_ARADDR(M27_AXI_ARADDR),
    .M27_AXI_ARLEN(M27_AXI_ARLEN),
    .M27_AXI_ARSIZE(M27_AXI_ARSIZE),//=clogb2((M27_AXI_DATA_WIDTH/8)-1);
    .M27_AXI_ARBURST(M27_AXI_ARBURST),//=2'b01;
    .M27_AXI_ARLOCK(M27_AXI_ARLOCK),//=1'b0;
    .M27_AXI_ARCACHE(M27_AXI_ARCACHE),//=4'b0010;
    .M27_AXI_ARPROT(M27_AXI_ARPROT),//=3'h0;
    .M27_AXI_ARQOS(M27_AXI_ARQOS),//=4'h0;
    .M27_AXI_ARVALID(M27_AXI_ARVALID),
    .M27_AXI_ARREADY(M27_AXI_ARREADY),
    .M27_AXI_RID(M27_AXI_RID),
    .M27_AXI_RDATA(M27_AXI_RDATA),
    .M27_AXI_RRESP(M27_AXI_RRESP),//ignore
    .M27_AXI_RLAST(M27_AXI_RLAST),
    .M27_AXI_RVALID(M27_AXI_RVALID),
    .M27_AXI_RREADY(M27_AXI_RREADY),


	//AXI to Mem28 for HBM wt
    .M28_AXI_AWID(M28_AXI_AWID),
    .M28_AXI_AWADDR(M28_AXI_AWADDR),
    .M28_AXI_AWLEN(M28_AXI_AWLEN),
    .M28_AXI_AWSIZE(M28_AXI_AWSIZE),//=clogb2((M28_AXI_DATA_WIDTH/8)-1);
    .M28_AXI_AWBURST(M28_AXI_AWBURST),//=2'b01;
    .M28_AXI_AWLOCK(M28_AXI_AWLOCK),//1'b0;
    .M28_AXI_AWCACHE(M28_AXI_AWCACHE),//=4'b0010
    .M28_AXI_AWPROT(M28_AXI_AWPROT),//=3'h0;
    .M28_AXI_AWQOS(M28_AXI_AWQOS),//=4'h0;
    .M28_AXI_AWVALID(M28_AXI_AWVALID),
    .M28_AXI_AWREADY(M28_AXI_AWREADY),
    .M28_AXI_WDATA(M28_AXI_WDATA),
    .M28_AXI_WSTRB(M28_AXI_WSTRB),
    .M28_AXI_WLAST(M28_AXI_WLAST),
    .M28_AXI_WVALID(M28_AXI_WVALID),
    .M28_AXI_WREADY(M28_AXI_WREADY),
    .M28_AXI_BID(M28_AXI_BID),//ignore
    .M28_AXI_BRESP(M28_AXI_BRESP),//ignore
    .M28_AXI_BVALID(M28_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M28_AXI_BREADY(M28_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M28_AXI_ARID(M28_AXI_ARID),
    .M28_AXI_ARADDR(M28_AXI_ARADDR),
    .M28_AXI_ARLEN(M28_AXI_ARLEN),
    .M28_AXI_ARSIZE(M28_AXI_ARSIZE),//=clogb2((M28_AXI_DATA_WIDTH/8)-1);
    .M28_AXI_ARBURST(M28_AXI_ARBURST),//=2'b01;
    .M28_AXI_ARLOCK(M28_AXI_ARLOCK),//=1'b0;
    .M28_AXI_ARCACHE(M28_AXI_ARCACHE),//=4'b0010;
    .M28_AXI_ARPROT(M28_AXI_ARPROT),//=3'h0;
    .M28_AXI_ARQOS(M28_AXI_ARQOS),//=4'h0;
    .M28_AXI_ARVALID(M28_AXI_ARVALID),
    .M28_AXI_ARREADY(M28_AXI_ARREADY),
    .M28_AXI_RID(M28_AXI_RID),
    .M28_AXI_RDATA(M28_AXI_RDATA),
    .M28_AXI_RRESP(M28_AXI_RRESP),//ignore
    .M28_AXI_RLAST(M28_AXI_RLAST),
    .M28_AXI_RVALID(M28_AXI_RVALID),
    .M28_AXI_RREADY(M28_AXI_RREADY),


	//AXI to Mem29 for HBM wt
    .M29_AXI_AWID(M29_AXI_AWID),
    .M29_AXI_AWADDR(M29_AXI_AWADDR),
    .M29_AXI_AWLEN(M29_AXI_AWLEN),
    .M29_AXI_AWSIZE(M29_AXI_AWSIZE),//=clogb2((M29_AXI_DATA_WIDTH/8)-1);
    .M29_AXI_AWBURST(M29_AXI_AWBURST),//=2'b01;
    .M29_AXI_AWLOCK(M29_AXI_AWLOCK),//1'b0;
    .M29_AXI_AWCACHE(M29_AXI_AWCACHE),//=4'b0010
    .M29_AXI_AWPROT(M29_AXI_AWPROT),//=3'h0;
    .M29_AXI_AWQOS(M29_AXI_AWQOS),//=4'h0;
    .M29_AXI_AWVALID(M29_AXI_AWVALID),
    .M29_AXI_AWREADY(M29_AXI_AWREADY),
    .M29_AXI_WDATA(M29_AXI_WDATA),
    .M29_AXI_WSTRB(M29_AXI_WSTRB),
    .M29_AXI_WLAST(M29_AXI_WLAST),
    .M29_AXI_WVALID(M29_AXI_WVALID),
    .M29_AXI_WREADY(M29_AXI_WREADY),
    .M29_AXI_BID(M29_AXI_BID),//ignore
    .M29_AXI_BRESP(M29_AXI_BRESP),//ignore
    .M29_AXI_BVALID(M29_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M29_AXI_BREADY(M29_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M29_AXI_ARID(M29_AXI_ARID),
    .M29_AXI_ARADDR(M29_AXI_ARADDR),
    .M29_AXI_ARLEN(M29_AXI_ARLEN),
    .M29_AXI_ARSIZE(M29_AXI_ARSIZE),//=clogb2((M29_AXI_DATA_WIDTH/8)-1);
    .M29_AXI_ARBURST(M29_AXI_ARBURST),//=2'b01;
    .M29_AXI_ARLOCK(M29_AXI_ARLOCK),//=1'b0;
    .M29_AXI_ARCACHE(M29_AXI_ARCACHE),//=4'b0010;
    .M29_AXI_ARPROT(M29_AXI_ARPROT),//=3'h0;
    .M29_AXI_ARQOS(M29_AXI_ARQOS),//=4'h0;
    .M29_AXI_ARVALID(M29_AXI_ARVALID),
    .M29_AXI_ARREADY(M29_AXI_ARREADY),
    .M29_AXI_RID(M29_AXI_RID),
    .M29_AXI_RDATA(M29_AXI_RDATA),
    .M29_AXI_RRESP(M29_AXI_RRESP),//ignore
    .M29_AXI_RLAST(M29_AXI_RLAST),
    .M29_AXI_RVALID(M29_AXI_RVALID),
    .M29_AXI_RREADY(M29_AXI_RREADY),


	//AXI to Mem30 for HBM wt
    .M30_AXI_AWID(M30_AXI_AWID),
    .M30_AXI_AWADDR(M30_AXI_AWADDR),
    .M30_AXI_AWLEN(M30_AXI_AWLEN),
    .M30_AXI_AWSIZE(M30_AXI_AWSIZE),//=clogb2((M30_AXI_DATA_WIDTH/8)-1);
    .M30_AXI_AWBURST(M30_AXI_AWBURST),//=2'b01;
    .M30_AXI_AWLOCK(M30_AXI_AWLOCK),//1'b0;
    .M30_AXI_AWCACHE(M30_AXI_AWCACHE),//=4'b0010
    .M30_AXI_AWPROT(M30_AXI_AWPROT),//=3'h0;
    .M30_AXI_AWQOS(M30_AXI_AWQOS),//=4'h0;
    .M30_AXI_AWVALID(M30_AXI_AWVALID),
    .M30_AXI_AWREADY(M30_AXI_AWREADY),
    .M30_AXI_WDATA(M30_AXI_WDATA),
    .M30_AXI_WSTRB(M30_AXI_WSTRB),
    .M30_AXI_WLAST(M30_AXI_WLAST),
    .M30_AXI_WVALID(M30_AXI_WVALID),
    .M30_AXI_WREADY(M30_AXI_WREADY),
    .M30_AXI_BID(M30_AXI_BID),//ignore
    .M30_AXI_BRESP(M30_AXI_BRESP),//ignore
    .M30_AXI_BVALID(M30_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M30_AXI_BREADY(M30_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M30_AXI_ARID(M30_AXI_ARID),
    .M30_AXI_ARADDR(M30_AXI_ARADDR),
    .M30_AXI_ARLEN(M30_AXI_ARLEN),
    .M30_AXI_ARSIZE(M30_AXI_ARSIZE),//=clogb2((M30_AXI_DATA_WIDTH/8)-1);
    .M30_AXI_ARBURST(M30_AXI_ARBURST),//=2'b01;
    .M30_AXI_ARLOCK(M30_AXI_ARLOCK),//=1'b0;
    .M30_AXI_ARCACHE(M30_AXI_ARCACHE),//=4'b0010;
    .M30_AXI_ARPROT(M30_AXI_ARPROT),//=3'h0;
    .M30_AXI_ARQOS(M30_AXI_ARQOS),//=4'h0;
    .M30_AXI_ARVALID(M30_AXI_ARVALID),
    .M30_AXI_ARREADY(M30_AXI_ARREADY),
    .M30_AXI_RID(M30_AXI_RID),
    .M30_AXI_RDATA(M30_AXI_RDATA),
    .M30_AXI_RRESP(M30_AXI_RRESP),//ignore
    .M30_AXI_RLAST(M30_AXI_RLAST),
    .M30_AXI_RVALID(M30_AXI_RVALID),
    .M30_AXI_RREADY(M30_AXI_RREADY),


	//AXI to Mem31 for HBM wt
    .M31_AXI_AWID(M31_AXI_AWID),
    .M31_AXI_AWADDR(M31_AXI_AWADDR),
    .M31_AXI_AWLEN(M31_AXI_AWLEN),
    .M31_AXI_AWSIZE(M31_AXI_AWSIZE),//=clogb2((M31_AXI_DATA_WIDTH/8)-1);
    .M31_AXI_AWBURST(M31_AXI_AWBURST),//=2'b01;
    .M31_AXI_AWLOCK(M31_AXI_AWLOCK),//1'b0;
    .M31_AXI_AWCACHE(M31_AXI_AWCACHE),//=4'b0010
    .M31_AXI_AWPROT(M31_AXI_AWPROT),//=3'h0;
    .M31_AXI_AWQOS(M31_AXI_AWQOS),//=4'h0;
    .M31_AXI_AWVALID(M31_AXI_AWVALID),
    .M31_AXI_AWREADY(M31_AXI_AWREADY),
    .M31_AXI_WDATA(M31_AXI_WDATA),
    .M31_AXI_WSTRB(M31_AXI_WSTRB),
    .M31_AXI_WLAST(M31_AXI_WLAST),
    .M31_AXI_WVALID(M31_AXI_WVALID),
    .M31_AXI_WREADY(M31_AXI_WREADY),
    .M31_AXI_BID(M31_AXI_BID),//ignore
    .M31_AXI_BRESP(M31_AXI_BRESP),//ignore
    .M31_AXI_BVALID(M31_AXI_BVALID),//Bvalid and Bread means a a write response.
    .M31_AXI_BREADY(M31_AXI_BREADY),//Bvalid and Bread means a a write response.
    .M31_AXI_ARID(M31_AXI_ARID),
    .M31_AXI_ARADDR(M31_AXI_ARADDR),
    .M31_AXI_ARLEN(M31_AXI_ARLEN),
    .M31_AXI_ARSIZE(M31_AXI_ARSIZE),//=clogb2((M31_AXI_DATA_WIDTH/8)-1);
    .M31_AXI_ARBURST(M31_AXI_ARBURST),//=2'b01;
    .M31_AXI_ARLOCK(M31_AXI_ARLOCK),//=1'b0;
    .M31_AXI_ARCACHE(M31_AXI_ARCACHE),//=4'b0010;
    .M31_AXI_ARPROT(M31_AXI_ARPROT),//=3'h0;
    .M31_AXI_ARQOS(M31_AXI_ARQOS),//=4'h0;
    .M31_AXI_ARVALID(M31_AXI_ARVALID),
    .M31_AXI_ARREADY(M31_AXI_ARREADY),
    .M31_AXI_RID(M31_AXI_RID),
    .M31_AXI_RDATA(M31_AXI_RDATA),
    .M31_AXI_RRESP(M31_AXI_RRESP),//ignore
    .M31_AXI_RLAST(M31_AXI_RLAST),
    .M31_AXI_RVALID(M31_AXI_RVALID),
    .M31_AXI_RREADY(M31_AXI_RREADY)
);


AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(M_AXI_ID_WIDTH),
    .S_AXI_DATA_WIDTH(`AXI_DAT_WIDTH),
    .S_AXI_ADDR_WIDTH(`DDR_WIDTH),
    .BW_div_FREQ_100(1300)
)AXI_HP_Slave_DDR00
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
    .S_AXI_AWID    (M_AXI_AWID),
    .S_AXI_AWADDR  (M_AXI_AWADDR),
    .S_AXI_AWLEN   (M_AXI_AWLEN),
    .S_AXI_AWSIZE  (M_AXI_AWSIZE),
    .S_AXI_AWBURST (M_AXI_AWBURST),
    .S_AXI_AWLOCK  (M_AXI_AWLOCK),
    .S_AXI_AWCACHE (M_AXI_AWCACHE),
    .S_AXI_AWPROT  (M_AXI_AWPROT),
    .S_AXI_AWQOS   (M_AXI_AWQOS),
    .S_AXI_AWVALID (M_AXI_AWVALID),
    .S_AXI_AWREADY (M_AXI_AWREADY),
    .S_AXI_WDATA   (M_AXI_WDATA),
    .S_AXI_WSTRB   (M_AXI_WSTRB),
    .S_AXI_WLAST   (M_AXI_WLAST),
    .S_AXI_WVALID  (M_AXI_WVALID),
    .S_AXI_WREADY  (M_AXI_WREADY),
    .S_AXI_BID     (M_AXI_BID),
    .S_AXI_BRESP   (M_AXI_BRESP),
    .S_AXI_BVALID  (M_AXI_BVALID),
    .S_AXI_BREADY  (M_AXI_BREADY),
    .S_AXI_ARID    (M_AXI_ARID),
    .S_AXI_ARADDR  (M_AXI_ARADDR),
    .S_AXI_ARLEN   (M_AXI_ARLEN),
    .S_AXI_ARSIZE  (M_AXI_ARSIZE),
    .S_AXI_ARBURST (M_AXI_ARBURST),
    .S_AXI_ARLOCK  (M_AXI_ARLOCK),
    .S_AXI_ARCACHE (M_AXI_ARCACHE),
    .S_AXI_ARPROT  (M_AXI_ARPROT),
    .S_AXI_ARQOS   (M_AXI_ARQOS),
    .S_AXI_ARVALID (M_AXI_ARVALID),
    .S_AXI_ARREADY (M_AXI_ARREADY),
    .S_AXI_RID     (M_AXI_RID),
    .S_AXI_RDATA   (M_AXI_RDATA),
    .S_AXI_RRESP   (M_AXI_RRESP),
    .S_AXI_RLAST   (M_AXI_RLAST),
    .S_AXI_RVALID  (M_AXI_RVALID),
    .S_AXI_RREADY  (M_AXI_RREADY)
);



HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR00
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M00_AXI_AWID),
	.S_AXI_AWADDR  (M00_AXI_AWADDR),
	.S_AXI_AWLEN   (M00_AXI_AWLEN),
	.S_AXI_AWSIZE  (M00_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M00_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M00_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M00_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M00_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M00_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M00_AXI_AWVALID),
	.S_AXI_AWREADY (M00_AXI_AWREADY),
	.S_AXI_WDATA   (M00_AXI_WDATA),
	.S_AXI_WSTRB   (M00_AXI_WSTRB),
	.S_AXI_WLAST   (M00_AXI_WLAST),
	.S_AXI_WVALID  (M00_AXI_WVALID),
	.S_AXI_WREADY  (M00_AXI_WREADY),
	.S_AXI_BID     (M00_AXI_BID),//ignore
	.S_AXI_BRESP   (M00_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M00_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M00_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M00_AXI_ARID),
	.S_AXI_ARADDR  (M00_AXI_ARADDR),
	.S_AXI_ARLEN   (M00_AXI_ARLEN),
	.S_AXI_ARSIZE  (M00_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M00_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M00_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M00_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M00_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M00_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M00_AXI_ARVALID),
	.S_AXI_ARREADY (M00_AXI_ARREADY),
	.S_AXI_RID     (M00_AXI_RID),
	.S_AXI_RDATA   (M00_AXI_RDATA),
	.S_AXI_RRESP   (M00_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M00_AXI_RLAST),
	.S_AXI_RVALID  (M00_AXI_RVALID),
	.S_AXI_RREADY  (M00_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR01
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M01_AXI_AWID),
	.S_AXI_AWADDR  (M01_AXI_AWADDR),
	.S_AXI_AWLEN   (M01_AXI_AWLEN),
	.S_AXI_AWSIZE  (M01_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M01_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M01_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M01_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M01_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M01_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M01_AXI_AWVALID),
	.S_AXI_AWREADY (M01_AXI_AWREADY),
	.S_AXI_WDATA   (M01_AXI_WDATA),
	.S_AXI_WSTRB   (M01_AXI_WSTRB),
	.S_AXI_WLAST   (M01_AXI_WLAST),
	.S_AXI_WVALID  (M01_AXI_WVALID),
	.S_AXI_WREADY  (M01_AXI_WREADY),
	.S_AXI_BID     (M01_AXI_BID),//ignore
	.S_AXI_BRESP   (M01_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M01_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M01_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M01_AXI_ARID),
	.S_AXI_ARADDR  (M01_AXI_ARADDR),
	.S_AXI_ARLEN   (M01_AXI_ARLEN),
	.S_AXI_ARSIZE  (M01_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M01_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M01_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M01_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M01_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M01_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M01_AXI_ARVALID),
	.S_AXI_ARREADY (M01_AXI_ARREADY),
	.S_AXI_RID     (M01_AXI_RID),
	.S_AXI_RDATA   (M01_AXI_RDATA),
	.S_AXI_RRESP   (M01_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M01_AXI_RLAST),
	.S_AXI_RVALID  (M01_AXI_RVALID),
	.S_AXI_RREADY  (M01_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR02
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M02_AXI_AWID),
	.S_AXI_AWADDR  (M02_AXI_AWADDR),
	.S_AXI_AWLEN   (M02_AXI_AWLEN),
	.S_AXI_AWSIZE  (M02_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M02_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M02_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M02_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M02_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M02_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M02_AXI_AWVALID),
	.S_AXI_AWREADY (M02_AXI_AWREADY),
	.S_AXI_WDATA   (M02_AXI_WDATA),
	.S_AXI_WSTRB   (M02_AXI_WSTRB),
	.S_AXI_WLAST   (M02_AXI_WLAST),
	.S_AXI_WVALID  (M02_AXI_WVALID),
	.S_AXI_WREADY  (M02_AXI_WREADY),
	.S_AXI_BID     (M02_AXI_BID),//ignore
	.S_AXI_BRESP   (M02_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M02_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M02_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M02_AXI_ARID),
	.S_AXI_ARADDR  (M02_AXI_ARADDR),
	.S_AXI_ARLEN   (M02_AXI_ARLEN),
	.S_AXI_ARSIZE  (M02_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M02_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M02_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M02_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M02_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M02_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M02_AXI_ARVALID),
	.S_AXI_ARREADY (M02_AXI_ARREADY),
	.S_AXI_RID     (M02_AXI_RID),
	.S_AXI_RDATA   (M02_AXI_RDATA),
	.S_AXI_RRESP   (M02_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M02_AXI_RLAST),
	.S_AXI_RVALID  (M02_AXI_RVALID),
	.S_AXI_RREADY  (M02_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR03
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M03_AXI_AWID),
	.S_AXI_AWADDR  (M03_AXI_AWADDR),
	.S_AXI_AWLEN   (M03_AXI_AWLEN),
	.S_AXI_AWSIZE  (M03_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M03_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M03_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M03_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M03_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M03_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M03_AXI_AWVALID),
	.S_AXI_AWREADY (M03_AXI_AWREADY),
	.S_AXI_WDATA   (M03_AXI_WDATA),
	.S_AXI_WSTRB   (M03_AXI_WSTRB),
	.S_AXI_WLAST   (M03_AXI_WLAST),
	.S_AXI_WVALID  (M03_AXI_WVALID),
	.S_AXI_WREADY  (M03_AXI_WREADY),
	.S_AXI_BID     (M03_AXI_BID),//ignore
	.S_AXI_BRESP   (M03_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M03_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M03_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M03_AXI_ARID),
	.S_AXI_ARADDR  (M03_AXI_ARADDR),
	.S_AXI_ARLEN   (M03_AXI_ARLEN),
	.S_AXI_ARSIZE  (M03_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M03_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M03_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M03_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M03_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M03_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M03_AXI_ARVALID),
	.S_AXI_ARREADY (M03_AXI_ARREADY),
	.S_AXI_RID     (M03_AXI_RID),
	.S_AXI_RDATA   (M03_AXI_RDATA),
	.S_AXI_RRESP   (M03_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M03_AXI_RLAST),
	.S_AXI_RVALID  (M03_AXI_RVALID),
	.S_AXI_RREADY  (M03_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR04
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M04_AXI_AWID),
	.S_AXI_AWADDR  (M04_AXI_AWADDR),
	.S_AXI_AWLEN   (M04_AXI_AWLEN),
	.S_AXI_AWSIZE  (M04_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M04_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M04_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M04_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M04_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M04_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M04_AXI_AWVALID),
	.S_AXI_AWREADY (M04_AXI_AWREADY),
	.S_AXI_WDATA   (M04_AXI_WDATA),
	.S_AXI_WSTRB   (M04_AXI_WSTRB),
	.S_AXI_WLAST   (M04_AXI_WLAST),
	.S_AXI_WVALID  (M04_AXI_WVALID),
	.S_AXI_WREADY  (M04_AXI_WREADY),
	.S_AXI_BID     (M04_AXI_BID),//ignore
	.S_AXI_BRESP   (M04_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M04_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M04_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M04_AXI_ARID),
	.S_AXI_ARADDR  (M04_AXI_ARADDR),
	.S_AXI_ARLEN   (M04_AXI_ARLEN),
	.S_AXI_ARSIZE  (M04_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M04_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M04_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M04_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M04_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M04_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M04_AXI_ARVALID),
	.S_AXI_ARREADY (M04_AXI_ARREADY),
	.S_AXI_RID     (M04_AXI_RID),
	.S_AXI_RDATA   (M04_AXI_RDATA),
	.S_AXI_RRESP   (M04_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M04_AXI_RLAST),
	.S_AXI_RVALID  (M04_AXI_RVALID),
	.S_AXI_RREADY  (M04_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR05
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M05_AXI_AWID),
	.S_AXI_AWADDR  (M05_AXI_AWADDR),
	.S_AXI_AWLEN   (M05_AXI_AWLEN),
	.S_AXI_AWSIZE  (M05_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M05_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M05_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M05_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M05_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M05_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M05_AXI_AWVALID),
	.S_AXI_AWREADY (M05_AXI_AWREADY),
	.S_AXI_WDATA   (M05_AXI_WDATA),
	.S_AXI_WSTRB   (M05_AXI_WSTRB),
	.S_AXI_WLAST   (M05_AXI_WLAST),
	.S_AXI_WVALID  (M05_AXI_WVALID),
	.S_AXI_WREADY  (M05_AXI_WREADY),
	.S_AXI_BID     (M05_AXI_BID),//ignore
	.S_AXI_BRESP   (M05_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M05_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M05_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M05_AXI_ARID),
	.S_AXI_ARADDR  (M05_AXI_ARADDR),
	.S_AXI_ARLEN   (M05_AXI_ARLEN),
	.S_AXI_ARSIZE  (M05_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M05_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M05_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M05_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M05_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M05_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M05_AXI_ARVALID),
	.S_AXI_ARREADY (M05_AXI_ARREADY),
	.S_AXI_RID     (M05_AXI_RID),
	.S_AXI_RDATA   (M05_AXI_RDATA),
	.S_AXI_RRESP   (M05_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M05_AXI_RLAST),
	.S_AXI_RVALID  (M05_AXI_RVALID),
	.S_AXI_RREADY  (M05_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR06
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M06_AXI_AWID),
	.S_AXI_AWADDR  (M06_AXI_AWADDR),
	.S_AXI_AWLEN   (M06_AXI_AWLEN),
	.S_AXI_AWSIZE  (M06_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M06_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M06_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M06_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M06_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M06_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M06_AXI_AWVALID),
	.S_AXI_AWREADY (M06_AXI_AWREADY),
	.S_AXI_WDATA   (M06_AXI_WDATA),
	.S_AXI_WSTRB   (M06_AXI_WSTRB),
	.S_AXI_WLAST   (M06_AXI_WLAST),
	.S_AXI_WVALID  (M06_AXI_WVALID),
	.S_AXI_WREADY  (M06_AXI_WREADY),
	.S_AXI_BID     (M06_AXI_BID),//ignore
	.S_AXI_BRESP   (M06_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M06_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M06_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M06_AXI_ARID),
	.S_AXI_ARADDR  (M06_AXI_ARADDR),
	.S_AXI_ARLEN   (M06_AXI_ARLEN),
	.S_AXI_ARSIZE  (M06_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M06_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M06_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M06_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M06_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M06_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M06_AXI_ARVALID),
	.S_AXI_ARREADY (M06_AXI_ARREADY),
	.S_AXI_RID     (M06_AXI_RID),
	.S_AXI_RDATA   (M06_AXI_RDATA),
	.S_AXI_RRESP   (M06_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M06_AXI_RLAST),
	.S_AXI_RVALID  (M06_AXI_RVALID),
	.S_AXI_RREADY  (M06_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR07
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M07_AXI_AWID),
	.S_AXI_AWADDR  (M07_AXI_AWADDR),
	.S_AXI_AWLEN   (M07_AXI_AWLEN),
	.S_AXI_AWSIZE  (M07_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M07_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M07_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M07_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M07_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M07_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M07_AXI_AWVALID),
	.S_AXI_AWREADY (M07_AXI_AWREADY),
	.S_AXI_WDATA   (M07_AXI_WDATA),
	.S_AXI_WSTRB   (M07_AXI_WSTRB),
	.S_AXI_WLAST   (M07_AXI_WLAST),
	.S_AXI_WVALID  (M07_AXI_WVALID),
	.S_AXI_WREADY  (M07_AXI_WREADY),
	.S_AXI_BID     (M07_AXI_BID),//ignore
	.S_AXI_BRESP   (M07_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M07_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M07_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M07_AXI_ARID),
	.S_AXI_ARADDR  (M07_AXI_ARADDR),
	.S_AXI_ARLEN   (M07_AXI_ARLEN),
	.S_AXI_ARSIZE  (M07_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M07_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M07_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M07_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M07_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M07_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M07_AXI_ARVALID),
	.S_AXI_ARREADY (M07_AXI_ARREADY),
	.S_AXI_RID     (M07_AXI_RID),
	.S_AXI_RDATA   (M07_AXI_RDATA),
	.S_AXI_RRESP   (M07_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M07_AXI_RLAST),
	.S_AXI_RVALID  (M07_AXI_RVALID),
	.S_AXI_RREADY  (M07_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR08
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M08_AXI_AWID),
	.S_AXI_AWADDR  (M08_AXI_AWADDR),
	.S_AXI_AWLEN   (M08_AXI_AWLEN),
	.S_AXI_AWSIZE  (M08_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M08_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M08_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M08_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M08_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M08_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M08_AXI_AWVALID),
	.S_AXI_AWREADY (M08_AXI_AWREADY),
	.S_AXI_WDATA   (M08_AXI_WDATA),
	.S_AXI_WSTRB   (M08_AXI_WSTRB),
	.S_AXI_WLAST   (M08_AXI_WLAST),
	.S_AXI_WVALID  (M08_AXI_WVALID),
	.S_AXI_WREADY  (M08_AXI_WREADY),
	.S_AXI_BID     (M08_AXI_BID),//ignore
	.S_AXI_BRESP   (M08_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M08_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M08_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M08_AXI_ARID),
	.S_AXI_ARADDR  (M08_AXI_ARADDR),
	.S_AXI_ARLEN   (M08_AXI_ARLEN),
	.S_AXI_ARSIZE  (M08_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M08_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M08_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M08_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M08_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M08_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M08_AXI_ARVALID),
	.S_AXI_ARREADY (M08_AXI_ARREADY),
	.S_AXI_RID     (M08_AXI_RID),
	.S_AXI_RDATA   (M08_AXI_RDATA),
	.S_AXI_RRESP   (M08_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M08_AXI_RLAST),
	.S_AXI_RVALID  (M08_AXI_RVALID),
	.S_AXI_RREADY  (M08_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR09
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M09_AXI_AWID),
	.S_AXI_AWADDR  (M09_AXI_AWADDR),
	.S_AXI_AWLEN   (M09_AXI_AWLEN),
	.S_AXI_AWSIZE  (M09_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M09_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M09_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M09_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M09_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M09_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M09_AXI_AWVALID),
	.S_AXI_AWREADY (M09_AXI_AWREADY),
	.S_AXI_WDATA   (M09_AXI_WDATA),
	.S_AXI_WSTRB   (M09_AXI_WSTRB),
	.S_AXI_WLAST   (M09_AXI_WLAST),
	.S_AXI_WVALID  (M09_AXI_WVALID),
	.S_AXI_WREADY  (M09_AXI_WREADY),
	.S_AXI_BID     (M09_AXI_BID),//ignore
	.S_AXI_BRESP   (M09_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M09_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M09_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M09_AXI_ARID),
	.S_AXI_ARADDR  (M09_AXI_ARADDR),
	.S_AXI_ARLEN   (M09_AXI_ARLEN),
	.S_AXI_ARSIZE  (M09_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M09_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M09_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M09_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M09_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M09_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M09_AXI_ARVALID),
	.S_AXI_ARREADY (M09_AXI_ARREADY),
	.S_AXI_RID     (M09_AXI_RID),
	.S_AXI_RDATA   (M09_AXI_RDATA),
	.S_AXI_RRESP   (M09_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M09_AXI_RLAST),
	.S_AXI_RVALID  (M09_AXI_RVALID),
	.S_AXI_RREADY  (M09_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR10
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M10_AXI_AWID),
	.S_AXI_AWADDR  (M10_AXI_AWADDR),
	.S_AXI_AWLEN   (M10_AXI_AWLEN),
	.S_AXI_AWSIZE  (M10_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M10_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M10_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M10_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M10_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M10_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M10_AXI_AWVALID),
	.S_AXI_AWREADY (M10_AXI_AWREADY),
	.S_AXI_WDATA   (M10_AXI_WDATA),
	.S_AXI_WSTRB   (M10_AXI_WSTRB),
	.S_AXI_WLAST   (M10_AXI_WLAST),
	.S_AXI_WVALID  (M10_AXI_WVALID),
	.S_AXI_WREADY  (M10_AXI_WREADY),
	.S_AXI_BID     (M10_AXI_BID),//ignore
	.S_AXI_BRESP   (M10_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M10_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M10_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M10_AXI_ARID),
	.S_AXI_ARADDR  (M10_AXI_ARADDR),
	.S_AXI_ARLEN   (M10_AXI_ARLEN),
	.S_AXI_ARSIZE  (M10_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M10_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M10_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M10_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M10_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M10_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M10_AXI_ARVALID),
	.S_AXI_ARREADY (M10_AXI_ARREADY),
	.S_AXI_RID     (M10_AXI_RID),
	.S_AXI_RDATA   (M10_AXI_RDATA),
	.S_AXI_RRESP   (M10_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M10_AXI_RLAST),
	.S_AXI_RVALID  (M10_AXI_RVALID),
	.S_AXI_RREADY  (M10_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR11
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M11_AXI_AWID),
	.S_AXI_AWADDR  (M11_AXI_AWADDR),
	.S_AXI_AWLEN   (M11_AXI_AWLEN),
	.S_AXI_AWSIZE  (M11_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M11_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M11_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M11_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M11_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M11_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M11_AXI_AWVALID),
	.S_AXI_AWREADY (M11_AXI_AWREADY),
	.S_AXI_WDATA   (M11_AXI_WDATA),
	.S_AXI_WSTRB   (M11_AXI_WSTRB),
	.S_AXI_WLAST   (M11_AXI_WLAST),
	.S_AXI_WVALID  (M11_AXI_WVALID),
	.S_AXI_WREADY  (M11_AXI_WREADY),
	.S_AXI_BID     (M11_AXI_BID),//ignore
	.S_AXI_BRESP   (M11_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M11_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M11_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M11_AXI_ARID),
	.S_AXI_ARADDR  (M11_AXI_ARADDR),
	.S_AXI_ARLEN   (M11_AXI_ARLEN),
	.S_AXI_ARSIZE  (M11_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M11_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M11_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M11_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M11_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M11_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M11_AXI_ARVALID),
	.S_AXI_ARREADY (M11_AXI_ARREADY),
	.S_AXI_RID     (M11_AXI_RID),
	.S_AXI_RDATA   (M11_AXI_RDATA),
	.S_AXI_RRESP   (M11_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M11_AXI_RLAST),
	.S_AXI_RVALID  (M11_AXI_RVALID),
	.S_AXI_RREADY  (M11_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR12
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M12_AXI_AWID),
	.S_AXI_AWADDR  (M12_AXI_AWADDR),
	.S_AXI_AWLEN   (M12_AXI_AWLEN),
	.S_AXI_AWSIZE  (M12_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M12_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M12_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M12_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M12_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M12_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M12_AXI_AWVALID),
	.S_AXI_AWREADY (M12_AXI_AWREADY),
	.S_AXI_WDATA   (M12_AXI_WDATA),
	.S_AXI_WSTRB   (M12_AXI_WSTRB),
	.S_AXI_WLAST   (M12_AXI_WLAST),
	.S_AXI_WVALID  (M12_AXI_WVALID),
	.S_AXI_WREADY  (M12_AXI_WREADY),
	.S_AXI_BID     (M12_AXI_BID),//ignore
	.S_AXI_BRESP   (M12_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M12_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M12_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M12_AXI_ARID),
	.S_AXI_ARADDR  (M12_AXI_ARADDR),
	.S_AXI_ARLEN   (M12_AXI_ARLEN),
	.S_AXI_ARSIZE  (M12_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M12_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M12_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M12_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M12_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M12_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M12_AXI_ARVALID),
	.S_AXI_ARREADY (M12_AXI_ARREADY),
	.S_AXI_RID     (M12_AXI_RID),
	.S_AXI_RDATA   (M12_AXI_RDATA),
	.S_AXI_RRESP   (M12_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M12_AXI_RLAST),
	.S_AXI_RVALID  (M12_AXI_RVALID),
	.S_AXI_RREADY  (M12_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR13
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M13_AXI_AWID),
	.S_AXI_AWADDR  (M13_AXI_AWADDR),
	.S_AXI_AWLEN   (M13_AXI_AWLEN),
	.S_AXI_AWSIZE  (M13_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M13_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M13_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M13_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M13_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M13_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M13_AXI_AWVALID),
	.S_AXI_AWREADY (M13_AXI_AWREADY),
	.S_AXI_WDATA   (M13_AXI_WDATA),
	.S_AXI_WSTRB   (M13_AXI_WSTRB),
	.S_AXI_WLAST   (M13_AXI_WLAST),
	.S_AXI_WVALID  (M13_AXI_WVALID),
	.S_AXI_WREADY  (M13_AXI_WREADY),
	.S_AXI_BID     (M13_AXI_BID),//ignore
	.S_AXI_BRESP   (M13_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M13_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M13_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M13_AXI_ARID),
	.S_AXI_ARADDR  (M13_AXI_ARADDR),
	.S_AXI_ARLEN   (M13_AXI_ARLEN),
	.S_AXI_ARSIZE  (M13_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M13_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M13_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M13_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M13_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M13_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M13_AXI_ARVALID),
	.S_AXI_ARREADY (M13_AXI_ARREADY),
	.S_AXI_RID     (M13_AXI_RID),
	.S_AXI_RDATA   (M13_AXI_RDATA),
	.S_AXI_RRESP   (M13_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M13_AXI_RLAST),
	.S_AXI_RVALID  (M13_AXI_RVALID),
	.S_AXI_RREADY  (M13_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR14
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M14_AXI_AWID),
	.S_AXI_AWADDR  (M14_AXI_AWADDR),
	.S_AXI_AWLEN   (M14_AXI_AWLEN),
	.S_AXI_AWSIZE  (M14_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M14_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M14_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M14_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M14_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M14_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M14_AXI_AWVALID),
	.S_AXI_AWREADY (M14_AXI_AWREADY),
	.S_AXI_WDATA   (M14_AXI_WDATA),
	.S_AXI_WSTRB   (M14_AXI_WSTRB),
	.S_AXI_WLAST   (M14_AXI_WLAST),
	.S_AXI_WVALID  (M14_AXI_WVALID),
	.S_AXI_WREADY  (M14_AXI_WREADY),
	.S_AXI_BID     (M14_AXI_BID),//ignore
	.S_AXI_BRESP   (M14_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M14_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M14_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M14_AXI_ARID),
	.S_AXI_ARADDR  (M14_AXI_ARADDR),
	.S_AXI_ARLEN   (M14_AXI_ARLEN),
	.S_AXI_ARSIZE  (M14_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M14_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M14_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M14_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M14_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M14_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M14_AXI_ARVALID),
	.S_AXI_ARREADY (M14_AXI_ARREADY),
	.S_AXI_RID     (M14_AXI_RID),
	.S_AXI_RDATA   (M14_AXI_RDATA),
	.S_AXI_RRESP   (M14_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M14_AXI_RLAST),
	.S_AXI_RVALID  (M14_AXI_RVALID),
	.S_AXI_RREADY  (M14_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR15
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M15_AXI_AWID),
	.S_AXI_AWADDR  (M15_AXI_AWADDR),
	.S_AXI_AWLEN   (M15_AXI_AWLEN),
	.S_AXI_AWSIZE  (M15_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M15_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M15_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M15_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M15_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M15_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M15_AXI_AWVALID),
	.S_AXI_AWREADY (M15_AXI_AWREADY),
	.S_AXI_WDATA   (M15_AXI_WDATA),
	.S_AXI_WSTRB   (M15_AXI_WSTRB),
	.S_AXI_WLAST   (M15_AXI_WLAST),
	.S_AXI_WVALID  (M15_AXI_WVALID),
	.S_AXI_WREADY  (M15_AXI_WREADY),
	.S_AXI_BID     (M15_AXI_BID),//ignore
	.S_AXI_BRESP   (M15_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M15_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M15_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M15_AXI_ARID),
	.S_AXI_ARADDR  (M15_AXI_ARADDR),
	.S_AXI_ARLEN   (M15_AXI_ARLEN),
	.S_AXI_ARSIZE  (M15_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M15_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M15_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M15_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M15_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M15_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M15_AXI_ARVALID),
	.S_AXI_ARREADY (M15_AXI_ARREADY),
	.S_AXI_RID     (M15_AXI_RID),
	.S_AXI_RDATA   (M15_AXI_RDATA),
	.S_AXI_RRESP   (M15_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M15_AXI_RLAST),
	.S_AXI_RVALID  (M15_AXI_RVALID),
	.S_AXI_RREADY  (M15_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR16
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M16_AXI_AWID),
	.S_AXI_AWADDR  (M16_AXI_AWADDR),
	.S_AXI_AWLEN   (M16_AXI_AWLEN),
	.S_AXI_AWSIZE  (M16_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M16_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M16_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M16_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M16_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M16_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M16_AXI_AWVALID),
	.S_AXI_AWREADY (M16_AXI_AWREADY),
	.S_AXI_WDATA   (M16_AXI_WDATA),
	.S_AXI_WSTRB   (M16_AXI_WSTRB),
	.S_AXI_WLAST   (M16_AXI_WLAST),
	.S_AXI_WVALID  (M16_AXI_WVALID),
	.S_AXI_WREADY  (M16_AXI_WREADY),
	.S_AXI_BID     (M16_AXI_BID),//ignore
	.S_AXI_BRESP   (M16_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M16_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M16_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M16_AXI_ARID),
	.S_AXI_ARADDR  (M16_AXI_ARADDR),
	.S_AXI_ARLEN   (M16_AXI_ARLEN),
	.S_AXI_ARSIZE  (M16_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M16_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M16_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M16_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M16_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M16_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M16_AXI_ARVALID),
	.S_AXI_ARREADY (M16_AXI_ARREADY),
	.S_AXI_RID     (M16_AXI_RID),
	.S_AXI_RDATA   (M16_AXI_RDATA),
	.S_AXI_RRESP   (M16_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M16_AXI_RLAST),
	.S_AXI_RVALID  (M16_AXI_RVALID),
	.S_AXI_RREADY  (M16_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR17
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M17_AXI_AWID),
	.S_AXI_AWADDR  (M17_AXI_AWADDR),
	.S_AXI_AWLEN   (M17_AXI_AWLEN),
	.S_AXI_AWSIZE  (M17_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M17_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M17_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M17_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M17_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M17_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M17_AXI_AWVALID),
	.S_AXI_AWREADY (M17_AXI_AWREADY),
	.S_AXI_WDATA   (M17_AXI_WDATA),
	.S_AXI_WSTRB   (M17_AXI_WSTRB),
	.S_AXI_WLAST   (M17_AXI_WLAST),
	.S_AXI_WVALID  (M17_AXI_WVALID),
	.S_AXI_WREADY  (M17_AXI_WREADY),
	.S_AXI_BID     (M17_AXI_BID),//ignore
	.S_AXI_BRESP   (M17_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M17_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M17_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M17_AXI_ARID),
	.S_AXI_ARADDR  (M17_AXI_ARADDR),
	.S_AXI_ARLEN   (M17_AXI_ARLEN),
	.S_AXI_ARSIZE  (M17_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M17_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M17_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M17_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M17_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M17_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M17_AXI_ARVALID),
	.S_AXI_ARREADY (M17_AXI_ARREADY),
	.S_AXI_RID     (M17_AXI_RID),
	.S_AXI_RDATA   (M17_AXI_RDATA),
	.S_AXI_RRESP   (M17_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M17_AXI_RLAST),
	.S_AXI_RVALID  (M17_AXI_RVALID),
	.S_AXI_RREADY  (M17_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR18
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M18_AXI_AWID),
	.S_AXI_AWADDR  (M18_AXI_AWADDR),
	.S_AXI_AWLEN   (M18_AXI_AWLEN),
	.S_AXI_AWSIZE  (M18_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M18_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M18_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M18_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M18_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M18_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M18_AXI_AWVALID),
	.S_AXI_AWREADY (M18_AXI_AWREADY),
	.S_AXI_WDATA   (M18_AXI_WDATA),
	.S_AXI_WSTRB   (M18_AXI_WSTRB),
	.S_AXI_WLAST   (M18_AXI_WLAST),
	.S_AXI_WVALID  (M18_AXI_WVALID),
	.S_AXI_WREADY  (M18_AXI_WREADY),
	.S_AXI_BID     (M18_AXI_BID),//ignore
	.S_AXI_BRESP   (M18_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M18_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M18_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M18_AXI_ARID),
	.S_AXI_ARADDR  (M18_AXI_ARADDR),
	.S_AXI_ARLEN   (M18_AXI_ARLEN),
	.S_AXI_ARSIZE  (M18_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M18_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M18_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M18_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M18_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M18_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M18_AXI_ARVALID),
	.S_AXI_ARREADY (M18_AXI_ARREADY),
	.S_AXI_RID     (M18_AXI_RID),
	.S_AXI_RDATA   (M18_AXI_RDATA),
	.S_AXI_RRESP   (M18_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M18_AXI_RLAST),
	.S_AXI_RVALID  (M18_AXI_RVALID),
	.S_AXI_RREADY  (M18_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR19
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M19_AXI_AWID),
	.S_AXI_AWADDR  (M19_AXI_AWADDR),
	.S_AXI_AWLEN   (M19_AXI_AWLEN),
	.S_AXI_AWSIZE  (M19_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M19_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M19_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M19_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M19_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M19_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M19_AXI_AWVALID),
	.S_AXI_AWREADY (M19_AXI_AWREADY),
	.S_AXI_WDATA   (M19_AXI_WDATA),
	.S_AXI_WSTRB   (M19_AXI_WSTRB),
	.S_AXI_WLAST   (M19_AXI_WLAST),
	.S_AXI_WVALID  (M19_AXI_WVALID),
	.S_AXI_WREADY  (M19_AXI_WREADY),
	.S_AXI_BID     (M19_AXI_BID),//ignore
	.S_AXI_BRESP   (M19_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M19_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M19_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M19_AXI_ARID),
	.S_AXI_ARADDR  (M19_AXI_ARADDR),
	.S_AXI_ARLEN   (M19_AXI_ARLEN),
	.S_AXI_ARSIZE  (M19_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M19_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M19_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M19_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M19_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M19_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M19_AXI_ARVALID),
	.S_AXI_ARREADY (M19_AXI_ARREADY),
	.S_AXI_RID     (M19_AXI_RID),
	.S_AXI_RDATA   (M19_AXI_RDATA),
	.S_AXI_RRESP   (M19_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M19_AXI_RLAST),
	.S_AXI_RVALID  (M19_AXI_RVALID),
	.S_AXI_RREADY  (M19_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR20
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M20_AXI_AWID),
	.S_AXI_AWADDR  (M20_AXI_AWADDR),
	.S_AXI_AWLEN   (M20_AXI_AWLEN),
	.S_AXI_AWSIZE  (M20_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M20_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M20_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M20_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M20_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M20_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M20_AXI_AWVALID),
	.S_AXI_AWREADY (M20_AXI_AWREADY),
	.S_AXI_WDATA   (M20_AXI_WDATA),
	.S_AXI_WSTRB   (M20_AXI_WSTRB),
	.S_AXI_WLAST   (M20_AXI_WLAST),
	.S_AXI_WVALID  (M20_AXI_WVALID),
	.S_AXI_WREADY  (M20_AXI_WREADY),
	.S_AXI_BID     (M20_AXI_BID),//ignore
	.S_AXI_BRESP   (M20_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M20_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M20_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M20_AXI_ARID),
	.S_AXI_ARADDR  (M20_AXI_ARADDR),
	.S_AXI_ARLEN   (M20_AXI_ARLEN),
	.S_AXI_ARSIZE  (M20_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M20_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M20_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M20_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M20_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M20_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M20_AXI_ARVALID),
	.S_AXI_ARREADY (M20_AXI_ARREADY),
	.S_AXI_RID     (M20_AXI_RID),
	.S_AXI_RDATA   (M20_AXI_RDATA),
	.S_AXI_RRESP   (M20_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M20_AXI_RLAST),
	.S_AXI_RVALID  (M20_AXI_RVALID),
	.S_AXI_RREADY  (M20_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR21
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M21_AXI_AWID),
	.S_AXI_AWADDR  (M21_AXI_AWADDR),
	.S_AXI_AWLEN   (M21_AXI_AWLEN),
	.S_AXI_AWSIZE  (M21_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M21_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M21_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M21_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M21_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M21_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M21_AXI_AWVALID),
	.S_AXI_AWREADY (M21_AXI_AWREADY),
	.S_AXI_WDATA   (M21_AXI_WDATA),
	.S_AXI_WSTRB   (M21_AXI_WSTRB),
	.S_AXI_WLAST   (M21_AXI_WLAST),
	.S_AXI_WVALID  (M21_AXI_WVALID),
	.S_AXI_WREADY  (M21_AXI_WREADY),
	.S_AXI_BID     (M21_AXI_BID),//ignore
	.S_AXI_BRESP   (M21_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M21_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M21_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M21_AXI_ARID),
	.S_AXI_ARADDR  (M21_AXI_ARADDR),
	.S_AXI_ARLEN   (M21_AXI_ARLEN),
	.S_AXI_ARSIZE  (M21_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M21_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M21_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M21_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M21_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M21_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M21_AXI_ARVALID),
	.S_AXI_ARREADY (M21_AXI_ARREADY),
	.S_AXI_RID     (M21_AXI_RID),
	.S_AXI_RDATA   (M21_AXI_RDATA),
	.S_AXI_RRESP   (M21_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M21_AXI_RLAST),
	.S_AXI_RVALID  (M21_AXI_RVALID),
	.S_AXI_RREADY  (M21_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR22
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M22_AXI_AWID),
	.S_AXI_AWADDR  (M22_AXI_AWADDR),
	.S_AXI_AWLEN   (M22_AXI_AWLEN),
	.S_AXI_AWSIZE  (M22_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M22_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M22_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M22_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M22_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M22_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M22_AXI_AWVALID),
	.S_AXI_AWREADY (M22_AXI_AWREADY),
	.S_AXI_WDATA   (M22_AXI_WDATA),
	.S_AXI_WSTRB   (M22_AXI_WSTRB),
	.S_AXI_WLAST   (M22_AXI_WLAST),
	.S_AXI_WVALID  (M22_AXI_WVALID),
	.S_AXI_WREADY  (M22_AXI_WREADY),
	.S_AXI_BID     (M22_AXI_BID),//ignore
	.S_AXI_BRESP   (M22_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M22_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M22_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M22_AXI_ARID),
	.S_AXI_ARADDR  (M22_AXI_ARADDR),
	.S_AXI_ARLEN   (M22_AXI_ARLEN),
	.S_AXI_ARSIZE  (M22_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M22_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M22_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M22_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M22_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M22_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M22_AXI_ARVALID),
	.S_AXI_ARREADY (M22_AXI_ARREADY),
	.S_AXI_RID     (M22_AXI_RID),
	.S_AXI_RDATA   (M22_AXI_RDATA),
	.S_AXI_RRESP   (M22_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M22_AXI_RLAST),
	.S_AXI_RVALID  (M22_AXI_RVALID),
	.S_AXI_RREADY  (M22_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR23
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M23_AXI_AWID),
	.S_AXI_AWADDR  (M23_AXI_AWADDR),
	.S_AXI_AWLEN   (M23_AXI_AWLEN),
	.S_AXI_AWSIZE  (M23_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M23_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M23_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M23_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M23_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M23_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M23_AXI_AWVALID),
	.S_AXI_AWREADY (M23_AXI_AWREADY),
	.S_AXI_WDATA   (M23_AXI_WDATA),
	.S_AXI_WSTRB   (M23_AXI_WSTRB),
	.S_AXI_WLAST   (M23_AXI_WLAST),
	.S_AXI_WVALID  (M23_AXI_WVALID),
	.S_AXI_WREADY  (M23_AXI_WREADY),
	.S_AXI_BID     (M23_AXI_BID),//ignore
	.S_AXI_BRESP   (M23_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M23_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M23_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M23_AXI_ARID),
	.S_AXI_ARADDR  (M23_AXI_ARADDR),
	.S_AXI_ARLEN   (M23_AXI_ARLEN),
	.S_AXI_ARSIZE  (M23_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M23_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M23_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M23_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M23_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M23_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M23_AXI_ARVALID),
	.S_AXI_ARREADY (M23_AXI_ARREADY),
	.S_AXI_RID     (M23_AXI_RID),
	.S_AXI_RDATA   (M23_AXI_RDATA),
	.S_AXI_RRESP   (M23_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M23_AXI_RLAST),
	.S_AXI_RVALID  (M23_AXI_RVALID),
	.S_AXI_RREADY  (M23_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR24
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M24_AXI_AWID),
	.S_AXI_AWADDR  (M24_AXI_AWADDR),
	.S_AXI_AWLEN   (M24_AXI_AWLEN),
	.S_AXI_AWSIZE  (M24_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M24_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M24_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M24_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M24_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M24_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M24_AXI_AWVALID),
	.S_AXI_AWREADY (M24_AXI_AWREADY),
	.S_AXI_WDATA   (M24_AXI_WDATA),
	.S_AXI_WSTRB   (M24_AXI_WSTRB),
	.S_AXI_WLAST   (M24_AXI_WLAST),
	.S_AXI_WVALID  (M24_AXI_WVALID),
	.S_AXI_WREADY  (M24_AXI_WREADY),
	.S_AXI_BID     (M24_AXI_BID),//ignore
	.S_AXI_BRESP   (M24_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M24_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M24_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M24_AXI_ARID),
	.S_AXI_ARADDR  (M24_AXI_ARADDR),
	.S_AXI_ARLEN   (M24_AXI_ARLEN),
	.S_AXI_ARSIZE  (M24_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M24_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M24_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M24_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M24_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M24_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M24_AXI_ARVALID),
	.S_AXI_ARREADY (M24_AXI_ARREADY),
	.S_AXI_RID     (M24_AXI_RID),
	.S_AXI_RDATA   (M24_AXI_RDATA),
	.S_AXI_RRESP   (M24_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M24_AXI_RLAST),
	.S_AXI_RVALID  (M24_AXI_RVALID),
	.S_AXI_RREADY  (M24_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR25
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M25_AXI_AWID),
	.S_AXI_AWADDR  (M25_AXI_AWADDR),
	.S_AXI_AWLEN   (M25_AXI_AWLEN),
	.S_AXI_AWSIZE  (M25_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M25_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M25_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M25_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M25_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M25_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M25_AXI_AWVALID),
	.S_AXI_AWREADY (M25_AXI_AWREADY),
	.S_AXI_WDATA   (M25_AXI_WDATA),
	.S_AXI_WSTRB   (M25_AXI_WSTRB),
	.S_AXI_WLAST   (M25_AXI_WLAST),
	.S_AXI_WVALID  (M25_AXI_WVALID),
	.S_AXI_WREADY  (M25_AXI_WREADY),
	.S_AXI_BID     (M25_AXI_BID),//ignore
	.S_AXI_BRESP   (M25_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M25_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M25_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M25_AXI_ARID),
	.S_AXI_ARADDR  (M25_AXI_ARADDR),
	.S_AXI_ARLEN   (M25_AXI_ARLEN),
	.S_AXI_ARSIZE  (M25_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M25_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M25_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M25_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M25_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M25_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M25_AXI_ARVALID),
	.S_AXI_ARREADY (M25_AXI_ARREADY),
	.S_AXI_RID     (M25_AXI_RID),
	.S_AXI_RDATA   (M25_AXI_RDATA),
	.S_AXI_RRESP   (M25_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M25_AXI_RLAST),
	.S_AXI_RVALID  (M25_AXI_RVALID),
	.S_AXI_RREADY  (M25_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR26
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M26_AXI_AWID),
	.S_AXI_AWADDR  (M26_AXI_AWADDR),
	.S_AXI_AWLEN   (M26_AXI_AWLEN),
	.S_AXI_AWSIZE  (M26_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M26_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M26_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M26_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M26_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M26_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M26_AXI_AWVALID),
	.S_AXI_AWREADY (M26_AXI_AWREADY),
	.S_AXI_WDATA   (M26_AXI_WDATA),
	.S_AXI_WSTRB   (M26_AXI_WSTRB),
	.S_AXI_WLAST   (M26_AXI_WLAST),
	.S_AXI_WVALID  (M26_AXI_WVALID),
	.S_AXI_WREADY  (M26_AXI_WREADY),
	.S_AXI_BID     (M26_AXI_BID),//ignore
	.S_AXI_BRESP   (M26_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M26_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M26_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M26_AXI_ARID),
	.S_AXI_ARADDR  (M26_AXI_ARADDR),
	.S_AXI_ARLEN   (M26_AXI_ARLEN),
	.S_AXI_ARSIZE  (M26_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M26_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M26_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M26_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M26_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M26_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M26_AXI_ARVALID),
	.S_AXI_ARREADY (M26_AXI_ARREADY),
	.S_AXI_RID     (M26_AXI_RID),
	.S_AXI_RDATA   (M26_AXI_RDATA),
	.S_AXI_RRESP   (M26_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M26_AXI_RLAST),
	.S_AXI_RVALID  (M26_AXI_RVALID),
	.S_AXI_RREADY  (M26_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR27
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M27_AXI_AWID),
	.S_AXI_AWADDR  (M27_AXI_AWADDR),
	.S_AXI_AWLEN   (M27_AXI_AWLEN),
	.S_AXI_AWSIZE  (M27_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M27_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M27_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M27_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M27_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M27_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M27_AXI_AWVALID),
	.S_AXI_AWREADY (M27_AXI_AWREADY),
	.S_AXI_WDATA   (M27_AXI_WDATA),
	.S_AXI_WSTRB   (M27_AXI_WSTRB),
	.S_AXI_WLAST   (M27_AXI_WLAST),
	.S_AXI_WVALID  (M27_AXI_WVALID),
	.S_AXI_WREADY  (M27_AXI_WREADY),
	.S_AXI_BID     (M27_AXI_BID),//ignore
	.S_AXI_BRESP   (M27_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M27_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M27_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M27_AXI_ARID),
	.S_AXI_ARADDR  (M27_AXI_ARADDR),
	.S_AXI_ARLEN   (M27_AXI_ARLEN),
	.S_AXI_ARSIZE  (M27_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M27_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M27_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M27_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M27_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M27_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M27_AXI_ARVALID),
	.S_AXI_ARREADY (M27_AXI_ARREADY),
	.S_AXI_RID     (M27_AXI_RID),
	.S_AXI_RDATA   (M27_AXI_RDATA),
	.S_AXI_RRESP   (M27_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M27_AXI_RLAST),
	.S_AXI_RVALID  (M27_AXI_RVALID),
	.S_AXI_RREADY  (M27_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR28
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M28_AXI_AWID),
	.S_AXI_AWADDR  (M28_AXI_AWADDR),
	.S_AXI_AWLEN   (M28_AXI_AWLEN),
	.S_AXI_AWSIZE  (M28_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M28_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M28_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M28_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M28_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M28_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M28_AXI_AWVALID),
	.S_AXI_AWREADY (M28_AXI_AWREADY),
	.S_AXI_WDATA   (M28_AXI_WDATA),
	.S_AXI_WSTRB   (M28_AXI_WSTRB),
	.S_AXI_WLAST   (M28_AXI_WLAST),
	.S_AXI_WVALID  (M28_AXI_WVALID),
	.S_AXI_WREADY  (M28_AXI_WREADY),
	.S_AXI_BID     (M28_AXI_BID),//ignore
	.S_AXI_BRESP   (M28_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M28_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M28_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M28_AXI_ARID),
	.S_AXI_ARADDR  (M28_AXI_ARADDR),
	.S_AXI_ARLEN   (M28_AXI_ARLEN),
	.S_AXI_ARSIZE  (M28_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M28_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M28_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M28_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M28_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M28_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M28_AXI_ARVALID),
	.S_AXI_ARREADY (M28_AXI_ARREADY),
	.S_AXI_RID     (M28_AXI_RID),
	.S_AXI_RDATA   (M28_AXI_RDATA),
	.S_AXI_RRESP   (M28_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M28_AXI_RLAST),
	.S_AXI_RVALID  (M28_AXI_RVALID),
	.S_AXI_RREADY  (M28_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR29
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M29_AXI_AWID),
	.S_AXI_AWADDR  (M29_AXI_AWADDR),
	.S_AXI_AWLEN   (M29_AXI_AWLEN),
	.S_AXI_AWSIZE  (M29_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M29_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M29_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M29_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M29_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M29_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M29_AXI_AWVALID),
	.S_AXI_AWREADY (M29_AXI_AWREADY),
	.S_AXI_WDATA   (M29_AXI_WDATA),
	.S_AXI_WSTRB   (M29_AXI_WSTRB),
	.S_AXI_WLAST   (M29_AXI_WLAST),
	.S_AXI_WVALID  (M29_AXI_WVALID),
	.S_AXI_WREADY  (M29_AXI_WREADY),
	.S_AXI_BID     (M29_AXI_BID),//ignore
	.S_AXI_BRESP   (M29_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M29_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M29_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M29_AXI_ARID),
	.S_AXI_ARADDR  (M29_AXI_ARADDR),
	.S_AXI_ARLEN   (M29_AXI_ARLEN),
	.S_AXI_ARSIZE  (M29_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M29_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M29_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M29_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M29_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M29_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M29_AXI_ARVALID),
	.S_AXI_ARREADY (M29_AXI_ARREADY),
	.S_AXI_RID     (M29_AXI_RID),
	.S_AXI_RDATA   (M29_AXI_RDATA),
	.S_AXI_RRESP   (M29_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M29_AXI_RLAST),
	.S_AXI_RVALID  (M29_AXI_RVALID),
	.S_AXI_RREADY  (M29_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR30
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M30_AXI_AWID),
	.S_AXI_AWADDR  (M30_AXI_AWADDR),
	.S_AXI_AWLEN   (M30_AXI_AWLEN),
	.S_AXI_AWSIZE  (M30_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M30_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M30_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M30_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M30_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M30_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M30_AXI_AWVALID),
	.S_AXI_AWREADY (M30_AXI_AWREADY),
	.S_AXI_WDATA   (M30_AXI_WDATA),
	.S_AXI_WSTRB   (M30_AXI_WSTRB),
	.S_AXI_WLAST   (M30_AXI_WLAST),
	.S_AXI_WVALID  (M30_AXI_WVALID),
	.S_AXI_WREADY  (M30_AXI_WREADY),
	.S_AXI_BID     (M30_AXI_BID),//ignore
	.S_AXI_BRESP   (M30_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M30_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M30_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M30_AXI_ARID),
	.S_AXI_ARADDR  (M30_AXI_ARADDR),
	.S_AXI_ARLEN   (M30_AXI_ARLEN),
	.S_AXI_ARSIZE  (M30_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M30_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M30_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M30_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M30_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M30_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M30_AXI_ARVALID),
	.S_AXI_ARREADY (M30_AXI_ARREADY),
	.S_AXI_RID     (M30_AXI_RID),
	.S_AXI_RDATA   (M30_AXI_RDATA),
	.S_AXI_RRESP   (M30_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M30_AXI_RLAST),
	.S_AXI_RVALID  (M30_AXI_RVALID),
	.S_AXI_RREADY  (M30_AXI_RREADY)
);


HBM_AXI_HP_Slave_DDR #
(
    .S_AXI_ID_WIDTH(1),
    .S_AXI_DATA_WIDTH(`HBM_AXI_DATA_WIDTH),
    .S_AXI_ADDR_WIDTH(`HBM_ADDR_WIDTH),
    .BW_div_FREQ_100(1300)
)HBM_AXI_HP_Slave_DDR31
(
    .S_AXI_ACLK    (clk),
    .S_AXI_ARESETN (rst_n),
	.S_AXI_AWID    (M31_AXI_AWID),
	.S_AXI_AWADDR  (M31_AXI_AWADDR),
	.S_AXI_AWLEN   (M31_AXI_AWLEN),
	.S_AXI_AWSIZE  (M31_AXI_AWSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_AWBURST (M31_AXI_AWBURST),//=2'b01),
	.S_AXI_AWLOCK  (M31_AXI_AWLOCK),//1'b0),
	.S_AXI_AWCACHE (M31_AXI_AWCACHE),//=4'b10
	.S_AXI_AWPROT  (M31_AXI_AWPROT),//=3'h0),
	.S_AXI_AWQOS   (M31_AXI_AWQOS),//=4'h0),
	.S_AXI_AWVALID (M31_AXI_AWVALID),
	.S_AXI_AWREADY (M31_AXI_AWREADY),
	.S_AXI_WDATA   (M31_AXI_WDATA),
	.S_AXI_WSTRB   (M31_AXI_WSTRB),
	.S_AXI_WLAST   (M31_AXI_WLAST),
	.S_AXI_WVALID  (M31_AXI_WVALID),
	.S_AXI_WREADY  (M31_AXI_WREADY),
	.S_AXI_BID     (M31_AXI_BID),//ignore
	.S_AXI_BRESP   (M31_AXI_BRESP),//ignore
	.S_AXI_BVALID  (M31_AXI_BVALID),//Bvalid and Bread means a a write response
	.S_AXI_BREADY  (M31_AXI_BREADY),//Bvalid and Bread means a a write response
	.S_AXI_ARID    (M31_AXI_ARID),
	.S_AXI_ARADDR  (M31_AXI_ARADDR),
	.S_AXI_ARLEN   (M31_AXI_ARLEN),
	.S_AXI_ARSIZE  (M31_AXI_ARSIZE),//=clogb2((`HBM_AXI_DATA_WIDTH/8)-1),
	.S_AXI_ARBURST (M31_AXI_ARBURST),//=2'b01),
	.S_AXI_ARLOCK  (M31_AXI_ARLOCK),//=1'b0),
	.S_AXI_ARCACHE (M31_AXI_ARCACHE),//=4'b10),
	.S_AXI_ARPROT  (M31_AXI_ARPROT),//=3'h0),
	.S_AXI_ARQOS   (M31_AXI_ARQOS),//=4'h0),
	.S_AXI_ARVALID (M31_AXI_ARVALID),
	.S_AXI_ARREADY (M31_AXI_ARREADY),
	.S_AXI_RID     (M31_AXI_RID),
	.S_AXI_RDATA   (M31_AXI_RDATA),
	.S_AXI_RRESP   (M31_AXI_RRESP),//ignore
	.S_AXI_RLAST   (M31_AXI_RLAST),
	.S_AXI_RVALID  (M31_AXI_RVALID),
	.S_AXI_RREADY  (M31_AXI_RREADY)
);


endmodule
