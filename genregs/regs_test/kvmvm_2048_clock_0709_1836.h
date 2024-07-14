void step1 (HANDLE& device, int token) {
// accel.hbm.mvm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_cfg;
LARGE_INTEGER stop_cfg;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0 = 0;
double time_sec1 = 0;
QueryPerformanceFrequency(&freq);
for (int i = 0; i < 1000; i++) {
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 2, 128);
  CSB_Write(device, 3, 16);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 16);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, ((((token + 32) - 1) / 32) * 32));
  CSB_Write(device, 8, (((((token + 32) - 1) / 32) * 32) % ((((token + 32) - 1) / 32) * 32)));
  CSB_Write(device, 9, 16);
  CSB_Write(device, 10, 0);
  CSB_Write(device, 11, 0);
  CSB_Write(device, 12, ((((((((((token + 32) - 1) / 32) * 32) * 128) * 4) >> 3) + (((((((token + 32) - 1) / 32) * 32) * 256) * 1) >> 3)) / ((((token + 32) - 1) / 32) * 32)) * 8));
  CSB_Write(device, 13, 4096);
  CSB_Write(device, 14, 0);
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
  CSB_Write(device, 26, 0);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1024);
  CSB_Write(device, 30, 1024);
  CSB_Write(device, 31, 1024);
  CSB_Write(device, 32, 1024);
  CSB_Write(device, 33, 287);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
#endif
#ifdef PRINT_STEP
printf("start: step1!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 += (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 += (unsigned long long)(stop_run.QuadPart - stop_cfg.QuadPart) / (double)freq.QuadPart;
}
fprintf(LOG, "step1,accel.hbm.mvm,%f,%f,%f,%f\n", time_sec0, time_sec1, time_sec0/double(1000.0), time_sec1/double(1000.0));
#endif
}

void step2 (HANDLE& device, int token) {
// accel.hbm.mvm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_cfg;
LARGE_INTEGER stop_cfg;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0 = 0;
double time_sec1 = 0;
QueryPerformanceFrequency(&freq);
for (int i = 0; i < 1000; i++) {
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 2, ((((token + 32) - 1) / 32) * 32));
  CSB_Write(device, 3, 16);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 16);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, 16);
  CSB_Write(device, 10, 4096);
  CSB_Write(device, 11, 6144);
  CSB_Write(device, 12, ((((((128 * ((((token + 128) - 1) / 128) * 128)) * 4) >> 3) + ((32768 * (((((((token + 128) - 1) / 128) * 128) + 2048) - 1) / 2048)) >> 3)) / 128) * 8));
  CSB_Write(device, 13, 0);
  CSB_Write(device, 14, 0);
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
  CSB_Write(device, 26, 0);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1024);
  CSB_Write(device, 30, 1024);
  CSB_Write(device, 31, 1024);
  CSB_Write(device, 32, 1024);
  CSB_Write(device, 33, 287);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
#endif
#ifdef PRINT_STEP
printf("start: step2!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 += (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 += (unsigned long long)(stop_run.QuadPart - stop_cfg.QuadPart) / (double)freq.QuadPart;
}
fprintf(LOG, "step2,accel.hbm.mvm,%f,%f,%f,%f\n", time_sec0, time_sec1, time_sec0/double(1000.0), time_sec1/double(1000.0));
#endif
}


void kvmvm_2048_clock_0709_1836(HANDLE& device, int token) {
  step1(device, token);
  step2(device, token);
}
