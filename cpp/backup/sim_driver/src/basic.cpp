#include <iostream>
#include <fstream>
#include <iomanip>
#include "basic.h"
#include "TOP_defines.h"
#include "FPGA_MEM.h"
using namespace std;

///// function for hardware//////////

struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit)
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

void Reshape(struct Mapped_Feature* feature, int height, int width, int chin)
{
	int new_line_stride = width * (MAX_DAT_DW / 8) * Tout;
	int new_surface_stride = new_line_stride * height;

	int new_require_bytes = new_surface_stride * ((chin + Tout - 1) / Tout);
	if (new_require_bytes == feature->payload_size) {
		feature->height = height;
		feature->width = width;
		feature->channel = chin;
		feature->line_stride = new_line_stride;
		feature->surface_stride = new_surface_stride;
	}
	else {
		std::cout << "Reshape Error!" << std::endl;
		exit(-1);
	}
}

std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, int axis) {
	struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
	if (ret == NULL)
	{
		exit(-1);
	}
	ret->scale = feature->scale;
	ret->height = feature->height;
	ret->width = feature->width;
	ret->channel = feature->channel;
	if (axis == 0) {
		ret->height = ret->height / 2;
		feature->height = ret->height;
	}
	else if (axis == 1) {
		ret->width = ret->width / 2;
		feature->width = ret->width;
	}
	else if (axis == 2) {
		ret->channel = ret->channel / 2;
		feature->channel = ret->channel;
	}
	ret->dat_bit = feature->dat_bit;
	ret->conv_out_scale = feature->conv_out_scale;

	ret->line_stride = ret->width * (MAX_DAT_DW / 8) * Tout;
	ret->surface_stride = ret->line_stride * ret->height;

	int require_bytes = ret->surface_stride * ((ret->channel + Tout - 1) / Tout);
	ret->payload_size = require_bytes;
	
	feature->line_stride = ret->line_stride;
	feature->surface_stride = ret->surface_stride;
	feature->payload_size = ret->payload_size;
	ret->payload = (short*)((uint64_t)feature->payload + feature->payload_size);
	return std::vector<struct Mapped_Feature*>({ feature, ret });
}

struct Mapped_Feature* Merge(std::vector<struct Mapped_Feature*> features) {
	struct Mapped_Feature* feature = features[0];
	int new_height = feature->height * 2;
	free(features[1]);
	feature->height = new_height;

	feature->line_stride = feature->width * (MAX_DAT_DW / 8) * Tout;
	feature->surface_stride = feature->line_stride * new_height;

	int require_bytes = feature->surface_stride * ((feature->channel + Tout - 1) / Tout);
	feature->payload_size = require_bytes;

	return feature;
}

struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit)
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