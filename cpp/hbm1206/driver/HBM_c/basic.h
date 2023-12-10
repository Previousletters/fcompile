#pragma once

#ifndef SRC_BASIC_H_
#define SRC_BASIC_H_

#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>

typedef uint64_t HANDLE;

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

void CSB_Write(HANDLE device, int addr, int data);

int CSB_Read(HANDLE device, int addr);

#endif /* SRC_BASIC_H_ */
