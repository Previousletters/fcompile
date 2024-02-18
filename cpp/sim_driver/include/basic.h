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
#include <numeric> 
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
    int life_cycle;
	friend std::ostream& operator<< (std::ostream& out, struct Mapped_Feature* A) {
		out << "Mapped_Feature(" << A->height << ", " << A->width << ", " << A->channel << ", 0x";
        out << std::hex << std::setw(9) << std::setfill('0')<< (uint64_t)A->payload << std::dec << ", ";
        out << A->payload_size << ");";
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
    int life_cycle;
	friend std::ostream& operator<< (std::ostream& out, struct Mapped_Weight* A) {
		out << "Mapped_Weight(" << A->Ky << ", " << A->Kx << ", " << A->in_ch << ", " << A->out_ch << ", 0x";
        out << std::hex << std::setw(9) << std::setfill('0')<< (uint64_t)A->payload << std::dec << ", ";
        out << A->payload_size << ");";
		return out;
	}
};

/////////////////////////////////
void CSB_Write(HANDLE user_device, int addr, int data);
void CSB_Read(HANDLE user_device, int addr, int data);
int CSB_Read(HANDLE user_device, int addr);

struct Mapped_Feature* InputAddr(struct Mapped_Feature*, const std::string&);
struct Mapped_Feature* WeightAddr(struct Mapped_Feature*, const std::string&);
struct Mapped_Weight* WeightAddr(struct Mapped_Weight*, const std::string&);
struct Mapped_Feature* OutputAddr(struct Mapped_Feature*, const std::string&);
void WeightAddrAnnotate(const std::string&);

struct Mapped_Feature* InputAddr(struct Mapped_Feature*, const std::string&, const std::string&);
struct Mapped_Weight* InputAddr(struct Mapped_Weight*, const std::string&, const std::string&);
struct Mapped_Feature* OutputAddr(struct Mapped_Feature*, const std::string&, const std::string&);

struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit, int life_cycle=0, int width_padding=0);
struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit, int life_cycle=0);
void Free_Feature(struct Mapped_Feature* feature);
void Free_Weight(struct Mapped_Weight* weight);
void Reshape(struct Mapped_Feature* feature, int height, int width, int chin);
void Reset(struct Mapped_Feature* feature, int height, int width, int chin);
void Reset(struct Mapped_Weight* weight, int in_ch, int out_ch);
std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, int axis);
std::vector<struct Mapped_Feature*> Split(struct Mapped_Feature* feature, const std::vector<int>& new_chs);

void AutoFree(struct Mapped_Feature*);
void AutoFree(struct Mapped_Weight*);
void ObjCheck(struct Mapped_Feature*);
void ObjCheck(struct Mapped_Weight*);

#endif /* SRC_BASIC_H_ */
