# include "codegen.h"

void CSB_Finish() {
    csbs.push_back(regop);
    regop.clear();
}

void Reg_Write(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(1, std::make_pair(addr, data)));
}

void Reg_Read(HANDLE user_device, int addr, int data) {
    regop.push_back(std::make_pair(0, std::make_pair(addr, data)));
}

void CSB_Save_Head(const std::string& file, const std::string& prefix) {
    std::ofstream source(file, std::ios::app);
    for (int i = 0; i < csbs.size(); i++) {
        source << "int " << prefix << i << "_cycle = " << csbs[i].size() << ";\n";
        source << "int " << prefix << i << "_regs[" << csbs[i].size() << "][" << 3 << "] = { ";
        for (auto& n : csbs[i]) {
            source << "{ ";
            source << n.first << ", " << n.second.first << ", " << n.second.second;
            source << " }, ";
        }
    }
    source << "};\n";
    source.close();
}

int Find_Max_Addr(regops_t& ops) {
    int result = 0;
    for (auto& op : ops) {
        if (op.second.first > result)
            result = op.second.first;
    }
    return result;
}

void CSB_Save_Model_Burst_Cfg(std::ofstream& source, regops_t& ops, const std::string& tab, int& id) {
    if (ops.size() == 1) {
        source <<
            R"CODE(#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
)CODE";
        source << tab << "CSB_Write(device, " << ops[0].second.first << ", " << ops[0].second.second << ");\n";
    }
    else if (ops.size() > 1) {
        int base = ops[0].second.first;
        int numb = Find_Max_Addr(ops) - base + 1;
        source << tab << "int cfg_reg_" << id << "[" << numb << "] = {";
        for (int i = 0; i < numb; i++) {
            int tp = 0;
            for (auto& op : ops) {
                if (op.second.first == base + i) {
                    tp = op.second.second;
                    break;
                }
            }
            source << tp << ", ";
        }
        source << "};\n";
        source << tab << "set_user_device_base_addr(device, cnn_base_addr + " << (int)(base << 2) << ");// " << base << "\n";
        source <<
            R"CODE(#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
)CODE";
        source << tab << "user_device_write_brust(device, " << numb * 4 << ", (BYTE*)cfg_reg_" << id << ");\n";
        id++;
    }
}

void CSB_Save_Model_Burst(std::ofstream& source, const std::string& prefix, const std::string& tab) {
    for (int i = 0; i < csbs.size(); i++) {
        source << "void " << prefix << " (HANDLE device) {\n";
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
        regops_t reg_cfg;
        int id = 0;
        for (auto n : csbs[i]) {
            if (n.first) {
                if (n.second.first == ignored[0] || n.second.first == ignored[1]) {
                    CSB_Save_Model_Burst_Cfg(source, reg_cfg, tab, id);
                    reg_cfg.clear();
                    source << tab << "CSB_Write(device, " << n.second.first << ", " << n.second.second << ");\n";
                }
                else {
                    reg_cfg.push_back(n);
                }
            }
            else {
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
        source << "}\n";
    }
}

void CSB_Save_Model(std::ofstream& source, const std::string& prefix, const std::string& tab) {
    for (int i = 0; i < csbs.size(); i++) {
        source << "void " << prefix << " (HANDLE device) {\n";
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
        int start = 1;
        int op_numb = 0;
        for (auto& n : csbs[i]) {
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
)CODE";
                source << "printf(\"op "<< op_numb << " cfg reg time = %fs \\n\",time_sec0);" << std::endl;
                source << "printf(\"op "<< op_numb << " run time     = %fs \\n\",time_sec1);" << std::endl;
                source << "#endif" << std::endl;
                op_numb++;
            }
        }
        source << "}\n";
    }
}

void CSB_Save_Model(const std::string& file, const std::string& prefix, const std::string& tab) {
    std::ofstream source(file);
    time_t now = time(0);
    char* dt = ctime(&now);
    source << "// generated at " << dt;
    for (int i = 0; i < csbs.size(); i++) {
        source << std::endl;
        source << "void " << prefix << i << "_op (HANDLE device) {\n";
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
        int start = 1;
        int op_numb = 0;
        for (auto& n : csbs[i]) {
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
)CODE";
                source << "printf(\"op "<< op_numb << " cfg reg time = %fs \\n\",time_sec0);" << std::endl;
                source << "printf(\"op "<< op_numb << " run time     = %fs \\n\",time_sec1);" << std::endl;
                source << "#endif" << std::endl;
                op_numb++;
            }
        }
        source << "}\n";
    }
    source.close();
}

addr_t node_ddrs;
addr_t node_hbms;
addr_t node_inputs;
addr_t node_outputs;

attr_t attr_inputs;
attr_t attr_weights;
attr_t attr_outputs;

extern unsigned int hbm_has_initialized;
extern unsigned int ddr_has_initialized;

void CSB_Clear() {
    hbm_has_initialized = 0;
    ddr_has_initialized = 0;
    csbs.clear();
    node_ddrs.clear();
    node_hbms.clear();
    node_inputs.clear();
    node_outputs.clear();
}

void Addr_Save_Model(std::ofstream& source, const std::string& prefix, const std::string& tab) {
    time_t now = time(0);
    char* dt = ctime(&now);
    source << "// generated at " << dt << std::endl;
    if (node_inputs.size()) {
        source << "// inputs payload and payload_size" << std::endl;
        std::cout << "Found " << node_inputs.size() << " inputs: ";
        for (auto& input : node_inputs) {
            std::cout << input.first << " ";
            source << "uint64_t " << prefix << input.first << "_payload" << " = " << "0x"
                   << std::hex << std::setw(9) << std::setfill('0')<< input.second.first << std::dec << ";" << std::endl;
            source << "int " << prefix << input.first << "_payload_size" << " = " << input.second.second << ";" << std::endl;
        }
        std::cout << std::endl;
        source << std::endl;
    }
    std::cout << std::endl;
    if (node_outputs.size()) {
        source << "// outputs payload and payload_size" << std::endl;
        std::cout << "Found " << node_outputs.size() << " outputs: ";
        for (auto& output : node_outputs) {
            std::cout << output.first << " ";
            source << "uint64_t " << prefix << output.first << "_payload" << " = " << "0x"
                   << std::hex << std::setw(9) << std::setfill('0')<< output.second.first << std::dec << ";" << std::endl;
            source << "int " << prefix << output.first << "_payload_size" << " = " << output.second.second << ";" << std::endl;
        }
        std::cout << std::endl;
        source << std::endl;
    }
    std::cout << std::endl;
    if (node_ddrs.size()) {
        source << "// ddrs payload and payload_size" << std::endl;
        std::cout << "Found " << node_ddrs.size() << " ddrs: ";
        for (auto& input : node_ddrs) {
            if (input.second.first == -1 && input.second.second == -1) {
                source << "// " << input.first << std::endl;
                continue;
            }
            std::cout << input.first << " ";
            source << "uint64_t " << prefix << input.first << "_payload" << " = " << "0x"
                   << std::hex << std::setw(9) << std::setfill('0')<< input.second.first << std::dec << ";" << std::endl;
            source << "int " << prefix << input.first << "_payload_size" << " = " << input.second.second << ";" << std::endl;
        }
        std::cout << std::endl;
        source << std::endl;
    }
    std::cout << std::endl;
    if (node_hbms.size()) {
        source << "// hbms payload and payload_size" << std::endl;
        std::cout << "Found " << node_hbms.size() << " hbms: ";
        for (auto& weight : node_hbms) {
            if (weight.second.first == -1 && weight.second.second == -1) {
                source << "// " << weight.first << std::endl;
                continue;
            }
            std::cout << weight.first << " ";
            source << "uint64_t " << prefix << weight.first << "_payload" << " = " << "0x"
                   << std::hex << std::setw(9) << std::setfill('0')<< weight.second.first << std::dec << ";" << std::endl;
            source << "int " << prefix << weight.first << "_payload_size" << " = " << weight.second.second << ";" << std::endl;
        }
        std::cout << std::endl;
        source << std::endl;
    }

}

void Attr_Save_Model(std::ofstream& source, const std::string& prefix, const std::string& tab) {
    time_t now = time(0);
    char* dt = ctime(&now);
    source << "// generated at " << dt << std::endl;
    source << "// inputs bin inf" << std::endl;
    std::cout << "Found " << attr_inputs.size() << " inputs: ";
    for (auto& input : attr_inputs) {
        std::cout << input.first << " ";
        source << "struct bin_inf* " << prefix << input.first << " = get_bin_inf(" << input.second.addr << ", " << input.second.size << ", \"" << input.second.fname << "\");" << std::endl;
    }
    std::cout << std::endl;
    source << std::endl;
    if (attr_outputs.size()) {
        source << "// outputs bin inf" << std::endl;
        std::cout << "Found " << attr_outputs.size() << " outputs: ";
        for (auto& output : attr_outputs) {
            std::cout << output.first << " ";
            source << "struct bin_inf* " << prefix << output.first << " = get_bin_inf(" << output.second.addr << ", " << output.second.size << ", \"" << output.second.fname << "\");" << std::endl;
        }
        std::cout << std::endl;
        source << std::endl;
    }

    source << "void write_input_DDR(HANDLE device) {" << std::endl;
    for (auto& input : attr_inputs) {
        source << tab << "h2cx_device_write_bin(device, " << prefix << input.first << ");" << std::endl;
    }
    source << "}\n" << std::endl;

    source << "void verify_input_DDR(HANDLE device) {" << std::endl;
    for (auto& input : attr_inputs) {
        source << tab << "verify_data_write(device, " << prefix << input.first << ");" << std::endl;
    }
    source << "}\n" << std::endl;

    source << "void read_output_DDR(HANDLE device) {" << std::endl;
    for (auto& output : attr_outputs) {
        source << tab << "c2hx_device_read_bin(device, " << prefix << output.first << ");" << std::endl;
    }
    source << "}\n" << std::endl;
    if (attr_weights.size()) {
        source << "// weights HBM" << std::endl;
        std::cout << "Found " << attr_weights.size() << " weights: ";
        source <<"void write_weight_HBM(HANDLE device, int group) {" << std::endl;
        source << tab << "char file_name[200]" << std::endl;
        source << tab << "for (int i = 0; i < group; i++) {" << std::endl;
        for (auto& weight : attr_weights) {
            std::cout << weight.first << " ";
            source << tab << tab << "sprintf(file_name, \"" << weight.second.fname <<"\", i);" << std::endl;
            source << tab << tab << "struct bin_inf* " << prefix << weight.first << " = get_bin_inf(" << weight.second.addr << ", " << weight.second.size << ", file_name);" << std::endl;
            source << tab << tab << "h2cx_device_write_bin(device, " << prefix << weight.first << ");" << std::endl;
        }
        source << tab << "}" << std::endl;
        std::cout << std::endl;
        source << "}\n" << std::endl;
        source <<"void verify_weight_HBM(HANDLE device, int group) {" << std::endl;
        source << tab << "char file_name[200]" << std::endl;
        source << tab << "for (int i = 0; i < group; i++) {" << std::endl;
        for (auto& weight : attr_weights) {
            source << tab << tab << "sprintf(file_name, \"" << weight.second.fname <<"\", i);" << std::endl;
            source << tab << tab << "struct bin_inf* " << prefix << weight.first << " = get_bin_inf(" << weight.second.addr << ", " << weight.second.size << ", file_name);" << std::endl;
            source << tab << tab << "verify_data_write(device, " << prefix << weight.first << ");" << std::endl;
        }
        source << tab << "}" << std::endl;
        source << "}\n" << std::endl;
    }
}
