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
import argparse
import numpy as np
from time import strftime, localtime

import dlavm.utils


def debug_expr(data, kvcache, last_token, path_prefix):
    prefix = "debug_"
    data_path = path_prefix + "/BLOCK00/"
    qk_weight = adr.hbm.const_hbm(prefix + "qk_weight", data_path + "MVM_BN_write_to_HBM_bin/MVMBN0_0_HBM_DDR_%02d.bin", [4096, 128*34])
    qk_bn = adr.hbm.const_ddr(prefix + "qk_bn_bias", data_path + "MVM_BN_DDR_bin/MVMBN0_0_wt_and_bias_in_DDR.bin", [2*128*34])
    v_weight = adr.hbm.const_hbm(prefix + "v_weight", data_path + "MVM_BN_write_to_HBM_bin/MVMBN0_1_HBM_DDR_%02d.bin", [4096, 128*2])
    v_bn = adr.hbm.const_ddr(prefix + "v_bn_bias", data_path + "MVM_BN_DDR_bin/MVMBN0_1_wt_and_bias_in_DDR.bin", [2*128*2])
    qkv_weight = adr.hbm.const_hbm(prefix + "qkv_weight", data_path + "MVM_BN_write_to_HBM_bin/MVMBN0_HBM_DDR_%02d.bin", [4096, 128*36])
    qkv_bn = adr.hbm.const_ddr(prefix + "qkv_bn_bias", data_path + "MVM_BN_DDR_bin/MVMBN0_wt_and_bias_in_DDR.bin", [2*128*36])
 
    qk_data = adr.hbm.mvm_bn(data, qk_weight, qk_bn, kvcache=kvcache, last_token=last_token)
    v_data = adr.hbm.mvm_bn(data, v_weight, v_bn, kvcache=kvcache, last_token=last_token)
    v_data = adr.hbm.cache(v_data)

    qkv_data = adr.hbm.mvm_bn(data, qkv_weight, qkv_bn, padding=1, kvcache=kvcache, last_token=last_token)
    qkv_data.prefix = "cache"

    return adr.tuple([qk_data, v_data, qkv_data])


def debug_main_dynamic(device=dlavm.device.HBM0321, aux_cfg=0, wt2hbm=0, py=0, path_prefix = "BLOCK_write_data"):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    token = ne.Var("token", device.MAX_TOKEN)
    name = f"debug_{device.MAX_TOKEN}"
    name += "_aux" if aux_cfg else ""
    name += "_wt2hbm" if wt2hbm else ""
    name += "_" + strftime('%m%d_%H%M', localtime())
    data = adr.hbm.var_ddr("data_in", [1, token, 4096])
    kvcache = ne.Var("kvcache", 1)
    last_token = ne.Var("last_token", device.MAX_TOKEN)

    output = debug_expr(data, kvcache=kvcache, last_token=last_token, path_prefix=path_prefix)
    output = transform.infer_type(output, device)

    storage_addrs = {"weight": 0x200000000, "cache": "weight", "runtime": "cache", "hbm": 0x0}
    if aux_cfg:
        if wt2hbm:
            expr, source, storage, _, params, debug_params = backend.cfg_wt2hbm(output, name, storage_addrs)
        else:
            expr, source, storage, _, params, debug_params = backend.cfg_head(output, name, storage_addrs)
        debug_params = debug_params.reshape((-1, ))
        cmd_path = os.path.join("output", name + ".bin")
        with open(cmd_path, "wb") as f:
            f.write(params)
        print(cmd_path)
    elif wt2hbm:
        expr, source, storage, _ = backend.csb_wt2hbm_head(output, name, storage_addrs)
    elif py:
        expr, source, storage, _ = backend.csb_python(output, name, storage_addrs)
    else:
        expr, source, storage, _ = backend.csb_test_head_ops(output, name, storage_addrs)
    if py:
        save_path = os.path.join("output", name + ".py")
    else:
        save_path = os.path.join("output", name + ".h")
    log_path = os.path.join("output", name + ".log")
    with open(save_path, "w") as f:
        f.write(source)
    dlavm.utils.LOG_WITH_PREFIX("expression", str(expr))
    dlavm.utils.LOG_WITH_PREFIX("storage", str(storage))
    dlavm.utils.LOG_EXPORT(log_path)
    print(save_path)
    print(log_path)


def kvmvm_main_dynamic(token, device=dlavm.device.HBM0321, aux_cfg=0, wt2hbm=0, clock=0, py=0, path_prefix = "BLOCK_write_data"):
    sys.setrecursionlimit(3000)  # 将默认的递归深度修改为3000
    # token = ne.Var("token", device.MAX_TOKEN)
    name = f"kvmvm_{token}"
    name += "_aux" if aux_cfg else ""
    name += "_clock" if clock else ""
    name += "_wt2hbm" if wt2hbm else ""
    name += "_" + strftime('%m%d_%H%M', localtime())

    data = adr.hbm.var_ddr("data_in", [1, 16, 128])
    weight0 = adr.hbm.const_hbm("weight0", None, [128, token])
    weight1 = adr.hbm.const_hbm("weight1", None, [token, 128])
    output = adr.hbm.mvm(data, weight0)
    output = adr.hbm.mvm(output, weight1)

    output = transform.infer_type(output, device)

    storage_addrs = {"runtime": 0x200000000, "hbm": 0x0}
    if aux_cfg:
        if wt2hbm:
            expr, source, storage, _, params, debug_params = backend.cfg_wt2hbm(output, name, storage_addrs)
        else:
            expr, source, storage, _, params, debug_params = backend.cfg_head(output, name, storage_addrs)
        debug_params = debug_params.reshape((-1, ))
        cmd_path = os.path.join("output", name + ".bin")
        with open(cmd_path, "wb") as f:
            f.write(params)
        print(cmd_path)
    elif wt2hbm:
        expr, source, storage, _ = backend.csb_wt2hbm_head(output, name, storage_addrs)
    elif clock:
        expr, source, storage, _ = backend.csb_test_clock_ops(output, name, storage_addrs)
    elif py:
        expr, source, storage, _ = backend.csb_python(output, name, storage_addrs)
    else:
        expr, source, storage, _ = backend.csb_test_head_ops(output, name, storage_addrs)
    if py:
        save_path = os.path.join("output", name + ".py")
    else:
        save_path = os.path.join("output", name + ".h")
    log_path = os.path.join("output", name + ".log")
    with open(save_path, "w") as f:
        f.write(source)
    dlavm.utils.LOG_WITH_PREFIX("expression", str(expr))
    dlavm.utils.LOG_WITH_PREFIX("storage", str(storage))
    dlavm.utils.LOG_EXPORT(log_path)
    print(save_path)
    print(log_path)


if __name__ == "__main__":
    device = dlavm.device.HBM0603

    parser = argparse.ArgumentParser()
    parser.add_argument("--py", action="store_true", default=False, help="python backend mode")
    parser.add_argument("--aux", action="store_true", default=False, help="aux cfg backend mode")
    parser.add_argument("--clock", action="store_true", default=False, help="aux cfg backend mode")
    parser.add_argument("--wt2hbm", action="store_true", default=False, help="wt2hbm backend mode")
    parser.add_argument("--prefix", type=str, default="BLOCK_write_data", help="set datapath prefix")
    parser.add_argument("--maxtoken", type=int, default=device.MAX_TOKEN, help="set max token")
    args = parser.parse_args()

    device.MAX_TOKEN = args.maxtoken

    # debug_main_dynamic(device=device, aux_cfg=args.aux, wt2hbm=args.wt2hbm, py=args.py, path_prefix=args.prefix)
    # for i in [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]:
    kvmvm_main_dynamic(ne.Var("token", device.MAX_TOKEN), device=device, aux_cfg=args.aux, wt2hbm=args.wt2hbm, py=args.py, clock=args.clock, path_prefix=args.prefix)
    log = dlavm.utils.GET_LOG()