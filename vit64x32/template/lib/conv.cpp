#include "conv.h"



struct Conv_Cfg Get_Conv_Cfg(int Hin, int Win, int CHin, int CHout,
	int Kx, int Ky, int Sx, int Sy, int Px, int Py, int data_width, int mp_mode)
{
	struct Conv_Cfg ret;

	int overlap;
	int dat_num_per_row;
	int out_width, out_height;
	int dat_banks_min;//mininum required dat banks
	int wt_banks_min;//mininum required wt banks

	int dat_buf_num;
	int best_dat_banks = 0;
	int best_wt_banks = 0;
	int best_Hout_Split_Times = 0;
	int best_CHout_Split_Times = 0;
	int best_in_height_first = 0;
	int best_in_height_middle = 0;
	int best_in_height_last = 0;
	int best_out_height_first = 0;
	int best_out_height_middle = 0;
	int best_out_height_last = 0;
	int best_out_ch_slice = 0;
	int best_out_ch_slice_last = 0;
	int mininum_bw = 0;
	int best_method = 0;
	int wt_factor = MAX_DAT_DW / data_width;//8bit: wt_factor=1,  2bit:wt_factor=4

	int Tin_L;
	switch (mp_mode) //mode0 = Booth; mode1 = HPS; mode2 = BSC; mode3 = LPC
	{
		case 0: Tin_L = Tin * MAX_DAT_DW / data_width; break;
		case 1: Tin_L = Tin * MAX_DAT_DW / data_width; break;
		case 2: Tin_L = (data_width == 8) ? Tin : (Tin * 2 * MAX_DAT_DW / data_width); break;
		case 3: Tin_L = Tin * int(pow(2,(MAX_DAT_DW / data_width))); break;
		default: Tin_L = Tin * MAX_DAT_DW / data_width; break;
	}

	out_width = ((Win + 2*Px - Kx) / Sx + 1);
	out_height = ((Hin + 2*Py - Ky) / Sy + 1);
	overlap = Ky - Sy;
	dat_num_per_row = Win * ((CHin + Tin_L - 1) / Tin_L);
	dat_banks_min = (dat_num_per_row * Ky + BRAM_DEPTH - 1) / BRAM_DEPTH;
	wt_banks_min = (Kx * Ky * Tout * ((CHin + Tin_L - 1) / Tin_L) + BRAM_DEPTH - 1) / BRAM_DEPTH;

	if ((dat_banks_min + wt_banks_min) > BRAM_NUM)
	{
		printf("Error: CBUF entries not enough!!!\n");
		return ret;
	}

	for (dat_buf_num = dat_banks_min; dat_buf_num <= BRAM_NUM - wt_banks_min; dat_buf_num = dat_buf_num + 1)
	{
		int Hout_Split_Times;
		int in_height_first, in_height_middle, in_height_last;
		int out_height_first, out_height_middle, out_height_last;

		int CHout_Split_Times; int wt_banks;
		int out_ch_slice, out_ch_slice_last;

		int total_bw_if_reuse_wt;
		int total_bw_if_reuse_dat;

		wt_banks = BRAM_NUM - dat_buf_num;
		out_ch_slice = ((BRAM_DEPTH * wt_banks) / (Kx * Ky * Tout * ((CHin + Tin_L - 1) / Tin_L))) * Tout;

		if (out_ch_slice >= CHout)
		{
			out_ch_slice = CHout;
			CHout_Split_Times = 1;
		}
		else
		{
			CHout_Split_Times = (CHout + out_ch_slice - 1) / out_ch_slice;
		}

		if (CHout % out_ch_slice == 0)
			out_ch_slice_last = out_ch_slice;
		else
			out_ch_slice_last = CHout % out_ch_slice;

		out_height_first = ((BRAM_DEPTH * dat_buf_num) / dat_num_per_row + Py - Ky) / Sy + 1;
		in_height_first = (out_height_first - 1) * Sy + Ky - Py;

		out_height_middle = ((BRAM_DEPTH * dat_buf_num) / dat_num_per_row - Ky) / Sy + 1;
		in_height_middle = (out_height_middle - 1) * Sy + Ky;

		if (out_height_first >= out_height)
		{
			out_height_first = out_height;
			in_height_first = Hin;
		}

		if ((out_height - out_height_first) % out_height_middle == 0)
		{
			Hout_Split_Times = (out_height - out_height_first) / out_height_middle + 1;
			out_height_last = out_height_middle;
		}
		else
		{
			Hout_Split_Times = (out_height - out_height_first) / out_height_middle + 2;
			out_height_last = (out_height - out_height_first) % out_height_middle;
		}
		in_height_last = Hin - in_height_first + overlap - (Hout_Split_Times - 2) * (in_height_middle - overlap);

		total_bw_if_reuse_wt = (dat_num_per_row * Hin + dat_num_per_row * overlap * (Hout_Split_Times - 1)) * CHout_Split_Times + Kx * Ky * CHout * ((CHin + Tin_L - 1) / Tin_L) / wt_factor;
		total_bw_if_reuse_dat = Hout_Split_Times * Kx * Ky * CHout * ((CHin + Tin_L - 1) / Tin_L) / wt_factor + dat_num_per_row * Hin + dat_num_per_row * overlap * (Hout_Split_Times - 1);

		if ((mininum_bw == 0) || (total_bw_if_reuse_wt < mininum_bw) || (total_bw_if_reuse_dat < mininum_bw)) 
		{
			best_Hout_Split_Times = Hout_Split_Times;
			best_in_height_first = in_height_first;
			best_in_height_middle = in_height_middle;
			best_in_height_last = in_height_last;
			best_out_height_first = out_height_first;
			best_out_height_middle = out_height_middle;
			best_out_height_last = out_height_last;
			best_CHout_Split_Times = CHout_Split_Times;
			best_out_ch_slice = out_ch_slice;
			best_out_ch_slice_last = out_ch_slice_last;
		}

		if (mininum_bw == 0)
		{
			if (total_bw_if_reuse_wt < total_bw_if_reuse_dat)
			{
				best_dat_banks = dat_buf_num;
				best_wt_banks = BRAM_NUM - dat_buf_num;
				mininum_bw = total_bw_if_reuse_wt;
				best_method = 0;
			}
			else
			{
				best_dat_banks = dat_buf_num;
				best_wt_banks = BRAM_NUM - dat_buf_num;
				mininum_bw = total_bw_if_reuse_dat;
				best_method = 1;
			}
		}
		else
		{
			if (total_bw_if_reuse_wt < mininum_bw)
			{
				best_dat_banks = dat_buf_num;
				best_wt_banks = BRAM_NUM - dat_buf_num;
				mininum_bw = total_bw_if_reuse_wt;
				best_method = 0;
			}
			if (total_bw_if_reuse_dat < mininum_bw)
			{	
				best_dat_banks = dat_buf_num;
				best_wt_banks = BRAM_NUM - dat_buf_num;
				mininum_bw = total_bw_if_reuse_dat;
				best_method = 1;
			}
		}
	}

	ret.CHin = CHin;
	ret.Tin_L = Tin_L;
	ret.Win = Win;
	ret.CHout = CHout;
	ret.overlap = overlap;
	ret.Kx = Kx;
	ret.Ky = Ky;
	ret.Sx = Sx;
	ret.Sy = Sy;
	ret.pad_x = Px;
	ret.pad_y = Py;
	ret.dat_banks = best_dat_banks;
	ret.method = best_method;
	ret.out_width = out_width;
	ret.out_height = out_height;
	ret.dat_num_per_row = dat_num_per_row;
	ret.wt_size_in_bytes = ((Tin_L * data_width) / 8) * Kx * Ky * CHout * ((CHin + Tin_L - 1) / Tin_L);
	ret.Hout_Split_Times = best_Hout_Split_Times;
	ret.in_height_first = best_in_height_first;
	ret.in_height_middle = best_in_height_middle;
	ret.in_height_last = best_in_height_last;
	ret.out_height_first = best_out_height_first;
	ret.out_height_middle = best_out_height_middle;
	ret.out_height_last = best_out_height_last;
	ret.CHout_Split_Times = best_CHout_Split_Times;
	ret.out_ch_slice = best_out_ch_slice;
	ret.out_ch_slice_last = best_out_ch_slice_last;
	return ret;
}

void Save_Conv_Cfg(const char* cfg_name, struct Conv_Cfg conv_cfg, FILE* fp)
{

	fprintf(fp, "struct Conv_Cfg %s={%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d};\n", cfg_name,
		conv_cfg.CHin,
		conv_cfg.Tin_L,
		conv_cfg.Win,
		conv_cfg.CHout,
		conv_cfg.overlap,
		conv_cfg.Kx,
		conv_cfg.Ky,
		conv_cfg.Sx,
		conv_cfg.Sy,
		conv_cfg.pad_x,
		conv_cfg.pad_y,
		conv_cfg.dat_banks,
		conv_cfg.method,
		conv_cfg.out_width,
		conv_cfg.out_height,
		conv_cfg.dat_num_per_row,
		conv_cfg.wt_size_in_bytes,
		conv_cfg.Hout_Split_Times,
		conv_cfg.in_height_first,
		conv_cfg.in_height_middle,
		conv_cfg.in_height_last,
		conv_cfg.out_height_first,
		conv_cfg.out_height_middle,
		conv_cfg.out_height_last,
		conv_cfg.CHout_Split_Times,
		conv_cfg.out_ch_slice,
		conv_cfg.out_ch_slice_last);
}

void Mul_Conv_Soft(struct Conv_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out)
{
	int out_truncate = 0;
	unsigned int positive_overflow = 0;
	unsigned int negetive_overflow = 0;

	int max_dat = (1 << (feature_out->dat_bit - 1)) - 1;
	int min_dat = -1 * (1 << (feature_out->dat_bit - 1));

	int shift_direction;
	shift_direction = feature_in->scale + wt->scale - feature_out->scale;// +(feature_in->dat_bit - feature_out->dat_bit);
	if (shift_direction < 0) {
		out_truncate = -shift_direction;
		//printf("��ʱ��out_truncateΪ%d,feature_out->scaleΪ%d��feature_in->scaleΪ%d,wt->scaleΪ%d\n", out_truncate, feature_out->scale, feature_in->scale, wt->scale);
	}
	else {
		out_truncate = shift_direction;
		//printf("��ʱ��out_truncateΪ%d,\n", out_truncate);
	}
	printf("****************************\n");
	printf("in_dat_scale=%d,\n", feature_in->scale);
	printf("wt_scale=%d,\n", wt->scale);
	printf("out_dat_scale=%d,\n", feature_out->scale);
	printf("in_dat_bit=%d,\n", feature_in->dat_bit);
	printf("out_dat_bit=%d,\n", feature_out->dat_bit);
	printf("---\n");
	printf("shift_direction:  %s,\n", shift_direction > 0 ? "right_shift" : "left_shift");
	printf("shift bit number=%d,\n", out_truncate);
	printf("---\n");
	printf("max_dat=%d,\n", max_dat);
	printf("min_dat=%d,\n", min_dat);

	for (int i = 0; i < feature_out->channel; i++)
	{
		for (int j = 0; j < feature_out->height; j = j + 1)//feature_out->height
		{
			for (int k = 0; k < feature_out->width; k = k + 1)//feature_out->width
			{
				int32_t tp1;
				int32_t tp2;
				int32_t tp_sat, bias_dat;

				tp1 = 0;
				for (int kj = 0; kj < conv_cfg.Ky; kj++)
					for (int ki = 0; ki < conv_cfg.Kx; ki++)
					{
						for (int chi = 0; chi < feature_in->channel; chi++)//
						{
							int data;
							int axis_h;
							int axis_w;
							axis_h = conv_cfg.Sy * j + kj - conv_cfg.pad_y; axis_w = conv_cfg.Sx * k + ki - conv_cfg.pad_x;
							if ((axis_h < 0) || (axis_h >= feature_in->height) || (axis_w < 0) || (axis_w >= feature_in->width))//padding 0
								data = 0;
							else
								data = *Get_Element_Soft(feature_in, axis_h, axis_w, chi);
							tp1 = tp1 + data * (*Get_Weight_Soft(wt, kj, ki, chi, i));
						}
					}
				if (shift_direction >= 0) {
					tp2 = tp1 >> out_truncate;
					if ((out_truncate != 0) && (tp2 != 0x7fff))
						tp2 = tp2 + ((tp1 & (1 << (out_truncate - 1))) != 0);
				}
				else {
					tp2 = tp1 << out_truncate;
				}

				if (tp2 < min_dat) {
					tp_sat = min_dat;
					negetive_overflow = negetive_overflow + 1;
					//printf("tp2=%d,tp1=%d,out_truncate=%d,(feature_in->scale + wt->scale - feature_out->scale)=%d,channel=%d,height=%d,width=%d,\n", tp2, tp1, out_truncate, (feature_in->scale + wt->scale - feature_out->scale), i, j, k);
				}
				else
					if (tp2 > max_dat) {
						tp_sat = max_dat;
						positive_overflow = positive_overflow + 1;
						//printf("tp2=%d,tp1=%d,out_truncate=%d,(feature_in->scale + wt->scale - feature_out->scale)=%d,channel=%d,height=%d,width=%d,\n", tp2, tp1, out_truncate, (feature_in->scale + wt->scale - feature_out->scale),i, j, k);
					}
					else
						tp_sat = tp2;

				if (relu_en && tp_sat < 0)
					tp_sat = 0;
				*Get_Element_Soft(feature_out, j, k, i) = tp_sat;
			}
		}
	}
	printf("�����=%d", positive_overflow);
	printf("�����=%d", negetive_overflow);
	printf("\n");
}

void Mul_Conv_Bias_Soft(struct Conv_Cfg conv_cfg, int relu_en, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* bias, struct Mapped_Feature* feature_out)
{
	int out_truncate, out_truncate2;
	unsigned int positive_overflow = 0;
	unsigned int negetive_overflow = 0;

	if (int(feature_in->scale + wt->scale - feature_out->scale) < 0) {
		out_truncate = (feature_out->scale - feature_in->scale - wt->scale);
		//printf("��ʱ��out_truncateΪ%d,feature_out->scaleΪ%d��feature_in->scaleΪ%d,wt->scaleΪ%d\n", out_truncate, feature_out->scale, feature_in->scale, wt->scale);
	}
	else {
		out_truncate = (feature_in->scale + wt->scale - feature_out->scale);
		//printf("��ʱ��out_truncateΪ%d,\n", out_truncate);
	}

	if (int(feature_out->scale) < 0) {
		out_truncate2 = (0 - feature_out->scale);
		//printf("��ʱ��out_truncate2Ϊ%d,feature_out->scaleΪ%d,\n", out_truncate2, feature_out->scale);
	}
	else {
		out_truncate2 = (feature_out->scale);
		//printf("��ʱ��out_truncate2Ϊ%d,\n", out_truncate2);
	}
	int max_dat = (1 << (feature_out->dat_bit - 1)) - 1;
	int min_dat = -1 * (1 << (feature_out->dat_bit - 1));
	printf("****************************\n");
	printf("in_dat_scale=%d,\n", feature_in->scale);
	printf("wt_scale=%d,\n", wt->scale);
	printf("out_dat_scale=%d,\n", feature_out->scale);
	printf("in_dat_bit=%d,\n", feature_in->dat_bit);
	printf("out_dat_bit=%d,\n", feature_out->dat_bit);
	printf("max_dat=%d,\n", max_dat);
	printf("min_dat=%d,\n", min_dat);

	for (int i = 0; i < feature_out->channel; i++)
	{
		for (int j = 0; j < feature_out->height; j = j + 1)//feature_out->height
		{
			for (int k = 0; k < feature_out->width; k = k + 1)//feature_out->width
			{
				int32_t tp1;
				int32_t tp2;
				int32_t tp_sat, bias_dat;

				tp1 = 0;
				for (int kj = 0; kj < conv_cfg.Ky; kj++)
					for (int ki = 0; ki < conv_cfg.Kx; ki++)
					{
						for (int chi = 0; chi < feature_in->channel; chi++)//
						{
							int data;
							int axis_h;
							int axis_w;
							axis_h = conv_cfg.Sy * j + kj - conv_cfg.pad_y; axis_w = conv_cfg.Sx * k + ki - conv_cfg.pad_x;
							if ((axis_h < 0) || (axis_h >= feature_in->height) || (axis_w < 0) || (axis_w >= feature_in->width))//padding 0
								data = 0;
							else
								data = *Get_Element_Soft(feature_in, axis_h, axis_w, chi);
							tp1 = tp1 + data * (*Get_Weight_Soft(wt, kj, ki, chi, i));
						}
					}
				if (int(feature_in->scale + wt->scale - feature_out->scale) >= 0) {
					tp2 = tp1 >> out_truncate;
					if ((out_truncate != 0) && (tp2 != 0x7fff))
						tp2 = tp2 + ((tp1 & (1 << (out_truncate - 1))) != 0);
				}
				else {
					tp2 = tp1 << out_truncate;
				}
				if (tp2 < min_dat) {
					tp_sat = min_dat;
					negetive_overflow = negetive_overflow + 1;
					//printf("tp2=%d,tp1=%d,out_truncate=%d,(feature_in->scale + wt->scale - feature_out->scale)=%d,channel=%d,height=%d,width=%d,\n", tp2, tp1, out_truncate, (feature_in->scale + wt->scale - feature_out->scale), i, j, k);
				}
				else
					if (tp2 > max_dat) {
						tp_sat = max_dat;
						positive_overflow = positive_overflow + 1;
						//printf("tp2=%d,tp1=%d,out_truncate=%d,(feature_in->scale + wt->scale - feature_out->scale)=%d,channel=%d,height=%d,width=%d,\n", tp2, tp1, out_truncate, (feature_in->scale + wt->scale - feature_out->scale),i, j, k);
					}
					else
						tp_sat = tp2;

				int64_t tp_sat2;
				if (bias == NULL)
					tp_sat2 = tp_sat;
				else
				{
					int64_t main_dat;
					int64_t aux_dat;
					int64_t tp, result_32;
					main_dat = tp_sat << (bias->scale);
					bias_dat = *Get_Element_Soft(bias, 0, 0, i);
					if (int(feature_out->scale) < 0) {
						aux_dat = bias_dat >> (out_truncate2);
						if ((out_truncate2 != 0) && (aux_dat != 0x7fff))
							aux_dat = aux_dat + ((bias_dat & (1 << (out_truncate2 - 1))) != 0);
					}
					else {
						aux_dat = bias_dat << (out_truncate2);
					}
					tp = main_dat + aux_dat;
					result_32 = tp >> bias->scale;
					if ((bias->scale != 0) && (result_32 != 0x7fff))
						result_32 = result_32 + ((tp & (1 << (bias->scale - 1))) != 0);

					if (result_32 < min_dat)
						tp_sat2 = min_dat;
					else
						if (result_32 > max_dat)
							tp_sat2 = max_dat;
						else
							tp_sat2 = result_32;
				}
				if (relu_en && tp_sat2 < 0)
					tp_sat2 = 0;

				*Get_Element_Soft(feature_out, j, k, i) = tp_sat2;
			}
		}
	}
	printf("�����=%d", positive_overflow);
	printf("�����=%d", negetive_overflow);
	printf("\n");
}


///// function for hardware//////////
#ifdef Run_on_FPGA
void FPGA_Conv(struct Conv_Cfg conv_cfg, int gelu_en, int relu_en, int Conv_Out_and_In_mode, struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out)
{

	int in_dw = wt->wt_bit;
	int out_dw = feature_out->dat_bit;
	int wt_num_div_Tin = (conv_cfg.Kx * conv_cfg.Ky * conv_cfg.CHout * ((conv_cfg.CHin + conv_cfg.Tin_L - 1) / conv_cfg.Tin_L));
	int Tin_factor = MAX_DAT_DW / in_dw;
	
	int CH_in_res_Tin_div_Tout_minus1;
	CH_in_res_Tin_div_Tout_minus1 = (conv_cfg.Tin_L/Tout-1);

	int shift_sign = 0;//right shift
	int shift_value = (feature_in->scale + wt->scale) - feature_out->scale;
	if (shift_value < 0)
	{
		shift_value = feature_out->scale - (feature_in->scale + wt->scale);
		shift_sign = 1; //left shift
	}
	int shift_sacle = 32 * shift_sign + shift_value;

	//printf("\n Tin_L=%6d", conv_cfg.Tin_L);
	//printf("\n wt_num_div_Tin=%6d", wt_num_div_Tin);
	//printf("\n shift_sign=%6d", shift_sign);
	//printf("\n shift_value=%6d", shift_value);
	//printf("\n CH_in_res_Tin_div_Tout_minus1=%6d", CH_in_res_Tin_div_Tout_minus1);
	//printf("\n in_dw=%6d", in_dw);
	//printf("\n out_dw=%6d", out_dw);
	//printf("\n shift_sacle=%6d", shift_sacle);


	if (conv_cfg.method == 0)//Hout_Split_Times->CHout_Split_Times
	{
		for (int n = 0; n < conv_cfg.CHout_Split_Times; n = n + 1)
			for (int k = 0; k < conv_cfg.Hout_Split_Times; k = k + 1)
			{
				int in_height_single, out_height_single;
				int line_offset_in, line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_wt_reuse_single;//cdma_dat_reuse_single=0

				if (n != conv_cfg.CHout_Split_Times - 1)
					ch_out_single = conv_cfg.out_ch_slice;
				else
					ch_out_single = conv_cfg.out_ch_slice_last;

				if (k == 0)
				{
					line_offset_in = 0; line_offset_out = 0; pad_y_single = conv_cfg.pad_y; cdma_wt_reuse_single = 0;
				}
				else
				{
					line_offset_in = (conv_cfg.in_height_first - conv_cfg.overlap) + (k - 1) * (conv_cfg.in_height_middle - conv_cfg.overlap);
					line_offset_out = conv_cfg.out_height_first + (k - 1) * conv_cfg.out_height_middle;
					pad_y_single = 0;
					cdma_wt_reuse_single = 1;
				}

				if (k == 0)
				{
					in_height_single = conv_cfg.in_height_first; out_height_single = conv_cfg.out_height_first;
				}
				else
					if (k == conv_cfg.Hout_Split_Times - 1)
					{
						in_height_single = conv_cfg.in_height_last; out_height_single = conv_cfg.out_height_last;
					}
					else
					{
						in_height_single = conv_cfg.in_height_middle; out_height_single = conv_cfg.out_height_middle;
					}

				FPGA_Conv_single_time(Tin_factor, conv_cfg.Tin_L, CH_in_res_Tin_div_Tout_minus1, shift_sacle,
					conv_cfg.CHin, in_height_single, conv_cfg.Win, ch_out_single,
					conv_cfg.Kx, conv_cfg.Ky, conv_cfg.Sx, conv_cfg.Sy,
					conv_cfg.pad_x, pad_y_single, gelu_en, relu_en, Conv_Out_and_In_mode, out_dw,
					((uint64_t)feature_in->payload) + feature_in->line_stride * line_offset_in, feature_in->surface_stride, feature_in->line_stride, feature_in->scale,
					((uint64_t)wt->payload) + conv_cfg.wt_size_in_bytes / conv_cfg.CHout * conv_cfg.out_ch_slice * n, conv_cfg.wt_size_in_bytes / conv_cfg.CHout * ch_out_single * 8 / (Tout * in_dw), wt_num_div_Tin / conv_cfg.CHout * ch_out_single, wt->scale,
					((uint64_t)feature_out->payload) + feature_out->line_stride * line_offset_out + feature_out->surface_stride * n * (conv_cfg.out_ch_slice / Tout), feature_out->surface_stride, feature_out->line_stride, feature_out->scale,//conv_out_scale
					conv_cfg.out_width, out_height_single, conv_cfg.dat_banks, 0, cdma_wt_reuse_single);

			}
	}
	else//CHout_Split_Times->Hout_Split_Times
	{
		for (int k = 0; k < conv_cfg.Hout_Split_Times; k = k + 1)
			for (int n = 0; n < conv_cfg.CHout_Split_Times; n = n + 1)
			{
				int in_height_single, out_height_single;
				int line_offset_in, line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_dat_reuse_single;//cdma_wt_reuse_single=0

				if (n != conv_cfg.CHout_Split_Times - 1)
					ch_out_single = conv_cfg.out_ch_slice;
				else
					ch_out_single = conv_cfg.out_ch_slice_last;

				if (n == 0)
					cdma_dat_reuse_single = 0;
				else
					cdma_dat_reuse_single = 1;

				if (k == 0)
				{
					line_offset_in = 0; line_offset_out = 0; pad_y_single = conv_cfg.pad_y;
				}
				else
				{
					line_offset_in = (conv_cfg.in_height_first - conv_cfg.overlap) + (k - 1) * (conv_cfg.in_height_middle - conv_cfg.overlap);
					line_offset_out = conv_cfg.out_height_first + (k - 1) * conv_cfg.out_height_middle;
					pad_y_single = 0;
				}


				if (k == 0)
				{
					in_height_single = conv_cfg.in_height_first; out_height_single = conv_cfg.out_height_first;
				}
				else
					if (k == conv_cfg.Hout_Split_Times - 1)
					{
						in_height_single = conv_cfg.in_height_last;
						out_height_single = conv_cfg.out_height_last;
					}
					else
					{
						in_height_single = conv_cfg.in_height_middle;
						out_height_single = conv_cfg.out_height_middle;
					}
									
				FPGA_Conv_single_time(Tin_factor, conv_cfg.Tin_L, CH_in_res_Tin_div_Tout_minus1, shift_sacle,
					conv_cfg.CHin, in_height_single, conv_cfg.Win, ch_out_single,
					conv_cfg.Kx, conv_cfg.Ky, conv_cfg.Sx, conv_cfg.Sy,
					conv_cfg.pad_x, pad_y_single, gelu_en, relu_en, Conv_Out_and_In_mode, out_dw,
					((uint64_t)feature_in->payload) + feature_in->line_stride * line_offset_in, feature_in->surface_stride, feature_in->line_stride, feature_in->scale,
					((uint64_t)wt->payload) + conv_cfg.wt_size_in_bytes / conv_cfg.CHout * conv_cfg.out_ch_slice * n, conv_cfg.wt_size_in_bytes / conv_cfg.CHout * ch_out_single * 8 / (in_dw * Tout), wt_num_div_Tin / conv_cfg.CHout * ch_out_single, wt->scale,
					((uint64_t)feature_out->payload) + feature_out->line_stride * line_offset_out + feature_out->surface_stride * n * (conv_cfg.out_ch_slice / Tout), feature_out->surface_stride, feature_out->line_stride, feature_out->scale,//conv_out_scale
					conv_cfg.out_width, out_height_single, conv_cfg.dat_banks, cdma_dat_reuse_single, 0);
			}
	}
}

void FPGA_Conv_single_time(int Tin_factor, int Tin_L, int CH_in_res_Tin_div_Tout_minus1, int shift_sacle,
	int CHin, int Hin, int Win, int CHout,
	int Kx, int Ky, int Sx, int Sy,
	int pad_x, int pad_y, int gelu_en, int relu_en, int Conv_Out_and_In_mode, int out_dw,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride, int feature_in_scale,
	int wt_base_addr, int wt_num_div_Tout, int wt_num_div_Tin, int wt_scale,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, int feature_out_scale,
	int out_width, int out_height, int dat_buf_num, int cdma_dat_reuse, int cdma_wt_reuse)
{

	int rdata;

	//cfg conv_path
	CSB_Write(2, Tin_factor);
	CSB_Write(3, dat_buf_num);
	CSB_Write(4, Win);
	CSB_Write(5, Hin);
	CSB_Write(6, Win * Hin);
	CSB_Write(7, (CHin + Tout - 1) / Tout);
	CSB_Write(8, (CHin + Tin_L - 1) / Tin_L);
	CSB_Write(31, CH_in_res_Tin_div_Tout_minus1);
	CSB_Write(9, pad_y);
	CSB_Write(10, pad_x);
	CSB_Write(11, Sx);
	CSB_Write(12, Sy);
	CSB_Write(13, Kx);
	CSB_Write(14, Ky);
	CSB_Write(15, out_width);
	CSB_Write(16, out_width*out_height);
	CSB_Write(17, out_height);

	CSB_Write(18, CHout);
	CSB_Write(19, ((CHout + Tout - 1) / Tout));
	CSB_Write(33, ((out_width * out_height + Tout - 1) / Tout));
	CSB_Write(34, feature_out_base);
	CSB_Write(35, feature_out_surface_stride);
	CSB_Write(36, feature_out_line_stride);
	CSB_Write(38, (gelu_en << 1) | relu_en);
	CSB_Write(39, (out_dw - 1));
	CSB_Write(39, feature_out_scale);// gelu in scale
	
	CSB_Write(20, feature_in_base);
	CSB_Write(21, feature_in_surface_stride);
	CSB_Write(22, feature_in_line_stride);
	CSB_Write(23, (wt_num_div_Tout / Tin_factor));
	CSB_Write(32, wt_num_div_Tin);
	CSB_Write(24, wt_base_addr);
	CSB_Write(25, (cdma_wt_reuse << 1) | cdma_dat_reuse);
	CSB_Write(26, shift_sacle);
	
	CSB_Write(47, Conv_Out_and_In_mode);
//printf("\n Win * Hin=%6d", Win * Hin);
//printf("\n out_width*out_height=%6d", out_width*out_height);
//printf("\n CHout=%6d", CHout);

	CSB_Write(0, 0b011111);//Kick off the run. Conv Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

	while (CSB_Read(37) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for SPD_wdma done\r\n");
	}
	
	CSB_Write(47, 0);
}


void FPGA_Vit_Conv0(struct Conv_Cfg conv_cfg, int relu_en, int Vit_Conv0_Out_and_In_Mode,struct Mapped_Feature* feature_in, struct Mapped_Weight* wt, struct Mapped_Feature* feature_out)
{

	int in_dw = wt->wt_bit;
	int out_dw = feature_out->dat_bit;
	int wt_num_div_Tin = (1 * conv_cfg.Ky * conv_cfg.CHout * ((conv_cfg.CHin + conv_cfg.Tin_L - 1) / conv_cfg.Tin_L));
	int Tin_factor = MAX_DAT_DW / in_dw;
	
	int CH_in_res_Tin_div_Tout_minus1;
	CH_in_res_Tin_div_Tout_minus1 = (conv_cfg.Tin_L/Tout-1);

	int shift_sign = 0;//right shift
	int shift_value = (feature_in->scale + wt->scale) - feature_out->scale;
	if (shift_value < 0)
	{
		shift_value = feature_out->scale - (feature_in->scale + wt->scale);
		shift_sign = 1; //left shift
	}
	int shift_sacle = 32 * shift_sign + shift_value;

	//printf("\n Win=%6d", conv_cfg.Win);
	//printf("\n Kx=%6d", conv_cfg.Kx);
	//printf("\n wt_num_div_Tin=%6d", wt_num_div_Tin);
	//printf("\n Hout_Split_Times=%6d", conv_cfg.Hout_Split_Times);
	//printf("\n CHout_Split_Times=%6d\n", conv_cfg.CHout_Split_Times);


	if (conv_cfg.method == 0)//Hout_Split_Times->CHout_Split_Times
	{
		for (int n = 0; n < conv_cfg.CHout_Split_Times; n = n + 1)
			for (int k = 0; k < conv_cfg.Hout_Split_Times; k = k + 1)
			{
				int in_height_single, out_height_single;
				int line_offset_in, line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_wt_reuse_single;//cdma_dat_reuse_single=0

				if (n != conv_cfg.CHout_Split_Times - 1)
					ch_out_single = conv_cfg.out_ch_slice;
				else
					ch_out_single = conv_cfg.out_ch_slice_last;

				if (k == 0)
				{
					line_offset_in = 0; line_offset_out = 0; pad_y_single = conv_cfg.pad_y; cdma_wt_reuse_single = 0;
				}
				else
				{
					line_offset_in = (conv_cfg.in_height_first - conv_cfg.overlap) + (k - 1) * (conv_cfg.in_height_middle - conv_cfg.overlap);
					line_offset_out = conv_cfg.out_height_first + (k - 1) * conv_cfg.out_height_middle;
					pad_y_single = 0;
					cdma_wt_reuse_single = 1;
				}

				if (k == 0)
				{
					in_height_single = conv_cfg.in_height_first; out_height_single = conv_cfg.out_height_first;
				}
				else
					if (k == conv_cfg.Hout_Split_Times - 1)
					{
						in_height_single = conv_cfg.in_height_last; out_height_single = conv_cfg.out_height_last;
					}
					else
					{
						in_height_single = conv_cfg.in_height_middle; out_height_single = conv_cfg.out_height_middle;
					}

				FPGA_Vit_Conv0_single_time(Tin_factor, conv_cfg.Tin_L, CH_in_res_Tin_div_Tout_minus1, shift_sacle,
					conv_cfg.CHin, in_height_single, conv_cfg.Win, ch_out_single,
					conv_cfg.Kx, conv_cfg.Ky, conv_cfg.Sx, conv_cfg.Sy,
					conv_cfg.pad_x, pad_y_single, relu_en, Vit_Conv0_Out_and_In_Mode, out_dw,
					((uint64_t)feature_in->payload) + feature_in->line_stride * line_offset_in, feature_in->surface_stride, feature_in->line_stride, feature_in->scale,
					((uint64_t)wt->payload) + conv_cfg.wt_size_in_bytes / conv_cfg.CHout * conv_cfg.out_ch_slice * n, conv_cfg.wt_size_in_bytes / conv_cfg.CHout * ch_out_single * 8 / (Tout * in_dw), wt_num_div_Tin / conv_cfg.CHout * ch_out_single, wt->scale,
					((uint64_t)feature_out->payload) + feature_out->line_stride * line_offset_out + feature_out->surface_stride * n * (conv_cfg.out_ch_slice / Tout), feature_out->surface_stride, feature_out->line_stride, feature_out->scale,//conv_out_scale
					conv_cfg.out_width, out_height_single, conv_cfg.dat_banks, 0, cdma_wt_reuse_single);

			}
	}
	else//CHout_Split_Times->Hout_Split_Times
	{
		for (int k = 0; k < conv_cfg.Hout_Split_Times; k = k + 1)
			for (int n = 0; n < conv_cfg.CHout_Split_Times; n = n + 1)
			{
				int in_height_single, out_height_single;
				int line_offset_in, line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_dat_reuse_single;//cdma_wt_reuse_single=0

				if (n != conv_cfg.CHout_Split_Times - 1)
					ch_out_single = conv_cfg.out_ch_slice;
				else
					ch_out_single = conv_cfg.out_ch_slice_last;

				if (n == 0)
					cdma_dat_reuse_single = 0;
				else
					cdma_dat_reuse_single = 1;

				if (k == 0)
				{
					line_offset_in = 0; line_offset_out = 0; pad_y_single = conv_cfg.pad_y;
				}
				else
				{
					line_offset_in = (conv_cfg.in_height_first - conv_cfg.overlap) + (k - 1) * (conv_cfg.in_height_middle - conv_cfg.overlap);
					line_offset_out = conv_cfg.out_height_first + (k - 1) * conv_cfg.out_height_middle;
					pad_y_single = 0;
				}


				if (k == 0)
				{
					in_height_single = conv_cfg.in_height_first; out_height_single = conv_cfg.out_height_first;
				}
				else
					if (k == conv_cfg.Hout_Split_Times - 1)
					{
						in_height_single = conv_cfg.in_height_last;
						out_height_single = conv_cfg.out_height_last;
					}
					else
					{
						in_height_single = conv_cfg.in_height_middle;
						out_height_single = conv_cfg.out_height_middle;
					}
									
				FPGA_Vit_Conv0_single_time(Tin_factor, conv_cfg.Tin_L, CH_in_res_Tin_div_Tout_minus1, shift_sacle,
					conv_cfg.CHin, in_height_single, conv_cfg.Win, ch_out_single,
					conv_cfg.Kx, conv_cfg.Ky, conv_cfg.Sx, conv_cfg.Sy,
					conv_cfg.pad_x, pad_y_single, relu_en, Vit_Conv0_Out_and_In_Mode, out_dw,
					((uint64_t)feature_in->payload) + feature_in->line_stride * line_offset_in, feature_in->surface_stride, feature_in->line_stride, feature_in->scale,
					((uint64_t)wt->payload) + conv_cfg.wt_size_in_bytes / conv_cfg.CHout * conv_cfg.out_ch_slice * n, conv_cfg.wt_size_in_bytes / conv_cfg.CHout * ch_out_single * 8 / (in_dw * Tout), wt_num_div_Tin / conv_cfg.CHout * ch_out_single, wt->scale,
					((uint64_t)feature_out->payload) + feature_out->line_stride * line_offset_out + feature_out->surface_stride * n * (conv_cfg.out_ch_slice / Tout), feature_out->surface_stride, feature_out->line_stride, feature_out->scale,//conv_out_scale
					conv_cfg.out_width, out_height_single, conv_cfg.dat_banks, cdma_dat_reuse_single, 0);
			}
	}
}


void FPGA_Vit_Conv0_single_time(int Tin_factor, int Tin_L, int CH_in_res_Tin_div_Tout_minus1, int shift_sacle,
	int CHin, int Hin, int Win, int CHout,
	int Kx, int Ky, int Sx, int Sy,
	int pad_x, int pad_y, int relu_en, int Vit_Conv0_Out_and_In_Mode, int out_dw,
	int feature_in_base, int feature_in_surface_stride, int feature_in_line_stride, int feature_in_scale,
	int wt_base_addr, int wt_num_div_Tout, int wt_num_div_Tin, int wt_scale,
	int feature_out_base, int feature_out_surface_stride, int feature_out_line_stride, int feature_out_scale,
	int out_width, int out_height, int dat_buf_num, int cdma_dat_reuse, int cdma_wt_reuse)
{

	int rdata;

	//cfg conv_path
	CSB_Write(0, 0b1000000);//Kick off the conv_Vit0

	//printf("\n Win =%6d", Win);
	//printf("\n Win * Hin=%6d", Win * Hin);
	//printf("\n out_width*out_height=%6d", out_width*out_height);
	//printf("\n CHout=%6d", CHout);
	//printf("\n CHin=%6d", CHin);
	//printf("\n Sx=%6d", Sx);
	//printf("\n Kx=%6d", Kx);
	//printf("\n wt_num_div_Tout=%6d", wt_num_div_Tout);
	
	
	CSB_Write(2, Tin_factor);
	CSB_Write(3, dat_buf_num);
	CSB_Write(4, Win);
	CSB_Write(5, Hin);
	CSB_Write(6, Win * Hin);
	CSB_Write(7, (CHin + Tout - 1) / Tout);
	CSB_Write(8, (CHin + Tin_L - 1) / Tin_L);
	CSB_Write(31, 0);
	CSB_Write(9, pad_y);
	CSB_Write(10, pad_x);
	CSB_Write(11, Sx);
	CSB_Write(12, Sy);
	CSB_Write(13, Kx);
	CSB_Write(14, Ky);
	CSB_Write(15, out_width);
	CSB_Write(16, out_width*out_height);
	CSB_Write(17, out_height);

	CSB_Write(18, CHout);
	CSB_Write(19, ((CHout + Tout - 1) / Tout));
	CSB_Write(33, ((out_width * out_height + Tout - 1) / Tout));
	CSB_Write(34, feature_out_base);
	CSB_Write(35, feature_out_surface_stride);
	CSB_Write(36, feature_out_line_stride);
	CSB_Write(38, relu_en);
	CSB_Write(39, (out_dw - 1));
	CSB_Write(20, feature_in_base);
	CSB_Write(21, feature_in_surface_stride);
	CSB_Write(22, feature_in_line_stride);
	CSB_Write(23, (wt_num_div_Tout / Tin_factor));
	CSB_Write(32, wt_num_div_Tin);
	CSB_Write(24, wt_base_addr);
	CSB_Write(25, (cdma_wt_reuse << 1) | cdma_dat_reuse);
	CSB_Write(26, shift_sacle);

	CSB_Write(47, Vit_Conv0_Out_and_In_Mode);
	CSB_Write(0, 0b011111);//Kick off the run. Conv Path: fsm_wt,fsm_dat,buf_init,dma_wt,dma_dat

	while (CSB_Read(37) != 1)
	{
		//usleep(10);
		//xil_printf("Wait for SPD_wdma done\r\n");
	}
	CSB_Write(47, 0);
}
#endif