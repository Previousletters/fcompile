#include <iostream>
#include <fstream>
#include <sstream>
#include "basic.h"
#include "conv.h"
#include "matrix.h"
#include "others.h"
#include "codegen.h"
#include "approx_pwlf_act.h"

int chatglm_block_main(int seq)
{
    /*
     * /---------------\
     * | Inputs define |
     * \---------------/
     * data             : feature
     * ln_weight        : feature
     * q_weight         : weight
     * q_bias           : feature
     * k_weight         : weight
     * k_bias           : feature
     * v_weight         : weight
     * v_bias           : feature
     * emb_q_pos        : feature
     * emb_k_pos        : feature
     * score_rsqrt      : feature
     * score_rsqrt      : feature
     * mvm_weight       : weight
     * mvm_bn           : feature
     * post_atten_ln    : feature
     * dense_h_to_4h_0  : weight
     * silu_act         : feature (act)
     * dense_4h_to_h_0  : weight
     */
    struct Mapped_Feature* data = InputAddr(Malloc_Feature(seq, 1, 4096, 0, 0, 16), "data");
    struct Mapped_Feature* ln_weight = InputAddr(Malloc_Feature(1, 1, 4096 * 2, 0, 0, 16), "ln_weight");
    struct Mapped_Weight* q_weight = InputAddr(Malloc_Weight(1, 1, 4096, 128 * 32, 0, 4), "q_weight");
    struct Mapped_Feature* q_bias = InputAddr(Malloc_Feature(1, 1, 256 * 32, 0, 0, 16), "q_bias");
    struct Mapped_Weight* k_weight = InputAddr(Malloc_Weight(1, 1, 4096, 128 * 2, 0, 4), "k_weight");
    struct Mapped_Feature* k_bias = InputAddr(Malloc_Feature(1, 1, 256 * 2, 0, 0, 16), "k_bias");
    struct Mapped_Weight* v_weight = InputAddr(Malloc_Weight(1, 1, 4096, 128 * 2, 0, 4), "v_weight");
    struct Mapped_Feature* v_bias = InputAddr(Malloc_Feature(1, 1, 256 * 2, 0, 0, 16), "v_bias");
    struct Mapped_Feature* emb_q_pos = InputAddr(Malloc_Feature(1, 1, 256 * 32, 0, 0, 16), "emb_q_pos");
    struct Mapped_Feature* emb_k_pos = InputAddr(Malloc_Feature(1, 1, 256 * 2, 0, 0, 16), "emb_k_pos");
    struct Mapped_Feature* score_rsqrt = InputAddr(Malloc_Feature(32, seq, seq, 0, 0, 16), "score_rsqrt");
    struct Mapped_Weight* mvm_weight = InputAddr(Malloc_Weight(1, 1, 4096, 4096, 0, 4), "mvm_weight");
    struct Mapped_Feature* mvm_bn = InputAddr(Malloc_Feature(1, 1, 4096*2, 0, 0, 16), "mvm_bn");
    struct Mapped_Feature* post_atten_ln = InputAddr(Malloc_Feature(1, 1, 4096 * 2, 0, 0, 16), "post_atten_ln");
    struct Mapped_Weight* dense_h_to_4h_0 = InputAddr(Malloc_Weight(1, 1, 4096, 27392, 0, 4), "dense_h_to_4h_0");
    struct Mapped_Feature* silu_act = InputAddr(Malloc_Feature(1, 1, 16 * 3, 0, 0, 16), "silu_act");
    struct Mapped_Weight* dense_4h_to_h_0 = InputAddr(Malloc_Weight(1, 1, 13696, 4096, 0, 4), "dense_4h_to_h_0");

    // LayerNorm
    struct Mapped_Feature* ln_out = Malloc_Feature(seq, 1, 4096, 0, 0, 16);
    FPGA_Run_LN(data, ln_weight, ln_out, 0, 0, 0);

    // Get QKV
    struct Mapped_Feature* q_data = Malloc_Feature(seq, 1, 128 * 32, 0, 0, 16);
    struct Mapped_Feature* k_data = Malloc_Feature(seq, 1, 128 * 2, 0, 0, 16);
    struct Mapped_Feature* v_data = Malloc_Feature(seq, 1, 128 * 2, 0, 0, 16);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, ln_out, q_weight, q_bias, q_data, 1, 0, 0, 0, 0, 0, 28, 0);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, ln_out, k_weight, k_bias, k_data, 1, 0, 0, 0, 0, 0, 28, 0);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, ln_out, v_weight, v_bias, v_data, 1, 0, 0, 0, 0, 0, 28, 0);
    std::cout << "q_data" << ": " << q_data << std::endl;
    std::cout << "k_data" << ": " << k_data << std::endl;
    std::cout << "v_data" << ": " << v_data << std::endl;

    // Rotary Pos Emb and Reshape (seq, 1, ch) -> (1, seq, ch)
    struct Mapped_Feature* emb_qury = Malloc_Feature(seq, 1, 128 * 32, 0, 0, 16);
    struct Mapped_Feature* emb_key  = Malloc_Feature(seq, 1, 128 * 2, 0, 0, 16);
    FPGA_Run_PosEmb(q_data, emb_q_pos, emb_qury, 100, 0);
    FPGA_Run_PosEmb(k_data, emb_k_pos, emb_key, 100, 0);
    Reshape(emb_qury, 32, seq, 128); // emb_qury: (32, seq, 128) // !!!!Warnning!!!!
    Reshape(emb_key, 1, seq, 128 * 2); // emb_key: (1, seq, 128*2) // !!!!Warnning!!!!
    Reshape(v_data, 1, seq, 128 * 2); // v_data: (1, seq, 128*2) // !!!!Warnning!!!!
    std::cout << "emb_qury" << ": " << emb_qury << std::endl;
    std::cout << "emb_key" << ": " << emb_key << std::endl;
    std::cout << "v_data" << ": " << v_data << std::endl;

    // Q*Kt // !!!!Warnning!!!!
    struct Mapped_Feature* scores = Malloc_Feature(32, seq, seq, 0, 0, 16);
    std::vector<struct Mapped_Feature*> emb_qury_split = Split(emb_qury, 0);
    struct Mapped_Weight* emb_key_t = Malloc_Weight(1, 1, 128*2, seq, 0, 4);
    std::vector<struct Mapped_Feature*> scores_split = Split(scores, 0);
    FPGA_Run_Transpose(emb_key, emb_key_t, 0, 0, 28, 0);
    for (int i = 0; i < 2; i++) {
        // QKt Path is not used !!! Check !!!
        FPGA_RunHBM_MVM(0, mode_mvm, emb_qury_split[i], emb_key_t, scores_split[i], 2, 0, 0, 0, 0, 0, 28, 0);
    }
    std::cout << "scores" << ": " << scores << std::endl;

    // softmax(element wire rsqurt)
    struct Mapped_Feature* scores_r = Malloc_Feature(32, seq, seq, 0, 0, 16);
    FPGA_Run_Elementwise(element_mul, scores, score_rsqrt, scores_r, 0);
    FPGA_Run_Softmax(scores_r, scores, 0); // use the before address
    std::cout << "scores" << ": " << scores << std::endl;

    // attention output
    scores_split = Split(scores, 0);
    struct Mapped_Feature* atten_output = Malloc_Feature(32, seq, 128, 0, 0, 16);
    std::vector<struct Mapped_Feature*> atten_out_split = Split(atten_output, 0);
    struct Mapped_Weight* v_data_w = Malloc_Weight(1, 1, seq, 128 * 2, 0, 4);
    FPGA_Run_Feature2Weight(v_data, v_data_w, 0, 0, 28, 0);
    for (int i = 0; i < 2; i++) {
        // Attention Path is not used !!! Check !!!
        FPGA_RunHBM_MVM(0, mode_mvm, scores_split[i], v_data_w, atten_out_split[i], 2, 0, 0, 0, 0, 0, 28, 0);
    }
    Reshape(atten_output, seq, 1, 128 * 32); // v_data: (seq, 1, 128 * 32)
    std::cout << "atten_output" << ": " << atten_output << std::endl;

    // mvm_bn_res
    struct Mapped_Feature* res_output = Malloc_Feature(seq, 1, 4096, 0, 0, 16);
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn_res, atten_output, mvm_weight, mvm_bn, data, res_output, 1, 0, 0, 0, 0, 0, 28, 0);
    std::cout << "res_output" << ": " << res_output << std::endl;

    // LayerNorm -> [? FC_BN -> ACT ?] -> FC_BN -> ElementWise Mul
    struct Mapped_Feature* post_norm_output = Malloc_Feature(seq, 1, 4096, 0, 0, 16);
    FPGA_Run_LN(res_output, post_atten_ln, post_norm_output, 0, 0, 0);
    struct Mapped_Feature* dense_4h_output = Malloc_Feature(seq, 1, 27392, 0, 0, 16);
    FPGA_RunHBM_MVM(0, mode_mvm, post_norm_output, dense_h_to_4h_0, dense_4h_output, 1, 0, 0, 0, 0, 0, 28, 0);
    std::vector<struct Mapped_Feature*> dense_4h_output_split = Split(dense_4h_output, 2);
    struct Mapped_Feature* silu_output = Malloc_Feature(seq, 1, 13696, 0, 0, 16);
    struct Mapped_Feature* act_output = Malloc_Feature(seq, 1, 13696, 0, 0, 16);
    FPGA_Run_Activation((uint64_t)silu_act->payload, dense_4h_output_split[0], silu_output, 0);
    FPGA_Run_Elementwise(element_mul, silu_output, dense_4h_output_split[1], act_output, 0);
    struct Mapped_Feature* dense_h_output = Malloc_Feature(seq, 1, 4096, 0, 0, 16);
    FPGA_RunHBM_MVM(0, mode_mvm, act_output, dense_4h_to_h_0, dense_h_output, 1, 0, 0, 0, 0, 0, 28, 0);
    struct Mapped_Feature* atten_scores = OutputAddr(Malloc_Feature(seq, 1, 4096, 0, 0, 16), "atten_scores");
    FPGA_Run_Elementwise(element_mul, res_output, dense_h_output, atten_scores, 0);
    std::cout << "atten_scores" << ": " << atten_scores << std::endl;
    
    /*
     * /----------------\
     * | Code Generator |
     * \----------------/
     */
    CSB_Finish(); //
    std::string name = "ChatGLM_";
    std::ofstream source("./chatglm_model.h");
    Addr_Save_Model(source, "");
    CSB_Save_Model(source, name);
    CSB_Clear();
    return 0;
}

/*************************************************************************************************/
struct Mapped_Weight* transpose(struct Mapped_Feature* input, int log2_Bank_Step) {
    struct Mapped_Weight* output = Malloc_Weight(1, 1, input->channel, input->width, 0, 4);
    FPGA_Run_Transpose(input, output, 0, 0, log2_Bank_Step, 0);
    return output;
}

struct Mapped_Weight* feature2weight(struct Mapped_Feature* input, int log2_Bank_Step) {
    struct Mapped_Weight* output = Malloc_Weight(1, 1, input->width, input->channel, 0, 4);
    FPGA_Run_Feature2Weight(input, output, 0, 0, log2_Bank_Step, 0);
    return output;
}

struct Mapped_Feature* layernorm(struct Mapped_Feature* input, struct Mapped_Feature* wt_and_bias) {
    if (wt_and_bias->height != 1)
        std::cerr << "* WARNNING! layernorm wt_and_bias height wants 1, but get " << wt_and_bias->height << std::endl;
    if (wt_and_bias->width != 1)
        std::cerr << "* WARNNING! layernorm wt_and_bias width wants 1, but get " << wt_and_bias->width << std::endl;
    if (wt_and_bias->channel != input->channel*2) 
        std::cerr << "* WARNNING! layernorm wt_and_bias channel wants input->channel*2(" << input->channel*2
                  << "), but get " << wt_and_bias->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16);
    FPGA_Run_LN(input, wt_and_bias, output, 0, 0, 0);
    return output;
}

struct Mapped_Feature* softmax(struct Mapped_Feature* input) {
    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16);
    FPGA_Run_Softmax(input, output, 0, 0);
    return output;
}

struct Mapped_Feature* activate(struct Mapped_Feature* input, struct Mapped_Feature* parameters) {
    if (parameters->channel*parameters->height*parameters->width > 48) 
        std::cerr << "* WARNNING! activate parameters needs 48x16 bits, but get " << parameters->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16);
    FPGA_Run_Activation((uint64_t)parameters->payload, input, output, 0, 0);
    return output;
}

struct Mapped_Feature* rotary_pos_emb(struct Mapped_Feature* input, struct Mapped_Feature* pos_in, int pos_num) {
    if (pos_in->height != 1)
        std::cerr << "* WARNNING! rotary_pos_emb pos_in height wants 1, but get " << pos_in->height << std::endl;
    if (pos_in->width != 1)
        std::cerr << "* WARNNING! rotary_pos_emb pos_in width wants 1, but get " << pos_in->width << std::endl;
    if (pos_in->channel != input->channel*2) 
        std::cerr << "* WARNNING! rotary_pos_emb pos_in channel wants input->channel*2(" << input->channel*2
                  << "), but get " << pos_in->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, input->channel, 0, 0, 16);
    FPGA_Run_PosEmb(input, pos_in, output, pos_num, 0, 0);
    return output;
}

struct Mapped_Feature* mvm(struct Mapped_Feature* input, struct Mapped_Weight* weight, int skip_factor, int log2_Bank_Step) {
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16);
    FPGA_RunHBM_MVM(0, mode_mvm, input, weight, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

struct Mapped_Feature* mvm_bn(struct Mapped_Feature* input, struct Mapped_Weight* weight, struct Mapped_Feature* bn, int skip_factor, int log2_Bank_Step) {
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;
    if (bn->height != 1)
        std::cerr << "* WARNNING! mvm_bn bn height wants 1, but get " << bn->height << std::endl;
    if (bn->width != 1)
        std::cerr << "* WARNNING! mvm_bn bn width wants 1, but get " << bn->width << std::endl;
    if (bn->channel != input->channel*2) 
        std::cerr << "* WARNNING! mvm_bn bn channel wants input->channel*2(" << input->channel*2
                  << "), but get " << bn->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16);
    FPGA_RunHBM_MVM_BN(0, mode_mvm_bn, input, weight, bn, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

struct Mapped_Feature* mvm_bn_res(struct Mapped_Feature* input, struct Mapped_Weight* weight, struct Mapped_Feature* bn, struct Mapped_Feature* res, int skip_factor, int log2_Bank_Step) {
    if (input->channel != weight->in_ch) 
        std::cerr << "* WARNNING! mvm_bn wants input->channel == weight->in_ch(" << weight->in_ch << "), but get " << input->channel << std::endl;
    if (bn->height != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn height wants 1, but get " << bn->height << std::endl;
    if (bn->width != 1)
        std::cerr << "* WARNNING! mvm_bn_res bn width wants 1, but get " << bn->width << std::endl;
    if (bn->channel != input->channel*2) 
        std::cerr << "* WARNNING! mvm_bn_res bn channel wants input->channel*2(" << input->channel*2
                  << "), but get " << bn->channel << std::endl;
    if (res->height != input->height)
        std::cerr << "* WARNNING! mvm_bn_res res height wants input->height(" << input->height << "), but get " << res->height << std::endl;
    if (res->width != input->width)
        std::cerr << "* WARNNING! mvm_bn_res res width wants input->width(" << input->width << "), but get " << res->width << std::endl;
    if (res->channel != weight->out_ch) 
        std::cerr << "* WARNNING! mvm_bn_res res channel wants weight->out_ch(" << weight->out_ch 
                  << "), but get " << res->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input->height, input->width, weight->out_ch, 0, 0, 16);
    FPGA_RunHBM_MVM_BN_Res(0, mode_mvm_bn, input, weight, bn, res, output, skip_factor, 0, 0, 0, 0, 0, log2_Bank_Step, 0);
    return output;
}

struct Mapped_Feature* add(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! add input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! add input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! add input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16);
    FPGA_Run_Elementwise(element_add, input_a, input_b, output, 0, 0);
    return output;
}

struct Mapped_Feature* minus(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! minus input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! minus input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! minus input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16);
    FPGA_Run_Elementwise(element_minus, input_a, input_b, output, 0, 0);
    return output;
}

struct Mapped_Feature* mul(struct Mapped_Feature* input_a, struct Mapped_Feature* input_b) {
    if (input_a->height != input_b->height)
        std::cerr << "* WARNNING! mul input_a height wants input_b->height(" << input_b->height << "), but get " << input_a->height << std::endl;
    if (input_a->width != input_b->width)
        std::cerr << "* WARNNING! mul input_a width wants input_b->width(" << input_b->width << "), but get " << input_a->width << std::endl;
    if (input_a->channel != input_b->channel) 
        std::cerr << "* WARNNING! mul input_a channel wants input_b->channel(" << input_b->channel << "), but get " << input_a->channel << std::endl;

    struct Mapped_Feature* output = Malloc_Feature(input_a->height, input_a->width, input_a->channel, 0, 0, 16);
    FPGA_Run_Elementwise(element_mul, input_a, input_b, output, 0, 0);
    return output;
}
/*************************************************************************************************/

int accel_op_test() {
    struct Mapped_Feature* trans = InputAddr(Malloc_Feature(1, 64, 256, 0, 0, 16), "transpose_in"); // Hin, Win, CHin: Height -> Hin*Win
    struct Mapped_Weight* weight = transpose(trans, 28);
    weight = OutputAddr(weight, "transpose_out");
    std::cout << weight << std::endl;
    CSB_Finish(); // 函数结束标志，生成 <name>0(HANDLE device) 函数

    //struct Mapped_Feature* data = InputAddr(Malloc_Feature(19, 1, 2048, 0, 0, 16), "mvm_in"); // Hin, Win, CHin: Height -> Hin*Win
    //struct Mapped_Feature* output = mvm(data, weight, 1, 28);
    //std::cout << output << std::endl;
    //CSB_Finish(); // 函数结束标志，生成 <name>1(HANDLE device) 函数

    /*
     * /----------------\
     * | Code Generator |
     * \----------------/
     */
    std::string name = "Transpose";
    std::ofstream source("./test_model.h");
    Addr_Save_Model(source, "");
    CSB_Save_Model(source, name);
    CSB_Clear(); // CSB记录清空，重新开始记录
    return 0;
}


int main() {
    // chatglm_block_main(19);
    accel_op_test();
    return 0;
}
