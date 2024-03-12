#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "FPGA_MEM.h"
#include "basic.h"
#include "conv.h"
#include "matrix.h"
#include "others.h"
#include "codegen.h"

/*************************************************************************************************/
/*                                        算子封装                                               */
/*************************************************************************************************/
/*
 * name: Transpose
 * infer: Feature(1, W, C) -> Weight(1, 1, C, W)
 */
struct Mapped_Weight* transpose(struct Mapped_Feature* input, int life_cycle=1, int log2_Bank_Step=28) {
    struct Mapped_Weight* output = Malloc_Weight(1, 1, input->channel, input->width, 0, 4, life_cycle);
    FPGA_Run_Transpose(input, output, 0, 0, log2_Bank_Step, 0);
    return output;
}

/*
 * name: Feature2Weight
 * infer: Feature(1, W, C) -> Weight(1, 1, W, C)
 */
struct Mapped_Weight* feature2weight(struct Mapped_Feature* input, int life_cycle=1, int log2_Bank_Step=28) {
    struct Mapped_Weight* output = Malloc_Weight(1, 1, input->width, input->channel, 0, 4, life_cycle);
    FPGA_Run_Feature2Weight(input, output, 0, 0, log2_Bank_Step, 0);
    return output;
}

/*
 * name: LayerNorm
 * infer: Feature(H, W, C), Feature(1, 1, 2xC) -> Feature(H, W, C)
 */
struct Mapped_Feature* layernorm(struct Mapped_Feature* input, struct Mapped_Feature* wt_and_bias, int life_cycle=1) {
    if (wt_and_bias->height != 1)
        std::cerr << "* WARNNING! layernorm wt_and_bias height wants 1, but get " << wt_and_bias->height << std::endl;
    if (wt_and_bias->width != 1)
        std::cerr << "* WARNNING! layernorm wt_and_bias width wants 1, but get " << wt_and_bias->width << std::endl;
    if (wt_and_bias->channel != input->channel*2) 
        std::cerr << "* WARNNING! layernorm wt_and_bias channel wants input->channel*2(" << input->channel*2
                  << "), but get " << wt_and_bias->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16, life_cycle);
    FPGA_Run_LN(input, wt_and_bias, output, 0, 0, 0);
    return output;
}

/*
 * name: Softmax
 * infer: Feature(H, W, C) -> Feature(H, W, C)
 */
struct Mapped_Feature* softmax(struct Mapped_Feature* input, int life_cycle=1) {
    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16, life_cycle);
    FPGA_Run_Softmax(input, output, 0, 0);
    return output;
}

/*
 * name: Activate
 * infer: Feature(H, W, C), parameters=Feature(1, 1, 32x3)(16bit) -> Feature(H, W, C)
 */
struct Mapped_Feature* activate(struct Mapped_Feature* input, struct Mapped_Feature* parameters, int life_cycle=1) {
    if (parameters->channel*parameters->height*parameters->width > 96) 
        std::cerr << "* WARNNING! activate parameters needs 96x16 bits, but get " << parameters->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16, life_cycle);
    FPGA_Run_Activation((uint64_t)parameters->payload, input, output, 0, 0);
    return output;
}

/*
 * name: PosEmb
 * infer: Feature(H, W, C), Feature(X, X, X)(考虑数据大小即可，驱动只传入地址) -> Feature(H, W, C)
 */
struct Mapped_Feature* rotary_pos_emb(struct Mapped_Feature* input, struct Mapped_Feature* pos_in, int pos_num, int life_cycle=1) {
    if (pos_in->height != 1)
        std::cerr << "* WARNNING! rotary_pos_emb pos_in height wants 1, but get " << pos_in->height << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16, life_cycle);
    FPGA_Run_PosEmb(input, pos_in, output, pos_num, 0, 0);
    return output;
}

/*
 * name: MVM
 * infer: Feature(H, W, Cin), Weight(1, 1, Cin, Cout) -> Feature(H, W, Cout)
 */
struct Mapped_Feature* mvm(struct Mapped_Feature* input, struct Mapped_Weight* weight, int skip_factor, int life_cycle=1, int log2_Bank_Step=28) {
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16, life_cycle);
    FPGA_RunHBM_MVM(0, mode_mvm, input, weight, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

/*
 * name: MVM_BN
 * infer: Feature(H, W, Cin), Weight(1, 1, Cin, Cout), Feature(1, 1, 2xCout) -> Feature(H, W, Cout)
 */
struct Mapped_Feature* mvm_bn(struct Mapped_Feature* input, struct Mapped_Weight* weight, struct Mapped_Feature* bn, int skip_factor, int life_cycle=1, int log2_Bank_Step=28) {
    if (input == nullptr || weight == nullptr)
        exit(-1);
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;
    if (bn->height != 1)
        std::cerr << "* WARNNING! mvm_bn bn height wants 1, but get " << bn->height << std::endl;
    if (bn->width != 1)
        std::cerr << "* WARNNING! mvm_bn bn width wants 1, but get " << bn->width << std::endl;
    if (bn->channel != weight->out_ch*2) 
        std::cerr << "* WARNNING! mvm_bn bn channel wants input->channel*2(" << input->channel*2
                  << "), but get " << bn->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16, life_cycle);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, input, weight, bn, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

/*
 * name: MVM_BN_RES
 * infer: Feature(H, W, Cin), Weight(1, 1, Cin, Cout), Feature(1, 1, 2xCout), Feature(H, W, Cout) -> Feature(H, W, Cout)
 */
struct Mapped_Feature* mvm_bn_res(struct Mapped_Feature* input, struct Mapped_Weight* weight, struct Mapped_Feature* bn, struct Mapped_Feature* res, int skip_factor, int life_cycle=1, int log2_Bank_Step=28) {
    if (input == nullptr || weight == nullptr || bn == nullptr)
        exit(-1);
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn_res wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;
    if (bn->height != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn height wants 1, but get " << bn->height << std::endl;
    if (bn->width != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn width wants 1, but get " << bn->width << std::endl;
    if (bn->channel != weight->out_ch*2) 
        std::cerr << "* WARNNING! mvm_bn_res bn channel wants input->channel*2(" << input->channel*2
                  << "), but get " << bn->channel << std::endl;
    if (res->height != input->height)
        std::cerr << "* WARNNING! mvm_bn_res res height wants input->height(" << input->height << "), but get " << res->height << std::endl;
    if (res->width != input->width)
        std::cerr << "* WARNNING! mvm_bn_res res width wants input->width(" << input->width << "), but get " << res->width << std::endl;
    if (res->channel != weight->out_ch) 
        std::cerr << "* WARNNING! mvm_bn_res res channel wants weight->out_ch(" << weight->out_ch 
                  << "), but get " << res->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16, life_cycle);
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn, input, weight, bn, res, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

/*
 * name: MVM_BN_RES_AUGMAX
 * infer: Feature(H, W, Cin), Weight(1, 1, Cin, Cout), Feature(1, 1, 2xCout), Feature(H, W, Cout) -> Feature(H, W, Cout)
 */
struct Mapped_Feature* mvm_bn_res_augmax(struct Mapped_Feature* input, struct Mapped_Weight* weight, struct Mapped_Feature* bn, struct Mapped_Feature* res, int skip_factor, int life_cycle=1, int log2_Bank_Step=28) {
    if (input == nullptr || weight == nullptr || bn == nullptr)
        exit(-1);
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn_res wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;
    if (bn->height != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn height wants 1, but get " << bn->height << std::endl;
    if (bn->width != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn width wants 1, but get " << bn->width << std::endl;
    if (bn->channel != weight->out_ch*2) 
        std::cerr << "* WARNNING! mvm_bn_res bn channel wants input->channel*2(" << input->channel*2
                  << "), but get " << bn->channel << std::endl;
    if (res->height != input->height)
        std::cerr << "* WARNNING! mvm_bn_res res height wants input->height(" << input->height << "), but get " << res->height << std::endl;
    if (res->width != input->width)
        std::cerr << "* WARNNING! mvm_bn_res res width wants input->width(" << input->width << "), but get " << res->width << std::endl;
    if (res->channel != weight->out_ch) 
        std::cerr << "* WARNNING! mvm_bn_res res channel wants weight->out_ch(" << weight->out_ch 
                  << "), but get " << res->channel << std::endl;

    struct Mapped_Feature* augmax = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16, life_cycle);
    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16, life_cycle);
    FPGA_RunHBM_MVM_BN_Res_AugMax(0, mode_mvm_bn_res_aug, input, weight, bn, res, output, augmax, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return augmax;
}

/*
 * name: ElementWise_Add
 * infer: Feature(H, W, C), Feature(H, W, C) -> Feature(H, W, C)
 */
struct Mapped_Feature* add(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b, int life_cycle=1) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! add input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! add input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! add input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16, life_cycle);
    FPGA_Run_Elementwise(element_add, input_a, input_b, output, 0, 0);
    return output;
}

/*
 * name: ElementWise_minus
 * infer: Feature(H, W, C), Feature(H, W, C) -> Feature(H, W, C)
 */
struct Mapped_Feature* minus(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b, int life_cycle=1) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! minus input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! minus input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! minus input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16, life_cycle);
    FPGA_Run_Elementwise(element_minus, input_a, input_b, output, 0, 0);
    return output;
}

/*
 * name: ElementWise_mul
 * infer: Feature(H, W, C), Feature(H, W, C) -> Feature(H, W, C)
 */
struct Mapped_Feature* mul(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b, int life_cycle=1) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! mul input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! mul input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! mul input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16, life_cycle);
    FPGA_Run_Elementwise(element_mul, input_a, input_b, output, 0, 0);
    return output;
}
/*************************************************************************************************/

struct Mapped_Feature* ChatGLM_Block_Ops(
                      int token, 
                      int end,
                      struct Mapped_Feature* embedded_input,
                      struct Mapped_Feature* LN_k_bias,
                      struct Mapped_Weight* qkv_weight,
                      struct Mapped_Feature* qkv_bn_bias,
                      struct Mapped_Feature* pos_emb,
                      struct Mapped_Feature* rsqrt,
                      struct Mapped_Feature* mask,
                      struct Mapped_Weight* atten_weight,
                      struct Mapped_Feature* atten_bn,
                      struct Mapped_Feature* post_atten_k_bias,
                      struct Mapped_Weight* dense_h_to_4h_0,
                      struct Mapped_Feature* dense_h_to_4h_bn,
                      struct Mapped_Weight* dense_4h_to_h_0,
                      struct Mapped_Feature* dense_4h_to_h_bn,
                      struct Mapped_Feature* silu_act
                      ) {
    // OP 1, 2
    auto ln_output = layernorm(embedded_input, LN_k_bias, 1); // (1, token, 4096)
    Reshape(ln_output, token, 1, 4096); // (token, 1, 4096)
    auto qkv_output = mvm_bn(ln_output, qkv_weight, qkv_bn_bias, 1); // (token, 1, 128*36)
    auto qkv_split = Split(qkv_output, {4096, 128, 128, 128, 128});
    Reshape(qkv_split[0], 32, token, 128); // (32, token, 128)
    Reshape(qkv_split[1],  1, token, 128); // ( 1, token, 128)
    Reshape(qkv_split[2],  1, token, 128); // ( 1, token, 128)
    Reshape(qkv_split[3],  1, token, 128); // ( 1, token, 128)
    Reshape(qkv_split[4],  1, token, 128); // ( 1, token, 128)
    std::cout << ln_output << std::endl;
    std::cout << qkv_output << std::endl;
    std::cout << qkv_split[0] << std::endl;
    std::cout << qkv_split[1] << std::endl;
    std::cout << qkv_split[2] << std::endl;
    std::cout << qkv_split[3] << std::endl;
    std::cout << qkv_split[4] << std::endl;

    auto scores = Malloc_Feature(32, token, token, 0, 0, 16, 1);
    // Split(f, axis=0) 复制一份Feature并且将复制的Feature的地址做Hin=1情况下的偏移，返回size=2的vector
    std::vector<struct Mapped_Feature*> scores_split = Split(scores, 0);

    // OP 3
    auto emb_query = rotary_pos_emb(qkv_split[0], pos_emb, token);
    std::vector<struct Mapped_Feature*> emb_query_split = Split(emb_query, 0); 

    // OP 4, 5, 6
    auto emb_key_0 = rotary_pos_emb(qkv_split[1], pos_emb, token);
    auto quan_key_0 = transpose(emb_key_0);
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn_res, emb_query_split[0], quan_key_0, rsqrt, mask, scores_split[0], 2, 0, 0, 0, 0, 0, 28, 0);

    // OP 7, 8, 9, 10
    auto emb_key_1 = rotary_pos_emb(qkv_split[2], pos_emb, token);
    auto quan_key_1 = transpose(emb_key_1);
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn_res, emb_query_split[1], quan_key_1, rsqrt, mask, scores_split[1], 2, 0, 0, 0, 0, 0, 28, 0);
    auto softmax_scores = softmax(scores);

    auto atten_output = Malloc_Feature(32, token, 128, 0, 0, 16, 1);
    std::vector<struct Mapped_Feature*> softmax_split = Split(softmax_scores, 0); 
    std::vector<struct Mapped_Feature*> atten_output_split = Split(atten_output, 0); 

    // OP 11, 12, 13, 14
    auto quan_value_0 = feature2weight(qkv_split[3]);
    FPGA_RunHBM_MVM(0, mode_mvm, softmax_split[0], quan_value_0, atten_output_split[0], 2, 0, 0, 0, 0, 0, 28, 0);
    auto quan_value_1 = feature2weight(qkv_split[4]);
    FPGA_RunHBM_MVM(0, mode_mvm, softmax_split[1], quan_value_1, atten_output_split[1], 2, 0, 0, 0, 0, 0, 28, 0);
    std::cout << "atten_output: " << atten_output << std::endl;

    Reshape(atten_output, token, 1, 128*32);
    Reshape(embedded_input, token, 1, 128*32);
    // OP 15
    auto res_output = mvm_bn_res(atten_output, atten_weight, atten_bn, embedded_input, 1, 2);
    std::cout << "res_output: " << res_output << std::endl;

    // OP 16, 17
    Reshape(res_output, 1, token, 128*32);
    auto post_norm_output = layernorm(res_output, post_atten_k_bias);
    Reshape(post_norm_output, token, 1, 128*32);
    auto dense_4h_output = mvm_bn(post_norm_output, dense_h_to_4h_0, dense_h_to_4h_bn, 1);

    // OP 18-ACT，实际为2个算子
    std::vector<struct Mapped_Feature*> dense_4h_output_split = Split(dense_4h_output, 2);
    auto silu_output = activate(dense_4h_output_split[0], silu_act);
    auto act_output = mul(silu_output, dense_4h_output_split[1]);
    std::cout << "act_output: " << act_output << std::endl;

    // OP 19
    Reshape(res_output, token, 1, 128*32);
    if (end) {
        auto atten_scores = mvm_bn_res_augmax(act_output, dense_4h_to_h_0, dense_4h_to_h_bn, res_output, 1, 2);
        std::cout << "atten_scores: " << atten_scores << std::endl;
        return atten_scores;
    } else {
        auto atten_scores = mvm_bn_res(act_output, dense_4h_to_h_0, dense_4h_to_h_bn, res_output, 1, 2);
        std::cout << "atten_scores: " << atten_scores << std::endl;
        return atten_scores;
    }
}

void ChatGLM_Block_Ops_v1(
    int token,
    int end,
    struct Mapped_Feature* mask,
    struct Mapped_Feature* pos_emb,
    struct Mapped_Feature* silu_act,
    struct Mapped_Feature* LN_k_bias,
    struct Mapped_Weight* qkv_weight,
    struct Mapped_Feature* qkv_bn_bias,
    struct Mapped_Feature* rsqrt_bn,
    struct Mapped_Weight* atten_weight,
    struct Mapped_Feature* atten_bn,
    struct Mapped_Feature* post_atten_k_bias,
    struct Mapped_Weight* dense_h_to_4h_0,
    struct Mapped_Feature* dense_h_to_4h_bn_0,
    struct Mapped_Weight* dense_h_to_4h_1,
    struct Mapped_Feature* dense_h_to_4h_bn_1,
    struct Mapped_Weight* dense_4h_to_h,
    struct Mapped_Feature* dense_4h_to_h_bn,
    struct Mapped_Feature* ddr_buffer_a,
    struct Mapped_Feature* ddr_buffer_b,
    struct Mapped_Feature* ddr_buffer_c,
    struct Mapped_Feature* ddr_buffer_d,
    struct Mapped_Feature* ddr_buffer_e,
    struct Mapped_Feature* ddr_buffer_f,
    struct Mapped_Feature* ddr_buffer_g,
    struct Mapped_Feature* ddr_buffer_h,
    struct Mapped_Weight* hbm_buffer_trp0,
    struct Mapped_Weight* hbm_buffer_trp1,
    struct Mapped_Weight* hbm_buffer_f2w0,
    struct Mapped_Weight* hbm_buffer_f2w1
) {
    // step 1 a -> b
    Reset(ddr_buffer_a, 1, token, 4096);
    Reset(ddr_buffer_b, 1, token, 4096);
    FPGA_Run_LN(ddr_buffer_a, LN_k_bias, ddr_buffer_b, 0, 0);

    // step 2 b -> c
    Reset(ddr_buffer_b, 1, token, 4096);
    Reset(ddr_buffer_c, 1, token, 4608);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, ddr_buffer_b, qkv_weight, qkv_bn_bias, ddr_buffer_c, 1, 0, 0, 0, 0, 0, 28, 0);

    std::vector<struct Mapped_Feature*> qkv_split = Split(ddr_buffer_c, {4096, 128, 128, 128, 128});

    // step 3 c[:4096] -> b
    Reset(qkv_split[0], 32, token, 128);
    Reset(ddr_buffer_b, 32, token, 128);
    FPGA_Run_PosEmb(qkv_split[0], pos_emb, ddr_buffer_b, token, 0);

    Reset(ddr_buffer_e, 32, token, 32);
    std::vector<struct Mapped_Feature*> b_split = Split(ddr_buffer_b, 0);
    std::vector<struct Mapped_Feature*> e_split_0 = Split(ddr_buffer_e, 0);

    // step 4 c[4096:4096+128] -> d
    Reset(qkv_split[1], 1, token, 128);
    Reset(ddr_buffer_d, 1, token, 128);
    FPGA_Run_PosEmb(qkv_split[1], pos_emb, ddr_buffer_d, token, 0);

    // step 5 d -> hbm_trp0
    Reset(hbm_buffer_trp0, 128, 32);
    FPGA_Run_Transpose(ddr_buffer_d, hbm_buffer_trp0, 0, 0, 28, 0);

    // step 6 b[offset=0], hbm_trp0 -> e[offset=0]
    Reset(b_split[0], 16, token, 128);
    FPGA_RunHBM_MVM_BN_Res_afterTRP(0, mode_mvm_bn_res_trp, b_split[0], hbm_buffer_trp0, rsqrt_bn, mask, e_split_0[0], 1, 0, 0, 0, 0, 0, 28, 0);

    // step 7 c[4096+128:4096+128*2] -> d
    Reset(qkv_split[2], 1, token, 128);
    Reset(ddr_buffer_d, 1, token, 128);
    FPGA_Run_PosEmb(qkv_split[2], pos_emb, ddr_buffer_d, token, 0);

    // step 8 d -> hbm_trp1
    Reset(hbm_buffer_trp1, 128, 32);
    FPGA_Run_Transpose(ddr_buffer_d, hbm_buffer_trp1, 0, 0, 28, 0);

    // step 9 b[offset=tokenx128], hbm_trp1 -> e[offset=tokenx128]
    Reset(b_split[1], 16, token, 128);
    FPGA_RunHBM_MVM_BN_Res_afterTRP(0, mode_mvm_bn_res_trp, b_split[1], hbm_buffer_trp1, rsqrt_bn, mask, e_split_0[1], 1, 0, 0, 0, 0, 0, 28, 0);

    // step 10 e -> f
    Reset(ddr_buffer_e, 32, token, token);
    Reset(ddr_buffer_f, 32, token, token);
    FPGA_Run_Softmax(ddr_buffer_e, ddr_buffer_f, 0);

    Reset(ddr_buffer_e, 32, token, 128);
    std::vector<struct Mapped_Feature*> e_split_1 = Split(ddr_buffer_e, 0);
    std::vector<struct Mapped_Feature*> f_split = Split(ddr_buffer_f, 0);

    // step 11 c[4096+128*2:4096+128*3] -> hbm_f2w0
    Reset(qkv_split[3], 1, token, 128);
    Reset(hbm_buffer_f2w0, token, 128);
    FPGA_Run_Feature2Weight(qkv_split[3], hbm_buffer_f2w0, 0, 0, 28, 0);

    // step 12 f[offset=0], hbm_f2w0 -> e[offset=0]
    FPGA_RunHBM_MVM_afterF2W(0, mode_mvm_f2w, f_split[0], hbm_buffer_f2w0, e_split_1[0], 2, 0, 0, 0, 0, 0, 28, 0);

    // step 13 c[4096+128*3:4096+128*4] -> hbm_f2w1
    Reset(qkv_split[4], 1, token, 128);
    Reset(hbm_buffer_f2w1, token, 128);
    FPGA_Run_Feature2Weight(qkv_split[4], hbm_buffer_f2w1, 0, 0, 28, 0);

    // step 14 f[offset=tokenx128], hbm_f2w1 -> e[offset=tokenx128]
    FPGA_RunHBM_MVM_afterF2W(0, mode_mvm_f2w, f_split[1], hbm_buffer_f2w1, e_split_1[1], 2, 0, 0, 0, 0, 0, 28, 0);

    // step 15 e, x, x, a, -> b
    Reset(ddr_buffer_e, 1, token, 4096);
    Reset(ddr_buffer_a, 1, token, 4096);
    Reset(ddr_buffer_b, 1, token, 4096); // WARNING!
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn_res, ddr_buffer_e, atten_weight, atten_bn, ddr_buffer_a, ddr_buffer_b, 1, 0, 0, 0, 0, 0, 28, 0);

    // step 16 b -> a
    Reset(ddr_buffer_b, 1, token, 4096);
    Reset(ddr_buffer_a, 1, token, 4096);
    FPGA_Run_LN(ddr_buffer_b, post_atten_k_bias, ddr_buffer_a, 0, 0);

    // step 17 a, x, x -> g
    Reset(ddr_buffer_a, 1, token, 4096);
    Reset(ddr_buffer_g, 1, token, 13696);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, ddr_buffer_a, dense_h_to_4h_0, dense_h_to_4h_bn_0, ddr_buffer_g, 1, 0, 0, 0, 0, 0, 28, 0);

    // step 18 g -> h
    Reset(ddr_buffer_g, token, 1, 13696);
    Reset(ddr_buffer_h, token, 1, 13696);
    FPGA_Run_Activation((uint64_t)silu_act->payload, ddr_buffer_g, ddr_buffer_h, 0, 0);

    // step 19 a, x, x, h -> g
    Reset(ddr_buffer_a, 1, token, 4096);
    Reset(ddr_buffer_h, 1, token, 13696);
    Reset(ddr_buffer_g, 1, token, 13696);
    FPGA_RunHBM_MVM_BN_Res(2, mode_mvm_bn_res, ddr_buffer_a, dense_h_to_4h_1, dense_h_to_4h_bn_1, ddr_buffer_h, ddr_buffer_g, 1, 0, 0, 0, 0, 0, 28, 0);

    // step 20 g, x, x, b -> a
    Reset(ddr_buffer_g, 1, token, 13696);
    Reset(ddr_buffer_b, 1, token, 4096);
    Reset(ddr_buffer_a, 1, token, 4096);
    if (end) {
        Reset(ddr_buffer_e, 1, token, 4096);
        FPGA_RunHBM_MVM_BN_Res_ArgMax(0, mode_mvm_bn_res_aug, ddr_buffer_g, dense_4h_to_h, dense_4h_to_h_bn, ddr_buffer_b, ddr_buffer_e, ddr_buffer_a, 1, 0, 0, 0, 0, 0, 28, 0);
    } else {
        FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn_res, ddr_buffer_g, dense_4h_to_h, dense_4h_to_h_bn, ddr_buffer_b, ddr_buffer_a, 1, 0, 0, 0, 0, 0, 28, 0);
    }
}

int ChatGLM_Block(int token, int block_size, int maxtoken) {
    WeightAddrAnnotate("Global weights for token=" + std::to_string(token));
    struct Mapped_Feature* pos_emb = WeightAddr(Malloc_Feature(1, 128, 64, 0, 0, 16), "pos_emb"); // WARNING!
    struct Mapped_Feature* silu_act = WeightAddr(Malloc_Feature(1, 1, 32 * 3, 0, 0, 8), "silu_act");
    struct Mapped_Feature* mask = WeightAddr(Malloc_Feature(32, token, token, 0, 0, 16), "mask_" + std::to_string(token));

    std::vector<struct Mapped_Feature*> LN_k_bias;
    std::vector<struct Mapped_Feature*> qkv_bn_bias;
    std::vector<struct Mapped_Feature*> rsqrt_bn;
    std::vector<struct Mapped_Feature*> atten_bn;
    std::vector<struct Mapped_Feature*> post_atten_k_bias;
    std::vector<struct Mapped_Feature*> dense_h_to_4h_bn_0;
    std::vector<struct Mapped_Feature*> dense_h_to_4h_bn_1;
    std::vector<struct Mapped_Feature*> dense_4h_to_h_bn;

    std::vector<struct Mapped_Weight*> qkv_weight;
    std::vector<struct Mapped_Weight*> atten_weight;
    std::vector<struct Mapped_Weight*> dense_h_to_4h_0;
    std::vector<struct Mapped_Weight*> dense_h_to_4h_1;
    std::vector<struct Mapped_Weight*> dense_4h_to_h;

    for (int i = 0; i < block_size; i++) {
        std::stringstream prefix_;
        prefix_ << "BLOCK" << std::setw(2) << std::setfill('0') << i << "_";
        std::string prefix = prefix_.str();
        WeightAddrAnnotate("Block " + std::to_string(i) + " start");
        LN_k_bias.push_back(WeightAddr(Malloc_Feature(1, 1, 4096*2, 0, 0, 16), prefix + "LN_k_bias"));
        qkv_bn_bias.push_back(WeightAddr(Malloc_Feature(1, 1, 2*128*36, 0, 0, 16), prefix + "qkv_bn_bias"));
        rsqrt_bn.push_back(WeightAddr(Malloc_Feature(1, 1, 2*128*32, 0, 0, 16), prefix + "rsqrt_bn"));
        atten_bn.push_back(WeightAddr(Malloc_Feature(1, 1, 4096*2, 0, 0, 16), prefix + "atten_bn"));
        post_atten_k_bias.push_back(WeightAddr(Malloc_Feature(1, 1, 4096*2, 0, 0, 16), prefix + "post_atten_k_bias"));
        dense_h_to_4h_bn_0.push_back(WeightAddr(Malloc_Feature(1, 1, 13696*2, 0, 0, 16), prefix + "dense_h_to_4h_bn_0"));
        dense_h_to_4h_bn_1.push_back(WeightAddr(Malloc_Feature(1, 1, 13696*2, 0, 0, 16), prefix + "dense_h_to_4h_bn_1"));
        dense_4h_to_h_bn.push_back(WeightAddr(Malloc_Feature(1, 1, 4096*2, 0, 0, 16), prefix + "dense_4h_to_h_bn"));

        qkv_weight.push_back(WeightAddr(Malloc_Weight(1, 1, 4096, 128*36, 0, 4), prefix + "qkv_weight"));
        atten_weight.push_back(WeightAddr(Malloc_Weight(1, 1, 4096, 4096, 0, 4), prefix + "atten_weight"));
        dense_h_to_4h_0.push_back(WeightAddr(Malloc_Weight(1, 1, 4096, 13696, 0, 4), prefix + "dense_h_to_4h_0"));
        dense_h_to_4h_1.push_back(WeightAddr(Malloc_Weight(1, 1, 4096, 13696, 0, 4), prefix + "dense_h_to_4h_1"));
        dense_4h_to_h.push_back(WeightAddr(Malloc_Weight(1, 1, 13696, 4096, 0, 4), prefix + "dense_4h_to_h"));
        WeightAddrAnnotate("Block " + std::to_string(i) + " finish");
    }

    auto ddr_buffer_a = InputAddr(Malloc_Feature(1, maxtoken, 4096, 0, 0, 16), "runtime_ddr_A");
    auto ddr_buffer_b = InputAddr(Malloc_Feature(1, maxtoken, 4096, 0, 0, 16), "runtime_ddr_B");
    auto ddr_buffer_c = InputAddr(Malloc_Feature(1, maxtoken, 4608, 0, 0, 16), "runtime_ddr_C");
    auto ddr_buffer_d = InputAddr(Malloc_Feature(1, maxtoken, 4608, 0, 0, 16), "runtime_ddr_D");
    auto ddr_buffer_e = InputAddr(Malloc_Feature(maxtoken, maxtoken, 32, 0, 0, 16), "runtime_ddr_E");
    auto ddr_buffer_f = InputAddr(Malloc_Feature(maxtoken, maxtoken, 32, 0, 0, 16), "runtime_ddr_F");
    auto ddr_buffer_g = InputAddr(Malloc_Feature(1, maxtoken, 13696, 0, 0, 16), "runtime_ddr_G");
    auto ddr_buffer_h = InputAddr(Malloc_Feature(1, maxtoken, 13696, 0, 0, 16), "runtime_ddr_H");

    std::vector<struct Mapped_Weight*> hbm_buffer_trp0;
    std::vector<struct Mapped_Weight*> hbm_buffer_trp1;
    std::vector<struct Mapped_Weight*> hbm_buffer_f2w0;
    std::vector<struct Mapped_Weight*> hbm_buffer_f2w1;
    for (int i = 0; i < block_size; i++) {
        std::stringstream prefix_;
        prefix_ << "BLOCK" << std::setw(2) << std::setfill('0') << i << "_";
        std::string prefix = prefix_.str();
        hbm_buffer_trp0.push_back(WeightAddr(Malloc_Weight(1, 1, 128*2, token, 0, 4), prefix + "runtime_hbm_trp0"));
        hbm_buffer_trp1.push_back(WeightAddr(Malloc_Weight(1, 1, 128*2, token, 0, 4), prefix + "runtime_hbm_trp1"));
        hbm_buffer_f2w0.push_back(WeightAddr(Malloc_Weight(1, 1, 128*2, ((token+127)/128)*128, 0, 4), prefix + "runtime_hbm_f2w0"));
        hbm_buffer_f2w1.push_back(WeightAddr(Malloc_Weight(1, 1, 128*2, ((token+127)/128)*128, 0, 4), prefix + "runtime_hbm_f2w1"));
    }

    FPGA_HBM_malloc(1024*1024);

    for (int i = 0; i < block_size; i++) {
        ChatGLM_Block_Ops_v1(token, 
                             i == block_size-1,
                             mask, 
                             pos_emb, 
                             silu_act,
                             LN_k_bias[i], 
                             qkv_weight[i], 
                             qkv_bn_bias[i], 
                             rsqrt_bn[i],
                             atten_weight[i], 
                             atten_bn[i], 
                             post_atten_k_bias[i], 
                             dense_h_to_4h_0[i], 
                             dense_h_to_4h_bn_0[i], 
                             dense_h_to_4h_1[i], 
                             dense_h_to_4h_bn_1[i],
                             dense_4h_to_h[i], 
                             dense_4h_to_h_bn[i], 
                             ddr_buffer_a,
                             ddr_buffer_b,
                             ddr_buffer_c,
                             ddr_buffer_d,
                             ddr_buffer_e,
                             ddr_buffer_f,
                             ddr_buffer_g,
                             ddr_buffer_h,
                             hbm_buffer_trp0[i],
                             hbm_buffer_trp1[i],
                             hbm_buffer_f2w0[i],
                             hbm_buffer_f2w1[i]
                             );
    }

    /*
     * /----------------\
     * | Code Generator |
     * \----------------/
     */
    CSB_Finish(); // 函数结束标志，生成 <name>0(HANDLE device) 函数
    std::string name = "ChatGLM_Block_" + std::to_string(token);
    std::ofstream source(name + ".h");
    Addr_Save_Model(source, "");
    CSB_Save_Model(source, name);
    //CSB_Save_Model_Burst(source, name);
    CSB_Clear(); // CSB记录清空，重新开始记录
    return 0;
}


// 单次程序只能生成一次，因为程序中的DDR、HBM地址信息，以及为了进行codegen所记录的Addr或者Attr都未清理
int main() {
    // test_n(19);
    // ACT_19x27392(19);
    // SCORES_32x19x19(19);
    // ChatGLM_Block(19, 28);
    // for (int i = 1; i <= 128; i++) {
    //     ChatGLM_Block(i, 28, 128);
    // }
    //

    auto data_in = InputAddr(Malloc_Feature(1, 19, 4096, 0, 0, 16), "mvm_data_k");
    auto q_weight = WeightAddr(Malloc_Weight(1, 1, 4096, 128*1, 0, 4), "k_weight");
    auto q_bn_bias = WeightAddr(Malloc_Feature(1, 1, 2*128*1, 0, 0, 16), "k_bn_bias");
    auto q_output = mvm_bn(data_in, q_weight, q_bn_bias, 1); // (token, 1, 128*36)
    std::cout << q_output << std::endl;

    /*
     * /----------------\
     * | Code Generator |
     * \----------------/
     */
    CSB_Finish(); // 函数结束标志，生成 <name>0(HANDLE device) 函数
    std::string name = "MVM_BN_4096x128";
    std::ofstream source(name + ".h");
    Addr_Save_Model(source, "");
    //CSB_Save_Model(source, name);
    CSB_Save_Model_Burst(source, name);
    CSB_Clear(); // CSB记录清空，重新开始记录
    return 0;
}
