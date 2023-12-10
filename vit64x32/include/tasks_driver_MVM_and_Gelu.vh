`define length197 197

task gelu_task(input real x, output real y);
begin
    automatic real x_t;
    automatic real tanh_o;
    automatic real tanh_i;
    automatic real pi = 3.1415926535897932;
    automatic real cst_1;
    automatic real cst_2 = 0.044715;
    cst_1 = $sqrt(2/pi);
    
    x_t = x;
    tanh_i = cst_1 * (x_t + cst_2 * $pow(x_t,3));
    tanh_o= $tanh(tanh_i);
    y  = 0.5 * x_t * (1 + tanh_o);
end
endtask


task Run_Activation_Software(input bit [`DAT_DW_L0-1:0] In_Feature [`Hout_L0][`Wout_L0][`CHout_L0], 
                    output bit [`DAT_DW_L1-1:0] Out_Feature [`Hout_L0][`Wout_L0][`CHout_L0] );
begin
    bit [`DAT_DW_L0-1:0]  In[`CHout_L0][`length197];
    real In_real[`CHout_L0][`length197];
    bit [`DAT_DW_L1-1:0] Out[`CHout_L0][`length197];
    real Out_real[`CHout_L0][`length197];

    for(int i=0;i<`Hout_L0;i++)
        for(int j=0;j<`Wout_L0;j++)
            for(int k=0;k<`CHout_L0;k++)
            begin
                if(i*`Wout_L0+j<`length197)
                begin
                    In[k][i*`Wout_L0+j]=In_Feature[i][j][k];
//                    $display("**In[CH%d][%d]=%d",k,i*`Wout_L0+j,$signed(In_Feature[i][j][k]));
                    In_real[k][i*`Wout_L0+j]=$itor($signed(In_Feature[i][j][k]))   /  $itor(1<<`in_scale);
//                    $display("In_real[CH%d][%d]=%.8f",k,i*`Wout_L0+j,In_real[k][i*`Wout_L0+j]);

                    gelu_task(In_real[k][i*`Wout_L0+j], Out_real[k][i*`Wout_L0+j]);
                    
                    Out_Feature[i][j][k]=$itor(Out_real[k][i*`Wout_L0+j]) * $itor(1<<`out_scale);
//                    $display("**Out_real[CH%d][%d]=%.5f",k,i*`Wout_L0+j,Out_real[k][i*`Wout_L0+j]); 
//                    $display("Out_Feature[H%d][W%d][CH%d]=%d",i,j,k,$signed(Out_Feature[i][j][k])); 

                end
            end
            
end
endtask


task RunMVM_soft(input bit[31:0]CHin, bit[31:0]Hin,bit[31:0]Win,bit[31:0]CHout,
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

                feature_out[j][k][i]=(j*out_width+k)>=`Height?0:$signed(tp_sat);
//				$display("feature_out[%0d][%0d][%0d]=%0d",j,k,i,$signed(feature_out[j][k][i]));
			end
		end
end
endtask

task RunMVM(input bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,
		bit[31:0] Kx,bit[31:0] Ky,bit[31:0] Sx,bit[31:0] Sy,bit[31:0] pad_x, bit[31:0] pad_y, bit relu_en,bit[4:0]L0_DW,bit[4:0]L1_DW, 
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,bit [4:0]feature_in_scale,
		bit[31:0] wt_base_addr,bit [31:0]wt_size_in_bytes,bit [31:0]wt_num_div_Tin,bit[4:0] wt_scale,bit[4:0] conv_out_scale,      
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,bit [4:0]feature_out_scale);
begin
	integer overlap;
	integer dat_num_per_row;
	integer out_width,out_height;
	integer dat_banks_min;//mininum required dat banks
	integer wt_banks_min;//mininum required wt banks

	integer dat_buf_num;
	integer best_dat_banks;
	integer mininum_bw;
	integer best_method;
	integer wt_factor;
	
	mininum_bw=0;

	out_width=((Win+2*pad_x-Kx)/Sx+1);
	out_height=((Hin+2*pad_y-Ky)/Sy+1);	
	overlap=Ky-Sy;
	dat_num_per_row=Win*`slice_of_CHin_L0;
	dat_banks_min=(dat_num_per_row*Ky+`BRAM_DEPTH-1)/`BRAM_DEPTH;
	wt_banks_min=(Kx*Ky*`Tout*`slice_of_CHin_L0+`BRAM_DEPTH-1)/`BRAM_DEPTH;

	if((dat_banks_min+wt_banks_min)>`BRAM_NUM)
	begin
	    $display("=======================================================================");
	    $display("===================   FPGA BRAM not enough!    ========================");
	    $display("=======================================================================");
		$display("dat_banks_min=%0d,wt_banks_min=%0d",dat_banks_min,wt_banks_min);
        $finish;
	end

	for(dat_buf_num=dat_banks_min;dat_buf_num<=`BRAM_NUM-wt_banks_min;dat_buf_num=dat_buf_num+1)
	begin
		integer Hout_Split_Times;
		integer in_height_first,in_height_middle,in_height_last;	
		integer out_height_first,out_height_middle,out_height_last;
		
		integer CHout_Split_Times;integer wt_banks;
		integer out_ch_slice,out_ch_slice_last;

		integer total_bw_if_reuse_wt;
		integer total_bw_if_reuse_dat;

		wt_banks=`BRAM_NUM-dat_buf_num;
		out_ch_slice=( (`BRAM_DEPTH*wt_banks)/(Kx*Ky*`Tout*`slice_of_CHin_L0) ) *`Tout;

  
		if(out_ch_slice>=CHout)
		begin
			out_ch_slice=CHout;
			CHout_Split_Times=1;
		end
		else
		begin
			CHout_Split_Times=(CHout+out_ch_slice-1)/out_ch_slice;
		end
	
		if(CHout%out_ch_slice==0)
			out_ch_slice_last=out_ch_slice;
		else
			out_ch_slice_last=CHout%out_ch_slice;

		out_height_first=((`BRAM_DEPTH*dat_buf_num)/dat_num_per_row+pad_y-Ky)/Sy+1;
		in_height_first=(out_height_first-1)*Sy+Ky-pad_y;

		out_height_middle=((`BRAM_DEPTH*dat_buf_num)/dat_num_per_row-Ky)/Sy+1;
		in_height_middle=(out_height_middle-1)*Sy+Ky;

		if(out_height_first>=out_height)
		begin
			out_height_first=out_height;
			in_height_first=Hin;
		end

		if((out_height-out_height_first)%out_height_middle == 0)
		begin
			Hout_Split_Times=(out_height-out_height_first)/out_height_middle+1;
			out_height_last=out_height_middle;
		end
		else
		begin
			Hout_Split_Times=(out_height-out_height_first)/out_height_middle+2;
			out_height_last=(out_height-out_height_first)%out_height_middle;
		end
		in_height_last=Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap);
		total_bw_if_reuse_wt=(dat_num_per_row*Hin+dat_num_per_row*overlap*(Hout_Split_Times-1))*CHout_Split_Times+Kx*Ky*CHout*`slice_of_CHin_L0;
		total_bw_if_reuse_dat=Hout_Split_Times*Kx*Ky*CHout*`slice_of_CHin_L0+dat_num_per_row*Hin+dat_num_per_row*overlap*(Hout_Split_Times-1);

		$display("total_bw_if_reuse_wt=%0d,total_bw_if_reuse_dat=%0d\n",total_bw_if_reuse_wt,total_bw_if_reuse_dat);

		if((mininum_bw==0)||(total_bw_if_reuse_wt<mininum_bw))
		begin
			best_dat_banks=dat_buf_num;
			mininum_bw=total_bw_if_reuse_wt;
			best_method=0;
		end
		
		if((mininum_bw==0)||(total_bw_if_reuse_dat<mininum_bw))
		begin
			best_dat_banks=dat_buf_num;
			mininum_bw=total_bw_if_reuse_dat;
			best_method=1;
		end
	end

	dat_buf_num=best_dat_banks;
	begin
		integer Hout_Split_Times;
		integer in_height_first,in_height_middle,in_height_last;	
		integer out_height_first,out_height_middle,out_height_last;
		
		integer CHout_Split_Times;
		integer wt_banks;
		integer out_ch_slice,out_ch_slice_last;

		integer total_bw_if_reuse_wt;
		integer total_bw_if_reuse_dat;

		wt_banks=`BRAM_NUM-dat_buf_num;
		out_ch_slice=( (`BRAM_DEPTH*wt_banks)/(Kx*Ky*`Tout*`slice_of_CHin_L0) ) *`Tout;

		if(out_ch_slice>=CHout)
		begin
			out_ch_slice=CHout;
			CHout_Split_Times=1;
		end
		else
		begin
			CHout_Split_Times=(CHout+out_ch_slice-1)/out_ch_slice;
		end
	
		if(CHout%out_ch_slice==0)
			out_ch_slice_last=out_ch_slice;
		else
			out_ch_slice_last=CHout%out_ch_slice;

		out_height_first=((`BRAM_DEPTH*dat_buf_num)/dat_num_per_row+pad_y-Ky)/Sy+1;
		in_height_first=(out_height_first-1)*Sy+Ky-pad_y;

		out_height_middle=((`BRAM_DEPTH*dat_buf_num)/dat_num_per_row-Ky)/Sy+1;
		in_height_middle=(out_height_middle-1)*Sy+Ky;

		if(out_height_first>=out_height)
		begin
			out_height_first=out_height;
			in_height_first=Hin;
		end

		if((out_height-out_height_first)%out_height_middle == 0)
		begin
			Hout_Split_Times=(out_height-out_height_first)/out_height_middle+1;
			out_height_last=out_height_middle;
		end
		else
		begin
			Hout_Split_Times=(out_height-out_height_first)/out_height_middle+2;
			out_height_last=(out_height-out_height_first)%out_height_middle;
		end
		in_height_last=Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap);

		$display("best_method=%0d,best_dat_banks=%0d",best_method,best_dat_banks);
		$display("CH_out=%0d,out_ch_slice=%0d,out_ch_slice_last=%0d,CHout_Split_Times=%0d",CHout,out_ch_slice,out_ch_slice_last,CHout_Split_Times);
		$display("in_width=%0d,out_width=%0d",Win,out_width);
		$display("in_height=%0d,out_height=%0d,Hout_Split_Times=%0d",Hin,out_height,Hout_Split_Times);
		$display("in_height_first=%0d,in_height_middle=%0d,in_height_last=%0d",in_height_first,in_height_middle,in_height_last);
		$display("out_height_first=%0d,out_height_middle=%0d,out_height_last=%0d",out_height_first,out_height_middle,out_height_last);
        $display("best_method=%0d",best_method);
		if(best_method==0)
		begin
			for(integer n=0;n<CHout_Split_Times;n=n+1)
				for(integer k=0;k<Hout_Split_Times;k=k+1)
				begin
					integer in_height_single,out_height_single;
					integer line_offset_in,line_offset_out;
					integer CH_in_single;//
					integer CH_out_single;
					integer pad_y_single;
					bit dma_wt_reuse_single;//dma_dat_reuse_single=0

					CH_in_single=CHin;//						

					if(n!=CHout_Split_Times-1)
						CH_out_single=out_ch_slice;
					else
						CH_out_single=out_ch_slice_last;

					if(k==0)
						begin line_offset_in=0;line_offset_out=0;pad_y_single=pad_y;dma_wt_reuse_single=0;end
					else
						begin 
							line_offset_in=(in_height_first-overlap)+(k-1)*(in_height_middle-overlap);
							line_offset_out=out_height_first+(k-1)*out_height_middle;
							pad_y_single=0;
							dma_wt_reuse_single=1;
						end

					if(k==0)
						begin in_height_single=in_height_first;out_height_single=out_height_first;end
					else
						if(k==Hout_Split_Times-1)
							begin in_height_single=in_height_last;out_height_single=out_height_last;end
						else
							begin in_height_single=in_height_middle;out_height_single=out_height_middle;end

					//if(m==0)
					RunMVM_single_time(CH_in_single,in_height_single,Win,CH_out_single,
							Kx,Ky,Sx,Sy,pad_x,pad_y_single,relu_en,L0_DW,L1_DW,
							feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,feature_in_scale,
							wt_base_addr+wt_size_in_bytes/CHout*out_ch_slice*n,wt_size_in_bytes/CHout*CH_out_single*8/(`WT_DW_L0*`Tout),wt_num_div_Tin/CHout*CH_out_single,wt_scale,conv_out_scale, 
							feature_out_base+feature_out_line_stride*line_offset_out+feature_out_surface_stride*n*(out_ch_slice/`Tout),feature_out_surface_stride,feature_out_line_stride,feature_out_scale,
							out_width,out_height_single,best_dat_banks,0,dma_wt_reuse_single);
				end
		end
		else//CHout_Split first, Hout_Split_Times
		begin
			for(integer k=0;k<Hout_Split_Times;k=k+1)
				for(integer n=0;n<CHout_Split_Times;n=n+1)
				begin
					integer in_height_single,out_height_single;
					integer line_offset_in,line_offset_out;

					integer CH_in_single;//
					integer CH_out_single;
					integer wt_size_single;
					integer pad_y_single;

					bit dma_dat_reuse_single;//dma_wt_reuse_single=0

					CH_in_single=CHin;//

					if(n!=CHout_Split_Times-1)
						CH_out_single=out_ch_slice;
					else
						CH_out_single=out_ch_slice_last;

					if(n==0)
						dma_dat_reuse_single=0;
					else
						dma_dat_reuse_single=1;

					if(k==0)
						begin line_offset_in=0;line_offset_out=0;pad_y_single=pad_y;;end
					else
						begin 
							line_offset_in=(in_height_first-overlap)+(k-1)*(in_height_middle-overlap);
							line_offset_out=out_height_first+(k-1)*out_height_middle;
							pad_y_single=0;
						end

					if(k==0)
						begin in_height_single=in_height_first;out_height_single=out_height_first;end
					else
						if(k==Hout_Split_Times-1)
							begin in_height_single=in_height_last;out_height_single=out_height_last;end
						else
							begin in_height_single=in_height_middle;out_height_single=out_height_middle;end

					// if(m==0)
					RunMVM_single_time(CH_in_single,in_height_single,Win,CH_out_single,
							Kx,Ky,Sx,Sy,pad_x,pad_y_single,relu_en,L0_DW,L1_DW,
							feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,feature_in_scale,
							wt_base_addr+wt_size_in_bytes/CHout*out_ch_slice*n,wt_size_in_bytes/CHout*CH_out_single*8/(`WT_DW_L0*`Tout),wt_num_div_Tin/CHout*CH_out_single,wt_scale,conv_out_scale,
							feature_out_base+feature_out_line_stride*line_offset_out+feature_out_surface_stride*n*(out_ch_slice/`Tout),feature_out_surface_stride,feature_out_line_stride,feature_out_scale,
							out_width,out_height_single,best_dat_banks,dma_dat_reuse_single,0);
				end
		end
	end
end
endtask


task RunMVM_single_time(bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,
		bit[31:0] Kx,bit[31:0] Ky,bit[31:0] Sx,bit[31:0] Sy,
		bit[31:0] pad_x, bit[31:0] pad_y,bit relu_en,bit [4:0]L0_DW,bit [4:0]L1_DW,
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,bit [4:0]feature_in_scale,
		bit[31:0] wt_base_addr,bit[31:0] wt_num_div_Tout,bit[31:0] wt_num_div_Tin,bit[4:0] wt_scale,bit[4:0] conv_out_scale,										  
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,bit [4:0]feature_out_scale,
		bit[31:0] out_width,bit[31:0] out_height,bit[31:0] dat_buf_num,bit dma_dat_reuse,bit dma_wt_reuse);
begin
    bit gelu;
    bit [`log2_scale-2:0]shift_value;
    bit shift_sign;
    
	int reg_bias;
	bit [31:0]rdata;
	bit [3:0]Tin_factor;
    
    bit [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1;
    CH_in_res_Tin_div_Tout_minus1=(`Tin_L0/`Tout-1);
    
    $display("CHin=%0d",CHin);
    $display("CH_in_res_Tin_div_Tout_minus1=%0d",CH_in_res_Tin_div_Tout_minus1);
    
    Tin_factor=(`MAX_DAT_DW/`DAT_DW_L0);

    shift_sign=0;//right shift
    shift_value[`log2_scale-2:0]=(feature_in_scale+wt_scale)-conv_out_scale;
    if($signed(shift_value)<0)
    begin
        shift_value[`log2_scale-2:0]=conv_out_scale-(feature_in_scale+wt_scale);
        shift_sign=1; //left shift
        $display("need LEFT shift! shift_value=%0d",shift_value);
    end  
    else
    begin  
        $display("need RIGHT shift, shift_value=%0d",shift_value);
        $display("Start RunConv_single_time!\n");
    end
    
	gelu=`GELU_EN_L0;
	
	//cfg conv_path
	CSB_Write(2,Tin_factor);	
	CSB_Write(3,dat_buf_num);
	CSB_Write(4,Win);
	CSB_Write(5,Hin);
	CSB_Write(6,Win*Hin);
	CSB_Write(7,(CHin+`Tout-1)/`Tout);
	CSB_Write(8,(CHin+`Tin_L0-1)/`Tin_L0);
	CSB_Write(31,CH_in_res_Tin_div_Tout_minus1);
	CSB_Write(9,pad_y);
	CSB_Write(10,pad_x);
	CSB_Write(11,Sx);
	CSB_Write(12,Sy);
	CSB_Write(13,Kx);
	CSB_Write(14,Ky);
	CSB_Write(15,out_width);
	CSB_Write(16,out_width*out_height);
	CSB_Write(17,out_height);
	CSB_Write(18,CHout);
	CSB_Write(19,((CHout+`Tout-1)/`Tout));
	CSB_Write(33,((out_width*out_height+`Tout-1)/`Tout));
    CSB_Write(34,feature_out_base);
    CSB_Write(35,feature_out_surface_stride);
    CSB_Write(36,feature_out_line_stride);		
    CSB_Write(38,{gelu,relu_en});	
    CSB_Write(39,(L1_DW-1));	
    CSB_Write(44,`in_scale);// gelu in scale
	CSB_Write(48,`Height);//effect_pixel
	CSB_Write(20,feature_in_base);
	CSB_Write(21,feature_in_surface_stride);
	CSB_Write(22,feature_in_line_stride);
	CSB_Write(23,(wt_num_div_Tout/Tin_factor));
	CSB_Write(32,wt_num_div_Tin);
	CSB_Write(24,wt_base_addr);
	CSB_Write(25,{dma_wt_reuse,dma_dat_reuse});
    CSB_Write(26,{shift_sign,shift_value});
    CSB_Write(47,{1'b0,1'b0});//Conv_Out_to_BRAM_mode_r<=wdata[1], Conv_In_from_BRAM_mode_r<=wdata[0]
	//Kick off the run
	CSB_Write(0,32'b011111);//Conv Path: fsm_us,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

	CSB_Read(37,rdata);
	while(rdata!==32'b1) CSB_Read(37,rdata);

end
endtask
