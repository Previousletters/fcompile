// generated at Fri Mar  1 15:30:04 2024

// inputs payload and payload_size
uint64_t mvm_data_k_payload = 0x200000000;
int mvm_data_k_payload_size = 155648;

// ddrs payload and payload_size
uint64_t k_bn_bias_payload = 0x200026000;
int k_bn_bias_payload_size = 512;

// hbms payload and payload_size
uint64_t k_weight_payload = 0x000000000;
int k_weight_payload_size = 8448;

void MVM_BN_4096x128 (HANDLE device) {
#ifdef REGS_DEBUG
LARGE_INTEGER start_cfg;
LARGE_INTEGER stop_cfg;
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
double time_sec1;
QueryPerformanceFrequency(&freq);
#endif
    int cfg_reg_0[31] = {4096, 19, 1, 19, 1, 128, 128, 19, 0, 0, 16896, 156160, 0, 28, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 155648, 0, 0, 1216, 1216, 1216, 1216, };
    set_user_device_base_addr(device, cnn_base_addr + 8);// 2
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
    user_device_write_brust(device, 124, (BYTE*)cfg_reg_0);
    CSB_Write(device, 33, 799);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
    while( CSB_Read(device, 1) != 1 ) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("cfg reg time = %fs \n",time_sec0);
printf("run time     = %fs \n",time_sec1);
#endif
}
