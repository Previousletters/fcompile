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
import numpy as np
from time import strftime, localtime

import dlavm.utils


def chatglm_block_dynamic(data, pos_weight, silu_weight, token, index, arg_max, kvcache, last_token=0):
    prefix = "BLOCK%02d_" % index
    data_path = "BLOCK_write_data/BLOCK%02d/" % index
    ln_k_bias = adr.hbm.const_ddr(prefix + "ln_k_bias", data_path + "LN_DDR_bin/LN0_wt_in_DDR.bin", [4096*2])
    qkv_weight = adr.hbm.const_hbm(prefix + "qkv_weight", data_path + "MVM_BN_write_to_HBM_bin/MVMBN0_HBM_DDR_%02d.bin", [4096, 128*36])
    qkv_bn = adr.hbm.const_ddr(prefix + "qkv_bn_bias", data_path + "MVM_BN_DDR_bin/MVMBN0_wt_and_bias_in_DDR.bin", [2*128*36])
    atten_weight = adr.hbm.const_hbm(prefix + "atten_weight", data_path + "MVM_BN_RES_write_to_HBM_bin/MVMBNRES0_HBM_DDR_%02d.bin", [4096, 4096])
    atten_bias = adr.hbm.const_ddr(prefix + "atten_bn", data_path + "MVM_BN_RES_DDR_bin/MVMBNRES0_wt_and_bias_in_DDR.bin", [4096*2])
    post_k_bias = adr.hbm.const_ddr(prefix + "post_k_bias", data_path + "LN_DDR_bin/LN1_wt_in_DDR.bin", [4096*2])
    h_to_4h_wt_0 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_0", data_path + "MVM_BN_write_to_HBM_bin/MVMBN1_HBM_DDR_%02d.bin", [4096, 13696])
    h_to_4h_wt_1 = adr.hbm.const_hbm(prefix + "h_to_4h_wt_1", data_path + "MVM_BN_RES_write_to_HBM_bin/MVMBNRES1_HBM_DDR_%02d.bin", [4096, 13696])
    h_to_4h_bn_0 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_0", data_path + "MVM_BN_DDR_bin/MVMBN1_wt_and_bias_in_DDR.bin", [13696*2])
    h_to_4h_bn_1 = adr.hbm.const_ddr(prefix + "h_to_4h_bn_1", data_path + "MVM_BN_RES_DDR_bin/MVMBNRES1_wt_and_bias_in_DDR.bin", [13696*2])
    dense_4h_to_4h_wt = adr.hbm.const_hbm(prefix + "dense_4h_to_h_wt", data_path + "MVM_BN_RES_write_to_HBM_bin/MVMBNRES2_HBM_DDR_%02d.bin", [13696, 4096])
    dense_4h_to_4h_bn = adr.hbm.const_ddr(prefix + "dense_4h_to_h_bn", data_path + "MVM_BN_RES_DDR_bin/MVMBNRES2_wt_and_bias_in_DDR.bin", [4096*2])

    ln_out = adr.hbm.layer_norm(data, ln_k_bias, rms=1, kvcache=kvcache, last_token=last_token)

    qkv_data = adr.hbm.mvm_bn(ln_out, qkv_weight, qkv_bn, padding=1, kvcache=kvcache, last_token=last_token)
    qkv_data.prefix = "cache"
    qkv_data = adr.reshape(qkv_data, [36, token, 128])
    qkv_data = adr.split(qkv_data, 0, [34, 2])
    qk_data, v_data = qkv_data[0], qkv_data[1]

    qk_data = adr.hbm.pos_emb(qk_data, pos_weight, kvcache=kvcache, last_token=last_token)
    qk_data.prefix = "cache"
    qk_data = adr.split(qk_data, 0, [32, 2])
    atten_out = adr.hbm.mvm_afterTRP(qk_data[0], qk_data[1], kvcache=kvcache, last_token=last_token)
    atten_out.prefix = "cache"
    atten_out = adr.hbm.softmax(atten_out, kvcache=kvcache, last_token=last_token)
    atten_out.prefix = "cache"
    atten_out = adr.hbm.mvm_afterF2W(atten_out, v_data, kvcache=kvcache, last_token=last_token)
    atten_out = adr.reshape(atten_out, [1, token, 4096])

    res_out = adr.hbm.mvm_bn_res(atten_out, atten_weight, atten_bias, data, kvcache=kvcache, last_token=last_token)
    post_atten = adr.hbm.layer_norm(res_out, post_k_bias, rms=1, kvcache=kvcache, last_token=last_token)
    dense_4h_out0 = adr.hbm.mvm_bn(post_atten, h_to_4h_wt_0, h_to_4h_bn_0, kvcache=kvcache, last_token=last_token)
    act_output = adr.hbm.activate(dense_4h_out0, silu_weight, kvcache=kvcache, last_token=last_token)
    dense_4h_out = adr.hbm.mvm_bn_res(post_atten, h_to_4h_wt_1, h_to_4h_bn_1, act_output, res_mul=1, kvcache=kvcache, last_token=last_token)
    if arg_max:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, kvcache=kvcache, last_token=last_token)
        ln_k_bias = adr.hbm.const_ddr("Final_LN_k_bias", "BLOCK_write_data/BLOCK27/LN_DDR_bin/LN2_wt_in_DDR.bin", [4096*2])
        ln_out = adr.hbm.layer_norm(output, ln_k_bias, rms=1, kvcache=1, kvcache_offset=ne.If(kvcache, 0, 1), last_token=last_token)
        output_wt = adr.hbm.const_hbm("Output_Layer_wt", "BLOCK_write_data/BLOCK27/MVM_BN_DDR_bin/MVMBN_Argmax_0_HBM_DDR_%02d.bin", [4096, 65024])
        output_bn = adr.hbm.const_ddr("Output_Layer_bn", "BLOCK_write_data/BLOCK27/MVM_BN_RES_write_to_HBM_bin/MVMBN_Argmax_wt_and_bias_in_DDR.bin", [65024*2])
        mvm_out = adr.hbm.mvm_bn(ln_out, output_wt, output_bn, arg_max=arg_max, kvcache=1)
        arg_out = mvm_out[1]
        return arg_out
    else:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, kvcache=kvcache, last_token=last_token)
        return output


def chatglm_main_dynamic(debug, block_size=28, without_arg_max=1, device=dlavm.device.HBM0321, aux_cfg=0):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    token = ne.Var("token", device.MAX_TOKEN)
    name = f"chatglm_dynamic_control_{device.MAX_TOKEN}"
    if debug:
        name += "_" + strftime('%m%d_%H%M', localtime())
    data = adr.hbm.var_ddr("data_in", [1, token, 4096])
    pos_weight = adr.hbm.const_ddr("pos_emb", "rw_data/pos_in/pos_in_4_token128_ch32.bin", [1, 128*2, 64])
    pos_weight.prefix = "global"
    silu_weight = adr.hbm.const_ddr("silu_act", "BLOCK_write_data/ACT_parameter_in_DDR.bin", [32*128], adr.DataEnum.fp16)
    silu_weight.prefix = "global"
    kvcache = ne.Var("kvcache", 1)
    last_token = ne.Var("last_token", device.MAX_TOKEN)
    for n in range(block_size):
        data = chatglm_block_dynamic(data, pos_weight, silu_weight, token, n, n==block_size-1+without_arg_max, kvcache=kvcache, last_token=last_token)
    output = data

    output = infer_type(output, device)
    from dlavm.driver import config
    config.tb_sim_path = "~/accel/hbm0321/HBM_sv"
    config.tb_debug = 0
    if aux_cfg:
        expr, source, storage, _, params, debug_params = backend.cfg_head(output, name, {"global": 0x200000000, "weight": "global", "cache": "weight", "runtime": "cache", "cfg": "runtime", "hbm": 0x0})
        debug_params = debug_params.reshape((-1, ))
        with open("./test/aux_cfg_dynamic/aux_cfg.bin", "wb") as f:
            f.write(params)
    else:
        expr, source, storage, _ = backend.csb_test_head_ops(output, name, {"global": 0x200000000, "weight": "global", "cache": "weight", "runtime": "cache", "hbm": 0x0})
    save_path = os.path.join("test", name + ".h")
    with open(save_path, "w") as f:
        f.write(source)
    # print(expr)
    # print(storage)
    print(save_path)


def chatglm_block(data, pos_weight, silu_weight, token, index, arg_max, kvcache, max_token):
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

    ln_out = adr.hbm.layer_norm(data, ln_k_bias, rms=1, kvcache=kvcache)
    ln_out = adr.realloc(ln_out, [1, max_token, 4096])

    qkv_data = adr.hbm.mvm_bn(ln_out, qkv_weight, qkv_bn, padding=1, kvcache=kvcache)
    qkv_data.prefix = "cache"
    qkv_data = adr.realloc(qkv_data, [1, max_token, 36*128])
    qkv_data = adr.reshape(qkv_data, [36, max_token, 128], force=1)
    qkv_data = adr.split(qkv_data, 0, [34, 2])
    qk_data = adr.reshape(qkv_data[0], [34, token, 128], force=1)
    v_data = adr.reshape(qkv_data[1], [2, token, 128], force=1)

    qk_data = adr.hbm.pos_emb(qk_data, pos_weight, kvcache=kvcache)
    qk_data.prefix = "cache"
    qk_data = adr.realloc(qk_data, [34, max_token, 128])
    qk_data = adr.reshape(qk_data, [34, max_token, 128], force=1)
    qk_data = adr.split(qk_data, 0, [32, 2])
    q_data = adr.reshape(qk_data[0], [32, token, 128], force=1)
    k_data = adr.reshape(qk_data[1], [2, token, 128], force=1)
    atten_out = adr.hbm.mvm_afterTRP(q_data, k_data, kvcache=kvcache)
    atten_out.prefix = "cache"
    atten_out = adr.realloc(atten_out, [32, max_token, 128])
    atten_out = adr.reshape(atten_out, [32, token, 128], force=1)
    atten_out = adr.hbm.softmax(atten_out, kvcache=kvcache)
    atten_out.prefix = "cache"
    atten_out = adr.realloc(atten_out, [32, max_token, 128])
    atten_out = adr.reshape(atten_out, [32, token, 128], force=1)
    atten_out = adr.hbm.mvm_afterF2W(atten_out, v_data, kvcache=kvcache)
    atten_out = adr.realloc(atten_out, [32, max_token, 128])
    atten_out = adr.reshape(atten_out, [1, token, 4096], force=1)

    res_out = adr.hbm.mvm_bn_res(atten_out, atten_weight, atten_bias, data, kvcache=kvcache)
    res_out = adr.realloc(res_out, [1, max_token, 4096])

    post_atten = adr.hbm.layer_norm(res_out, post_k_bias, rms=1, kvcache=kvcache)
    post_atten = adr.realloc(post_atten, [1, max_token, 4096])

    dense_4h_out0 = adr.hbm.mvm_bn(post_atten, h_to_4h_wt_0, h_to_4h_bn_0, kvcache=kvcache)
    dense_4h_out0 = adr.realloc(dense_4h_out0, [1, max_token, 13696])

    act_output = adr.hbm.activate(dense_4h_out0, silu_weight, kvcache=kvcache)
    act_output = adr.realloc(act_output, [1, max_token, 13696])

    dense_4h_out = adr.hbm.mvm_bn_res(post_atten, h_to_4h_wt_1, h_to_4h_bn_1, act_output, res_mul=1, kvcache=kvcache)
    dense_4h_out = adr.realloc(dense_4h_out, [1, max_token, 13696])
    if arg_max:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, kvcache=kvcache)
        output = adr.realloc(output, [1, max_token, 4096])
        # output = adr.split(output, 1, [18, 1])
        ln_k_bias = adr.hbm.const_ddr("Final_LN_k_bias", None, [4096*2])
        ln_out = adr.hbm.layer_norm(output, ln_k_bias, rms=1, kvcache=1, kvcache_offset=1)
        ln_out = adr.realloc(ln_out, [1, max_token, 4096])
        output_wt = adr.hbm.const_hbm("Output_Layer_wt", None, [4096, 65024])
        output_bn = adr.hbm.const_ddr("Output_Layer_bn", None, [65024*2])
        mvm_out = adr.hbm.mvm_bn(ln_out, output_wt, output_bn, arg_max=arg_max, kvcache=1)
        arg_out = adr.realloc(mvm_out[1], [1, max_token, 4096])
        return arg_out
    else:
        output = adr.hbm.mvm_bn_res(dense_4h_out, dense_4h_to_4h_wt, dense_4h_to_4h_bn, res_out, kvcache=kvcache)
        output = adr.realloc(output, [1, max_token, 4096])
        return output


def chatglm_main(kvcache, debug, token=19, block_size=2, without_arg_max=1, device=dlavm.device.HBM0321, aux_cfg=0):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    if kvcache:
        name = f"chatglm_with_kvcache_{block_size}"
    else:
        name = f"chatglm_without_kvcache_block_{block_size}"
    if debug:
        name += "_" + strftime('%m%d_%H%M', localtime())

    data = adr.hbm.var_ddr("data_in", [1, token, 4096])
    pos_weight = adr.hbm.const_ddr("pos_emb", None, [1, 128*2, 64])
    pos_weight.prefix = "global"
    silu_weight = adr.hbm.const_ddr("silu_act", None, [32*128], adr.DataEnum.fp16)
    silu_weight.prefix = "global"
    max_token = device.MAX_TOKEN
    for n in range(block_size):
        data = adr.realloc(data, [1, max_token, 4096])
        data = chatglm_block(data, pos_weight, silu_weight, token, n, n==block_size-1+without_arg_max, kvcache=kvcache, max_token=max_token)
    output = data

    output = infer_type(output, device)
    from dlavm.driver import config
    config.tb_sim_path = "~/accel/hbm0321/HBM_sv"
    config.tb_debug = 0
    if aux_cfg:
        expr, source, storage, _, params, debug_params = backend.cfg_head(output, name, {"global": 0x200000000, "weight": "global", "cache": "weight", "runtime": "cache", "cfg": "runtime", "hbm": 0x0})
        debug_params = debug_params.reshape((-1, ))
        for i in range(27):
            result = debug_params[i*288:i*288+288]
            with open("./test/aux_cfg_token19/BLOCK%02d.bin"%i, "wb") as f:
                f.write(result.tobytes())
        result = debug_params[27*288:27*288+304]
        print(result)
        with open("./test/aux_cfg_token19/BLOCK27.txt", "w") as f:
            for i in result:
                f.write(str(i)+"\n")
    else:
        # expr, source, storage, _ = backend.csb_test_head_ops(output, name, {"global": 0x200000000, "weight": "global", "cache": "weight", "runtime": "cache", "hbm": 0x0})
        expr, source, storage, _ = backend.csb_wt2hbm_head(output, name, {"global": 0x200000000, "weight": "global", "cache": "weight", "runtime": "cache", "hbm": 0x0})
    save_path = os.path.join("test", name + ".h")
    with open(save_path, "w") as f:
        f.write(source)
    # print(expr)
    # print(storage)
    print(save_path)


if __name__ == "__main__":
    device = dlavm.device.HBM0424
    # device = dlavm.device.HBM0507
    # chatglm_main(kvcache=0, debug=1, block_size=28, without_arg_max=0, token=19, device=device, aux_cfg=0)
    chatglm_main_dynamic(debug=1, block_size=28, without_arg_max=0, device=device, aux_cfg=1)
    log = dlavm.utils.GET_LOG()
    # print(log["graph"][-3:])
