#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "basic.h"
#include "conv.h"
#include "matrix.h"
#include "others.h"
#include "approx_pwlf_act.h"
#include "src/conv.h"
#include "src/others.h"

#define base_Tin 128
#define Tout 32

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


	ret->payload = 0;
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

	int require_bytes = ((base_Tin * wt_bit) / 8) * Kx * Ky * out_ch * ((in_ch + base_Tin - 1) / base_Tin);
	ret->payload_size = require_bytes;

	ret->payload = 0;
	//printf("**** weight->payload=%d\n", ret->payload);
	return ret;
}

int main_mvm(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int win = 19;
    int hin = (mat_h + win - 1) / win;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int relu_en = 0;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    struct MVM_Cfg cfg = Get_MVM_Cfg(hin, win, chin, chout, in_dw, wt_dw, wt_sc_dw, 2);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_MVM(cfg, relu_en, mode_mvm, feature_in, weight, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_mvm_bn(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int hin = mat_h;
    int win = 1;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int relu_en = 0;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_bn_base_addr = 0x4000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_bn = Malloc_Feature(1, 1, 2*chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    struct MVM_Cfg cfg = Get_MVM_Cfg(hin, win, chin, chout, in_dw, wt_dw, wt_sc_dw, 1);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_bn->payload = (short*)dat_bn_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_MVM_BN(cfg, relu_en, mode_mvm, feature_in, weight, feature_bn, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_mvm_bn_res(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int hin = mat_h;
    int win = 1;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int relu_en = 0;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_bn_base_addr = 0x4000000;
    int dat_add_base_addr = 0x6000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_bn = Malloc_Feature(1, 1, 2*chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_add = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    struct MVM_Cfg cfg = Get_MVM_Cfg(hin, win, chin, chout, in_dw, wt_dw, wt_sc_dw, 1);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_bn->payload = (short*)dat_bn_base_addr;
    feature_add->payload = (short*)dat_add_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_MVM_BN_Res(cfg, relu_en, mode_mvm, feature_in, weight, feature_bn, feature_add, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_fc(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int win = 1;
    int hin = 1;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_RunHBM_FC(mode_fc, feature_in, weight, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_fc_bn(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int win = 1;
    int hin = 1;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int relu_en = 0;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_bn_base_addr = 0x4000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_bn = Malloc_Feature(1, 1, 2*chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_bn->payload = (short*)dat_bn_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_RunHBM_FC_BN(relu_en, mode_fc, feature_in, weight, feature_bn, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_fc_bn_res(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int win = 1;
    int hin = 1;
    int chin = mat_w;
    int chout = mat_o;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int relu_en = 0;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_bn_base_addr = 0x4000000;
    int dat_add_base_addr = 0x6000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    struct Mapped_Feature* feature_bn = Malloc_Feature(1, 1, 2*chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_add = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)wt_base_addr;
    feature_bn->payload = (short*)dat_bn_base_addr;
    feature_add->payload = (short*)dat_add_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_RunHBM_FC_BN_Res(relu_en, mode_fc, feature_in, weight, feature_bn, feature_add, feature_out, 0, 0, 0, 0);
    return 0;
}

int main_act(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int win = mat_h;
    int hin = 1;
    int chin = mat_w;
    int chout = chin;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x2000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_Run_Activation(wt_base_addr, feature_in, feature_out, 0);
    return 0;
}

int main_ln(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int hin = mat_h;
    int win = 1;
    int chin = mat_w;
    int chout = chin;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x0000000;
    int wt_base_addr = 0x1000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_wt = Malloc_Feature(ky, kx, chin*2, scale, conv_out_scale, wt_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    feature_wt->payload = (short*)wt_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_Run_LN(feature_in, feature_wt, feature_out, mat_h, 0, 0);
    return 0;
}

int main_softmax(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int win = mat_h;
    int hin = 2;
    int chin = mat_w;
    int chout = chin;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_Run_Softmax(feature_in, feature_out, 0);
    return 0;
}

int main_transpose(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int win = mat_h;
    int hin = 1;
    int chin = mat_w;
    int chout = win;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x0000000;
    int wt_base_addr = 0x2000000;
    int dat_out_base_addr = 0x0000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)dat_out_base_addr;
    FPGA_Run_Transpose(feature_in, weight, 0);
    return 0;
}

int main_feature2weight(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int hin = mat_h;
    int win = 1;
    int chin = mat_w;
    int chout = win;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x0000000;
    int wt_base_addr = 0x2000000;
    int dat_out_base_addr = 0x0000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Weight* weight = Malloc_Weight(ky, kx, chin, chout, scale, wt_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    weight->payload = (short*)dat_out_base_addr;
    FPGA_Run_Feature2Weight(feature_in, weight, mat_h, 0);
    return 0;
}

int main_elementwise(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_o = atoi(argv[4]);
    int hin = mat_h;
    int win = mat_w;
    int chin = mat_o;
    int chout = chin;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int in_a_base_addr = 0x0000000;
    int in_b_base_addr = 0x1000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_a = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_b = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_a->payload = (short*)in_a_base_addr;
    feature_b->payload = (short*)in_b_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_Run_Elementwise(element_minus, feature_a, feature_b, feature_out, 0);
    return 0;
}

int main_posemb(int argc, char** argv) {
    int mat_h = atoi(argv[2]);
    int mat_w = atoi(argv[3]);
    int mat_c = atoi(argv[4]);
    int Pos_Num = 100;
    int hin = mat_h;
    int win = mat_w;
    int chin = mat_c;
    int chout = chin;
    int scale = 0;
    int conv_out_scale = 0;
    int in_dw = 16;
    int ky = 1;
    int kx = 1;
    int wt_dw = 4;
    int hout = hin;
    int wout = win;
    int wt_sc_dw = 0;
    int dat_in_base_addr = 0x1000000;
    int wt_base_addr = 0x0000000;
    int dat_out_base_addr = 0x8000000;
    struct Mapped_Feature* feature_in = Malloc_Feature(hin, win, chin, scale, conv_out_scale, in_dw);
    struct Mapped_Feature* feature_wt = Malloc_Feature(ky, kx, chin*2, scale, conv_out_scale, wt_dw);
    struct Mapped_Feature* feature_out = Malloc_Feature(hout, wout, chout, scale, conv_out_scale, in_dw);
    feature_in->payload = (short*)dat_in_base_addr;
    feature_wt->payload = (short*)wt_base_addr;
    feature_out->payload = (short*)dat_out_base_addr;
    FPGA_Run_PosEmb(feature_in, feature_wt, feature_out, Pos_Num);
    return 0;
}

int main(int argc, char** argv) {
    if (!strcmp(argv[1], "ln")) {
        return main_ln(argc, argv);
    } else if (!strcmp(argv[1], "mvm")) {
        return main_mvm(argc, argv);
    } else if (!strcmp(argv[1], "mvm_bn")) {
        return main_mvm_bn(argc, argv);
    } else if (!strcmp(argv[1], "mvm_bn_res")) {
        return main_mvm_bn_res(argc, argv);
    } else if (!strcmp(argv[1], "fc")) {
        return main_fc(argc, argv);
    } else if (!strcmp(argv[1], "fc_bn")) {
        return main_fc_bn(argc, argv);
    } else if (!strcmp(argv[1], "fc_bn_res")) {
        return main_fc_bn_res(argc, argv);
    } else if (!strcmp(argv[1], "act")) {
        return main_act(argc, argv);
    } else if (!strcmp(argv[1], "softmax")) {
        return main_softmax(argc, argv);
    } else if (!strcmp(argv[1], "transpose")) {
        return main_transpose(argc, argv);
    } else if (!strcmp(argv[1], "feature2weight")) {
        return main_feature2weight(argc, argv);
    } else if (!strcmp(argv[1], "elementwise")) {
        return main_elementwise(argc, argv);
    } else if (!strcmp(argv[1], "pos_emb")) {
        return main_posemb(argc, argv);
    } else {
        printf("\"%s\"\n", argv[1]);
        return 1;
    }
}
