#include "fpga_lib.h"

void CSB_Write(HANDLE device, int addr, int data) {
    ((int*)device)[addr] = data;
}

int CSB_Read(HANDLE device, int addr) {
    return ((int*)device)[addr];
}