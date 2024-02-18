#include "FPGA_MEM.h"
#include "basic.h"
using namespace std;

/***************************************** DDR **********************************************/

vector<mem_control_block> ddr_mcb(1);
unsigned int ddr_has_initialized=0;

void *FPGA_DDR_malloc(unsigned int numbytes)
{
	//make sure the address is aligned to MIN_BLOCK_SIZE
	numbytes=((numbytes+MIN_BLOCK_SIZE-1)/MIN_BLOCK_SIZE)*MIN_BLOCK_SIZE;
	 
	if(!ddr_has_initialized)
	{
        ddr_mcb.clear();
        mem_control_block new_ddr_mcb;
		new_ddr_mcb.available=1;
		new_ddr_mcb.blocksize=FPGA_DDR_SIZE;
		new_ddr_mcb.board_address=FPGA_DDR_BASE_ADDRESS;
        ddr_mcb.push_back(new_ddr_mcb);
		ddr_has_initialized=1;
	}
	//Debug_ddr_mcb();

	unsigned int current_ddr_mcb_num=0;
	void * memory_location = FPGA_NULL;
	while(current_ddr_mcb_num<ddr_mcb.size())
	{
		if(ddr_mcb[current_ddr_mcb_num].available)
		{
			if(ddr_mcb[current_ddr_mcb_num].blocksize>=numbytes)
			{
				//printf("Malloc Success:%d\n",NULL);
				memory_location=(void *)ddr_mcb[current_ddr_mcb_num].board_address;
				ddr_mcb[current_ddr_mcb_num].available=0;
				if(ddr_mcb[current_ddr_mcb_num].blocksize>numbytes)
				{
					mem_control_block new_ddr_mcb;
					new_ddr_mcb.available=1;
					new_ddr_mcb.blocksize=ddr_mcb[current_ddr_mcb_num].blocksize-numbytes;
					new_ddr_mcb.board_address =ddr_mcb[current_ddr_mcb_num].board_address +numbytes;
					ddr_mcb.insert(ddr_mcb.begin()+current_ddr_mcb_num+1,new_ddr_mcb);
				}
				ddr_mcb[current_ddr_mcb_num].blocksize=numbytes;
				break;
			}
		}
		current_ddr_mcb_num++;
	}

	return memory_location;
}

void FPGA_DDR_free(void *firstbyte, int real_size) 
{
	unsigned int current_ddr_mcb_num=-1;
	for(int i=0;i<ddr_mcb.size();i++)
	{
		if(ddr_mcb[i].board_address == (uint64_t)firstbyte)
		{
			current_ddr_mcb_num=i;
			break;
		}
		if(ddr_mcb[i].board_address < (uint64_t)firstbyte && ddr_mcb[i].board_address + ddr_mcb[i].blocksize > (uint64_t)firstbyte)
		{
			current_ddr_mcb_num=i;
			break;
		}
	}
    if (current_ddr_mcb_num == -1)
        return;
    if (ddr_mcb[current_ddr_mcb_num].board_address == (uint64_t)firstbyte)
        if (real_size == ddr_mcb[current_ddr_mcb_num].blocksize) {
            ddr_mcb[current_ddr_mcb_num].available=1;
        } else if (real_size == 0) {
            std::cout << "*Free* : Noting to do!" << std::endl;
        } else if (real_size < ddr_mcb[current_ddr_mcb_num].blocksize) {
            mem_control_block new_ddr_mcb_1;
            new_ddr_mcb_1.available = 0;
            new_ddr_mcb_1.board_address=(uint64_t)firstbyte + real_size;
            new_ddr_mcb_1.blocksize = ddr_mcb[current_ddr_mcb_num].blocksize - real_size;
            ddr_mcb.insert(ddr_mcb.begin()+current_ddr_mcb_num+1,new_ddr_mcb_1);
            ddr_mcb[current_ddr_mcb_num].available=1;
            ddr_mcb[current_ddr_mcb_num].blocksize = real_size;
        }
    else {
        ddr_mcb[current_ddr_mcb_num].blocksize = (uint64_t)firstbyte - ddr_mcb[current_ddr_mcb_num].board_address;
        mem_control_block new_ddr_mcb;
        new_ddr_mcb.available=1;
        new_ddr_mcb.board_address=(uint64_t)firstbyte;
        int diff_size = ddr_mcb[current_ddr_mcb_num].board_address + ddr_mcb[current_ddr_mcb_num].blocksize - (uint64_t)firstbyte;
        if (real_size == diff_size) {
            new_ddr_mcb.blocksize = diff_size;
            ddr_mcb.insert(ddr_mcb.begin()+current_ddr_mcb_num+1,new_ddr_mcb);
            current_ddr_mcb_num += 1;
        } else if (real_size == 0) {
            std::cout << "*Free* : Noting to do!" << std::endl;
        } else if (real_size < diff_size) {
            new_ddr_mcb.blocksize = real_size;
            ddr_mcb.insert(ddr_mcb.begin()+current_ddr_mcb_num+1,new_ddr_mcb);
            current_ddr_mcb_num += 1;
            mem_control_block new_ddr_mcb_1;
            new_ddr_mcb_1.available = 0;
            new_ddr_mcb_1.board_address=(uint64_t)firstbyte + real_size;
            new_ddr_mcb_1.blocksize = diff_size - real_size;
            ddr_mcb.insert(ddr_mcb.begin()+current_ddr_mcb_num+1,new_ddr_mcb_1);
        }

    }
	if(current_ddr_mcb_num!=ddr_mcb.size()-1)
	{
		if(ddr_mcb[current_ddr_mcb_num+1].available)
		{
			ddr_mcb[current_ddr_mcb_num].blocksize+=ddr_mcb[current_ddr_mcb_num+1].blocksize;
			ddr_mcb.erase(ddr_mcb.begin()+current_ddr_mcb_num+1,ddr_mcb.begin()+current_ddr_mcb_num+2);
		}
	}
	if(current_ddr_mcb_num!=0)
	{
		if(ddr_mcb[current_ddr_mcb_num-1].available)
		{
			ddr_mcb[current_ddr_mcb_num-1].blocksize+=ddr_mcb[current_ddr_mcb_num].blocksize;
			ddr_mcb.erase(ddr_mcb.begin()+current_ddr_mcb_num,ddr_mcb.begin()+current_ddr_mcb_num+1);
		}
	}
}

void FPGA_DDR_show() {
    std::cout << "******************* DDR MEM ********************" << std::endl;
    for (auto i : ddr_mcb) {
        std::cout << "* start: "
                  << std::hex << std::setw(9) << std::setfill('0') << i.board_address
                  << " end: "
                  << std::hex << std::setw(9) << std::setfill('0') << i.board_address + i.blocksize << std::dec
                  << " available: " << (int)i.available << " *" << std::endl;
        ;
    }
    std::cout << "************************************************" << std::endl;
}

/***************************************** HBM **********************************************/

vector<mem_control_block> hbm_mcb(1);
unsigned int hbm_has_initialized = 0;

void* FPGA_HBM_malloc(unsigned int numbytes)
{
	//make sure the address is aligned to MIN_BLOCK_SIZE
	numbytes = ((numbytes + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE) * MIN_BLOCK_SIZE;

	if (!hbm_has_initialized)
	{
        hbm_mcb.clear();
        mem_control_block new_hbm_mcb;
		new_hbm_mcb.available=1;
		new_hbm_mcb.blocksize=FPGA_HBM_SIZE;
		new_hbm_mcb.board_address=FPGA_HBM_BASE_ADDRESS;
        hbm_mcb.push_back(new_hbm_mcb);
		hbm_has_initialized = 1;
	}
	//Debug_hbm_mcb();

	unsigned int current_hbm_mcb_num = 0;
	void* memory_location = FPGA_NULL;
	while (current_hbm_mcb_num < hbm_mcb.size())
	{
		if (hbm_mcb[current_hbm_mcb_num].available)
		{
			if (hbm_mcb[current_hbm_mcb_num].blocksize >= numbytes)
			{
				//printf("Malloc Success:%d\n",NULL);
				memory_location = (void*)hbm_mcb[current_hbm_mcb_num].board_address;
				hbm_mcb[current_hbm_mcb_num].available = 0;
				if (hbm_mcb[current_hbm_mcb_num].blocksize > numbytes)
				{
					mem_control_block new_hbm_mcb;
					new_hbm_mcb.available = 1;
					new_hbm_mcb.blocksize = hbm_mcb[current_hbm_mcb_num].blocksize - numbytes;
					new_hbm_mcb.board_address = hbm_mcb[current_hbm_mcb_num].board_address + numbytes;
					hbm_mcb.insert(hbm_mcb.begin() + current_hbm_mcb_num + 1, new_hbm_mcb);
				}
				hbm_mcb[current_hbm_mcb_num].blocksize = numbytes;
				break;
			}
		}
		current_hbm_mcb_num++;
	}

	return memory_location;
}

void FPGA_HBM_free(void* firstbyte)
{
	unsigned int current_hbm_mcb_num = 0;
	for (int i = 0; i < hbm_mcb.size(); i++)
	{
		if (hbm_mcb[i].board_address == (uint64_t)firstbyte)
		{
			current_hbm_mcb_num = i;
			break;
		}
	}
    if (current_hbm_mcb_num == -1)
        return;
	hbm_mcb[current_hbm_mcb_num].available = 1;
	if (current_hbm_mcb_num != hbm_mcb.size() - 1)
	{
		if (hbm_mcb[current_hbm_mcb_num + 1].available)
		{
			hbm_mcb[current_hbm_mcb_num].blocksize += hbm_mcb[current_hbm_mcb_num + 1].blocksize;
			hbm_mcb.erase(hbm_mcb.begin() + current_hbm_mcb_num + 1, hbm_mcb.begin() + current_hbm_mcb_num + 2);
		}
	}
	if (current_hbm_mcb_num != 0)
	{
		if (hbm_mcb[current_hbm_mcb_num - 1].available)
		{
			hbm_mcb[current_hbm_mcb_num - 1].blocksize += hbm_mcb[current_hbm_mcb_num].blocksize;
			hbm_mcb.erase(hbm_mcb.begin() + current_hbm_mcb_num, hbm_mcb.begin() + current_hbm_mcb_num + 1);
		}
	}
}

void FPGA_HBM_show() {
    std::cout << "******************* HBM MEM ********************" << std::endl;
    for (auto i : hbm_mcb) {
        std::cout << "* start: "
                  << std::hex << std::setw(9) << std::setfill('0') << i.board_address
                  << " end: "
                  << std::hex << std::setw(9) << std::setfill('0') << i.board_address + i.blocksize << std::dec
                  << " available: " << (int)i.available << " *" << std::endl;
        ;
    }
    std::cout << "************************************************" << std::endl;
}
