#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <driver/basic.h>
#include <driver/TOP_defines.h>
#include <driver/FPGA_MEM.h>
using namespace std;

///// function for hardware//////////

namespace hbm {

uint64_t GetSizeFeature(int height, int width, int ch) {
    return height*width*((ch + Tout - 1) / Tout)*Tout*2;
}

uint64_t GetSizeWeight(int chin, int chout) {
	uint64_t require_bytes = 0;
    int wt_bit = 4;
    int CHout_div_Tout = (chout + Tout - 1) / Tout;
    int WT_CHin_Padding_with_Tin = int((chin + base_Tin - 1) / base_Tin) * base_Tin;
    int WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup;

    for(int i=0;i<CHout_div_Tout;i++)
        for(int j=0;j<WT_scale_group_nums;j++)
            for(int k=0;k<Tout/HBM_Port;k++)
                for(int n=0;n<HBM_AXI_DATA_WIDTH/32;n++)
                    require_bytes ++;

    for(int i=0;i<CHout_div_Tout;i++)
        for(int j=0;j<WT_scale_group_nums;j++)
            for(int k=0;k<Tout/HBM_Port;k++) {
                int wt_start_ch_in=j*WT_CH_Tgroup;
                int wt_end_ch_in=(j==WT_scale_group_nums-1)?WT_CHin_Padding_with_Tin:(j+1)*WT_CH_Tgroup;
                for(int n=wt_bit*wt_start_ch_in/32;n<wt_bit*wt_end_ch_in/32;n++)
                    require_bytes ++;
            }

	return require_bytes*4;
}

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
	int require_bytes = 0;
    int CHout_div_Tout = (out_ch + Tout - 1) / Tout;
    int WT_CHin_Padding_with_Tin = int((in_ch + base_Tin - 1) / base_Tin) * base_Tin;
    int WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup;

    for(int i=0;i<CHout_div_Tout;i++)
        for(int j=0;j<WT_scale_group_nums;j++)
            for(int k=0;k<Tout/HBM_Port;k++)
                for(int n=0;n<HBM_AXI_DATA_WIDTH/32;n++)
                    require_bytes ++;

    for(int i=0;i<CHout_div_Tout;i++)
        for(int j=0;j<WT_scale_group_nums;j++)
            for(int k=0;k<Tout/HBM_Port;k++) {
                int wt_start_ch_in=j*WT_CH_Tgroup;
                int wt_end_ch_in=(j==WT_scale_group_nums-1)?WT_CHin_Padding_with_Tin:(j+1)*WT_CH_Tgroup;
                for(int n=wt_bit*wt_start_ch_in/32;n<wt_bit*wt_end_ch_in/32;n++)
                    require_bytes ++;
            }
    
	ret->payload_size = require_bytes*4;

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
