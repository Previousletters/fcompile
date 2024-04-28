import numpy as np
from dlavm.adr import DataType, DataEnum
from dlavm.device import HBM0321
from dlavm.clib import WT_TRANS, BN_TRANS

def Test():
    # with open("./test/MVM_BN_Wqkv.bin", "rb") as f:
    #     weight = b"".join(f.readlines())
    #     tp_weight = np.frombuffer(weight, dtype="int32").reshape(4608, 4096).astype("uint8")
    #     np_weight = np.zeros((4608, 2048), dtype="uint8")
    #     for x in range(4608):
    #         for y in range(2048):
    #             np_weight[x, y] = (tp_weight[x, 2*y] & 0x0f) + (tp_weight[x, 2*y+1] & 0xf0)
    # with open("./test/MVM_BN_Wqkv_int4.bin", "wb") as f:
    #     f.write(np_weight.tobytes())

    with open("./test/MVM_BN_Wqkv_int4.bin", "rb") as f:
        weight = b"".join(f.readlines())
        np_weight = np.frombuffer(weight, dtype="int32").reshape(4608, 512)
    # with open("./test/MVM_BN_Wqkv.bin", "rb") as f:
    #     weight = b"".join(f.readlines())
    #     np_weight = np.frombuffer(weight, dtype="int32").reshape(4608, 4096)
    with open("./test/MVM_BN_Scaleqkv.bin", "rb") as f:
        scale = b"".join(f.readlines())
        np_scale = np.frombuffer(scale, dtype="float16").reshape(4608, 32)
    
    hbm_dtype = DataType(DataEnum.int4, DataEnum.hbm)
    require_bytes = HBM0321.malloc_bytes([4096, 4608], hbm_dtype)
    mapped_wt = WT_TRANS(np_weight, np_scale, require_bytes)
    print("success!")
    print(mapped_wt[0, 1])
    with open("./test/MVM_BN_write_to_HBM_bin/MVMBN0_HBM_DDR_01.bin", "rb") as f:
        target = b"".join(f.readlines())
        np_target = np.frombuffer(target, dtype="int32").reshape(require_bytes // 4)
    print(np_target[1])

    with open("./test/MVM_BN_Biasqkv.bin", "rb") as f:
        bias = b"".join(f.readlines())
        np_bias = np.frombuffer(bias, dtype="float16").reshape(1, 4608)
    ddr_dtype = DataType(DataEnum.fp16, DataEnum.ddr)
    require_bytes = HBM0321.malloc_bytes([4608*2], ddr_dtype)
    mapped_bn = BN_TRANS(np.ones(shape=(1, 4608), dtype="float16"), np_bias, require_bytes)

    with open("./test/MVM_BN_DDR_bin/MVMBN0_wt_and_bias_in_DDR.bin", "rb") as f:
        target = b"".join(f.readlines())
        np_target = np.frombuffer(target, dtype="int32").reshape(1, require_bytes // 4)
    print(mapped_bn[0, 0])
    print(np_target[0, 0])
    print(np.sum(np_target != mapped_bn))



Test()