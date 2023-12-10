

task RunConv_Res(input bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,
		bit[31:0] Kx,bit[31:0] Ky,bit[31:0] Sx,bit[31:0] Sy,bit[31:0] pad_x, bit[31:0] pad_y, bit relu_en,bit res_relu_en,bit[4:0]L0_DW,bit[4:0]L1_DW, 
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,bit [4:0]feature_in_scale,
		bit[31:0] wt_base_addr,bit [31:0]wt_size_in_bytes,bit [31:0]wt_num_div_Tin,bit[4:0] wt_scale,bit[4:0] conv_out_scale, 
		bit[31:0] ResAdd_base,bit[31:0] ResAdd_surface_stride,bit[31:0] ResAdd_line_stride,bit [4:0]ResAdd_scale,     
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
					RunConv_Res_single_time(CH_in_single,in_height_single,Win,CH_out_single,
							Kx,Ky,Sx,Sy,pad_x,pad_y_single,relu_en,res_relu_en,L0_DW,L1_DW,
							feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,feature_in_scale,
							wt_base_addr+wt_size_in_bytes/CHout*out_ch_slice*n,wt_size_in_bytes/CHout*CH_out_single*8/(`WT_DW_L0*`Tout),wt_num_div_Tin/CHout*CH_out_single,wt_scale,conv_out_scale, 
							ResAdd_base+ResAdd_line_stride*line_offset_out+ResAdd_surface_stride*n*(out_ch_slice/`Tout),ResAdd_surface_stride,ResAdd_line_stride,ResAdd_scale,
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
					RunConv_Res_single_time(CH_in_single,in_height_single,Win,CH_out_single,
							Kx,Ky,Sx,Sy,pad_x,pad_y_single,relu_en,res_relu_en,L0_DW,L1_DW,
							feature_in_base+feature_in_line_stride*line_offset_in,feature_in_surface_stride,feature_in_line_stride,feature_in_scale,
							wt_base_addr+wt_size_in_bytes/CHout*out_ch_slice*n,wt_size_in_bytes/CHout*CH_out_single*8/(`WT_DW_L0*`Tout),wt_num_div_Tin/CHout*CH_out_single,wt_scale,conv_out_scale,
							ResAdd_base+ResAdd_line_stride*line_offset_out+ResAdd_surface_stride*n*(out_ch_slice/`Tout),ResAdd_surface_stride,ResAdd_line_stride,ResAdd_scale,
							feature_out_base+feature_out_line_stride*line_offset_out+feature_out_surface_stride*n*(out_ch_slice/`Tout),feature_out_surface_stride,feature_out_line_stride,feature_out_scale,
							out_width,out_height_single,best_dat_banks,dma_dat_reuse_single,0);
				end
		end
	end
end
endtask


task RunConv_Res_single_time(bit[31:0] CHin,bit[31:0] Hin,bit[31:0] Win,bit[31:0] CHout,
		bit[31:0] Kx,bit[31:0] Ky,bit[31:0] Sx,bit[31:0] Sy,
		bit[31:0] pad_x, bit[31:0] pad_y,bit relu_en,bit res_relu_en,bit [4:0]L0_DW,bit [4:0]L1_DW,
		bit[31:0] feature_in_base,bit[31:0] feature_in_surface_stride,bit[31:0] feature_in_line_stride,bit [4:0]feature_in_scale,
		bit[31:0] wt_base_addr,bit[31:0] wt_num_div_Tout,bit[31:0] wt_num_div_Tin,bit[4:0] wt_scale,bit[4:0] conv_out_scale,										  
		bit[31:0] ResAdd_base,bit[31:0] ResAdd_surface_stride,bit[31:0] ResAdd_line_stride,bit [4:0]ResAdd_scale,
		bit[31:0] feature_out_base,bit[31:0] feature_out_surface_stride,bit[31:0] feature_out_line_stride,bit [4:0]feature_out_scale,
		bit[31:0] out_width,bit[31:0] out_height,bit[31:0] dat_buf_num,bit dma_dat_reuse,bit dma_wt_reuse);
begin
    
    bit [`log2_scale-2:0]shift_value;
    bit shift_sign;
    
	int reg_bias;
	bit [31:0]rdata;
	bit [3:0]Tin_factor;
	
    bit [`Max_log2Tin_minus_log2Tout:0]CH_in_res_Tin_div_Tout_minus1;
    if((CHin%`Tin_L0)==0)
        begin CH_in_res_Tin_div_Tout_minus1=(`Tin_L0/`Tout-1); end
    else   
        begin CH_in_res_Tin_div_Tout_minus1=(((CHin%`Tin_L0)+`Tout-1)/`Tout-1);end
    $display("CHin=%0d",CHin);
    $display("CH_in_res_Tin_div_Tout_minus1=%0d",CH_in_res_Tin_div_Tout_minus1);
    
    Tin_factor=(`MAX_DAT_DW/`DAT_DW_L0);

    shift_sign=0;//right shift
    shift_value[`log2_scale-2:0]=(feature_in_scale+wt_scale)-conv_out_scale;
    if($signed(shift_value)<0)
    begin
        shift_value[`log2_scale-2:0]=conv_out_scale-(feature_in_scale+wt_scale);
        shift_sign=1; //left shift
        $display("Error!!   shift_value < 0  !");
        $finish;
    end    
    $display("shift_sign=%0d",shift_sign);
    $display("shift_value=%0d",shift_value);
	$display("Start RunConv_single_time!");

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
    CSB_Write(38,relu_en);	
    CSB_Write(39,(L1_DW-1));	
    
	CSB_Write(20,feature_in_base);
	CSB_Write(21,feature_in_surface_stride);
	CSB_Write(22,feature_in_line_stride);
	CSB_Write(23,(wt_num_div_Tout/Tin_factor));
	CSB_Write(32,wt_num_div_Tin);
	CSB_Write(24,wt_base_addr);
	
    CSB_Write(40,ResAdd_base);
    CSB_Write(41,ResAdd_surface_stride);
    CSB_Write(42,ResAdd_line_stride);
    CSB_Write(43,res_relu_en);
    CSB_Write(44,ResAdd_scale);
    CSB_Write(45,conv_out_scale);
    CSB_Write(46,ResAdd_scale+conv_out_scale-feature_out_scale);
    CSB_Write(48,out_width*out_height);//effect_pixel

	CSB_Write(25,{dma_wt_reuse,dma_dat_reuse});
    CSB_Write(26,{shift_sign,shift_value});
	//Kick off the run
	CSB_Write(0,32'b111111);//Conv Path: ResAdd_dma,fsm_us,fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

	CSB_Read(37,rdata);
	while(rdata!==32'b1) CSB_Read(37,rdata);

end
endtask
