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
    FPGA_Run_LN(data, ln_weight, ln_out, seq, 0, 0);

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
    Reshape(emb_key, 2, seq, 128); // emb_key: (2, seq, 128)
    Reshape(v_data, 2, seq, 128); // v_data: (2, seq, 128)
    std::cout << "emb_qury" << ": " << emb_qury << std::endl;
    std::cout << "emb_key" << ": " << emb_key << std::endl;
    std::cout << "v_data" << ": " << v_data << std::endl;

    // Q*Kt // !!!!Warnning!!!!
    struct Mapped_Feature* scores = Malloc_Feature(32, seq, seq, 0, 0, 16);
    std::vector<struct Mapped_Feature*> emb_qury_split = Split(emb_qury, 0);
    struct Mapped_Weight* emb_key_t = Malloc_Weight(1, 1, 128*2, seq, 0, 4);
    std::vector<struct Mapped_Feature*> scores_split = Split(scores, 0);
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
    std::vector<struct Mapped_Feature*> atten_out_split = Split(v_data, 0);
    struct Mapped_Weight* v_data_w = Malloc_Weight(1, 1, seq, 128 * 2, 0, 4);
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
    FPGA_Run_LN(res_output, post_atten_ln, post_norm_output, seq, 0, 0);
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


int main() {
    chatglm_block_main(19);
    return 0;
}
