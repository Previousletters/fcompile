void chatglm_without_kvcache_0410_1700(HANDLE& device) {
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
// accel.hbm.layer_norm accel operator node, storage data in ddr2 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 1048576);
  CSB_Write(device, 195, 0);
  CSB_Write(device, 196, 1216);
  CSB_Write(device, 197, 1216);
  CSB_Write(device, 198, 1064960);
  CSB_Write(device, 199, 1216);
  CSB_Write(device, 200, 1216);
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, 19);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.layer_norm run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_bn accel operator node, storage data in ddr4 with 0 offset
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
  CSB_Write(device, 10, 1064960);
  CSB_Write(device, 11, 0);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 2131968);
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
  CSB_Write(device, 26, 2113536);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1216);
  CSB_Write(device, 30, 1216);
  CSB_Write(device, 31, 8192);
  CSB_Write(device, 32, 8192);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
  CSB_Write(device, 33, 799);
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_bn run time     = %fs \n",time_sec1);
#endif
// accel.hbm.pos_emb accel operator node, storage data in ddr6 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 5638144);
  CSB_Write(device, 195, 2131968);
  CSB_Write(device, 196, 32768);
  CSB_Write(device, 197, 8192);
  CSB_Write(device, 198, 5654528);
  CSB_Write(device, 199, 32768);
  CSB_Write(device, 200, 8192);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, 19);
  CSB_Write(device, 203, 34);
  CSB_Write(device, 204, 8192);
  CSB_Write(device, 205, 0);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 4);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.pos_emb cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.pos_emb run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_afterTRP accel operator node, storage data in ddr7 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 6703104);
  CSB_Write(device, 195, 5654528);
  CSB_Write(device, 196, 32768);
  CSB_Write(device, 197, 8192);
  CSB_Write(device, 198, 9160704);
  CSB_Write(device, 199, 32768);
  CSB_Write(device, 200, 8192);
  CSB_Write(device, 201, 4);
  CSB_Write(device, 202, 19);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 11688);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 2);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterTRP cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_afterTRP run time     = %fs \n",time_sec1);
#endif
// accel.hbm.softmax accel operator node, storage data in ddr8 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 1);
  CSB_Write(device, 195, 9160704);
  CSB_Write(device, 196, 32768);
  CSB_Write(device, 197, 8192);
  CSB_Write(device, 198, 10209280);
  CSB_Write(device, 199, 32768);
  CSB_Write(device, 200, 8192);
  CSB_Write(device, 201, 1);
  CSB_Write(device, 202, 32);
  CSB_Write(device, 203, 19);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 19);
  CSB_Write(device, 206, 0);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 8);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.softmax cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.softmax run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_afterF2W accel operator node, storage data in ddr7 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 3246080);
  CSB_Write(device, 195, 10209280);
  CSB_Write(device, 196, 32768);
  CSB_Write(device, 197, 8192);
  CSB_Write(device, 198, 9160704);
  CSB_Write(device, 199, 4864);
  CSB_Write(device, 200, 1216);
  CSB_Write(device, 201, 1);
  CSB_Write(device, 202, 19);
  CSB_Write(device, 203, 2);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 32);
  CSB_Write(device, 206, 15);
  CSB_Write(device, 207, 4);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 1);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_afterF2W cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_afterF2W run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_bn_res accel operator node, storage data in ddr8 with 0 offset
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
  CSB_Write(device, 10, 9160704);
  CSB_Write(device, 11, 304128);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 10209280);
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
  CSB_Write(device, 26, 11257856);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1216);
  CSB_Write(device, 30, 1216);
  CSB_Write(device, 31, 1216);
  CSB_Write(device, 32, 1216);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
  CSB_Write(device, 33, 1823);
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_bn_res run time     = %fs \n",time_sec1);
#endif
// accel.hbm.layer_norm accel operator node, storage data in ddr0 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 11274240);
  CSB_Write(device, 195, 10209280);
  CSB_Write(device, 196, 1216);
  CSB_Write(device, 197, 1216);
  CSB_Write(device, 198, 0);
  CSB_Write(device, 199, 1216);
  CSB_Write(device, 200, 1216);
  CSB_Write(device, 201, 128);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, 19);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 155648);
  CSB_Write(device, 206, 8);
  CSB_Write(device, 207, 1);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 32);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.layer_norm cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.layer_norm run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_bn accel operator node, storage data in ddr4 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, 19);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 19);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, 19);
  CSB_Write(device, 10, 0);
  CSB_Write(device, 11, 1478400);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 2131968);
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
  CSB_Write(device, 26, 11345408);
  CSB_Write(device, 27, 0);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1216);
  CSB_Write(device, 30, 1216);
  CSB_Write(device, 31, 1216);
  CSB_Write(device, 32, 1216);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
  CSB_Write(device, 33, 799);
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_bn run time     = %fs \n",time_sec1);
#endif
// accel.hbm.activate accel operator node, storage data in ddr6 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 194, 11400192);
  CSB_Write(device, 195, 2131968);
  CSB_Write(device, 196, 1216);
  CSB_Write(device, 197, 1216);
  CSB_Write(device, 198, 5654528);
  CSB_Write(device, 199, 1216);
  CSB_Write(device, 200, 1216);
  CSB_Write(device, 201, 428);
  CSB_Write(device, 202, 1);
  CSB_Write(device, 203, 19);
  CSB_Write(device, 204, 19);
  CSB_Write(device, 205, 428);
  CSB_Write(device, 206, 13696);
  CSB_Write(device, 207, 0);
  CSB_Write(device, 208, 0);
  CSB_Write(device, 209, 16);
  while(CSB_Read(device, 193) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.activate cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.activate run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_bn_res accel operator node, storage data in ddr4 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 2, 4096);
  CSB_Write(device, 3, 19);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 19);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 512);
  CSB_Write(device, 8, 384);
  CSB_Write(device, 9, 19);
  CSB_Write(device, 10, 0);
  CSB_Write(device, 11, 574464);
  CSB_Write(device, 12, 16896);
  CSB_Write(device, 13, 2131968);
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
  CSB_Write(device, 26, 11290624);
  CSB_Write(device, 27, 5654528);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1216);
  CSB_Write(device, 30, 1216);
  CSB_Write(device, 31, 1216);
  CSB_Write(device, 32, 1216);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
  CSB_Write(device, 33, 1823);
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_bn_res run time     = %fs \n",time_sec1);
#endif
// accel.hbm.mvm_bn_res accel operator node, storage data in ddr0 with 0 offset
#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif
  CSB_Write(device, 2, 13696);
  CSB_Write(device, 3, 19);
  CSB_Write(device, 4, 1);
  CSB_Write(device, 5, 19);
  CSB_Write(device, 6, 1);
  CSB_Write(device, 7, 128);
  CSB_Write(device, 8, 128);
  CSB_Write(device, 9, 19);
  CSB_Write(device, 10, 2131968);
  CSB_Write(device, 11, 2382336);
  CSB_Write(device, 12, 56576);
  CSB_Write(device, 13, 0);
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
  CSB_Write(device, 26, 11408384);
  CSB_Write(device, 27, 10209280);
  CSB_Write(device, 28, 0);
  CSB_Write(device, 29, 1216);
  CSB_Write(device, 30, 1216);
  CSB_Write(device, 31, 1216);
  CSB_Write(device, 32, 1216);
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif
  CSB_Write(device, 33, 1823);
  while(CSB_Read(device, 1) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("accel.hbm.mvm_bn_res cfg reg time = %fs \n",time_sec0);
printf("accel.hbm.mvm_bn_res run time     = %fs \n",time_sec1);
#endif
}
