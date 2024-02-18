#include <iostream>
#include <fstream>
#include <iomanip>
#include "basic.h"
#include "TOP_defines.h"
#include "FPGA_MEM.h"
using namespace std;

///// function for hardware//////////

struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit, int life_cycle, int width_padding)
{
	struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
	if (ret == NULL)
	{
		return NULL;
	}

    ret->life_cycle = life_cycle;
	ret->scale = scale;
	ret->height = height;
	ret->width = width;
	ret->channel = ch;
	ret->dat_bit = dat_bit;
	ret->conv_out_scale = conv_out_scale;

    if (dat_bit != 16 && dat_bit !=8)
        std::cerr << "Check error bit width!" << std::endl;

    if (width_padding)
        width = ((width + AXI_BURST_LEN_SOFTMAX - 1) / AXI_BURST_LEN_SOFTMAX) * AXI_BURST_LEN_SOFTMAX;
	ret->line_stride = width * (dat_bit / 8) * Tout;
	ret->surface_stride = ret->line_stride * height;

	int require_bytes = ret->surface_stride * ((ch + Tout - 1) / Tout);
	ret->payload_size = require_bytes;

    ret->payload = (short*)FPGA_DDR_malloc(require_bytes);
    if (ret->payload == FPGA_NULL)
    {
        free(ret);
        std::cout << "Cannot malloc DDR space! ERROR!" << std::endl;
        exit(-1);
    }
	//printf("**** feature->payload=%d\n", ret->payload);
	return ret;
}

struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit, int life_cycle)
{
	struct Mapped_Weight* ret = (struct Mapped_Weight*)malloc(sizeof(struct Mapped_Weight));
	if (ret == NULL)
		return NULL;

    ret->life_cycle = life_cycle;
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
    if (feature->payload != FPGA_NULL) FPGA_DDR_free(feature->payload, feature->payload_size);
    feature->payload_size=0;
}

void Free_Weight(struct Mapped_Weight* weight)
{
    if (weight->payload != FPGA_NULL) FPGA_HBM_free(weight->payload);
    weight->payload_size=0;
}

void Reshape(struct Mapped_Feature* feature, int height, int width, int chin)
{
    ObjCheck(feature);
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
        std::cout << feature << " -> (" << height << ", " << width << ", " << chin << ")\n";
		std::cout << "Reshape Error!" << std::endl;
		exit(-1);
	}
}

void Reset(struct Mapped_Feature* feature, int height, int width, int chin)
{
    ObjCheck(feature);
	int new_line_stride = width * (MAX_DAT_DW / 8) * Tout;
	int new_surface_stride = new_line_stride * height;

	int new_require_bytes = new_surface_stride * ((chin + Tout - 1) / Tout);
	if (new_require_bytes <= feature->payload_size) {
		feature->height = height;
		feature->width = width;
		feature->channel = chin;
		feature->line_stride = new_line_stride;
		feature->surface_stride = new_surface_stride;
	}
	else {
        std::cout << feature << " -> (" << height << ", " << width << ", " << chin << ")\n";
		std::cout << "Reset Error!" << std::endl;
		std::cout << new_require_bytes << " " << feature->payload_size << std::endl;
		exit(-1);
	}
}

void Reset(struct Mapped_Weight* weight, int in_ch, int out_ch)
{
    int wt_bit=4;
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
    
	int new_require_bytes = require_bytes*4;
	if (new_require_bytes <= weight->payload_size) {
        weight->in_ch = in_ch;
        weight->out_ch = out_ch;
	}
	else {
        std::cout << weight << " -> (" << in_ch << ", " << out_ch << ")\n";
		std::cout << "Reset Error!" << std::endl;
		exit(-1);
	}
}

std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, int axis) {
    ObjCheck(feature);
    struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
    if (ret == NULL)
    {
        exit(-1);
    }
    ret->life_cycle = feature->life_cycle;
    ret->scale = feature->scale;
    ret->height = feature->height;
    ret->width = feature->width;
    ret->channel = feature->channel;
    ret->dat_bit = feature->dat_bit;
    ret->conv_out_scale = feature->conv_out_scale;

    if (axis == 0) {
        ret->line_stride = ret->width * (MAX_DAT_DW / 8) * Tout;
        ret->surface_stride = ret->line_stride * ret->height;

        int require_bytes = ret->surface_stride * ((ret->channel + Tout - 1) / Tout);
        ret->payload_size = require_bytes;
        ret->payload = (short*)((uint64_t)feature->payload + ret->line_stride * 1 * ((ret->channel + Tout - 1) / Tout));
        return std::vector<struct Mapped_Feature*>({ feature, ret });
	}
	else if (axis == 2) {
		ret->channel = ret->channel / 2;
		feature->channel = ret->channel;
        ret->line_stride = ret->width * (MAX_DAT_DW / 8) * Tout;
        ret->surface_stride = ret->line_stride * ret->height;

        int require_bytes = ret->surface_stride * ((ret->channel + Tout - 1) / Tout);
        ret->payload_size = require_bytes;
        
        feature->line_stride = ret->line_stride;
        feature->surface_stride = ret->surface_stride;
        feature->payload_size = ret->payload_size;
        ret->payload = (short*)((uint64_t)feature->payload + feature->payload_size);
        return std::vector<struct Mapped_Feature*>({ feature, ret });
	} else {
        exit(-1);
        return {};
    }
}

std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, const std::vector<int>& new_chs) {
    ObjCheck(feature);
    std::vector<struct Mapped_Feature*> rets;
    int new_size = 0;
    uint64_t base_addr = (uint64_t)feature->payload;
    for (int i = 0; i < new_chs.size(); i++) {
        struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
        if (ret == NULL) {
            exit(-1);
        }
        ret->life_cycle = feature->life_cycle;
        ret->scale = feature->scale;
        ret->height = feature->height;
        ret->width = feature->width;
        ret->channel = new_chs[i];
        ret->dat_bit = feature->dat_bit;
        ret->conv_out_scale = feature->conv_out_scale;
        ret->line_stride = ret->width * (feature->dat_bit / 8) * Tout;
        ret->surface_stride = ret->line_stride * ret->height;

        int require_bytes = ret->surface_stride * ((ret->channel + Tout - 1) / Tout);
        ret->payload_size = require_bytes;
        ret->payload = (short*)base_addr;
        base_addr += ret->payload_size;
        new_size += require_bytes;
        rets.push_back(ret);
    }
    if (new_size > feature->payload_size) {
        std::cerr << "*Error* : Split前后数据大小发生了变化，原数据" << feature << ", 新channel：";
        for (const auto& i : new_chs) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
        exit(-1);
    }
    return rets;
}

void AutoFree(struct Mapped_Feature* feature) {
    if (feature->life_cycle == 1)
        Free_Feature(feature);
    else {
        feature->life_cycle -= 1;
    }
}

void AutoFree(struct Mapped_Weight* weight) {
    if (weight->life_cycle == 1)
        Free_Weight(weight);
    else {
        weight->life_cycle -= 1;
    }
}

void ObjCheck(struct Mapped_Feature* feature) {
    if (feature->payload_size==0) {
        std::cout << "*ERROR* : " << feature << " 生命周期结束，请检查" << std::endl;
        exit(-1);
    }
}

void ObjCheck(struct Mapped_Weight* weight) {
    if (weight->payload_size==0) {
        std::cout << "*ERROR* : " << weight << " 生命周期结束，请检查" << std::endl;
        exit(-1);
    }
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

struct Mapped_Feature* WeightAddr(struct Mapped_Feature* data, const std::string& name) {
	std::pair<uint64_t, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_ddrs.push_back(std::make_pair(name, data_addr));
	return data;
};

struct Mapped_Weight* WeightAddr(struct Mapped_Weight* data, const std::string& name) {
	std::pair<uint64_t, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_hbms.push_back(std::make_pair(name, data_addr));
	return data;
};

void WeightAddrAnnotate(const std::string& annotation) {
	std::pair<uint64_t, int> data_addr = std::make_pair(-1, -1);
	node_ddrs.push_back(std::make_pair(annotation, data_addr));
	node_hbms.push_back(std::make_pair(annotation, data_addr));
};


struct Mapped_Feature* InputAddr(struct Mapped_Feature* data, const std::string& name) {
	std::pair<uint64_t, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_inputs.push_back(std::make_pair(name, data_addr));
	return data;
};

struct Mapped_Feature* OutputAddr(struct Mapped_Feature* data, const std::string& name) {
	std::pair<uint64_t, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_outputs.push_back(std::make_pair(name, data_addr));
	return data;
};

struct Mapped_Feature* InputAddr(struct Mapped_Feature* data, const std::string& name, const std::string& fname) {
	get_inf_t data_addr = {(uint64_t)data->payload, (int)data->payload_size, fname};
	attr_inputs.push_back(std::make_pair(name, data_addr));
	return data;
};

struct Mapped_Weight* InputAddr(struct Mapped_Weight* data, const std::string& name, const std::string& fname) {
	get_inf_t data_addr = {(uint64_t)data->payload, (int)data->payload_size, fname};
	attr_weights.push_back(std::make_pair(name, data_addr));
	return data;
};

struct Mapped_Feature* OutputAddr(struct Mapped_Feature* data, const std::string& name, const std::string& fname) {
	get_inf_t data_addr = {(uint64_t)data->payload, (int)data->payload_size, fname};
	attr_outputs.push_back(std::make_pair(name, data_addr));
	return data;
};
