#include <iostream>
#include <fstream>
#include <iomanip>
#include "FPGA_DDR.h"
#include "basic.h"
using namespace std;
void *cnn_map_base;
void *mem_map_base;

/////////////////////////////////
void LoadData_From_txt(short *dat, const char *filename, int size)
{
    FILE *fp=fopen(filename,"rb");
    if(fp==NULL)
    {
        printf("Can't open file: %s\n",filename);
        return;
    }
    size_t rd_size=fread(dat,1,size,fp);
    if(rd_size!=size)
        printf("Load Data from file Error: %s\n",filename);

    fclose(fp);
}

void Load_Feature_From_bin_File(struct Mapped_Feature *feature,const char *filename)
{
	FILE *fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("Can't open file: %s\n",filename);
		return;
	}
	size_t rd_size=fread((void *)(((uint64_t)mem_map_base)+((uint64_t)feature->payload)),1,feature->payload_size,fp);
	if(rd_size!=feature->payload_size)
		printf("Load Feature From File Error: %s\n",filename);

	fclose(fp);
}

void Load_Weight_From_bin_File(struct Mapped_Weight *weight,const char *filename)
{
	FILE *fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("Can't open file: %s\n",filename);
		return;
	}
	size_t rd_size=fread((void *)(((uint64_t)mem_map_base)+(uint64_t)weight->payload),1,weight->payload_size,fp);
	if(rd_size!=weight->payload_size)
		printf("Load Weight From File Error: %s\n",filename);
	
	fclose(fp);
}


void Save_Feature_To_bin_File(struct Mapped_Feature *feature,const char *filename)
{
	FILE *fp=fopen(filename,"wb");
	if(fp==NULL)
	{
		printf("Can't open file: %s\n",filename);
		return;
	}

	fwrite((void *)(((uint64_t)mem_map_base)+(uint64_t)feature->payload),1,feature->payload_size,fp);
	fclose(fp);
}


///// function for software//////////
void Map_Feature_Soft(short* in, struct Mapped_Feature* feature)
{
	for (int i = 0; i < feature->height; i++)
		for (int j = 0; j < feature->width; j++)
			for (int k = 0; k < feature->channel; k = k + Tout)
			{
				unsigned int dst_base = (k / Tout) * feature->surface_stride / 2 + i * feature->line_stride / 2 + j * Tout;
				unsigned int src_base = i * feature->width * feature->channel + j * feature->channel;
				for (int kk = k; kk < k + Tout; kk++)
				{
					if (kk < feature->channel)
						feature->payload[dst_base + (kk - k)] = in[src_base + kk];
					else
						feature->payload[dst_base + (kk - k)] = 0;
				}
			}
}

void Map_Weight_Soft(short* kernel, struct Mapped_Weight* weight)
{
	int addr;
	addr = 0;

	for (int k = 0; k < weight->out_ch; k += Tout)
		for (int l = 0; l < weight->in_ch; l += Tin)
			for (int i = 0; i < weight->Ky; i++)
				for (int j = 0; j < weight->Kx; j++)
					for (int kk = k; kk < k + Tout; kk++)
					{
						if (kk < weight->out_ch)
						{
							for (int ll = l; ll < l + Tin; ll += Tout)
							{
								short tp[Tout];
								unsigned int src_base = i * weight->Kx * weight->in_ch * weight->out_ch + j * weight->in_ch * weight->out_ch + kk;
								for (int lll = ll; lll < ll + Tout; lll++)
								{
									if (lll < weight->in_ch)
										tp[lll - ll] = kernel[src_base + lll * weight->out_ch];
									else
										tp[lll - ll] = 0;
								}
								for (int cp = 0; cp < Tout; cp++)
									weight->payload[addr + cp] = tp[cp];
								addr = addr + Tout;
							}
						}
					}
}

struct Mapped_Feature* Malloc_Feature_Soft(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit)
{
	struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));//Ϊ�ṹ������ڴ�ռ䣬����ֵΪָ������ڴ���׵�ַ
	if (ret == NULL)
	{
		printf("failed to malloc structure\n");
		return NULL;
	}

	ret->scale = scale;
	ret->conv_out_scale = conv_out_scale;
	ret->height = height;
	ret->width = width;
	ret->channel = ch;
	ret->dat_bit = dat_bit;

	ret->line_stride = width * 2 * Tout;
	ret->surface_stride = ret->line_stride * height;


	unsigned int require_bytes = ret->surface_stride * ((ch + Tout - 1) / Tout);
	ret->payload_size = require_bytes;

	ret->payload = (short*)malloc(require_bytes);
	if (ret->payload == NULL)
	{
		free(ret);
		printf("failed to malloc payload:%d\n", require_bytes);
		return NULL;
	}

	return ret;
}

struct Mapped_Weight* Malloc_Weight_Soft(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit)
{
	struct Mapped_Weight* ret = (struct Mapped_Weight*)malloc(sizeof(struct Mapped_Weight));

	if (ret == NULL)
		return NULL;

	ret->scale = scale;
	ret->Ky = Ky;
	ret->Kx = Kx;
	ret->in_ch = in_ch;
	ret->out_ch = out_ch;
	ret->wt_bit = wt_bit;

	unsigned int require_bytes = (Tin * 2 * Kx * Ky * out_ch * ((in_ch + Tin - 1) / Tin));
	ret->payload_size = require_bytes;

	ret->payload = (short*)malloc(require_bytes);
	if (ret->payload == NULL)
	{
		free(ret);
		return NULL;
	}

	return ret;
}

short* Get_Element_Soft(struct Mapped_Feature* feature, int row, int col, int ch)//ֱ�Ӵ��ļ��н�bin������ȡ�����Ա��ӡ
{
	return (short*)(((uint64_t)feature->payload) + (ch / Tout) * feature->surface_stride + row * feature->line_stride + col * Tout * 2 + (ch % Tout) * 2);
}

short* Get_Weight_Soft(struct Mapped_Weight* weight, int n_h, int n_w, int n_cin, int n_cout)
{
	int in_ch_slice = weight->in_ch;
	unsigned int ch_group_now = ((n_cout / Tout) == ((weight->out_ch - 1) / Tout)) ? (((weight->out_ch - 1) % Tout) + 1) : Tout;
	unsigned int m = (n_cin / in_ch_slice);
	unsigned int n_cin_now = n_cin % in_ch_slice;
	unsigned int ch_in_now = (m == ((weight->in_ch - 1) / in_ch_slice)) ? (((weight->in_ch - 1) % in_ch_slice) + 1) : in_ch_slice;

	uint64_t addr_bias1 = m * in_ch_slice * weight->out_ch * weight->Kx * weight->Ky * 2;
	uint64_t addr_bias2 = ((n_cout / Tout) * Tout * ((ch_in_now + Tin - 1) / Tin) * weight->Kx * weight->Ky + (n_cin_now / Tin) * ch_group_now * weight->Kx * weight->Ky + (n_h * weight->Kx + n_w) * ch_group_now + (n_cout % Tout)) * Tin * 2 + (n_cin_now % Tin) * 2;
	return (short*)(((uint64_t)weight->payload) + addr_bias1 + addr_bias2);
}


///// function for hardware//////////
#ifdef Run_on_FPGA


void FPGA_Init()
{
	int fd;
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
		printf("Error: Can't open /dev/mem\n");

	cnn_map_base = mmap(0, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xA0000000);
	if (cnn_map_base == NULL)
		printf("Error: vpu_base mmap fail\n");

	mem_map_base = mmap(0, 0x100000000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x400000000);
	if (mem_map_base == NULL)
		printf("Error: mem_base mmap fail\n");
	
	//printf("**** cnn_map_base=%d\n", cnn_map_base);
	//printf("**** mem_map_base=%d\n", mem_map_base);
	
	printf("FPGA Init Done\n");
}
void CSB_Write(int addr, int data)
{
	*(uint32_t*)(((long int)cnn_map_base) + (addr << 2)) = data;
}

int CSB_Read(int addr)
{
	int rt = *(uint32_t*)(((long int)cnn_map_base) + (addr << 2));
	return rt;
}

void Map_Feature(char* in, struct Mapped_Feature* feature)
{
	for (int i = 0; i < feature->height; i++)
		for (int j = 0; j < feature->width; j++)
			for (int k = 0; k < feature->channel; k = k + Tout)
			{
				unsigned int dst_base = (k / Tout) * feature->surface_stride / (MAX_DAT_DW / 8) + i * feature->line_stride / (MAX_DAT_DW / 8) + j * Tout;
				unsigned int src_base = i * feature->width * feature->channel + j * feature->channel;
				for (int kk = k; kk < k + Tout; kk++)
				{
					if (kk < feature->channel)
						*(char*)(((uint64_t)mem_map_base) + ((uint64_t)(feature->payload + dst_base + (kk - k)))) = in[src_base + kk];
					else
						*(char*)(((uint64_t)mem_map_base) + ((uint64_t)(feature->payload + dst_base + (kk - k)))) = 0;
				}
			}
}

void Map_Weight(char* in, struct Mapped_Weight* weight) {
    memcpy((void *)(((uint64_t)mem_map_base)+((uint64_t)weight->payload)), in, weight->payload_size);
}

void DeMap_Feature(struct Mapped_Feature* feature, char* out)
{
	for (int i = 0; i < feature->height; i++)
		for (int j = 0; j < feature->width; j++)
			for (int k = 0; k < feature->channel; k = k + Tout)
			{
				unsigned int dst_base = (k / Tout) * feature->surface_stride / (MAX_DAT_DW / 8) + i * feature->line_stride / (MAX_DAT_DW / 8) + j * Tout;
				unsigned int src_base = i * feature->width * feature->channel + j * feature->channel;
				for (int kk = k; kk < k + Tout; kk++)
				{
					if (kk < feature->channel)
						out[src_base + kk] = *(char*)(((uint64_t)mem_map_base) + ((uint64_t)(feature->payload + dst_base + (kk - k))));
				}
			}
}

struct Mapped_Feature* Malloc_Feature(int height, int width, int ch, int scale, int conv_out_scale, int dat_bit)
{
	struct Mapped_Feature* ret = (struct Mapped_Feature*)malloc(sizeof(struct Mapped_Feature));
	if (ret == NULL)
	{
		return NULL;
	}

	ret->scale = scale;
	ret->height = height;
	ret->width = width;
	ret->channel = ch;
	ret->dat_bit = dat_bit;
	ret->conv_out_scale = conv_out_scale;

	ret->line_stride = width * (MAX_DAT_DW / 8) * Tout;
	ret->surface_stride = ret->line_stride * height;

	int require_bytes = ret->surface_stride * ((ch + Tout - 1) / Tout);
	ret->payload_size = require_bytes;


	ret->payload = (short*)FPGA_DDR_malloc(require_bytes);
	if (ret->payload == FPGA_NULL)
	{
		free(ret);
		return NULL;
	}
	
	//printf("**** feature->payload=%d\n", ret->payload);
	return ret;
}

struct Mapped_Weight* Malloc_Weight(int Ky, int Kx, int in_ch, int out_ch, int scale, int wt_bit)
{
	struct Mapped_Weight* ret = (struct Mapped_Weight*)malloc(sizeof(struct Mapped_Weight));
	if (ret == NULL)
		return NULL;

	ret->scale = scale;
	ret->Ky = Ky;
	ret->Kx = Kx;
	ret->in_ch = in_ch;
	ret->out_ch = out_ch;
	ret->wt_bit = wt_bit;

	int Tin_L = Tin * MAX_DAT_DW / wt_bit;
	int require_bytes = ((Tin_L * wt_bit) / 8) * Kx * Ky * out_ch * ((in_ch + Tin_L - 1) / Tin_L);
	ret->payload_size = require_bytes;

	ret->payload = (short*)FPGA_DDR_malloc(require_bytes);
	if (ret->payload == FPGA_NULL)
	{
		free(ret);
		return NULL;
	}
	
	//printf("**** weight->payload=%d\n", ret->payload);
	return ret;
}

signed char* Get_Element(struct Mapped_Feature* feature, int row, int col, int ch)
{
	return (signed char*)(((uint64_t)mem_map_base) + ((uint64_t)feature->payload) + (ch / Tout) * feature->surface_stride + row * feature->line_stride + col * Tout * (MAX_DAT_DW / 8) + (ch % Tout) * (MAX_DAT_DW / 8));
}

void Free_Feature(struct Mapped_Feature *feature)
{
	if(feature->payload!=FPGA_NULL) FPGA_DDR_free(feature->payload);
	free(feature);
}

void Free_Weight(struct Mapped_Weight *weight)
{
	if(weight->payload!=FPGA_NULL) FPGA_DDR_free(weight->payload);
	free(weight);
}

#endif
