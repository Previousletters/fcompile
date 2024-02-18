from genregs import adr
from genregs import ne
from genregs import backend
from genregs.transform import infer_type
import sys  # 导入sys模块


def chatglm_block(data, token, index, arg_max):
    prefix = "BLOCK%02d_" % index
    ln_k_bias = adr.hbm.const_ddr(prefix + "ln_k_bias", None, [4096*2])
    qkv_weight = adr.hbm.const_hbm(prefix + "qkv_weight", None, [4096, 128*36])
    k0_cache = adr.hbm.const_ddr(prefix + "k0_cache", None, [1, token-1, 128])
    k1_cache = adr.hbm.const_ddr(prefix + "k1_cache", None, [1, token-1, 128])
    v0_cache = adr.hbm.const_ddr(prefix + "v0_cache", None, [1, token-1, 128])
    v1_cache = adr.hbm.const_ddr(prefix + "v1_cache", None, [1, token-1, 128])
    pos_weight = adr.hbm.const_ddr(prefix + "pos_emb", None, [1, 1, 64])
    rsqrt = adr.hbm.const_ddr(prefix + "rsqrt", None, [token*2])
    mask = adr.hbm.const_ddr(prefix + "mask", None, [32, 1, token])
    atten_weight = adr.hbm.const_hbm(prefix + "atten_weight", None, [4096, 4096])
    atten_bias = adr.hbm.const_ddr(prefix + "atten_bn", None, [4096*2])
    post_k_bias = adr.hbm.const_ddr(prefix + "post_k_bias", None, [4096*2])
    h_to_4h_wt_0 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_0", None, [4096, 13696])
    h_to_4h_wt_1 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_1", None, [4096, 13696])
    h_to_4h_bn_0 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_0", None, [13696*2])
    h_to_4h_bn_1 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_1", None, [13696*2])
    silu_weight = adr.hbm.const_ddr(prefix + "silu_act", None, [32*3], adr.DataEnum.int8)
    dense_4h_to_4h_wt = adr.hbm.const_hbm(prefix + "dense_4h_to_h_wt", None, [13696, 4096])
    dense_4h_to_4h_bn = adr.hbm.const_ddr(prefix + "dense_4h_to_h_bn", None, [4096*2])

    ln_out = adr.hbm.layer_norm(data, ln_k_bias)
    qkv_data = adr.hbm.mvm(ln_out, qkv_weight)
    qkv_data = adr.split(qkv_data, 2, [4096, 128, 128, 128, 128])
    q_data = adr.reshape(qkv_data[0], [32, 1, 128])
    q_data = adr.hbm.pos_emb(q_data, pos_weight)
    k_data0 = adr.hbm.pos_emb(qkv_data[1], pos_weight)
    k_data0 = adr.cpu.cache(k_data0, k0_cache)
    k_data1 = adr.hbm.pos_emb(qkv_data[2], pos_weight)
    k_data1 = adr.cpu.cache(k_data1, k1_cache)
    k_data0 = adr.hbm.transpose(k_data0)
    k_data1 = adr.hbm.transpose(k_data1)
    scores = adr.hbm.mvm_bn_res(q_data, k_data0, k_data1, rsqrt, mask, skip=2)
    scores = adr.hbm.softmax(scores)
    v_data0 = adr.cpu.cache(qkv_data[3], v0_cache)
    v_data1 = adr.cpu.cache(qkv_data[4], v1_cache)
    v_data0 = adr.hbm.feature2weight(v_data0)
    v_data1 = adr.hbm.feature2weight(v_data1)
    atten_out = adr.hbm.mvm(scores, v_data0, v_data1, skip=2)
    atten_out = adr.reshape(atten_out, [1, 1, 4096])
    res_out = adr.hbm.mvm_bn_res(atten_out, atten_weight, atten_bias, data)
    post_atten = adr.hbm.layer_norm(res_out, post_k_bias)
    dense_4h_out0 = adr.hbm.mvm_bn(post_atten, h_to_4h_wt_0, h_to_4h_bn_0)
    act_output = adr.hbm.activate(dense_4h_out0, silu_weight)
    dense_4h_out = adr.hbm.mvm_bn_res(post_atten, h_to_4h_wt_1, h_to_4h_bn_1, act_output, res_mul=1)
    output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, arg_max=arg_max)
    return output


def chatglm_with_kvcache():
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    token = ne.Var("token", 128)
    data = adr.hbm.var_ddr("data", [1, 1, 4096])
    block_size = 28
    for n in range(block_size):
        data = chatglm_block(data, token, n, n==block_size-1)
    output = data[1]
    print(output)

    output = infer_type(output)
    expr, source, storage, _ = backend.csb_head(output, "chatglm", 0x200000000, 0x0)
    with open("source.h", "w") as f:
        f.write(source)
    print(expr)
    print(storage)
    

def check_testbench():
    from genregs.driver import config
    config.tb_sim_path = "/home/previous/accel/hbm0201/driver/HBM_sv"
    data = adr.hbm.var_ddr("data", [1, 1, 4096])
    weight = adr.hbm.const_hbm("weight", None, [4096, 128*36])
    output = adr.hbm.mvm(data, weight)
    print(output)

    output = infer_type(output)
    expr, source, storage, mod = backend.testbench(output, "chatglm", 0x200000000, 0x0)
    expr, source, storage, mod1 = backend.csb_head(output, "chatglm", 0x200000000, 0x0)
    with open("source.h", "w") as f:
        f.write(source)
    print(expr)
    print(storage)
    print(mod)
    print(mod1)


if __name__ == "__main__":
    # chatglm_with_kvcache()
    check_testbench()