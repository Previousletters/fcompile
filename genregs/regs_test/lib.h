#ifndef __LIB_H__
#define __LIB_H__

#include <iostream>
#include <vector>

typedef std::vector<std::pair<int, int>> HANDLE;

void CSB_Write(HANDLE&, int, int);
int CSB_Read(HANDLE&, int);

#endif
