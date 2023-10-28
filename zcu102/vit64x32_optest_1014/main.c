#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <basic.h>

void TVMWrapInit();
void TVMWrapRun();
void TVMWrapSetParams(char*);
void TVMWrapGetInputPtr(int);
void TVMWrapGetOutputPtr(int);

int main(void) {
    FPGA_Init();
    FILE* fp;
    fp = fopen("params.bin", "rb");
    int64_t n;
    fpos_t fpos; //当前位置
    fgetpos(fp, &fpos); //获取当前位置
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    fsetpos(fp,&fpos); //恢复之前的位置
    char* params = (char*)malloc(n);
    fread(params, 1, n, fp);
    TVMWrapInit();
    TVMWrapSetParams(params);
    TVMWrapRun();
    printf("Finish!\n");
    return 0;
}
