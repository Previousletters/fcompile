import os
import time
import dlavm
from dlavm import adr
from dlavm import ne
from dlavm import backend
from dlavm.transform import infer_type, offline_process, fused_ops
from dlavm import transform
import sys
import json
from time import strftime, localtime



def chatglm_block(data, pos_weight, silu_weight, token, index, arg_max, kvcache):
    prefix = "BLOCK%02d_" % index
    ln_k_bias = adr.hbm.const_ddr(prefix + "ln_k_bias", None, [4096*2])
    qkv_weight = adr.hbm.const_hbm(prefix + "qkv_weight", None, [4096, 128*36])
    qkv_bn = adr.hbm.const_ddr(prefix + "qkv_bn_bias", None, [2*128*36])
    atten_weight = adr.hbm.const_hbm(prefix + "atten_weight", None, [4096, 4096])
    atten_bias = adr.hbm.const_ddr(prefix + "atten_bn", None, [4096*2])
    post_k_bias = adr.hbm.const_ddr(prefix + "post_k_bias", None, [4096*2])
    h_to_4h_wt_0 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_0", None, [4096, 13696])
    h_to_4h_wt_1 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_1", None, [4096, 13696])
    h_to_4h_bn_0 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_0", None, [13696*2])
    h_to_4h_bn_1 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_1", None, [13696*2])
    dense_4h_to_4h_wt = adr.hbm.const_hbm(prefix + "dense_4h_to_h_wt", None, [13696, 4096])
    dense_4h_to_4h_bn = adr.hbm.const_ddr(prefix + "dense_4h_to_h_bn", None, [4096*2])

    ln_out = adr.hbm.layer_norm(data, ln_k_bias, rms=1)

    qkv_data = adr.hbm.mvm_bn(ln_out, qkv_weight, qkv_bn, padding=1, autofree=False)
    qkv_data = adr.realloc(qkv_data, [1, 128, 36*128])
    qkv_data = adr.split(qkv_data, -1, [34*128, 2*128])
    qk_data = adr.reshape(qkv_data[0], [34, token, 128])
    v_data = adr.reshape(qkv_data[1], [2, token, 128])

    qk_data = adr.hbm.pos_emb(qk_data, pos_weight, kvcache=kvcache)
    qk_data = adr.realloc(qk_data, [34, 128, 128])
    qk_data = adr.split(qk_data, 0, [32, 2])
    atten_out = adr.hbm.mvm_afterTRP(qk_data[0], qk_data[1], kvcache=kvcache)
    atten_out = adr.realloc(atten_out, [32, 128, 128])
    atten_out = adr.hbm.softmax(atten_out, kvcache=kvcache)
    atten_out = adr.realloc(atten_out, [32, 128, 128])
    atten_out = adr.hbm.mvm_afterF2W(atten_out, v_data, kvcache=kvcache)
    atten_out = adr.reshape(atten_out, [1, token, 4096])

    res_out = adr.hbm.mvm_bn_res(atten_out, atten_weight, atten_bias, data)
    post_atten = adr.hbm.layer_norm(res_out, post_k_bias, rms=0)
    dense_4h_out0 = adr.hbm.mvm_bn(post_atten, h_to_4h_wt_0, h_to_4h_bn_0)
    act_output = adr.hbm.activate(dense_4h_out0, silu_weight)
    dense_4h_out = adr.hbm.mvm_bn_res(post_atten, h_to_4h_wt_1, h_to_4h_bn_1, act_output, res_mul=1)
    if arg_max:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, arg_max=arg_max)
        return output[1]
    else:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out)
        return output


def chatglm_main(kvcache, debug, addr):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    # token = ne.Var("token", 19)
    if kvcache:
        token = 1
        name = "chatglm_with_kvcache"
    else:
        token = 19
        name = "chatglm_without_kvcache"
    if debug:
        name += "_" + strftime('%m%d_%H%M', localtime())
    data = adr.hbm.var_ddr("data", [1, token, 4096])
    pos_weight = adr.hbm.const_ddr("pos_emb", None, [1, 128, 64])
    silu_weight = adr.hbm.const_ddr("silu_act", None, [32*3], adr.DataEnum.int8)
    block_size = 1
    for n in range(block_size):
        data = chatglm_block(data, pos_weight, silu_weight, token, n, n==block_size+1, kvcache=0)
    output = data

    output = infer_type(output, dlavm.device.HBM0321)
    print(output)
    if addr:
        expr, storage = backend.graph_plan_memory(output, 0x200000000, 0x0)
        print(expr)
        print(storage)
    else:
        from dlavm.driver import config
        config.tb_sim_path = "./HBM_sv"
        config.tb_debug = 1
        expr, source, storage, _ = backend.testbench_test_head(output, name, 0x200000000, 0x0)
        save_path = os.path.join("test", name + ".h")
        with open(save_path, "w") as f:
            f.write(source)
        print(expr)
        print(storage)
        print(json.dumps(config.tb_macro_log,indent=3,ensure_ascii=False))
        print(save_path)


def test():
    token = 19
    name = "PosEmb_afterTRP"
    data = adr.hbm.var_ddr("data", [34, token, 128])

    pos_weight = adr.hbm.const_ddr("pos_emb", None, [1, 128, 64])
    output = adr.hbm.pos_emb(data, pos_weight)
    output = adr.reshape(output, [1, token, 34*128])
    output = adr.split(output, -1, [32*128, 2*128])
    q_data = adr.reshape(output[0], [32, token, 128])
    k_data = adr.reshape(output[1], [2, token, 128])
    output = adr.hbm.mvm_afterTRP(q_data, k_data)

    output = infer_type(output, dlavm.device.HBM0321)
    print(output)
    from dlavm.driver import config
    config.tb_sim_path = "/home/previous/accel/hbm0321/HBM_sv"
    config.tb_debug = 1
    expr, source, storage, _ = backend.testbench_test_head(output, name, 0x200000000, 0x0)
    save_path = os.path.join("test", name + ".h")
    with open(save_path, "w") as f:
        f.write(source)
    print(expr)
    print(storage)
    print(json.dumps(config.tb_macro_log,indent=3,ensure_ascii=False))
    print(save_path)


def atten_main(kvcache, debug, addr):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    # token = ne.Var("token", 19)
    if kvcache:
        name = "atten_with_kvcache"
    else:
        name = "atten_without_kvcache"
    if debug:
        name += "_" + strftime('%m%d_%H%M', localtime())

    qk_data = adr.hbm.var_ddr("qk_data", [34, 128, 128])
    v_data = adr.hbm.var_ddr("v_data", [2, 128, 128])
    pos_weight = adr.hbm.const_ddr("pos_emb", None, [1, 128, 64])

    output = adr.realloc(qk_data, [34, 128, 128])
    output = adr.reshape(output, [34, 128, 128])
    output = adr.hbm.pos_emb(output, pos_weight, kvcache=kvcache)
    output = adr.split(output, 0, [32, 2])
    output = adr.hbm.mvm_afterTRP(output[0], output[1], kvcache=kvcache)
    output = adr.hbm.softmax(output, kvcache=kvcache)
    output = adr.realloc(output, [32, 130, 128])
    output = adr.reshape(output, [32, 130, 128], force=1)
    output = adr.hbm.mvm_afterF2W(output, v_data, kvcache=kvcache)
    output = adr.reshape(output, [32, 128, 128], force=1)

    output = infer_type(output, dlavm.device.HBM0321)
    print(output)
    if addr:
        expr, storage = backend.graph_plan_memory(output, 0x200000000, 0x0)
        print(expr)
        print(storage)
    else:
        from dlavm.driver import config
        config.tb_sim_path = "/home/previous/accel/hbm0321/HBM_sv"
        config.tb_debug = 1
        expr, source, storage, _ = backend.testbench_test_head(output, name, 0x200000000, 0x0)
        save_path = os.path.join("test", name + ".h")
        with open(save_path, "w") as f:
            f.write(source)
        print(expr)
        print(storage)
        print(json.dumps(config.tb_macro_log,indent=3,ensure_ascii=False))
        print(save_path)



if __name__ == "__main__":
    chatglm_main(kvcache=0, debug=1, addr=0)
    # atten_main(kvcache=0, debug=1, addr=0)
    # test()