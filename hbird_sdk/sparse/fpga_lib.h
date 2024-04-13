#ifndef __FPGA_LIB_H__
#define __FPGA_LIB_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint64_t HANDLE;

void CSB_Write(HANDLE, int, int);

int CSB_Read(HANDLE, int);

#endif
