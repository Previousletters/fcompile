#include "lib.h"
#include <iostream>

void CSB_Write(HANDLE &device, int addr, int data) {
    device.push_back(std::make_pair(addr, data));
}

int CSB_Read(HANDLE &device, int addr) {
    device.push_back(std::make_pair(addr, 1));
    return 1;
}
