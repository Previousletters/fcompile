#ifndef CODEGEN_H
#define CODEGEN_H

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
//#include <windows.h>
typedef int HANDLE;

#define DDR_BASE_ADDRESS 0x200000000

// <if_read, <addr, wdata>>
struct get_inf_t {
    uint64_t addr;
    int size;
    std::string fname;
};
typedef std::vector<std::pair<int, std::pair<int, int>>> regops_t;
typedef std::vector<std::pair<std::string, std::pair<uint64_t, int>>> addr_t;
typedef std::vector<std::pair<std::string, get_inf_t>> attr_t;

static std::vector<regops_t> csbs;
static regops_t regop;
static int ignored[2] = { 33, 214 };
extern addr_t node_ddrs;
extern addr_t node_hbms;
extern addr_t node_inputs;
extern addr_t node_outputs;

extern attr_t attr_inputs;
extern attr_t attr_weights;
extern attr_t attr_outputs;

void CSB_Finish();
void CSB_Clear();
void Reg_Write(HANDLE user_device, int addr, int data);
void Reg_Read(HANDLE user_device, int addr, int data);
void CSB_Save_Model(std::ofstream& source, const std::string& prefix = "test", const std::string& tab = "    ");
void CSB_Save_Model(const std::string& file, const std::string& prefix = "test", const std::string& tab = "    ");
void CSB_Save_Model_Burst(std::ofstream& source, const std::string& prefix = "test", const std::string& tab = "    ");
void Addr_Save_Model(std::ofstream& source, const std::string& prefix = "", const std::string& tab = "    ");
void Attr_Save_Model(std::ofstream& source, const std::string& prefix = "", const std::string& tab = "    ");

#endif
