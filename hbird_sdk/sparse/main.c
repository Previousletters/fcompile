#include <stdio.h>
#include <time.h>
#include "fpga_lib.h"
#include "kernel.h"

int main() {
    HANDLE device = 0x48000000;
    printf("Init Sparse Booth Accel!\n");
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    main_test(device);
    clock_gettime(CLOCK_REALTIME, &end);
    printf("Run Finish!\n");

    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("main test taken %ld.%06lds\n", (long)elapsed, (long)((elapsed-(int)elapsed)*1e6));
    return 0;
}