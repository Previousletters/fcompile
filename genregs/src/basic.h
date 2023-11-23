#pragma once

#ifndef SRC_BASIC_H_
#define SRC_BASIC_H_

#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <time.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include <math.h>
#include "TOP_defines.h"

//#define EXPORT_DLL
#ifdef EXPORT_DLL
#define HBM_DLL __declspec(dllexport)
#else
#define HBM_DLL extern "C"
#endif
typedef int HANDLE;
#define FPGA_NULL ((void *)0xFFFFFFFF)
#define CNN_IP_BASE_ADDR 0x100000
#define Run_on_FPGA
//#define log2_WT_base_addr_Bank_Step (28)

typedef std::vector<std::pair<int, std::pair<int, int>>> regops_t;

struct Mapped_Feature
{
	short* payload;
	int payload_size;
	int surface_stride;
	int line_stride;
	int scale;
	int conv_out_scale;
	int height;
	int width;
	int channel;
	int dat_bit;
};

struct Mapped_Weight
{
	short* payload;
	int payload_size;
	int scale;
	int Ky;
	int Kx;
	int in_ch;
	int out_ch;
	int wt_bit;
};

/////////////////////////////////
void CSB_Finish();

void CSB_Write(HANDLE user_device, int addr, int data);

void CSB_Read(HANDLE user_device, int addr, int data);

void CSB_Save_Head(const std::string& file, const std::string& prefix="test");

void CSB_Save_Model(const std::string& file, const std::string& prefix="test", const std::string& tab = "    ");

void CSB_test(int test[][3]);

void CSB_Clear();

HBM_DLL struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit);
HBM_DLL struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit);
void Free_Feature(struct Mapped_Feature* feature);
void Free_Weight(struct Mapped_Weight* weight);
HBM_DLL int FP32_to_FP20(float fp32_i);

#endif /* SRC_BASIC_H_ */
