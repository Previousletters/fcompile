#include <iostream>
#include <fstream>
#include <iomanip>
#include "basic.h"

void CSB_Write(HANDLE device, int addr, int data) {
    printf("csb_cpp.append([%d, %d])\n", addr, data);
}

int CSB_Read(HANDLE device, int addr) {
    return 1;
};
