#include <iostream>
#include <fstream>
#include <iomanip>
#include "basic.h"
#include "TOP_defines.h"
#include "FPGA_DDR.h"
using namespace std;

///// function for hardware//////////

HBM_DLL struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit)
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

HBM_DLL struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit)
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

	int require_bytes = ((base_Tin * wt_bit) / 8) * Kx * Ky * out_ch * ((in_ch + base_Tin - 1) / base_Tin);
	ret->payload_size = require_bytes;

	ret->payload = (short*)FPGA_DDR_malloc(require_bytes);
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
    if (weight->payload != FPGA_NULL) FPGA_DDR_free(weight->payload);
    free(weight);
}

HBM_DLL int FP32_to_FP20(float fp32_i)
{
    int fp32_i_s, fp32_i_e, fp32_i_f;
    int fp20_o_s, fp20_o_e, fp20_o_m_tmp, fp20_o_m;
    int overflow_tmp, underflow_tmp;

    fp32_i_s = 1 & (*((int*)&fp32_i) >> 31); // {1{1'b1}}
    fp32_i_e = 255 & (*((int*)&fp32_i) >> 23); // {8{1'b1}}
    fp32_i_f = 8388607 & (*((int*)&fp32_i)); // {23{1'b1}}

    if (fp32_i_e < 84) {
        fp20_o_s = 0;
        fp20_o_e = 0;
        fp20_o_m = 0;
        overflow_tmp = 0;
        underflow_tmp = 1;
    }
    else if (fp32_i_e >= 159) {
        fp20_o_s = fp32_i_s;
        fp20_o_e = 0b111110;
        fp20_o_m = 0b1111111111111;
        overflow_tmp = 1;
        underflow_tmp = 0;
    }
    else {
        fp20_o_s = fp32_i_s;
        overflow_tmp = 0;
        underflow_tmp = 0;
        if (fp32_i_e >= 97) {
            fp20_o_e = fp32_i_e - 96;
            int c_tmp = 1 & (fp32_i_f >> 9);
            if (c_tmp)
                fp20_o_m_tmp = fp32_i_f + 0b10000000000;
            else
                fp20_o_m_tmp = fp32_i_f;
            fp20_o_m = ((1 << 13) - 1) & (fp20_o_m_tmp >> 10);
        }
        else {
            fp20_o_e = 0;
            int r_cnt = 97 - fp32_i_e;
            int i_m_tmp0 = (1 << 23) + fp32_i_f;
            int c_tmp = 1 & (i_m_tmp0 >> (9 + r_cnt));
            if (c_tmp) {
                int i_m_tmp1 = (i_m_tmp0 >> r_cnt) + 0b10000000000;
                fp20_o_m = ((1 << 13) - 1) & (i_m_tmp1 >> 10);
            }
            else {
                int i_m_tmp1 = (i_m_tmp0 >> r_cnt);
                fp20_o_m = ((1 << 13) - 1) & (i_m_tmp1 >> 10);
            }
        }
    }

    return (fp20_o_s << 19) + (fp20_o_e << 13) + fp20_o_m;
}

std::vector<regops_t> csbs;
regops_t regop;

void CSB_Finish() {
    csbs.push_back(regop);
    regop.clear();
}

void CSB_Write(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(1, std::make_pair(addr, data)));
}

void CSB_Read(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(0, std::make_pair(addr, data)));
}

void CSB_Save_Head(const std::string& file, const std::string& prefix) {
    std::ofstream source(file, std::ios::app);
    for (int i = 0; i < csbs.size(); i++) {
        source << "int " << prefix << i << "_cycle = " << csbs[i].size() << ";\n";
        source << "int " << prefix << i << "_regs[" << csbs[i].size() << "][" << 3 << "] = { ";
        for (auto& n : csbs[i]) {
            source << "{ ";
            source << n.first << ", " << n.second.first << ", " << n.second.second;
            source << " }, ";
        }
    }
    source << " };\n";
    source.close();
}

void CSB_Save_Model(const std::string& file, const std::string& prefix, const std::string& tab) {
    std::ofstream source(file, std::ios::app);
	time_t now = time(0);
	char* dt = ctime(&now);
	source << "// generated at " << dt;
    for (int i = 0; i < csbs.size(); i++) {
		source << std::endl;
        source << "void " << prefix << i << "_op (HANDLE device) {\n";
		for (auto& n : csbs[i]) {
			if (n.first) {
				source << tab << "CSB_Write(device, " << n.second.first << ", " << n.second.second << ");\n";
			}
			else {
				source << tab << "while( CSB_Read(device, " << n.second.first << ") != " << n.second.second << " ) {}\n";
			}
        }
		source << " }\n";
    }
    source.close();
}

void CSB_test(int test[][3]) {
    for (int i = 0; i < csbs[0].size(); i++) {
        if (test[i][1] != csbs[0][i].second.first || test[i][2] != csbs[0][i].second.second) {
            printf("Wrong!\n");
        }
    }
}

void CSB_Clear() {
    csbs.clear();
}
