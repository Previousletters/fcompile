#ifndef __EXPORT_CSB_JIT__
#define __EXPORT_CSB_JIT__

#include <map>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <iomanip>
#include <utility>

#include <svir/base.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>

#include <driver/basic.h>
#include "utils.h"

using namespace svir;

namespace backend {

class CSBCodeGen {
  public:
    CSBCodeGen(
        const std::vector<hbm::regops_t>& csb_regs,
        const std::vector<PtrInfo>& var_ddr,
        const std::vector<PtrInfo>& var_hbm,
        const std::vector<PtrInfo>& const_ddr,
        const std::vector<PtrInfo>& const_hbm,
        const std::vector<PtrInfo>& outputs
    ) : csb_regs(csb_regs), var_ddr(var_ddr), var_hbm(var_hbm),
        const_ddr(const_ddr), const_hbm(const_hbm), outputs(outputs) {}

    std::string JITSource(const std::string& call_name) {
        time_t now = time(0);
        char* dt = ctime(&now);
        source << "// auto generated at " << dt << std::endl;
        GenVars();
        GenConst();
        GenOutputs();
        GenCSB(call_name);
        return source.str();
    }

    void GenVars() {
        if (var_ddr.size()) {
            source << "// inputs address and size for DDR" << std::endl;
            for (auto& it : var_ddr) {
                source << "uint64_t " << it.name << "_addr = " << "0x" << std::hex 
                       << std::setw(9) << std::setfill('0')<< it.addr << std::dec << ";\n";
                source << "int " << it.name << "_size = " << it.size << ";" << std::endl; 
            }
            source << std::endl;
        }
        if (var_hbm.size()) {
            source << "// inputs address and size for HBM" << std::endl;
            for (auto& it : var_hbm) {
                source << "uint64_t " << it.name << "_addr = " << "0x" << std::hex 
                       << std::setw(9) << std::setfill('0')<< it.addr << std::dec << ";\n";
                source << "int " << it.name << "_size = " << it.size << ";" << std::endl; 
            }
            source << std::endl;
        }
    }

    void GenConst() {
        std::stringstream source_;
        if (const_ddr.size()) {
            uint64_t ddr_base = const_ddr[0].addr;
            uint64_t ddr_size = 0;
            source_ << "// DEBUG: each weight address and size for DDR" << std::endl;
            for (auto& it : const_ddr) {
                source_ << "uint64_t " << it.name << "_addr = " << "0x" << std::hex 
                        << std::setw(9) << std::setfill('0')<< it.addr << std::dec << ";\n";
                source_ << "uint64_t " << it.name << "_size = " << it.size << ";" << std::endl; 
                if (ddr_base + ddr_size == it.addr) {
                    ddr_size += it.size;
                } else {
                    ddr_size = 0;
                }
            }
            source_ << std::endl;
            if (ddr_size) {
                source << "// wrapper weights address and size for DDR" << std::endl;
                source << "uint64_t " << "const_ddr_addr = " << "0x" << std::hex 
                       << std::setw(9) << std::setfill('0')<< ddr_base << std::dec << ";\n";
                source << "uint64_t " << "const_ddr_size = " << ddr_size << ";" << std::endl; 
                source << std::endl;
            }
        }
        if (const_hbm.size()) {
            uint64_t hbm_base = const_hbm[0].addr;
            uint64_t hbm_size = 0;
            source_ << "// DEBUG: each weight address and size for HBM" << std::endl;
            for (auto& it : const_hbm) {
                source_ << "uint64_t " << it.name << "_addr = " << "0x" << std::hex 
                        << std::setw(9) << std::setfill('0')<< it.addr << std::dec << ";\n";
                source_ << "uint64_t " << it.name << "_size = " << it.size << ";" << std::endl; 
                if (hbm_base + hbm_size == it.addr) {
                    hbm_size += it.size;
                } else {
                    hbm_size = 0;
                }
            }
            source_ << std::endl;
            if (hbm_size) {
                source << "// wrapper weights address and size for HBM" << std::endl;
                source << "uint64_t " << "const_hbm_addr = " << "0x" << std::hex 
                       << std::setw(9) << std::setfill('0')<< hbm_base << std::dec << ";\n";
                source << "uint64_t " << "const_hbm_size = " << hbm_size << ";" << std::endl; 
                source << std::endl;
            }
        }
        source << source_.str();
    }

    void GenOutputs() {
        if (outputs.size()) {
            source << "// outputs address and size for DDR" << std::endl;
            for (auto& it : outputs) {
                source << "uint64_t " << it.name << "_addr = " << "0x" << std::hex 
                       << std::setw(9) << std::setfill('0')<< it.addr << std::dec << ";\n";
                source << "int " << it.name << "_size = " << it.size << ";" << std::endl; 
            }
            source << std::endl;
        }
    }

    void GenCSB(const std::string& call_name) {
        source << "void " << call_name << "(HANDLE device) {" << std::endl;
        source <<
                R"CODE(#ifdef REGS_DEBUG
LARGE_INTEGER start_cfg;
LARGE_INTEGER stop_cfg;
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
double time_sec1;
QueryPerformanceFrequency(&freq);
#endif
)CODE";
        for (auto& csbs : csb_regs) {
            int start = 1;
            for (auto& n : csbs) {
                if (n.first) {
                    if (start) {
                        source <<
                            R"CODE(#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
)CODE";
                        start = 0;
                    }
                    source << tab << "CSB_Write(device, " << n.second.first << ", " << n.second.second << ");\n";
                }
                else {
                    start = 1;
                    source <<
                        R"CODE(#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
)CODE";
                    source << tab << "while( CSB_Read(device, " << n.second.first << ") != " << n.second.second << " ) {}\n";
                    source <<
                        R"CODE(#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("cfg reg time = %fs \n",time_sec0);
printf("run time     = %fs \n",time_sec1);
#endif
)CODE";
                }
            }
        }
        source << "}" << std::endl;
    }

  private:
    const std::string tab = "  ";
    std::stringstream source;
    const std::vector<hbm::regops_t>& csb_regs;
    const std::vector<PtrInfo>& outputs;
    const std::vector<PtrInfo>& var_ddr;
    const std::vector<PtrInfo>& var_hbm;
    const std::vector<PtrInfo>& const_ddr;
    const std::vector<PtrInfo>& const_hbm;
};

};

#endif
