task CSB_Write(input bit[9:0] addr,input bit[31:0] data);
begin
	AXI_GP_Master_CPU.Xil_Out32(addr<<2,data);
end
endtask

task CSB_Read(input bit[9:0] addr,output bit[31:0] data);
begin
	AXI_GP_Master_CPU.Xil_In32(addr<<2,data);
end
endtask

task Map_Feature_Data(input bit [(`DAT_DW_L0-1):0] in[`Hin_L0][`Win_L0][`CHin_L0],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[`Win_L0*`Hin_L0*((`CHin_Exp_L0+`Tout-1)/`Tout)]);
begin
	for(int i=0;i<`Hin_L0;i++)
		for(int j=0;j<`Win_L0;j++)
		begin
			for(int k=0;k<`CHin_Exp_L0;k=k+`Tout)
			begin
				bit [`MAX_DAT_DW *`Tout-1:0]tp;
				tp=0;
				for(int kk=k;kk<k+`Tout;kk++)
				begin
					if(kk<`CHin_L0) 
					   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[i][j][kk]);
				end
				mem[`Win_L0*`Hin_L0*(k/`Tout)+i*`Win_L0+j]=tp;
			end
        end
end
endtask

task Map_Clstoken_Data(input bit [(`DAT_DW_L0-1):0] in[1][1][`CHin_L0],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[((`CHin_Exp_L0+`Tout-1)/`Tout)]);
begin
    for(int k=0;k<`CHin_Exp_L0;k=k+`Tout)
    begin
        bit [`MAX_DAT_DW *`Tout-1:0]tp;
        tp=0;
        for(int kk=k;kk<k+`Tout;kk++)
        begin
            if(kk<`CHin_L0) 
               tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[0][0][kk]);
        end
        mem[(k/`Tout)]=tp;
    end
end
endtask

task Map_Pool_Feature_Data(input bit [(`DAT_DW_L0-1):0] in[`Hin_L0][`Win_L0][`CHin_L0],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[`Win_L0*`Hin_L0*((`CHin_L0+`Tout-1)/`Tout)]);
begin
	for(int i=0;i<`Hin_L0;i++)
		for(int j=0;j<`Win_L0;j++)
		begin
			for(int k=0;k<`CHin_L0;k=k+`Tout)
			begin
				bit [`MAX_DAT_DW *`Tout-1:0]tp;
				tp=0;
				for(int kk=k;kk<k+`Tout;kk++)
				begin
					if(kk<`CHin_L0) 
					   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[i][j][kk]);
				end
				mem[`Win_L0*`Hin_L0*(k/`Tout)+i*`Win_L0+j]=tp;
			end
        end
end
endtask

task Map_Res_Data(input bit [(`DAT_DW_L1-1):0] in[`Hin_L0][`Win_L0][`CHout_L0],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[`Win_L0*`Hin_L0*((`CHout_L0+`Tout-1)/`Tout)]);
begin
	for(int i=0;i<`Hin_L0;i++)
		for(int j=0;j<`Win_L0;j++)
		begin
			for(int k=0;k<`CHout_L0;k=k+`Tout)
			begin
				bit [`MAX_DAT_DW *`Tout-1:0]tp;
				tp=0;
				for(int kk=k;kk<k+`Tout;kk++)
				begin
					if(kk<`CHout_L0) 
					   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[i][j][kk]);
				end
				mem[`Win_L0*`Hin_L0*(k/`Tout)+i*`Win_L0+j]=tp;
			end
        end
end
endtask

task Map_Array_B(input bit [(`DAT_DW_L0-1):0] in[`Hin_L0][`Win_L0][`CHout_L0],
                      output bit [`MAX_DAT_DW*`Tout-1:0]mem[`Win_L0*`Hin_L0*((`CHout_L0+`Tout-1)/`Tout)]);
begin
	for(int i=0;i<`Hin_L0;i++)
		for(int j=0;j<`Win_L0;j++)
		begin
			for(int k=0;k<`CHout_L0;k=k+`Tout)
			begin
				bit [`MAX_DAT_DW *`Tout-1:0]tp;
				tp=0;
				for(int kk=k;kk<k+`Tout;kk++)
				begin
					if(kk<`CHout_L0) 
					   tp[(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW]=$signed(in[i][j][kk]);
				end
				mem[`Win_L0*`Hin_L0*(k/`Tout)+i*`Win_L0+j]=tp;
			end
        end
end
endtask

task DeMap_Feature_Data(input bit [`MAX_DAT_DW *`Tout-1:0]mem[`Wout_L0*`Hout_L0*((`CHout_L0+`Tout-1)/`Tout)],
                        output bit [(`DAT_DW_L1-1):0] out[`Hout_L0][`Wout_L0][`CHout_L0]);
begin
	for(int i=0;i<`Hout_L0;i++)
		for(int j=0;j<`Wout_L0;j++)
			for(int k=0;k<`CHout_L0;k=k+`Tout)
			begin
				for(int kk=k;kk<k+`Tout;kk++)
				begin
					if(kk<`CHout_L0) 
					   out[i][j][kk]=mem[`Wout_L0*`Hout_L0*(k/`Tout)+i*`Wout_L0+j][(kk-k)*`MAX_DAT_DW+:`MAX_DAT_DW];
				end
			end
end
endtask

task Map_Weight_Tout(input bit [(`WT_DW_L0-1):0] weight_in[`Ky_L0][`Kx_L0][`CHin_L0][`CHout_L0],
                    output bit [`WT_DW_L0 *`Tout-1:0]output_wt[`slice_of_CHout_L0*`slice_of_CHin_L0*`Ky_L0*`Kx_L0*`Tout*`slice_of_Tin_div_Tout_L0]);
begin

    bit [`WT_DW_L0-1:0] weight_reorg[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout][`Tin_L0];
    bit [`WT_DW_L0*`Tin_L0-1:0] weight_Tin[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout];
    bit [`WT_DW_L0 *`Tout-1:0] weight_Tout[`slice_of_CHout_L0][`slice_of_CHin_L0][`Ky_L0][`Kx_L0][`Tout][`slice_of_Tin_div_Tout_L0];   

    integer addr;
    integer M;

    addr=0;
	for (int chout=0; chout<`slice_of_CHout_L0;chout=chout+1)
	begin
		for (int chin=0; chin<`slice_of_CHin_L0;chin=chin+1)
			for (int ky=0; ky<`Ky_L0;ky=ky+1)
				for (int kx=0; kx<`Kx_L0;kx=kx+1)
					for (int tout=0; tout<`Tout;tout=tout+1)
						for (int tin=0; tin<`Tin_L0;tin=tin+1)
						begin
							bit [`WT_DW_L0-1:0] tp1;
							if ((chout*`Tout+tout < `CHout_L0) && (chin*`Tin_L0+tin<`CHin_L0))
								tp1 = weight_in[ky][kx][chin*`Tin_L0+tin][chout*`Tout+tout];
							else
								tp1 = 0;
							weight_reorg[chout][chin][ky][kx][tout][tin] = tp1;
						end
    end
    
	for (int chout=0; chout<`slice_of_CHout_L0;chout=chout+1)
	begin
		for (int chin=0; chin<`slice_of_CHin_L0;chin=chin+1)
			for (int ky=0; ky<`Ky_L0;ky=ky+1)
				for (int kx=0; kx<`Kx_L0;kx=kx+1)
					for (int tout=0; tout<`Tout;tout=tout+1)
					begin
					    if (chout*`Tout+tout < `CHout_L0)
					    begin
                            bit [`WT_DW_L0*`Tin_L0-1:0] tp2;
                            tp2 = 0;
                            for (int tin=0; tin<`Tin_L0;tin=tin+1)
                            begin
                                tp2[`WT_DW_L0*tin+:`WT_DW_L0] =  weight_reorg[chout][chin][ky][kx][tout][tin];
                            end
                            weight_Tin[chout][chin][ky][kx][tout] = tp2;
    //						$display("output_wt_Tin[%0d][%0d][%0d][%0d][%0d]=%0d",chout,chin,ky,kx,tout,tp2);
                        end
					end
    end
    
	for (int chout=0; chout<`slice_of_CHout_L0;chout=chout+1)
	begin
		for (int chin=0; chin<`slice_of_CHin_L0;chin=chin+1)
			for (int ky=0; ky<`Ky_L0;ky=ky+1)
				for (int kx=0; kx<`Kx_L0;kx=kx+1)
					for (int tout=0; tout<`Tout;tout=tout+1)
					begin
                        if (chout*`Tout+tout < `CHout_L0)
                        begin
                            bit [`WT_DW_L0*`Tin_L0-1:0] tp3;
                            bit [`WT_DW_L0*`Tout-1:0] tp4;
                            tp3 = weight_Tin[chout][chin][ky][kx][tout];
                            tp4 = 0;
    //						$display("output_wt_Tin[%0d][%0d][%0d][%0d][%0d]=%0d",chout,chin,ky,kx,tout,tp3);
                            for (int tin_out=0; tin_out<`slice_of_Tin_div_Tout_L0;tin_out=tin_out+1)
                            begin
                                weight_Tout[chout][chin][ky][kx][tout][tin_out] = tp3[`WT_DW_L0 *`Tout*tin_out+:`WT_DW_L0 *`Tout];
                                tp4 = weight_Tout[chout][chin][ky][kx][tout][tin_out];
                                output_wt[addr] = tp4;
    //                            $display("output_wt_Tout[%0d]=%0d",addr,weight_Tout[chout][chin][ky][kx][tout][tin_out]);
    //                            $display("tp4[%0d]=%0d",addr,tp4);
    //                            $display("output_wt[%0d]=%0d",addr,output_wt[addr]);
                                addr = addr + 1;
                            end
                        end
					end	
	end
//	$display("addr=%0d",addr);
end	
endtask	


task RunConv_soft(input bit[31:0]CHin, bit[31:0]Hin,bit[31:0]Win,bit[31:0]CHout,
		bit[31:0]Kx, bit[31:0]Ky,bit[31:0]Sx,bit[31:0]Sy, bit[31:0]pad_x, bit[31:0]pad_y,
		bit [4:0]in_scale, bit[4:0]wt_scale, bit[4:0] out_scale, bit[4:0]L0_DW,bit[4:0]L1_DW,bit relu_en,
		bit[`DAT_DW_L0-1:0] feature_in[`Hin_L0][`Win_L0][`CHin_L0],
		bit[(`WT_DW_L0-1):0] W[`Ky_L0][`Kx_L0][`CHin_L0][`CHout_L0],
		output bit[(`MAX_BN_DW-1):0] feature_out[`Hout_L0][`Wout_L0][`CHout_L0]);
begin
	bit[31:0] out_width;
	bit[31:0] out_height;
	
    bit [`log2_scale-2:0]shift_value;
    bit shift_sign;
    bit [`log2_scale-1:0]out_truncate;
    shift_sign=0;//right shift
    shift_value[`log2_scale-2:0]=(in_scale+wt_scale)-out_scale;
    if($signed(shift_value)<0)
    begin
        shift_value=out_scale-(in_scale+wt_scale);
        shift_sign=1; //left shift
    end   
    out_truncate={shift_sign,shift_value};
    
	out_width=((Win+2*pad_x-Kx)/Sx+1);
	out_height=((Hin+2*pad_y-Ky)/Sy+1);
	//$display("Conv:out_width=%0d,out_height=%0d",out_width,out_height);

	for(int i=0;i<CHout;i++)
		for(int j=0;j<out_height;j++)
		begin
			for(int k=0;k<out_width;k++)
			begin
				bit signed [(`MAX_DW2+`base_log2Tin+`log2_KyKx-1):0] tp;
				bit [(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1):0]tp2;
				bit [(`DAT_DW_L1-1):0]tp_sat;
				tp=0;
				for(int kj=0;kj<Ky;kj++)
				begin
					for(int ki=0;ki<Kx;ki++)
					begin
						for(int chi=0;chi<CHin;chi++)
						begin
							bit[(`DAT_DW_L0-1):0] data;
							int axis_h;
							int axis_w;
							axis_h=Sy*j+kj-pad_y;axis_w=Sx*k+ki-pad_x;
							if( (axis_h<0) || (axis_h>=Hin) || (axis_w<0) || (axis_w>=Win) )//padding 0
								data=0;
							else
								data=feature_in[axis_h][axis_w][chi];
						
                            if(`DAT_DW_L0==1)
                                tp=tp + (data==W[kj][ki][chi][i]?1:0);
                            else
                                tp=tp + ($signed(data)*$signed(W[kj][ki][chi][i]));
                            
						end
//						$display("tp_once=%0d",$signed(tp));	
					end
				end

                //right shift and round
                if(!out_truncate[`log2_scale-1])
                begin
//                    $display("tp=%0d",$signed(tp));
                    tp2=$signed(tp)>>>out_truncate[`log2_scale-2:0];
                    if((out_truncate!=0)&&(tp2!={1'b0, {(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1){1'b1}} }))
                        tp2=tp2+tp[out_truncate-1];
//                    $display("tp2=%0d",$signed(tp2));    
                    
                    if(`DAT_DW_L1==1)
                        tp_sat=($signed(tp2)<0)?1:0;
                    else
                        begin
                        if( (tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1)]) & (!(&tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):`DAT_DW_L1-1])) ) //tp2 is negetive and tp2[32-2:15] is not all 1, means tp2 < 16'h8000(-2^15)
                            tp_sat={1'b1,{(`DAT_DW_L1-1){1'b0}}};//8'h80; //sat to -2^7
                        else
                        begin
                            if( (!tp2[`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):(`DAT_DW_L1-1)]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
                                tp_sat={1'b0,{(`DAT_DW_L1-1){1'b1}}};//`DW'h7fff;
                            else
                                tp_sat=tp2[(`DAT_DW_L1-1):0];
                        end
                    end
//                    $display("tp_sat=%0d",$signed(tp_sat));
                end
                else
                begin
                    tp2=$signed(tp)<<out_truncate[`log2_scale-2:0];
                    tp_sat=tp2[(`DAT_DW_L1-1):0];
                end

                feature_out[j][k][i]=$signed(tp_sat);
//				$display("feature_out[%0d][%0d][%0d]=%0d",j,k,i,$signed(feature_out[j][k][i]));
			end
		end
end
endtask


task RunConv_soft_BNdw_out(input bit[31:0]CHin, bit[31:0]Hin,bit[31:0]Win,bit[31:0]CHout,
		bit[31:0]Kx, bit[31:0]Ky,bit[31:0]Sx,bit[31:0]Sy, bit[31:0]pad_x, bit[31:0]pad_y,
		bit [4:0]in_scale, bit[4:0]wt_scale, bit[4:0] out_scale, bit[4:0]L0_DW,bit[4:0]L1_DW,bit relu_en,
		bit[`DAT_DW_L0-1:0] feature_in[`Hin_L0][`Win_L0][`CHin_L0],
		bit[(`WT_DW_L0-1):0] W[`Ky_L0][`Kx_L0][`CHin_L0][`CHout_L0],
		output bit[(`MAX_BN_DW-1):0] feature_out[`Hout_L0][`Wout_L0][`CHout_L0]);
begin
	bit[31:0] out_width;
	bit[31:0] out_height;
	
    bit [`log2_scale-2:0]shift_value;
    bit shift_sign;
    bit [`log2_scale-1:0]out_truncate;
    shift_sign=0;//right shift
    shift_value[`log2_scale-2:0]=(in_scale+wt_scale)-out_scale;
    if($signed(shift_value)<0)
    begin
        shift_value=out_scale-(in_scale+wt_scale);
        shift_sign=1; //left shift
    end   
    out_truncate={shift_sign,shift_value};
    
	out_width=((Win+2*pad_x-Kx)/Sx+1);
	out_height=((Hin+2*pad_y-Ky)/Sy+1);
	//$display("Conv:out_width=%0d,out_height=%0d",out_width,out_height);

	for(int i=0;i<CHout;i++)
		for(int j=0;j<out_height;j++)
		begin
			for(int k=0;k<out_width;k++)
			begin
				bit signed [(`MAX_DW2+`base_log2Tin+`log2_KyKx-1):0] tp;
				bit [(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1):0]tp2;
				bit [(`MAX_BN_DW-1):0]tp_sat;
				tp=0;
				for(int kj=0;kj<Ky;kj++)
				begin
					for(int ki=0;ki<Kx;ki++)
					begin
						for(int chi=0;chi<CHin;chi++)
						begin
							bit[(`DAT_DW_L0-1):0] data;
							int axis_h;
							int axis_w;
							axis_h=Sy*j+kj-pad_y;axis_w=Sx*k+ki-pad_x;
							if( (axis_h<0) || (axis_h>=Hin) || (axis_w<0) || (axis_w>=Win) )//padding 0
								data=0;
							else
								data=feature_in[axis_h][axis_w][chi];
						
                            if(`DAT_DW_L0==1)
                                tp=tp + (data==W[kj][ki][chi][i]?1:0);
                            else
                                tp=tp + ($signed(data)*$signed(W[kj][ki][chi][i]));
                            
						end
//						$display("tp_once=%0d",$signed(tp));	
					end
				end

                //right shift and round
                if(!out_truncate[`log2_scale-1])
                begin
//                    $display("tp=%0d",$signed(tp));
                    tp2=$signed(tp)>>>out_truncate[`log2_scale-2:0];
                    if((out_truncate!=0)&&(tp2!={1'b0, {(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1){1'b1}} }))
                        tp2=tp2+tp[out_truncate-1];
//                    $display("tp2=%0d",$signed(tp2));    
                    
                    if(`DAT_DW_L1==1)
                        tp_sat=($signed(tp2)<0)?1:0;
                    else
                        begin
                        if( (tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1)]) & (!(&tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):`MAX_BN_DW-1])) ) //tp2 is negetive and tp2[32-2:15] is not all 1, means tp2 < 16'h8000(-2^15)
                            tp_sat={1'b1,{(`MAX_BN_DW-1){1'b0}}};//8'h80; //sat to -2^7
                        else
                        begin
                            if( (!tp2[`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-1]) & (|tp2[(`MAX_DW2+`base_log2Tin+`log2_KyKx+`log2_other-2):(`MAX_BN_DW-1)]) ) //tp2 is positive and tp2>`MAX_DW2'h7fff
                                tp_sat={1'b0,{(`MAX_BN_DW-1){1'b1}}};//`DW'h7fff;
                            else
                                tp_sat=tp2[(`MAX_BN_DW-1):0];
                        end
                    end
//                    $display("tp_sat=%0d",$signed(tp_sat));
                end
                else
                begin
                    tp2=$signed(tp)<<out_truncate[`log2_scale-2:0];
                    tp_sat=tp2[(`MAX_BN_DW-1):0];
                end

                feature_out[j][k][i]=$signed(tp_sat);
//				$display("feature_out[%0d][%0d][%0d]=%0d",j,k,i,$signed(feature_out[j][k][i]));
			end
		end
end
endtask