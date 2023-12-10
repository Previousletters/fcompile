`include "TOP_defines.vh"


task Map_Weight_Tout(input bit [(`WT_DW-1):0] weight_in[`CHout][`CHin],
                    output bit [`WT_DW *`Tout-1:0]output_wt[`CHout_div_Tout*`WT_CHin_div_Tin*`Tout*`Tin_div_Tout]);
begin
  
    bit [`WT_DW-1:0]       weight_reorg[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin];
    bit [`WT_DW*`Tin-1:0] weight_Tin[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout];
    bit [`WT_DW*`Tout-1:0]  weight_Tout[`CHout_div_Tout][`WT_CHin_div_Tin][`Tout][`Tin_div_Tout];    

    integer addr;
    integer M;

    addr=0;
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
                for (int tin=0; tin<`Tin;tin=tin+1)
                begin
                    bit [`WT_DW-1:0] tp1;
                    tp1=0;
                    if ((chout*`Tout+tout < `CHout) && (chin*`Tin+tin<`CHin))
                        tp1 = weight_in[chout*`Tout+tout][chin*`Tin+tin];
                    else
                        tp1 = 0;
                    weight_reorg[chout][chin][tout][tin] = tp1;
//					$display("weight_reorg[chout%0d][chin%0d][tout%0d][tin%0d]=%0d",chout,chin,tout,tin,tp1);
                end
    
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
            begin
                bit [`WT_DW*`Tin-1:0] tp2;
                tp2 = 0;
                for (int tin=0; tin<`Tin;tin=tin+1)
                    tp2[`WT_DW*tin+:`WT_DW]=weight_reorg[chout][chin][tout][tin];
                weight_Tin[chout][chin][tout] = tp2;
//                $display("output_wt_Tin[chout%0d][chin%0d][tout%0d]=%0d",chout,chin,tout,tp2);
            end
    
	for (int chout=0; chout<`CHout_div_Tout;chout=chout+1)
		for (int chin=0; chin<`WT_CHin_div_Tin;chin=chin+1)
            for (int tout=0; tout<`Tout;tout=tout+1)
            begin
                 bit [`WT_DW*`Tin-1:0]tp3;
                 bit [`WT_DW*`Tout-1:0] tp4;
                 tp3 = weight_Tin[chout][chin][tout];
                 tp4 = 0;
//               $display("output_wt_Tin[chout%0d][chin%0d][tout%0d]=%0d",chout,chin,tout,tp3);
                 for (int tin_out=0; tin_out<`Tin_div_Tout;tin_out=tin_out+1)
                 begin
                     weight_Tout[chout][chin][tout][tin_out] = tp3[`WT_DW *`Tout*tin_out+:`WT_DW *`Tout];
                     tp4 = weight_Tout[chout][chin][tout][tin_out];
                     output_wt[addr] = tp4;
//                   $display("output_wt_Tout[%0d]=%0d",addr,weight_Tout[chout][chin][tout][tin_out]);
//                   $display("tp4[%0d]=%0d",addr,tp4);
//                   $display("output_wt[%0d]=%0h",addr,output_wt[addr]);
                     addr = addr + 1;
                 end
            end	
//	$display("addr=%0d",addr);
end	
endtask	



task RunMVM_soft(input bit[31:0]CHin, bit[31:0]Hin,bit[31:0]Win,bit[31:0]CHout,
		bit[`DAT_DW_L0-1:0] feature_in[`Tb][`Hin][`Win][`CHin],
		bit[(`WT_DW-1):0] Wt[`CHout][`CHin],
		bit [`WT_quant_scale_DW-1:0] wt_FP_scale[`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout],
		output bit[`MAX_BN_DW-1:0] feature_out[`Tb][`Hout][`Wout][`CHout],
		output real real_fp16_feature_out[`Tb][`Hout][`Wout][`CHout],
		output real real_fp32_feature_out[`Tb][`Hout][`Wout][`CHout]);
begin
	bit[31:0] Wout;
	bit[31:0] Hout;
    
	Wout=Win;
	Hout=Hin;
	//$display("Conv:Wout=%0d,Hout=%0d",Wout,Hout);

	for(int b=0;b<`Tb;b++)
        for(int chout=0;chout<CHout;chout++)
            for(int hout=0;hout<Hout;hout++)
            begin
                for(int wout=0;wout<Wout;wout++)
                begin
                    real real_tp_out_fp32,real_tp_out_fp16;
                    real_tp_out_fp32=0;

                    for(int chin=0;chin<CHin;chin++)
                    begin
                        bit[(`DAT_DW_L0-1):0] data;
                        real real_data,real_wt,real_wt_scale;
                        int hin,win;
                        hin=hout;
                        win=wout;
                        if( (hin<0) || (hin>=Hin) || (win<0) || (win>=Win) )//padding 0
                            data=0;
                        else
                            data=feature_in[b][hin][win][chin];
                        
                        FP16_to_real(data,real_data);
                        FP16_to_real(wt_FP_scale[chout/`Tout][chin/`T_quant_block][chout%`Tout],real_wt_scale);
                        real_wt=real'($signed(Wt[chout][chin]))*real_wt_scale;
                        
                        real_tp_out_fp32=real_tp_out_fp32 + real'(real_data)*real'(real_wt);
//                        $display("******INT_data=%b, INT_wt=%d",data,$signed(W[ky][kx][chin][chout]));
//                        $display("******INT_wt_scale=%b, real_wt_scale=%f",$signed(wt_FP_scale[chout/`Tout][chin/`base_Tin][chout%`Tout]),real_wt_scale);
//                        $display("real_data=%f, real_wt=%f, real_tp_out_fp32=%f",real_data,real_wt,real_tp_out_fp32);
                    end

                    real_to_FP16(real_tp_out_fp32,feature_out[b][hout][wout][chout]);
                    FP16_to_real(feature_out[b][hout][wout][chout],real_tp_out_fp16);
                    real_fp16_feature_out[b][hout][wout][chout]=real_tp_out_fp16;
                    real_fp32_feature_out[b][hout][wout][chout]=real_tp_out_fp32;
//                  $display("feature_out[%0d][%0d][%0d][%0d]=%0b",b,hout,wout,chout,$signed(feature_out[b][hout][wout][chout]));
//                  $display("fp32_out=%f, fp16_out=%f",real_tp_out_fp32,real_tp_out_fp16);
                end
            end
end
endtask



