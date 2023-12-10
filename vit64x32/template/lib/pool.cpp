#include "pool.h"

struct Pool_Cfg Get_Pool_Cfg(int Hin, int Win, int CHin,
	int Kx, int Ky, int Sx, int Sy, int Px, int Py)
{
	struct Pool_Cfg ret;

	int P_left;
	int P_right;
	int P_up;
	int P_down;

	int Wout;
	int Hout;
	int max_out_width;
	int max_out_height;
	int max_rows_of_in_height;

	int split_w_num;
	int first_width_in;
	int middle_width_in;
	int last_width_in;
	int first_width_out;
	int middle_width_out;
	int last_width_out;
	int first_1D_rd_addr_max;
	int middle_1D_rd_addr_max;
	int last_1D_rd_addr_max;

	int split_h_num;
	int first_height_in;
	int middle_height_in;
	int last_height_in;
	int first_height_out;
	int middle_height_out;
	int last_height_out;
	int first_2D_rd_addr_max;
	int middle_2D_rd_addr_max;
	int last_2D_rd_addr_max;

	int overlap_x;
	int overlap_y;
	int One_div_KxKy;

	if (MAX_DAT_DW == 16) { One_div_KxKy = 65536 / (Kx * Ky); }
	if (MAX_DAT_DW == 8) { One_div_KxKy = 256 / (Kx * Ky); }

	P_left = Px;
	P_up = Py;
	Wout = ((Win + 2 * Px - Kx) / Sx + 1);
	Hout = ((Hin + 2 * Py - Ky) / Sy + 1);

	//////////////////////width direction///////////////////////////
	overlap_x = (Kx - Sx);
	max_out_width = (POOL_1D_DAT_IN_DEP - Kx) / Sx + 1;

	if (max_out_width * Ky > POOL_2D_DAT_IN_DEP)
	{
		printf("POOL_2D_DAT_IN_MEM not enough!\n");
		return ret;
	}

	split_w_num = (Wout + max_out_width - 1) / max_out_width;//ceil(Wout/max_out_width)-1
	if (split_w_num == 1)
	{
		first_width_out = Wout;
		first_width_in = Win;
	}
	else
	{
		first_width_out = max_out_width;
		first_width_in = (first_width_out - 1) * Sx + Kx - Px;
	}

	middle_width_out = max_out_width;
	middle_width_in = max_out_width * Sx + overlap_x;

	if ((Wout - first_width_out) % middle_width_out == 0)
	{
		split_w_num = (Wout - first_width_out) / middle_width_out + 1;
		last_width_out = middle_width_out;
	}
	else
	{
		split_w_num = (Wout - first_width_out) / middle_width_out + 2;
		last_width_out = (Wout - first_width_out) % middle_width_out;
	}


	if (split_w_num == 1)
	{
		last_width_in = first_width_in;
		last_width_out = first_width_out;
	}
	else
		last_width_in = Win - first_width_in + overlap_x - (split_w_num - 2) * (middle_width_in - overlap_x);


	middle_1D_rd_addr_max = middle_width_out * Kx;
	first_1D_rd_addr_max = first_width_out * Kx - P_left;

	if (split_w_num == 1)
	{
		if (last_width_in >= (Kx + (last_width_out - 1) * Sx))
		{
			last_1D_rd_addr_max = last_width_out * Kx;
			P_right = 0;
		}
		else
		{
			P_right = ((Kx + (last_width_out - 1) * Sx)) - last_width_in - P_left;
			last_1D_rd_addr_max = last_width_out * Kx - P_right;
		}
	}
	else
	{
		if (last_width_in >= (Kx + (last_width_out - 1) * Sx))
		{
			last_1D_rd_addr_max = last_width_out * Kx;
			P_right = 0;
		}
		else
		{
			P_right = ((Kx + (last_width_out - 1) * Sx)) - last_width_in;
			last_1D_rd_addr_max = last_width_out * Kx - P_right;
		}
	}

	////////////////////// simple height direction///////////////////////////
	first_height_out = 1;
	middle_height_out = 1;
	last_height_out = 1;

	overlap_y = (Ky - Sy);
	split_h_num = Hout;
	first_height_in = (Ky - P_up);
	middle_height_in = Ky;
	last_height_in = Hin - first_height_in + Ky - Sy - (split_h_num - 2) * Sy;
	if (last_height_in > middle_height_in)
	{
		last_height_in = middle_height_in;
	}

	middle_2D_rd_addr_max = middle_height_out * Ky;
	first_2D_rd_addr_max = first_height_out * Ky - P_up;

	if (last_height_in == (Ky + (last_height_out - 1) * Sy))
	{
		last_2D_rd_addr_max = last_height_out * Ky;
		P_down = 0;
	}
	else
	{
		P_down = Ky - last_height_in;
		last_2D_rd_addr_max = last_height_out * Ky - P_down;
	}

	ret.Hin = Hin;
	ret.Hout = Hout;
	ret.Win = Win;
	ret.Wout = Wout;
	ret.out_ch_slice = (CHin + Tout - 1) / Tout;

	ret.Kx = Kx;
	ret.Ky = Ky;
	ret.Sx = Sx;
	ret.Sy = Sy;
	ret.P_left = Px;
	ret.P_up = P_up;
	ret.P_right = P_right;

	ret.split_w_num = split_w_num;
	ret.first_width_in = first_width_in;
	ret.middle_width_in = middle_width_in;
	ret.last_width_in = last_width_in;
	ret.first_width_out = first_width_out;
	ret.middle_width_out = middle_width_out;
	ret.last_width_out = last_width_out;
	ret.first_1D_rd_addr_max = first_1D_rd_addr_max;
	ret.middle_1D_rd_addr_max = middle_1D_rd_addr_max;
	ret.last_1D_rd_addr_max = last_1D_rd_addr_max;
	ret.split_h_num = split_h_num;
	ret.first_height_in = first_height_in;
	ret.middle_height_in = middle_height_in;
	ret.last_height_in = last_height_in;
	ret.overlap_x = overlap_x;
	ret.overlap_y = overlap_y;
	ret.first_2D_rd_addr_max = first_2D_rd_addr_max;
	ret.middle_2D_rd_addr_max = middle_2D_rd_addr_max;
	ret.last_2D_rd_addr_max = last_2D_rd_addr_max;
	ret.One_div_KxKy = One_div_KxKy;

	return ret;
}

void Save_Pool_Cfg(const char* cfg_name, struct Pool_Cfg pool_cfg, FILE* fp)
{

	fprintf(fp, "struct Pool_Cfg %s={%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d};\n", cfg_name,

		pool_cfg.Hin,
		pool_cfg.Hout,
		pool_cfg.Win,
		pool_cfg.Wout,
		pool_cfg.out_ch_slice,
		pool_cfg.Kx,
		pool_cfg.Ky,
		pool_cfg.Sx,
		pool_cfg.Sy,
		pool_cfg.P_left,
		pool_cfg.P_up,
		pool_cfg.P_right,
		pool_cfg.split_w_num,
		pool_cfg.first_width_in,
		pool_cfg.middle_width_in,
		pool_cfg.last_width_in,
		pool_cfg.first_width_out,
		pool_cfg.middle_width_out,
		pool_cfg.last_width_out,
		pool_cfg.first_1D_rd_addr_max,
		pool_cfg.middle_1D_rd_addr_max,
		pool_cfg.last_1D_rd_addr_max,
		pool_cfg.split_h_num,
		pool_cfg.first_height_in,
		pool_cfg.middle_height_in,
		pool_cfg.last_height_in,
		pool_cfg.overlap_x,
		pool_cfg.overlap_y,
		pool_cfg.first_2D_rd_addr_max,
		pool_cfg.middle_2D_rd_addr_max,
		pool_cfg.last_2D_rd_addr_max,
		pool_cfg.One_div_KxKy

	);
}

void Pool_Soft(int Kx, int Ky, int Sx, int Sy, int pooling_method,
	int pad_left, int pad_right, int pad_up, int pad_down,
	struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out)
{
	int32_t One_div_KxKy;
	int64_t product;

	if (MAX_DAT_DW == 16) { One_div_KxKy = 65536 / (Kx * Ky); }
	if (MAX_DAT_DW == 8) { One_div_KxKy = 256 / (Kx * Ky); }


	for (int i = 0; i < feature_out->channel; i++)
	{
		for (int j = 0; j < feature_out->height; j = j + 1)//feature_out->height
		{
			for (int k = 0; k < feature_out->width; k = k + 1)//feature_out->width
			{
				int32_t tp = 0;

				for (int kj = 0; kj < Ky; kj++)
				{
					for (int ki = 0; ki < Kx; ki++)
					{
						short data;
						int axis_h;
						int axis_w;
						axis_h = Sy * j + kj - pad_up;
						axis_w = Sx * k + ki - pad_left;
						if ((axis_h < 0) || (axis_h >= feature_in->height) || (axis_w < 0) || (axis_w >= feature_in->width))//padding 0
							data = 0;
						else
							data = *Get_Element_Soft(feature_in, axis_h, axis_w, i);

						switch (pooling_method)
						{
						case 0: {tp = (tp < data) ? tp : data; break; }
						case 1: {tp = (tp < data) ? data : tp; break; }
						case 2: {tp = tp + data; break; }
						}
					}
				}

				if (pooling_method == 2)
				{
					short res;
					product = tp * One_div_KxKy;
					res = product >> MAX_DAT_DW;
					res = res + ((product & (1 << (MAX_DAT_DW - 1))) != 0);
					*Get_Element_Soft(feature_out, j, k, i) = res;
				}
				else
					*Get_Element_Soft(feature_out, j, k, i) = tp;
			}
		}
	}
}


///// function for hardware//////////
#ifdef Run_on_FPGA
void FPGA_Pool(struct Pool_Cfg pool_cfg, int pooling_method, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out)
{
	int Tin_factor;
	Tin_factor = MAX_DAT_DW / (feature_in->dat_bit); //1 meams 8bit, 2 means 4bit, 4 means 2bit

	CSB_Write(pool_reg_bias + 2, pooling_method);//pooling_method
	CSB_Write(pool_reg_bias + 3, (uint64_t)(feature_in->payload));//feature_in_base_addr
	CSB_Write(pool_reg_bias + 4, feature_in->surface_stride);//feature_in_surface_stride_in
	CSB_Write(pool_reg_bias + 5, feature_in->line_stride);//feature_in_line_stride_in
	CSB_Write(pool_reg_bias + 6, (uint64_t)(feature_out->payload));//feature_out_base_addr
	CSB_Write(pool_reg_bias + 7, feature_out->surface_stride);//feature_out_surface_stride_in
	CSB_Write(pool_reg_bias + 8, feature_out->line_stride);//feature_out_line_stride_in
	CSB_Write(pool_reg_bias + 9, pool_cfg.out_ch_slice);//ch_in_div_Tout
	CSB_Write(pool_reg_bias + 10, pool_cfg.Hin);
	CSB_Write(pool_reg_bias + 11, pool_cfg.Hout);
	CSB_Write(pool_reg_bias + 28, pool_cfg.Win);
	CSB_Write(pool_reg_bias + 29, pool_cfg.Wout);
	CSB_Write(pool_reg_bias + 19, pool_cfg.Kx);
	CSB_Write(pool_reg_bias + 20, pool_cfg.Ky);
	CSB_Write(pool_reg_bias + 21, pool_cfg.Sx);
	CSB_Write(pool_reg_bias + 22, pool_cfg.Sy);
	CSB_Write(pool_reg_bias + 26, pool_cfg.P_left);//Px
	CSB_Write(pool_reg_bias + 27, pool_cfg.P_up);//Py
	CSB_Write(pool_reg_bias + 24, pool_cfg.P_right);//Padding_right

	CSB_Write(pool_reg_bias + 12, pool_cfg.split_w_num);//split_w_num
	CSB_Write(pool_reg_bias + 13, pool_cfg.first_width_in);//first_width_in
	CSB_Write(pool_reg_bias + 14, pool_cfg.middle_width_in);//middle_width_in
	CSB_Write(pool_reg_bias + 15, pool_cfg.last_width_in);//last_width_in
	CSB_Write(pool_reg_bias + 16, pool_cfg.first_width_out);//first_width_out
	CSB_Write(pool_reg_bias + 17, pool_cfg.middle_width_out);//middle_width_out
	CSB_Write(pool_reg_bias + 18, pool_cfg.last_width_out);//last_width_out
	CSB_Write(pool_reg_bias + 37, pool_cfg.first_1D_rd_addr_max);//first_1D_rd_addr_max
	CSB_Write(pool_reg_bias + 38, pool_cfg.middle_1D_rd_addr_max);//middle_1D_rd_addr_max
	CSB_Write(pool_reg_bias + 39, pool_cfg.last_1D_rd_addr_max);//last_1D_rd_addr_max         

	CSB_Write(pool_reg_bias + 31, pool_cfg.split_h_num);//split_h_num
	CSB_Write(pool_reg_bias + 32, pool_cfg.first_height_in);//first_height_in
	CSB_Write(pool_reg_bias + 33, pool_cfg.middle_height_in);//middle_height_in
	CSB_Write(pool_reg_bias + 34, pool_cfg.last_height_in);//last_height_in
	CSB_Write(pool_reg_bias + 35, pool_cfg.overlap_x);//overlap_x
	CSB_Write(pool_reg_bias + 36, pool_cfg.overlap_y);//overlap_y
	CSB_Write(pool_reg_bias + 40, pool_cfg.first_2D_rd_addr_max);//first_2D_rd_addr_max
	CSB_Write(pool_reg_bias + 41, pool_cfg.middle_2D_rd_addr_max);//middle_2D_rd_addr_max
	CSB_Write(pool_reg_bias + 42, pool_cfg.last_2D_rd_addr_max);//last_2D_rd_addr_max    

	CSB_Write(pool_reg_bias + 43, Tin_factor);//1 meams 8bit, 2 means 4bit, 4 means 2bit

	if (pooling_method == 2)
	{
		CSB_Write(pool_reg_bias + 23, pool_cfg.One_div_KxKy);
	}//One_div_KxKy

//Kick of the run
	CSB_Write(pool_reg_bias + 0, 1);//pdp_start

	//Waiting for pdp_wdma done
	while (CSB_Read(pool_reg_bias + 1) != 1)
	{
		//	usleep(10);
		//	printf("Wait for PDP_wdma done\r\n");
	}
}

#endif