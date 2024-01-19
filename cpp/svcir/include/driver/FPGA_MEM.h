#ifndef __FPGA_DDR__
#define __FPGA_DDR__

#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <driver/basic.h>

namespace hbm {

#define FPGA_DDR_BASE_ADDRESS 0x200000000
#define FPGA_DDR_SIZE         0x100000000
#define FPGA_HBM_BASE_ADDRESS 0x000000000
#define FPGA_HBM_SIZE         0x100000000
#define MIN_BLOCK_SIZE        (32*(8/8))

typedef struct{
unsigned char available;         /* whether block is avaiable */
uint64_t blocksize;          /* block size */
uint64_t board_address;  /* the address of MEM on FPGA board */
}mem_control_block;

#define FPGA_NULL ((void *)0xFFFFFFFF)

void *FPGA_DDR_malloc(unsigned int numbytes);
void FPGA_DDR_free(void *firstbyte);

void* FPGA_HBM_malloc(unsigned int numbytes);
void FPGA_HBM_free(void* firstbyte);

};

#endif
