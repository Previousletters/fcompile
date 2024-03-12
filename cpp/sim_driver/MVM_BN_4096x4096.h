// generated at Wed Feb 28 15:58:47 2024

// inputs payload and payload_size
uint64_t mvm_data_payload = 0x200000000;
int mvm_data_payload_size = 155648;

// ddrs payload and payload_size
uint64_t q_bn_bias_payload = 0x200026000;
int q_bn_bias_payload_size = 16384;

// hbms payload and payload_size
uint64_t q_weight_payload = 0x000000000;
int q_weight_payload_size = 270336;

void MVM_BN_4096x4096 (HANDLE device) {
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
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
    CSB_Write(device, 2, 4096);
    CSB_Write(device, 3, 19);
    CSB_Write(device, 4, 1);
    CSB_Write(device, 5, 19);
    CSB_Write(device, 6, 1);
    CSB_Write(device, 7, 512);
    CSB_Write(device, 8, 512);
    CSB_Write(device, 9, 19);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 0);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 172032);
    CSB_Write(device, 14, 7);
    CSB_Write(device, 15, 28);
    CSB_Write(device, 16, 0);
    CSB_Write(device, 17, 0);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 0);
    CSB_Write(device, 20, 0);
    CSB_Write(device, 21, 1);
    CSB_Write(device, 22, 1);
    CSB_Write(device, 23, 1);
    CSB_Write(device, 24, 1);
    CSB_Write(device, 25, 0);
    CSB_Write(device, 26, 155648);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 1216);
    CSB_Write(device, 30, 1216);
    CSB_Write(device, 31, 1216);
    CSB_Write(device, 32, 1216);
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
printf("op 0 cfg reg time = %fs \n",time_sec0);
printf("op 0 run time     = %fs \n",time_sec1);
#endif
}
