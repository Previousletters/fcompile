#include <iostream>
#include <fstream>
#include <iomanip>

#include <driver/basic.h>
#include <driver/TOP_defines.h>
#include <driver/FPGA_MEM.h>
using namespace std;

///// function for hardware//////////

namespace hbm {

Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit)
{
	struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
	if (ret == NULL)
	{
		return NULL;
	}

	ret->scale = scale;
	ret->height = height;
	ret->width = width;
	ret->channel = ch;
	ret->dat_bit = dat_bit;
	ret->conv_out_scale = conv_out_scale;

	ret->line_stride = width * (MAX_DAT_DW / 8) * Tout;
	ret->surface_stride = ret->line_stride * height;

	int require_bytes = ret->surface_stride * ((ch + Tout - 1) / Tout);
	ret->payload_size = require_bytes;

	ret->payload = (short*)FPGA_DDR_malloc(require_bytes);
	if (ret->payload == FPGA_NULL)
	{
		free(ret);
		return NULL;
	}
	
	//printf("**** feature->payload=%d\n", ret->payload);
	return ret;
}

Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit)
{
	struct Mapped_Weight* ret = (struct Mapped_Weight*)malloc(sizeof(struct Mapped_Weight));
	if (ret == NULL)
		return NULL;

	ret->scale = scale;
	ret->Ky = Ky;
	ret->Kx = Kx;
	ret->in_ch = in_ch;
	ret->out_ch = out_ch;
	ret->wt_bit = wt_bit;

	int require_bytes = ((base_Tin * wt_bit) / 8) * Kx * Ky * out_ch * ((in_ch + base_Tin - 1) / base_Tin) * 2;
	ret->payload_size = require_bytes;

	ret->payload = (short*)FPGA_HBM_malloc(require_bytes);
	if (ret->payload == FPGA_NULL)
	{
		free(ret);
		return NULL;
	}
	
	//printf("**** weight->payload=%d\n", ret->payload);
	return ret;
}

void Free_Feature(struct Mapped_Feature* feature)
{
    if (feature->payload != FPGA_NULL) FPGA_DDR_free(feature->payload);
    free(feature);
}

void Free_Weight(struct Mapped_Weight* weight)
{
    if (weight->payload != FPGA_NULL) FPGA_HBM_free(weight->payload);
    free(weight);
}

void CSB_Write(HANDLE user_device, int addr, int data)
{
	Reg_Write(user_device, addr, data);
}

void CSB_Read(HANDLE user_device, int addr, int data)
{
	Reg_Read(user_device, addr, data);
}

int CSB_Read(HANDLE user_device, int addr)
{
	Reg_Read(user_device, addr, 1);
	return 1;
}

regops_t CSB_Finish() {
    regops_t result(regop);
    regop.clear();
    return result;
}

void Reg_Write(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(1, std::make_pair(addr, data)));
}

void Reg_Read(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(0, std::make_pair(addr, data)));
}

};
