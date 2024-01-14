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
//#include <windows.h>
typedef int HANDLE;

#include <math.h>
#include "TOP_defines.h"
#include "codegen.h"

//typedef int HANDLE;
#define FPGA_NULL ((void *)0xFFFFFFFF)
#define Run_on_FPGA
//#define log2_WT_base_addr_Bank_Step (28)

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
	friend std::ostream& operator<< (std::ostream& out, struct Mapped_Feature* A) {
		out << "Mapped_Feature(" << A->height << ", " << A->width << ", " << A->channel << ");";
		return out;
	}
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
	friend std::ostream& operator<< (std::ostream& out, struct Mapped_Weight* A) {
		out << "Mapped_Weight(" << A->Ky << ", " << A->Kx << ", " << A->in_ch << ", " << A->out_ch << ");";
		return out;
	}
};

/////////////////////////////////
void CSB_Write(HANDLE user_device, int addr, int data);
void CSB_Read(HANDLE user_device, int addr, int data);
int CSB_Read(HANDLE user_device, int addr);

struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit);
struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit);
void Free_Feature(struct Mapped_Feature* feature);
void Free_Weight(struct Mapped_Weight* weight);
void Reshape(struct Mapped_Feature* feature, int height, int width, int chin);
std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, int axis);
struct Mapped_Feature* Merge(std::vector<struct Mapped_Feature*> features);

#endif /* SRC_BASIC_H_ */
