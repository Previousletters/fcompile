
// mod init
void new_step1 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 40960);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step1!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step2 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 0);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (5736448 + ((token - 1) * 64)) : 5736448));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 57344);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step2!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step3 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (5736448 + ((token - 1) * 64)) : 5736448));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (15173632 + ((token - 1) * 64)) : 15173632));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step3!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step4 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 23562240);
  CSB_Write(device, 195, (kvcache ? (15173632 + ((token - 1) * 64)) : 15173632));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (24086528 + ((token - 1) * 64)) : 24086528));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step4!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step5 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (24086528 + ((token - 1) * 64)) : 24086528));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (32475136 + ((token - 1) * 64)) : 32475136));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step5!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step6 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 14649344);
  CSB_Write(device, 195, (kvcache ? (32475136 + ((token - 1) * 64)) : 32475136));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step6!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step7 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 304128);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1025738752);
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
  CSB_Write(device, 26, 75776);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step7!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step8 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 92160);
  CSB_Write(device, 195, 1025738752);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step8!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step9 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 1478400);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 163328);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step9!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step10 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step10!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step11 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 574464);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 108544);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step11!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step12 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 2382336);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 218112);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step12!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step13 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 234496);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step13!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step14 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache4 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1025738752);
  CSB_Write(device, 11, 3287552);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (40863744 + ((token - 1) * 64)) : 40863744));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 250880);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step14!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step15 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache5 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (40863744 + ((token - 1) * 64)) : 40863744));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (50300928 + ((token - 1) * 64)) : 50300928));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step15!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step16 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache6 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 58689536);
  CSB_Write(device, 195, (kvcache ? (50300928 + ((token - 1) * 64)) : 50300928));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (59213824 + ((token - 1) * 64)) : 59213824));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step16!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step17 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache7 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (59213824 + ((token - 1) * 64)) : 59213824));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (67602432 + ((token - 1) * 64)) : 67602432));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step17!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step18 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 49776640);
  CSB_Write(device, 195, (kvcache ? (67602432 + ((token - 1) * 64)) : 67602432));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step18!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step19 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1025738752);
  CSB_Write(device, 11, 3591680);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 269312);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step19!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step20 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 285696);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step20!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step21 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 4765952);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 356864);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step21!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step22 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step22!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step23 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 3862016);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 302080);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step23!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step24 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 5669888);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 411648);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step24!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step25 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 428032);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step25!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step26 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache8 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 6575104);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (75991040 + ((token - 1) * 64)) : 75991040));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 444416);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step26!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step27 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache9 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (75991040 + ((token - 1) * 64)) : 75991040));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (85428224 + ((token - 1) * 64)) : 85428224));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step27!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step28 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache10 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 93816832);
  CSB_Write(device, 195, (kvcache ? (85428224 + ((token - 1) * 64)) : 85428224));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (94341120 + ((token - 1) * 64)) : 94341120));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step28!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step29 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache11 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (94341120 + ((token - 1) * 64)) : 94341120));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (102729728 + ((token - 1) * 64)) : 102729728));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step29!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step30 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 84903936);
  CSB_Write(device, 195, (kvcache ? (102729728 + ((token - 1) * 64)) : 102729728));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step30!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step31 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 6879232);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 462848);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step31!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step32 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 479232);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step32!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step33 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 8053504);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 550400);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step33!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step34 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step34!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step35 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 7149568);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 495616);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step35!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step36 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 8957440);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 605184);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step36!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step37 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 621568);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step37!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step38 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache12 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 9862656);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (111118336 + ((token - 1) * 64)) : 111118336));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 637952);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step38!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step39 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache13 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (111118336 + ((token - 1) * 64)) : 111118336));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (120555520 + ((token - 1) * 64)) : 120555520));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step39!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step40 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache14 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 128944128);
  CSB_Write(device, 195, (kvcache ? (120555520 + ((token - 1) * 64)) : 120555520));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (129468416 + ((token - 1) * 64)) : 129468416));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step40!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step41 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache15 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (129468416 + ((token - 1) * 64)) : 129468416));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (137857024 + ((token - 1) * 64)) : 137857024));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step41!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step42 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 120031232);
  CSB_Write(device, 195, (kvcache ? (137857024 + ((token - 1) * 64)) : 137857024));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step42!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step43 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 10166784);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 656384);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step43!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step44 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 672768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step44!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step45 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 11341056);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 743936);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step45!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step46 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step46!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step47 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 10437120);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 689152);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step47!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step48 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 12244992);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 798720);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step48!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step49 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 815104);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step49!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step50 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache16 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 13150208);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (146245632 + ((token - 1) * 64)) : 146245632));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 831488);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step50!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step51 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache17 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (146245632 + ((token - 1) * 64)) : 146245632));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (155682816 + ((token - 1) * 64)) : 155682816));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step51!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step52 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache18 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 164071424);
  CSB_Write(device, 195, (kvcache ? (155682816 + ((token - 1) * 64)) : 155682816));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (164595712 + ((token - 1) * 64)) : 164595712));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step52!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step53 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache19 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (164595712 + ((token - 1) * 64)) : 164595712));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (172984320 + ((token - 1) * 64)) : 172984320));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step53!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step54 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 155158528);
  CSB_Write(device, 195, (kvcache ? (172984320 + ((token - 1) * 64)) : 172984320));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step54!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step55 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 13454336);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 849920);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step55!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step56 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 866304);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step56!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step57 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 14628608);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 937472);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step57!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step58 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step58!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step59 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 13724672);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 882688);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step59!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step60 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 15532544);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 992256);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step60!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step61 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1008640);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step61!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step62 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache20 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 16437760);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (181372928 + ((token - 1) * 64)) : 181372928));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1025024);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step62!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step63 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache21 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (181372928 + ((token - 1) * 64)) : 181372928));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (190810112 + ((token - 1) * 64)) : 190810112));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step63!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step64 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache22 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 199198720);
  CSB_Write(device, 195, (kvcache ? (190810112 + ((token - 1) * 64)) : 190810112));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (199723008 + ((token - 1) * 64)) : 199723008));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step64!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step65 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache23 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (199723008 + ((token - 1) * 64)) : 199723008));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (208111616 + ((token - 1) * 64)) : 208111616));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step65!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step66 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 190285824);
  CSB_Write(device, 195, (kvcache ? (208111616 + ((token - 1) * 64)) : 208111616));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step66!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step67 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 16741888);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 1043456);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step67!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step68 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1059840);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step68!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step69 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 17916160);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 1131008);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step69!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step70 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step70!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step71 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 17012224);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 1076224);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step71!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step72 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 18820096);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 1185792);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step72!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step73 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1202176);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step73!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step74 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache24 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 19725312);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (216500224 + ((token - 1) * 64)) : 216500224));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1218560);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step74!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step75 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache25 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (216500224 + ((token - 1) * 64)) : 216500224));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (225937408 + ((token - 1) * 64)) : 225937408));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step75!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step76 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache26 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 234326016);
  CSB_Write(device, 195, (kvcache ? (225937408 + ((token - 1) * 64)) : 225937408));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (234850304 + ((token - 1) * 64)) : 234850304));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step76!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step77 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache27 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (234850304 + ((token - 1) * 64)) : 234850304));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (243238912 + ((token - 1) * 64)) : 243238912));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step77!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step78 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 225413120);
  CSB_Write(device, 195, (kvcache ? (243238912 + ((token - 1) * 64)) : 243238912));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step78!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step79 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 20029440);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 1236992);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step79!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step80 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1253376);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step80!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step81 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 21203712);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 1324544);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step81!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step82 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step82!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step83 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 20299776);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 1269760);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step83!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step84 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 22107648);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 1379328);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step84!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step85 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1395712);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step85!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step86 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache28 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 23012864);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (251627520 + ((token - 1) * 64)) : 251627520));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1412096);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step86!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step87 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache29 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (251627520 + ((token - 1) * 64)) : 251627520));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (261064704 + ((token - 1) * 64)) : 261064704));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step87!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step88 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache30 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 269453312);
  CSB_Write(device, 195, (kvcache ? (261064704 + ((token - 1) * 64)) : 261064704));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (269977600 + ((token - 1) * 64)) : 269977600));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step88!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step89 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache31 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (269977600 + ((token - 1) * 64)) : 269977600));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (278366208 + ((token - 1) * 64)) : 278366208));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step89!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step90 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 260540416);
  CSB_Write(device, 195, (kvcache ? (278366208 + ((token - 1) * 64)) : 278366208));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step90!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step91 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 23316992);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 1430528);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step91!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step92 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1446912);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step92!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step93 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 24491264);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 1518080);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step93!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step94 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step94!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step95 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 23587328);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 1463296);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step95!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step96 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 25395200);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 1572864);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step96!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step97 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1589248);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step97!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step98 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache32 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 26300416);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (286754816 + ((token - 1) * 64)) : 286754816));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1605632);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step98!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step99 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache33 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (286754816 + ((token - 1) * 64)) : 286754816));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (296192000 + ((token - 1) * 64)) : 296192000));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step99!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step100 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache34 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 304580608);
  CSB_Write(device, 195, (kvcache ? (296192000 + ((token - 1) * 64)) : 296192000));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (305104896 + ((token - 1) * 64)) : 305104896));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step100!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step101 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache35 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (305104896 + ((token - 1) * 64)) : 305104896));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (313493504 + ((token - 1) * 64)) : 313493504));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step101!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step102 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 295667712);
  CSB_Write(device, 195, (kvcache ? (313493504 + ((token - 1) * 64)) : 313493504));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step102!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step103 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 26604544);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 1624064);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step103!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step104 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1640448);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step104!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step105 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 27778816);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 1711616);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step105!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step106 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step106!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step107 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 26874880);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 1656832);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step107!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step108 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 28682752);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 1766400);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step108!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step109 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1782784);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step109!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step110 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache36 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 29587968);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (321882112 + ((token - 1) * 64)) : 321882112));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1799168);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step110!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step111 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache37 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (321882112 + ((token - 1) * 64)) : 321882112));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (331319296 + ((token - 1) * 64)) : 331319296));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step111!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step112 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache38 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 339707904);
  CSB_Write(device, 195, (kvcache ? (331319296 + ((token - 1) * 64)) : 331319296));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (340232192 + ((token - 1) * 64)) : 340232192));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step112!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step113 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache39 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (340232192 + ((token - 1) * 64)) : 340232192));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (348620800 + ((token - 1) * 64)) : 348620800));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step113!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step114 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 330795008);
  CSB_Write(device, 195, (kvcache ? (348620800 + ((token - 1) * 64)) : 348620800));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step114!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step115 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 29892096);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 1817600);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step115!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step116 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1833984);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step116!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step117 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 31066368);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 1905152);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step117!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step118 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step118!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step119 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 30162432);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 1850368);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step119!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step120 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 31970304);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 1959936);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step120!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step121 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 1976320);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step121!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step122 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache40 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 32875520);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (357009408 + ((token - 1) * 64)) : 357009408));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 1992704);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step122!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step123 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache41 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (357009408 + ((token - 1) * 64)) : 357009408));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (366446592 + ((token - 1) * 64)) : 366446592));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step123!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step124 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache42 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 374835200);
  CSB_Write(device, 195, (kvcache ? (366446592 + ((token - 1) * 64)) : 366446592));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (375359488 + ((token - 1) * 64)) : 375359488));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step124!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step125 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache43 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (375359488 + ((token - 1) * 64)) : 375359488));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (383748096 + ((token - 1) * 64)) : 383748096));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step125!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step126 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 365922304);
  CSB_Write(device, 195, (kvcache ? (383748096 + ((token - 1) * 64)) : 383748096));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step126!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step127 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 33179648);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 2011136);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step127!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step128 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2027520);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step128!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step129 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 34353920);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 2098688);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step129!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step130 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step130!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step131 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 33449984);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 2043904);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step131!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step132 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 35257856);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 2153472);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step132!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step133 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2169856);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step133!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step134 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache44 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 36163072);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (392136704 + ((token - 1) * 64)) : 392136704));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 2186240);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step134!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step135 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache45 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (392136704 + ((token - 1) * 64)) : 392136704));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (401573888 + ((token - 1) * 64)) : 401573888));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step135!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step136 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache46 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 409962496);
  CSB_Write(device, 195, (kvcache ? (401573888 + ((token - 1) * 64)) : 401573888));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (410486784 + ((token - 1) * 64)) : 410486784));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step136!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step137 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache47 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (410486784 + ((token - 1) * 64)) : 410486784));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (418875392 + ((token - 1) * 64)) : 418875392));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step137!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step138 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 401049600);
  CSB_Write(device, 195, (kvcache ? (418875392 + ((token - 1) * 64)) : 418875392));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step138!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step139 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 36467200);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 2204672);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step139!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step140 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2221056);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step140!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step141 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 37641472);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 2292224);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step141!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step142 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step142!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step143 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 36737536);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 2237440);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step143!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step144 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 38545408);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 2347008);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step144!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step145 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2363392);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step145!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step146 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache48 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 39450624);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (427264000 + ((token - 1) * 64)) : 427264000));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 2379776);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step146!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step147 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache49 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (427264000 + ((token - 1) * 64)) : 427264000));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (436701184 + ((token - 1) * 64)) : 436701184));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step147!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step148 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache50 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 445089792);
  CSB_Write(device, 195, (kvcache ? (436701184 + ((token - 1) * 64)) : 436701184));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (445614080 + ((token - 1) * 64)) : 445614080));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step148!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step149 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache51 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (445614080 + ((token - 1) * 64)) : 445614080));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (454002688 + ((token - 1) * 64)) : 454002688));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step149!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step150 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 436176896);
  CSB_Write(device, 195, (kvcache ? (454002688 + ((token - 1) * 64)) : 454002688));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step150!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step151 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 39754752);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 2398208);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step151!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step152 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2414592);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step152!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step153 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 40929024);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 2485760);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step153!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step154 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step154!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step155 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 40025088);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 2430976);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step155!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step156 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 41832960);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 2540544);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step156!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step157 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2556928);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step157!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step158 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache52 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 42738176);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (462391296 + ((token - 1) * 64)) : 462391296));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 2573312);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step158!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step159 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache53 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (462391296 + ((token - 1) * 64)) : 462391296));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (471828480 + ((token - 1) * 64)) : 471828480));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step159!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step160 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache54 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 480217088);
  CSB_Write(device, 195, (kvcache ? (471828480 + ((token - 1) * 64)) : 471828480));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (480741376 + ((token - 1) * 64)) : 480741376));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step160!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step161 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache55 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (480741376 + ((token - 1) * 64)) : 480741376));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (489129984 + ((token - 1) * 64)) : 489129984));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step161!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step162 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 471304192);
  CSB_Write(device, 195, (kvcache ? (489129984 + ((token - 1) * 64)) : 489129984));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step162!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step163 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 43042304);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 2591744);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step163!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step164 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2608128);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step164!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step165 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 44216576);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 2679296);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step165!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step166 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step166!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step167 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 43312640);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 2624512);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step167!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step168 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 45120512);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 2734080);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step168!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step169 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2750464);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step169!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step170 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache56 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 46025728);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (497518592 + ((token - 1) * 64)) : 497518592));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 2766848);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step170!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step171 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache57 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (497518592 + ((token - 1) * 64)) : 497518592));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (506955776 + ((token - 1) * 64)) : 506955776));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step171!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step172 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache58 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 515344384);
  CSB_Write(device, 195, (kvcache ? (506955776 + ((token - 1) * 64)) : 506955776));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (515868672 + ((token - 1) * 64)) : 515868672));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step172!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step173 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache59 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (515868672 + ((token - 1) * 64)) : 515868672));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (524257280 + ((token - 1) * 64)) : 524257280));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step173!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step174 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 506431488);
  CSB_Write(device, 195, (kvcache ? (524257280 + ((token - 1) * 64)) : 524257280));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step174!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step175 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 46329856);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 2785280);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step175!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step176 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2801664);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step176!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step177 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 47504128);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 2872832);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step177!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step178 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step178!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step179 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 46600192);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 2818048);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step179!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step180 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 48408064);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 2927616);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step180!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step181 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2944000);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step181!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step182 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache60 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 49313280);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (532645888 + ((token - 1) * 64)) : 532645888));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 2960384);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step182!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step183 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache61 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (532645888 + ((token - 1) * 64)) : 532645888));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (542083072 + ((token - 1) * 64)) : 542083072));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step183!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step184 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache62 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 550471680);
  CSB_Write(device, 195, (kvcache ? (542083072 + ((token - 1) * 64)) : 542083072));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (550995968 + ((token - 1) * 64)) : 550995968));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step184!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step185 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache63 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (550995968 + ((token - 1) * 64)) : 550995968));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (559384576 + ((token - 1) * 64)) : 559384576));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step185!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step186 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 541558784);
  CSB_Write(device, 195, (kvcache ? (559384576 + ((token - 1) * 64)) : 559384576));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step186!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step187 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 49617408);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 2978816);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step187!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step188 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 2995200);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step188!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step189 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 50791680);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 3066368);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step189!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step190 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step190!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step191 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 49887744);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3011584);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step191!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step192 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 51695616);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 3121152);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step192!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step193 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3137536);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step193!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step194 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache64 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 52600832);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (567773184 + ((token - 1) * 64)) : 567773184));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 3153920);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step194!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step195 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache65 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (567773184 + ((token - 1) * 64)) : 567773184));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (577210368 + ((token - 1) * 64)) : 577210368));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step195!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step196 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache66 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 585598976);
  CSB_Write(device, 195, (kvcache ? (577210368 + ((token - 1) * 64)) : 577210368));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (586123264 + ((token - 1) * 64)) : 586123264));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step196!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step197 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache67 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (586123264 + ((token - 1) * 64)) : 586123264));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (594511872 + ((token - 1) * 64)) : 594511872));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step197!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step198 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 576686080);
  CSB_Write(device, 195, (kvcache ? (594511872 + ((token - 1) * 64)) : 594511872));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step198!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step199 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 52904960);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 3172352);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step199!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step200 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3188736);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step200!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step201 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 54079232);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 3259904);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step201!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step202 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step202!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step203 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 53175296);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3205120);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step203!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step204 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 54983168);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 3314688);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step204!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step205 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3331072);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step205!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step206 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache68 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 55888384);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (602900480 + ((token - 1) * 64)) : 602900480));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 3347456);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step206!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step207 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache69 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (602900480 + ((token - 1) * 64)) : 602900480));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (612337664 + ((token - 1) * 64)) : 612337664));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step207!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step208 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache70 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 620726272);
  CSB_Write(device, 195, (kvcache ? (612337664 + ((token - 1) * 64)) : 612337664));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (621250560 + ((token - 1) * 64)) : 621250560));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step208!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step209 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache71 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (621250560 + ((token - 1) * 64)) : 621250560));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (629639168 + ((token - 1) * 64)) : 629639168));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step209!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step210 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 611813376);
  CSB_Write(device, 195, (kvcache ? (629639168 + ((token - 1) * 64)) : 629639168));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step210!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step211 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 56192512);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 3365888);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step211!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step212 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3382272);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step212!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step213 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 57366784);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 3453440);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step213!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step214 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step214!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step215 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 56462848);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3398656);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step215!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step216 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 58270720);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 3508224);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step216!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step217 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3524608);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step217!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step218 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache72 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 59175936);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (638027776 + ((token - 1) * 64)) : 638027776));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 3540992);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step218!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step219 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache73 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (638027776 + ((token - 1) * 64)) : 638027776));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (647464960 + ((token - 1) * 64)) : 647464960));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step219!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step220 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache74 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 655853568);
  CSB_Write(device, 195, (kvcache ? (647464960 + ((token - 1) * 64)) : 647464960));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (656377856 + ((token - 1) * 64)) : 656377856));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step220!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step221 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache75 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (656377856 + ((token - 1) * 64)) : 656377856));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (664766464 + ((token - 1) * 64)) : 664766464));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step221!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step222 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 646940672);
  CSB_Write(device, 195, (kvcache ? (664766464 + ((token - 1) * 64)) : 664766464));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step222!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step223 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 59480064);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 3559424);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step223!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step224 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3575808);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step224!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step225 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 60654336);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 3646976);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step225!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step226 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step226!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step227 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 59750400);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3592192);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step227!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step228 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 61558272);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 3701760);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step228!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step229 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3718144);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step229!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step230 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache76 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 62463488);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (673155072 + ((token - 1) * 64)) : 673155072));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 3734528);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step230!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step231 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache77 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (673155072 + ((token - 1) * 64)) : 673155072));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (682592256 + ((token - 1) * 64)) : 682592256));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step231!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step232 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache78 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 690980864);
  CSB_Write(device, 195, (kvcache ? (682592256 + ((token - 1) * 64)) : 682592256));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (691505152 + ((token - 1) * 64)) : 691505152));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step232!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step233 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache79 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (691505152 + ((token - 1) * 64)) : 691505152));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (699893760 + ((token - 1) * 64)) : 699893760));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step233!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step234 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 682067968);
  CSB_Write(device, 195, (kvcache ? (699893760 + ((token - 1) * 64)) : 699893760));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step234!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step235 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 62767616);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 3752960);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step235!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step236 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3769344);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step236!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step237 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 63941888);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 3840512);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step237!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step238 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step238!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step239 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 63037952);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3785728);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step239!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step240 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 64845824);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 3895296);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step240!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step241 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3911680);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step241!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step242 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache80 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 65751040);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (708282368 + ((token - 1) * 64)) : 708282368));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 3928064);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step242!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step243 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache81 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (708282368 + ((token - 1) * 64)) : 708282368));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (717719552 + ((token - 1) * 64)) : 717719552));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step243!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step244 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache82 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 726108160);
  CSB_Write(device, 195, (kvcache ? (717719552 + ((token - 1) * 64)) : 717719552));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (726632448 + ((token - 1) * 64)) : 726632448));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step244!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step245 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache83 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (726632448 + ((token - 1) * 64)) : 726632448));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (735021056 + ((token - 1) * 64)) : 735021056));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step245!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step246 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 717195264);
  CSB_Write(device, 195, (kvcache ? (735021056 + ((token - 1) * 64)) : 735021056));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step246!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step247 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 66055168);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 3946496);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step247!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step248 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 3962880);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step248!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step249 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 67229440);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 4034048);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step249!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step250 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step250!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step251 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 66325504);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 3979264);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step251!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step252 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 68133376);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 4088832);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step252!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step253 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4105216);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step253!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step254 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache84 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 69038592);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (743409664 + ((token - 1) * 64)) : 743409664));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 4121600);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step254!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step255 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache85 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (743409664 + ((token - 1) * 64)) : 743409664));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (752846848 + ((token - 1) * 64)) : 752846848));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step255!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step256 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache86 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 761235456);
  CSB_Write(device, 195, (kvcache ? (752846848 + ((token - 1) * 64)) : 752846848));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (761759744 + ((token - 1) * 64)) : 761759744));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step256!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step257 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache87 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (761759744 + ((token - 1) * 64)) : 761759744));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (770148352 + ((token - 1) * 64)) : 770148352));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step257!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step258 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 752322560);
  CSB_Write(device, 195, (kvcache ? (770148352 + ((token - 1) * 64)) : 770148352));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step258!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step259 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 69342720);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 4140032);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step259!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step260 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4156416);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step260!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step261 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 70516992);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 4227584);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step261!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step262 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step262!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step263 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 69613056);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 4172800);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step263!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step264 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 71420928);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 4282368);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step264!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step265 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4298752);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step265!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step266 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache88 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 72326144);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (778536960 + ((token - 1) * 64)) : 778536960));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 4315136);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step266!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step267 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache89 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (778536960 + ((token - 1) * 64)) : 778536960));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (787974144 + ((token - 1) * 64)) : 787974144));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step267!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step268 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache90 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 796362752);
  CSB_Write(device, 195, (kvcache ? (787974144 + ((token - 1) * 64)) : 787974144));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (796887040 + ((token - 1) * 64)) : 796887040));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step268!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step269 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache91 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (796887040 + ((token - 1) * 64)) : 796887040));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (805275648 + ((token - 1) * 64)) : 805275648));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step269!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step270 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 787449856);
  CSB_Write(device, 195, (kvcache ? (805275648 + ((token - 1) * 64)) : 805275648));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step270!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step271 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 72630272);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 4333568);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step271!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step272 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4349952);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step272!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step273 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 73804544);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 4421120);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step273!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step274 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step274!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step275 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 72900608);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 4366336);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step275!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step276 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 74708480);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 4475904);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step276!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step277 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4492288);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step277!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step278 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache92 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 75613696);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (813664256 + ((token - 1) * 64)) : 813664256));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 4508672);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step278!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step279 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache93 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (813664256 + ((token - 1) * 64)) : 813664256));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (823101440 + ((token - 1) * 64)) : 823101440));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step279!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step280 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache94 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 831490048);
  CSB_Write(device, 195, (kvcache ? (823101440 + ((token - 1) * 64)) : 823101440));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (832014336 + ((token - 1) * 64)) : 832014336));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step280!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step281 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache95 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (832014336 + ((token - 1) * 64)) : 832014336));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (840402944 + ((token - 1) * 64)) : 840402944));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step281!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step282 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 822577152);
  CSB_Write(device, 195, (kvcache ? (840402944 + ((token - 1) * 64)) : 840402944));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step282!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step283 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 75917824);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 4527104);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step283!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step284 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4543488);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step284!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step285 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 77092096);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 4614656);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step285!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step286 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step286!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step287 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 76188160);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 4559872);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step287!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step288 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 77996032);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 4669440);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step288!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step289 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4685824);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step289!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step290 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache96 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 78901248);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (848791552 + ((token - 1) * 64)) : 848791552));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 4702208);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step290!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step291 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache97 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (848791552 + ((token - 1) * 64)) : 848791552));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (858228736 + ((token - 1) * 64)) : 858228736));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step291!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step292 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache98 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 866617344);
  CSB_Write(device, 195, (kvcache ? (858228736 + ((token - 1) * 64)) : 858228736));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (867141632 + ((token - 1) * 64)) : 867141632));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step292!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step293 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache99 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (867141632 + ((token - 1) * 64)) : 867141632));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (875530240 + ((token - 1) * 64)) : 875530240));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step293!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step294 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 857704448);
  CSB_Write(device, 195, (kvcache ? (875530240 + ((token - 1) * 64)) : 875530240));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step294!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step295 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 79205376);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 4720640);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step295!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step296 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4737024);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step296!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step297 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 80379648);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 4808192);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step297!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step298 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step298!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step299 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 79475712);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 4753408);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step299!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step300 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 81283584);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 4862976);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step300!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step301 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4879360);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step301!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step302 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache100 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 82188800);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (883918848 + ((token - 1) * 64)) : 883918848));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 4895744);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step302!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step303 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache101 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (883918848 + ((token - 1) * 64)) : 883918848));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (893356032 + ((token - 1) * 64)) : 893356032));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step303!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step304 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache102 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 901744640);
  CSB_Write(device, 195, (kvcache ? (893356032 + ((token - 1) * 64)) : 893356032));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (902268928 + ((token - 1) * 64)) : 902268928));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step304!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step305 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache103 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (902268928 + ((token - 1) * 64)) : 902268928));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (910657536 + ((token - 1) * 64)) : 910657536));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step305!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step306 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 892831744);
  CSB_Write(device, 195, (kvcache ? (910657536 + ((token - 1) * 64)) : 910657536));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step306!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step307 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 82492928);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 4914176);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step307!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step308 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 4930560);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step308!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step309 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 83667200);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 5001728);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step309!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step310 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step310!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step311 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 82763264);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 4946944);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step311!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step312 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 84571136);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 5056512);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step312!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step313 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 5072896);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step313!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step314 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache104 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 85476352);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (919046144 + ((token - 1) * 64)) : 919046144));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 5089280);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step314!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step315 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache105 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (919046144 + ((token - 1) * 64)) : 919046144));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (928483328 + ((token - 1) * 64)) : 928483328));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step315!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step316 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache106 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 936871936);
  CSB_Write(device, 195, (kvcache ? (928483328 + ((token - 1) * 64)) : 928483328));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (937396224 + ((token - 1) * 64)) : 937396224));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step316!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step317 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache107 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (937396224 + ((token - 1) * 64)) : 937396224));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (945784832 + ((token - 1) * 64)) : 945784832));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step317!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step318 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 927959040);
  CSB_Write(device, 195, (kvcache ? (945784832 + ((token - 1) * 64)) : 945784832));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step318!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step319 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 85780480);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
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
  CSB_Write(device, 26, 5107712);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step319!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step320 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 5124096);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step320!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step321 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 86954752);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 5195264);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step321!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step322 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step322!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step323 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 86050816);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 5140480);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step323!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step324 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 87858688);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 5250048);
  CSB_Write(device, 27, 1158907904);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step324!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step325 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 5266432);
  CSB_Write(device, 195, 989300736);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step325!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step326 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in cache108 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 88763904);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, (kvcache ? (954173440 + ((token - 1) * 64)) : 954173440));
  CSB_Write(device, 14, 8);
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
  CSB_Write(device, 26, 5282816);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, 65536);
  CSB_Write(device, 32, 65536);
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step326!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step327 (HANDLE& device, int kvcache, int token) {
// accel.hbm.pos_emb accel operator node, storage data in cache109 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (kvcache ? (0 + ((token - 1) * 64)) : 0));
  CSB_Write(device, 195, (kvcache ? (954173440 + ((token - 1) * 64)) : 954173440));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (963610624 + ((token - 1) * 64)) : 963610624));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, (kvcache ? 1 : token));
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 65536);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
#ifdef PRINT_STEP
printf("new_step327!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step328 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterTRP accel operator node, storage data in cache110 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 971999232);
  CSB_Write(device, 195, (kvcache ? (963610624 + ((token - 1) * 64)) : 963610624));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (972523520 + ((token - 1) * 64)) : 972523520));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
#ifdef PRINT_STEP
printf("new_step328!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step329 (HANDLE& device, int kvcache, int token) {
// accel.hbm.softmax accel operator node, storage data in cache111 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, (1 - kvcache));
  CSB_Write(device, 195, (kvcache ? (972523520 + ((token - 1) * 64)) : 972523520));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, (kvcache ? (980912128 + ((token - 1) * 64)) : 980912128));
  CSB_Write(device, 199, 262144);
  CSB_Write(device, 200, 65536);
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, token);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, (kvcache ? 1 : token));
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
#ifdef PRINT_STEP
printf("new_step329!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step330 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_afterF2W accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 963086336);
  CSB_Write(device, 195, (kvcache ? (980912128 + ((token - 1) * 64)) : 980912128));
  CSB_Write(device, 196, 262144);
  CSB_Write(device, 197, 65536);
  CSB_Write(device, 198, 1158907904);
  CSB_Write(device, 199, (((64 * (kvcache ? 1 : token)) * 1) * 4));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, (((token + 32) - 1) / 32));
  CSB_Write(device, 202, token);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
#ifdef PRINT_STEP
printf("new_step330!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step331 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 89068032);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 997689344);
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
  CSB_Write(device, 26, 5301248);
  CSB_Write(device, 27, 989300736);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step331!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step332 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 5317632);
  CSB_Write(device, 195, 997689344);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 198, 989300736);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : (1 * token))) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : (1 * token))));
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 204, (kvcache ? 1 : (1 * token)));
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step332!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step333 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 90242304);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
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
  CSB_Write(device, 26, 5388800);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 799);
#ifdef PRINT_STEP
printf("new_step333!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step334 (HANDLE& device, int kvcache, int token) {
// accel.hbm.activate accel operator node, storage data in runtime2 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 32768);
  CSB_Write(device, 195, 1158907904);
  CSB_Write(device, 196, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 197, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 198, 1025738752);
  CSB_Write(device, 199, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 200, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, (kvcache ? 1 : token));
  CSB_Write(device, 204, (kvcache ? 1 : token));
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
#ifdef PRINT_STEP
printf("new_step334!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step335 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 989300736);
  CSB_Write(device, 11, 89338368);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 26);
  CSB_Write(device, 15, 28);
  CSB_Write(device, 16, 2);
  CSB_Write(device, 17, 0);
  CSB_Write(device, 18, 0);
  CSB_Write(device, 19, 0);
  CSB_Write(device, 20, 0);
  CSB_Write(device, 21, 1);
  CSB_Write(device, 22, 1);
  CSB_Write(device, 23, 1);
  CSB_Write(device, 24, 1);
  CSB_Write(device, 25, 0);
  CSB_Write(device, 26, 5334016);
  CSB_Write(device, 27, 1025738752);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step335!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step336 (HANDLE& device, int kvcache, int token) {
// accel.hbm.mvm_bn_res accel operator node, storage data in runtime0 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, (kvcache ? 1 : token));
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, (kvcache ? 1 : token));
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, (kvcache ? 1 : token));
  CSB_Write(device, 10, 1158907904);
  CSB_Write(device, 11, 91146240);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 989300736);
  CSB_Write(device, 14, 31);
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
  CSB_Write(device, 26, 5443584);
  CSB_Write(device, 27, 997689344);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 30, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 31, ((64 * (kvcache ? 1 : token)) * 1));
  CSB_Write(device, 32, (64 * (kvcache ? 1 : token)));
  CSB_Write(device, 33, 1823);
#ifdef PRINT_STEP
printf("new_step336!\n");
#endif
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step337 (HANDLE& device, int kvcache, int token) {
// accel.hbm.layer_norm accel operator node, storage data in runtime1 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 194, 5459968);
  CSB_Write(device, 195, ((kvcache ? 0 : 1) ? (989300736 + (((1 * token) - 1) * 64)) : 989300736));
  CSB_Write(device, 196, ((kvcache ? 0 : 1) ? ((64 * (1 * token)) * 1) : 64));
  CSB_Write(device, 197, ((kvcache ? 0 : 1) ? (64 * (1 * token)) : 64));
  CSB_Write(device, 198, 997689344);
  CSB_Write(device, 199, 64);
  CSB_Write(device, 200, 64);
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, 1);
  CSB_Write(device, 204, 1);
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
#ifdef PRINT_STEP
printf("new_step337!\n");
#endif
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}

void new_step338 (HANDLE& device) {
// accel.hbm.mvm_bn accel operator node, storage data in runtime3 with 0 offset
#ifdef REGS_DEBUG
LARGE_INTEGER start_run;
LARGE_INTEGER stop_run;
LARGE_INTEGER freq;
double time_sec0;
QueryPerformanceFrequency(&freq);
QueryPerformanceCounter(&start_run);
for (int i = 0; i < 1000; i=i+1) {
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, 1);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 1);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 512);
  CSB_Write(device, 9, 1);
  CSB_Write(device, 10, 997689344);
  CSB_Write(device, 11, 92051456);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 1158907904);
  CSB_Write(device, 14, 126);
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
  CSB_Write(device, 26, 5476352);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 64);
  CSB_Write(device, 30, 64);
  CSB_Write(device, 31, 64);
  CSB_Write(device, 32, 64);
  CSB_Write(device, 60, 1025738752);
  CSB_Write(device, 33, 2847);
#ifdef PRINT_STEP
printf("new_step338!\n");
#endif
  while(CSB_Read(device, 61) != 1) {}
#ifdef REGS_DEBUG
}
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn run time     = %fs(1000 times), %fs(1 times) \n",time_sec0, time_sec0/1000);
#endif
}


void chatglm_dynamic_control_1024_0428_1254(HANDLE& device, int token, int kvcache) {
  new_step1(device, kvcache, token);
  new_step2(device, kvcache, token);
  new_step3(device, kvcache, token);
  new_step4(device, kvcache, token);
  new_step5(device, kvcache, token);
  new_step6(device, kvcache, token);
  new_step7(device, kvcache, token);
  new_step8(device, kvcache, token);
  new_step9(device, kvcache, token);
  new_step10(device, kvcache, token);
  new_step11(device, kvcache, token);
  new_step12(device, kvcache, token);
  new_step13(device, kvcache, token);
  new_step14(device, kvcache, token);
  new_step15(device, kvcache, token);
  new_step16(device, kvcache, token);
  new_step17(device, kvcache, token);
  new_step18(device, kvcache, token);
  new_step19(device, kvcache, token);
  new_step20(device, kvcache, token);
  new_step21(device, kvcache, token);
  new_step22(device, kvcache, token);
  new_step23(device, kvcache, token);
  new_step24(device, kvcache, token);
  new_step25(device, kvcache, token);
  new_step26(device, kvcache, token);
  new_step27(device, kvcache, token);
  new_step28(device, kvcache, token);
  new_step29(device, kvcache, token);
  new_step30(device, kvcache, token);
  new_step31(device, kvcache, token);
  new_step32(device, kvcache, token);
  new_step33(device, kvcache, token);
  new_step34(device, kvcache, token);
  new_step35(device, kvcache, token);
  new_step36(device, kvcache, token);
  new_step37(device, kvcache, token);
  new_step38(device, kvcache, token);
  new_step39(device, kvcache, token);
  new_step40(device, kvcache, token);
  new_step41(device, kvcache, token);
  new_step42(device, kvcache, token);
  new_step43(device, kvcache, token);
  new_step44(device, kvcache, token);
  new_step45(device, kvcache, token);
  new_step46(device, kvcache, token);
  new_step47(device, kvcache, token);
  new_step48(device, kvcache, token);
  new_step49(device, kvcache, token);
  new_step50(device, kvcache, token);
  new_step51(device, kvcache, token);
  new_step52(device, kvcache, token);
  new_step53(device, kvcache, token);
  new_step54(device, kvcache, token);
  new_step55(device, kvcache, token);
  new_step56(device, kvcache, token);
  new_step57(device, kvcache, token);
  new_step58(device, kvcache, token);
  new_step59(device, kvcache, token);
  new_step60(device, kvcache, token);
  new_step61(device, kvcache, token);
  new_step62(device, kvcache, token);
  new_step63(device, kvcache, token);
  new_step64(device, kvcache, token);
  new_step65(device, kvcache, token);
  new_step66(device, kvcache, token);
  new_step67(device, kvcache, token);
  new_step68(device, kvcache, token);
  new_step69(device, kvcache, token);
  new_step70(device, kvcache, token);
  new_step71(device, kvcache, token);
  new_step72(device, kvcache, token);
  new_step73(device, kvcache, token);
  new_step74(device, kvcache, token);
  new_step75(device, kvcache, token);
  new_step76(device, kvcache, token);
  new_step77(device, kvcache, token);
  new_step78(device, kvcache, token);
  new_step79(device, kvcache, token);
  new_step80(device, kvcache, token);
  new_step81(device, kvcache, token);
  new_step82(device, kvcache, token);
  new_step83(device, kvcache, token);
  new_step84(device, kvcache, token);
  new_step85(device, kvcache, token);
  new_step86(device, kvcache, token);
  new_step87(device, kvcache, token);
  new_step88(device, kvcache, token);
  new_step89(device, kvcache, token);
  new_step90(device, kvcache, token);
  new_step91(device, kvcache, token);
  new_step92(device, kvcache, token);
  new_step93(device, kvcache, token);
  new_step94(device, kvcache, token);
  new_step95(device, kvcache, token);
  new_step96(device, kvcache, token);
  new_step97(device, kvcache, token);
  new_step98(device, kvcache, token);
  new_step99(device, kvcache, token);
  new_step100(device, kvcache, token);
  new_step101(device, kvcache, token);
  new_step102(device, kvcache, token);
  new_step103(device, kvcache, token);
  new_step104(device, kvcache, token);
  new_step105(device, kvcache, token);
  new_step106(device, kvcache, token);
  new_step107(device, kvcache, token);
  new_step108(device, kvcache, token);
  new_step109(device, kvcache, token);
  new_step110(device, kvcache, token);
  new_step111(device, kvcache, token);
  new_step112(device, kvcache, token);
  new_step113(device, kvcache, token);
  new_step114(device, kvcache, token);
  new_step115(device, kvcache, token);
  new_step116(device, kvcache, token);
  new_step117(device, kvcache, token);
  new_step118(device, kvcache, token);
  new_step119(device, kvcache, token);
  new_step120(device, kvcache, token);
  new_step121(device, kvcache, token);
  new_step122(device, kvcache, token);
  new_step123(device, kvcache, token);
  new_step124(device, kvcache, token);
  new_step125(device, kvcache, token);
  new_step126(device, kvcache, token);
  new_step127(device, kvcache, token);
  new_step128(device, kvcache, token);
  new_step129(device, kvcache, token);
  new_step130(device, kvcache, token);
  new_step131(device, kvcache, token);
  new_step132(device, kvcache, token);
  new_step133(device, kvcache, token);
  new_step134(device, kvcache, token);
  new_step135(device, kvcache, token);
  new_step136(device, kvcache, token);
  new_step137(device, kvcache, token);
  new_step138(device, kvcache, token);
  new_step139(device, kvcache, token);
  new_step140(device, kvcache, token);
  new_step141(device, kvcache, token);
  new_step142(device, kvcache, token);
  new_step143(device, kvcache, token);
  new_step144(device, kvcache, token);
  new_step145(device, kvcache, token);
  new_step146(device, kvcache, token);
  new_step147(device, kvcache, token);
  new_step148(device, kvcache, token);
  new_step149(device, kvcache, token);
  new_step150(device, kvcache, token);
  new_step151(device, kvcache, token);
  new_step152(device, kvcache, token);
  new_step153(device, kvcache, token);
  new_step154(device, kvcache, token);
  new_step155(device, kvcache, token);
  new_step156(device, kvcache, token);
  new_step157(device, kvcache, token);
  new_step158(device, kvcache, token);
  new_step159(device, kvcache, token);
  new_step160(device, kvcache, token);
  new_step161(device, kvcache, token);
  new_step162(device, kvcache, token);
  new_step163(device, kvcache, token);
  new_step164(device, kvcache, token);
  new_step165(device, kvcache, token);
  new_step166(device, kvcache, token);
  new_step167(device, kvcache, token);
  new_step168(device, kvcache, token);
  new_step169(device, kvcache, token);
  new_step170(device, kvcache, token);
  new_step171(device, kvcache, token);
  new_step172(device, kvcache, token);
  new_step173(device, kvcache, token);
  new_step174(device, kvcache, token);
  new_step175(device, kvcache, token);
  new_step176(device, kvcache, token);
  new_step177(device, kvcache, token);
  new_step178(device, kvcache, token);
  new_step179(device, kvcache, token);
  new_step180(device, kvcache, token);
  new_step181(device, kvcache, token);
  new_step182(device, kvcache, token);
  new_step183(device, kvcache, token);
  new_step184(device, kvcache, token);
  new_step185(device, kvcache, token);
  new_step186(device, kvcache, token);
  new_step187(device, kvcache, token);
  new_step188(device, kvcache, token);
  new_step189(device, kvcache, token);
  new_step190(device, kvcache, token);
  new_step191(device, kvcache, token);
  new_step192(device, kvcache, token);
  new_step193(device, kvcache, token);
  new_step194(device, kvcache, token);
  new_step195(device, kvcache, token);
  new_step196(device, kvcache, token);
  new_step197(device, kvcache, token);
  new_step198(device, kvcache, token);
  new_step199(device, kvcache, token);
  new_step200(device, kvcache, token);
  new_step201(device, kvcache, token);
  new_step202(device, kvcache, token);
  new_step203(device, kvcache, token);
  new_step204(device, kvcache, token);
  new_step205(device, kvcache, token);
  new_step206(device, kvcache, token);
  new_step207(device, kvcache, token);
  new_step208(device, kvcache, token);
  new_step209(device, kvcache, token);
  new_step210(device, kvcache, token);
  new_step211(device, kvcache, token);
  new_step212(device, kvcache, token);
  new_step213(device, kvcache, token);
  new_step214(device, kvcache, token);
  new_step215(device, kvcache, token);
  new_step216(device, kvcache, token);
  new_step217(device, kvcache, token);
  new_step218(device, kvcache, token);
  new_step219(device, kvcache, token);
  new_step220(device, kvcache, token);
  new_step221(device, kvcache, token);
  new_step222(device, kvcache, token);
  new_step223(device, kvcache, token);
  new_step224(device, kvcache, token);
  new_step225(device, kvcache, token);
  new_step226(device, kvcache, token);
  new_step227(device, kvcache, token);
  new_step228(device, kvcache, token);
  new_step229(device, kvcache, token);
  new_step230(device, kvcache, token);
  new_step231(device, kvcache, token);
  new_step232(device, kvcache, token);
  new_step233(device, kvcache, token);
  new_step234(device, kvcache, token);
  new_step235(device, kvcache, token);
  new_step236(device, kvcache, token);
  new_step237(device, kvcache, token);
  new_step238(device, kvcache, token);
  new_step239(device, kvcache, token);
  new_step240(device, kvcache, token);
  new_step241(device, kvcache, token);
  new_step242(device, kvcache, token);
  new_step243(device, kvcache, token);
  new_step244(device, kvcache, token);
  new_step245(device, kvcache, token);
  new_step246(device, kvcache, token);
  new_step247(device, kvcache, token);
  new_step248(device, kvcache, token);
  new_step249(device, kvcache, token);
  new_step250(device, kvcache, token);
  new_step251(device, kvcache, token);
  new_step252(device, kvcache, token);
  new_step253(device, kvcache, token);
  new_step254(device, kvcache, token);
  new_step255(device, kvcache, token);
  new_step256(device, kvcache, token);
  new_step257(device, kvcache, token);
  new_step258(device, kvcache, token);
  new_step259(device, kvcache, token);
  new_step260(device, kvcache, token);
  new_step261(device, kvcache, token);
  new_step262(device, kvcache, token);
  new_step263(device, kvcache, token);
  new_step264(device, kvcache, token);
  new_step265(device, kvcache, token);
  new_step266(device, kvcache, token);
  new_step267(device, kvcache, token);
  new_step268(device, kvcache, token);
  new_step269(device, kvcache, token);
  new_step270(device, kvcache, token);
  new_step271(device, kvcache, token);
  new_step272(device, kvcache, token);
  new_step273(device, kvcache, token);
  new_step274(device, kvcache, token);
  new_step275(device, kvcache, token);
  new_step276(device, kvcache, token);
  new_step277(device, kvcache, token);
  new_step278(device, kvcache, token);
  new_step279(device, kvcache, token);
  new_step280(device, kvcache, token);
  new_step281(device, kvcache, token);
  new_step282(device, kvcache, token);
  new_step283(device, kvcache, token);
  new_step284(device, kvcache, token);
  new_step285(device, kvcache, token);
  new_step286(device, kvcache, token);
  new_step287(device, kvcache, token);
  new_step288(device, kvcache, token);
  new_step289(device, kvcache, token);
  new_step290(device, kvcache, token);
  new_step291(device, kvcache, token);
  new_step292(device, kvcache, token);
  new_step293(device, kvcache, token);
  new_step294(device, kvcache, token);
  new_step295(device, kvcache, token);
  new_step296(device, kvcache, token);
  new_step297(device, kvcache, token);
  new_step298(device, kvcache, token);
  new_step299(device, kvcache, token);
  new_step300(device, kvcache, token);
  new_step301(device, kvcache, token);
  new_step302(device, kvcache, token);
  new_step303(device, kvcache, token);
  new_step304(device, kvcache, token);
  new_step305(device, kvcache, token);
  new_step306(device, kvcache, token);
  new_step307(device, kvcache, token);
  new_step308(device, kvcache, token);
  new_step309(device, kvcache, token);
  new_step310(device, kvcache, token);
  new_step311(device, kvcache, token);
  new_step312(device, kvcache, token);
  new_step313(device, kvcache, token);
  new_step314(device, kvcache, token);
  new_step315(device, kvcache, token);
  new_step316(device, kvcache, token);
  new_step317(device, kvcache, token);
  new_step318(device, kvcache, token);
  new_step319(device, kvcache, token);
  new_step320(device, kvcache, token);
  new_step321(device, kvcache, token);
  new_step322(device, kvcache, token);
  new_step323(device, kvcache, token);
  new_step324(device, kvcache, token);
  new_step325(device, kvcache, token);
  new_step326(device, kvcache, token);
  new_step327(device, kvcache, token);
  new_step328(device, kvcache, token);
  new_step329(device, kvcache, token);
  new_step330(device, kvcache, token);
  new_step331(device, kvcache, token);
  new_step332(device, kvcache, token);
  new_step333(device, kvcache, token);
  new_step334(device, kvcache, token);
  new_step335(device, kvcache, token);
  new_step336(device, kvcache, token);
  new_step337(device, kvcache, token);
  new_step338(device);
}
