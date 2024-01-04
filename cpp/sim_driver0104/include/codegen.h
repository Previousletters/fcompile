#ifndef CODEGEN_H
#define CODEGEN_H

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <windows.h>
typedef int HANDLE;

// <if_read, <addr, wdata>>
typedef std::vector<std::pair<int, std::pair<int, int>>> regops_t;
typedef std::vector<std::pair<std::string, std::pair<int, int>>> addr_t;

static std::vector<regops_t> csbs;
static regops_t regop;
static int ignored[2] = { 64, 192 };
extern addr_t node_inputs;
extern addr_t node_outputs;

template<typename T>
T InputAddr(T data, const std::string& name) {
	std::pair<int, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_inputs.push_back(std::make_pair(name, data_addr));
	return data;
};
template<typename T>
T OutputAddr(T data, const std::string& name) {
	std::pair<int, int> data_addr = std::make_pair((uint64_t)data->payload, (int)data->payload_size);
	node_outputs.push_back(std::make_pair(name, data_addr));
	return data;
};

void CSB_Finish();
void CSB_Clear();
void Reg_Write(HANDLE user_device, int addr, int data);
void Reg_Read(HANDLE user_device, int addr, int data);
void CSB_Save_Model(std::ofstream& source, const std::string& prefix = "test", const std::string& tab = "    ");
void CSB_Save_Model(const std::string& file, const std::string& prefix = "test", const std::string& tab = "    ");
void CSB_Save_Model_Burst(const std::string& file, const std::string& prefix = "test", const std::string& tab = "    ");
void Addr_Save_Model(std::ofstream& source, const std::string& prefix = "", const std::string& tab = "    ");

#endif
