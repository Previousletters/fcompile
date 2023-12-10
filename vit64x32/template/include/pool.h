
#pragma once

#ifndef SRC_POOL_H_
#define SRC_POOL_H_

#include "basic.h"


///// function for software//////////
struct Pool_Cfg
{
	int Hin;
	int Hout;
	int Win;
	int Wout;
	int out_ch_slice;
	int Kx;
	int Ky;
	int Sx;
	int Sy;
	int P_left;
	int P_up;
	int P_right;

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
	int overlap_x;
	int overlap_y;
	int first_2D_rd_addr_max;
	int middle_2D_rd_addr_max;
	int last_2D_rd_addr_max;
	int One_div_KxKy;
};

struct Pool_Cfg Get_Pool_Cfg(int Hin, int Win, int CHin, int Kx, int Ky, int Sx, int Sy, int Px, int Py);

void Save_Pool_Cfg(const char* cfg_name, struct Pool_Cfg conv_cfg, FILE* fp);

void Pool_Soft(int Kx, int Ky, int Sx, int Sy, int pooling_method,
	int pad_left, int pad_right, int pad_up, int pad_down,
	struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out);

///// function for hardware//////////
#ifdef Run_on_FPGA
void FPGA_Pool(struct Pool_Cfg pool_cfg, int pooling_method, struct Mapped_Feature* feature_in, struct Mapped_Feature* feature_out);
#endif

#endif /* SRC_POOL_H_ */
