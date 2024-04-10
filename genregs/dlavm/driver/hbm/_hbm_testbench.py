from ...adr import Op, Tensor, Constant
from ..basic import TestbenchSIM
from .conv import *
from .matrix import *
from ...clib import WT_TRANS, BN_TRANS


def MVMTestbench(args, output, attrs):
    if attrs["skip"] == 1:
        dtensor, wtensor = args[0], args[1]
        dshape, wshape = dtensor[0].shape, wtensor[0].shape
        daddrs, waddrs, oaddrs = dtensor[1], wtensor[1], output[1]
        define = {
            "log2_WT_base_addr_Bank_Step": attrs["log2_step"], 
            "Hin": dshape[0], "Win": dshape[1], "Height": dshape[0]*dshape[1],
            "Width_in": wshape[0], "Width_out": wshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "HBM00_WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
        return TestbenchSIM("testbench_HBM_MVM", define)


def MVMDriver(args, output, attrs):
    if attrs["skip"] == 1:
        return FPGA_RunHBM_MVM(0, 0b00100011111, args[0], args[1], [None, None], [None, None], output, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    elif attrs["skip"] == 2:
        regs = []
        dshape, ddtype, daddrs = args[0][0].shape, args[0][0].dtype, args[0][1]
        oshape, odtype, oaddrs = output[0].shape, output[0].dtype, output[1]
        Tout = args[0][0].device.Tout
        input_offset = [args[0][0], (daddrs & 0xffffffff) + dshape[1] * (dshape[2] + Tout - 1) // Tout * ddtype.get_bytes()]
        output_offset = [output[0], (oaddrs & 0xffffffff) + oshape[1] * (oshape[2] + Tout - 1) // Tout * odtype.get_bytes()]
        regs += FPGA_RunHBM_MVM_F2W(0, 0b00100011111, args[0], args[1], output, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        regs += FPGA_RunHBM_MVM_F2W(0, 0b00100011111, input_offset, args[2], output_offset, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        return regs
    else:
        print("Error! Not support skip > 2")
        exit(-1)


Op.Get("accel.hbm.mvm").attrs["cfg_id"] = 0b00001000
Op.Get("accel.hbm.mvm").attrs["testbench"] = MVMTestbench


def MVMBNDriver(args, output, attrs):
    if attrs["skip"] == 1:
        return FPGA_RunHBM_MVM(0, 0b01100011111, args[0], args[1], args[2], [None, None], output, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    else:
        print("Error! Not support skip > 1")
        exit(-1)


def MVMBNTestbench(args, output, attrs):
    if attrs["skip"] == 1:
        dtensor, wtensor, btensor = args[0], args[1], args[2]
        dshape, wshape, bshape = dtensor[0].shape, wtensor[0].shape, btensor[0].shape
        daddrs, waddrs, baddrs, oaddrs = dtensor[1], wtensor[1], btensor[1], output[1]
        if attrs["padding"]:
            define = {
                "log2_WT_base_addr_Bank_Step": attrs["log2_step"], "Token": dshape[1],
                "Head": dshape[0], 
                "DAT_OUT_LINE_STRIDE": dtensor[0].device.Pixel_Data_Bytes*dtensor[0].device.MAX_TOKEN,
                "DAT_OUT_SURFACE_STRIDE": dtensor[0].device.Pixel_Data_Bytes*dtensor[0].device.MAX_TOKEN*dshape[0],
                "Width_in": wshape[0], "Width_out": wshape[1],
                "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
                "HBM00_WT_BASE_ADDR":  waddrs & 0xffffffff,
                "BN_BASE_ADDR":  baddrs & 0xffffffff,
                "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
            }
        else:
            define = {
                "log2_WT_base_addr_Bank_Step": attrs["log2_step"], "Token": dshape[1],
                "Head": dshape[0], 
                "Width_in": wshape[0], "Width_out": wshape[1],
                "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
                "HBM00_WT_BASE_ADDR":  waddrs & 0xffffffff,
                "BN_BASE_ADDR":  baddrs & 0xffffffff,
                "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
            }
        return TestbenchSIM("testbench_HBM_MVM_bn", define)


Op.Get("accel.hbm.mvm_bn").attrs["cfg_id"] = 0b01001000
Op.Get("accel.hbm.mvm_bn").attrs["testbench"] = MVMBNTestbench


def MVMBNResDriver(args, output, attrs):
    if attrs["arg_max"] and attrs["skip"] == 1:
        output0, output1 = output
        return FPGA_RunHBM_MVM_BN_Res_ArgMax(attrs["res_mode"], 0b111100011111, args[0], args[1], args[2], args[3], output0, output1, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    elif attrs["arg_max"] == 0 and attrs["skip"] == 1:
        return FPGA_RunHBM_MVM(attrs["res_mode"], 0b11100011111, args[0], args[1], args[2], args[3], output, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    elif attrs["skip"] == 2:
        regs = []
        dshape, ddtype, daddrs = args[0][0].shape, args[0][0].dtype, args[0][1]
        oshape, odtype, oaddrs = output[0].shape, output[0].dtype, output[1]
        Tout = args[0][0].device.Tout
        input_offset = [args[0][0], (daddrs & 0xffffffff) + dshape[1] * (dshape[2] + Tout - 1) // Tout * ddtype.get_bytes()]
        output_offset = [output[0], (oaddrs & 0xffffffff) + oshape[1] * (oshape[2] + Tout - 1) // Tout * odtype.get_bytes()]
        regs += FPGA_RunHBM_MVM_BN_Res_afterTRP(attrs["res_mode"], 0b11100011111, args[0], args[1], args[3], args[4], output, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        regs += FPGA_RunHBM_MVM_BN_Res_afterTRP(attrs["res_mode"], 0b11100011111, input_offset, args[2], args[3], args[4], output_offset, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        return regs
    else:
        print("Error! Not support this attrs: ")
        print(attrs)
        exit(-1)


def MVMBNResTestbench(args, output, attrs):
    if attrs["skip"] == 1 and attrs["arg_max"] == 0:
        dtensor, wtensor, btensor, rtensor = args[0], args[1], args[2], args[3]
        dshape, wshape, bshape, rshape = dtensor[0].shape, wtensor[0].shape, btensor[0].shape, rtensor[0].shape
        daddrs, waddrs, baddrs, raddrs, oaddrs = dtensor[1], wtensor[1], btensor[1], rtensor[1], output[1]
        define = {
            "log2_WT_base_addr_Bank_Step": attrs["log2_step"], 
            "Head": dshape[0], "Token": dshape[1],
            "Width_in": wshape[0], "Width_out": wshape[1], "EW_MODE": attrs["mul_mode"],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "HBM00_WT_BASE_ADDR":  waddrs & 0xffffffff,
            "BN_BASE_ADDR":  baddrs & 0xffffffff,
            "Res_Add_BASE_ADDR":  raddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
        return TestbenchSIM("testbench_HBM_MVM_bn_res", define)
    elif attrs["skip"] == 1 and attrs["arg_max"] == 1:
        dtensor, wtensor, btensor, rtensor = args[0], args[1], args[2], args[3]
        dshape, wshape, bshape, rshape = dtensor[0].shape, wtensor[0].shape, btensor[0].shape, rtensor[0].shape
        daddrs, waddrs, baddrs, raddrs, oaddrs = dtensor[1], wtensor[1], btensor[1], rtensor[1], output[1]
        define = {
            "log2_WT_base_addr_Bank_Step": attrs["log2_step"], 
            "Head": dshape[0], "Token": dshape[1],
            "Width_in": wshape[0], "Width_out": wshape[1], "EW_MODE": attrs["mul_mode"],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "HBM00_WT_BASE_ADDR":  waddrs & 0xffffffff,
            "BN_BASE_ADDR":  baddrs & 0xffffffff,
            "Res_Add_BASE_ADDR":  raddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs[0] & 0xffffffff,
            "AUGMAX_OUT_ADDR": oaddrs[1] & 0xffffffff,
        }
        return TestbenchSIM("testbench_HBM_MVM_bn_res_Argmax", define)


Op.Get("accel.hbm.mvm_bn_res").attrs["cfg_id"] = 0b11001000
Op.Get("accel.hbm.mvm_bn_res").attrs["testbench"] = MVMBNResTestbench


def MVMafterTRPTestbench(args, output, attrs):
    dtensor, wtensor = args[0], args[1]
    dshape, wshape = dtensor[0].shape, wtensor[0].shape
    daddrs, waddrs, oaddrs = dtensor[1], wtensor[1], output[1]
    if attrs["kvcache"]:
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": 19,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    elif attrs["padding"]:
        max_token = dtensor[0].device.MAX_TOKEN
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": wshape[1],
            "Win": max_token, "Wout": max_token, "CHout": max_token,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    else:
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": wshape[1],
            "Win": dshape[1], "Wout": dshape[1], "CHout": dshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    return TestbenchSIM("testbench_HBM_MVM_afterTRP", define)


def MVMafterTRPDriver(args, output, attrs):
    msg = "Not support accel.hbm.mvm_afterTRP, please wait!"
    raise RuntimeError(msg)


Op.Get("accel.hbm.mvm_afterTRP").attrs["testbench"] = MVMafterTRPTestbench


def MVMafterF2WTestbench(args, output, attrs):
    dtensor, wtensor = args[0], args[1]
    dshape, wshape = dtensor[0].shape, wtensor[0].shape
    daddrs, waddrs, oaddrs = dtensor[1], wtensor[1], output[1]
    if attrs["kvcache"]:
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": 19,
            "Wout": dshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    elif attrs["padding"]:
        max_token = dtensor[0].device.MAX_TOKEN
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": wshape[1],
            "Win": max_token, "Wout": wshape[1], "CHout": max_token,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    else:
        define = {
            "KV_cache_mode": attrs["kvcache"], 
            "Feature_Head": dshape[0],
            "Weight_Head": wshape[0],
            "Token": wshape[1],
            "Win": dshape[1], "Wout": wshape[1], "CHout": wshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "WT_BASE_ADDR":  waddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    return TestbenchSIM("testbench_HBM_MVM_afterF2W", define)


def MVMafterF2WDriver(args, output, attrs):
    msg = "Not support accel.hbm.mvm_afterF2W, please wait!"
    raise RuntimeError(msg)


Op.Get("accel.hbm.mvm_afterF2W").attrs["testbench"] = MVMafterF2WTestbench


def AddDriver(args, output, attrs):
    return []

Op.Get("accel.hbm.add").attrs["cfg_id"] = 0b00000001
Op.Get("accel.hbm.add").attrs["driver"] = AddDriver


def SoftmaxDriver(args, output, attrs):
    return FPGA_Run_Softmax(args[0], output, 0)


def SoftmaxTestbench(args, output, attrs):
    dtensor = args[0]
    dshape = dtensor[0].shape
    daddrs, oaddrs = dtensor[1], output[1]
    if attrs["kvcache"]:
        define = {
            "KV_cache_mode": attrs["kvcache"],
            "Feature_Head": dshape[0],
            "Token": 19,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    elif attrs["padding"]:
        max_token = dtensor[0].device.MAX_TOKEN
        define = {
            "KV_cache_mode": attrs["kvcache"],
            "Feature_Head": dshape[0],
            "Token": 19,
            "Win": max_token, "Wout": max_token, "CHout": max_token,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    else:
        define = {
            "KV_cache_mode": attrs["kvcache"],
            "Feature_Head": dshape[0],
            "Token": dshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    return TestbenchSIM("testbench_SOFTMAX", define)


Op.Get("accel.hbm.softmax").attrs["cfg_id"] = 0b00000101
Op.Get("accel.hbm.softmax").attrs["testbench"] = SoftmaxTestbench


def LayerNormDriver(args, output, attrs):
    return FPGA_Run_LN(args[0], args[1], output, attrs["rms"], 0)


def LayerNormTestbench(args, output, attrs):
    dtensor, btensor = args[0], args[1]
    dshape, bshape = dtensor[0].shape, btensor[0].shape
    daddrs, baddrs, oaddrs = dtensor[1], btensor[1], output[1]
    define = {
        "Token": dshape[0]*dshape[1], "Width_in": dshape[2],
        "RMS_Norm": attrs["rms"],
        "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
        "LN_WT_BASE_ADDR":  baddrs & 0xffffffff,
        "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
    }
    return TestbenchSIM("testbench_LN", define)


Op.Get("accel.hbm.layer_norm").attrs["cfg_id"] = 0b00000111
Op.Get("accel.hbm.layer_norm").attrs["testbench"] = LayerNormTestbench


def PosEmbDriver(args, output, attrs):
    return FPGA_Run_PosEmb(args[0], args[1], output, 0)


def PosEmbTestbench(args, output, attrs):
    dtensor, ptensor = args[0], args[1]
    dshape, oshape = dtensor[0].shape, output[0].shape
    daddrs, paddrs, oaddrs = dtensor[1], ptensor[1], output[1]
    if attrs["kvcache"]:
        define = {
            "Feature_Head": dshape[0], "Token": 19, "KV_cache_mode": attrs["kvcache"],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "POS_IN_BASE_ADDR":  paddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    elif attrs["padding"]:
        max_token = dtensor[0].device.MAX_TOKEN
        define = {
            "Feature_Head": dshape[0], "Token": dshape[1], "KV_cache_mode": attrs["kvcache"],
            "Win": max_token, "Wout": max_token,
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "POS_IN_BASE_ADDR":  paddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    else:
        define = {
            "Feature_Head": dshape[0], "Token": dshape[1], "KV_cache_mode": attrs["kvcache"],
            "Win": dshape[1], "Wout": dshape[1],
            "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
            "POS_IN_BASE_ADDR":  paddrs & 0xffffffff,
            "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
        }
    return TestbenchSIM("testbench_EMB", define)


Op.Get("accel.hbm.pos_emb").attrs["cfg_id"] = 0b00000100
Op.Get("accel.hbm.pos_emb").attrs["testbench"] = PosEmbTestbench


def TransposeDriver(args, output, attrs):
    return FPGA_Run_Transpose(args[0], output, attrs["out_and_in_mode"], attrs["log2_step"], 0)


def TransposeTestbench(args, output, attrs):
    dtensor = args[0]
    dshape = dtensor[0].shape
    daddrs, oaddrs = dtensor[1], output[1]
    define = {
        "log2_WT_base_addr_Bank_Step": attrs["log2_step"],
        "Token": dshape[-2], "Width_in": dshape[-1],
        "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
        "HBM00_WT_BASE_ADDR": oaddrs & 0xffffffff,
    }
    return TestbenchSIM("testbench_TRANSPOSE_HBM", define)


Op.Get("accel.hbm.transpose").attrs["cfg_id"] = 0b00000011
Op.Get("accel.hbm.transpose").attrs["testbench"] = TransposeTestbench


def Feature2WeightDriver(args, output, attrs):
    return FPGA_Run_Feature2Weight(args[0], output, attrs["out_and_in_mode"], attrs["log2_step"], 0)


def Feature2WeightTestbench(args, output, attrs):
    dtensor = args[0]
    dshape = dtensor[0].shape
    daddrs, oaddrs = dtensor[1], output[1]
    define = {
        "log2_WT_base_addr_Bank_Step": attrs["log2_step"], 
        "Token": dshape[-2], "Width_in": dshape[-1],
        "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
        "HBM00_WT_BASE_ADDR":  oaddrs & 0xffffffff,
    }
    return TestbenchSIM("testbench_Feature2Weight_HBM", define)


Op.Get("accel.hbm.feature2weight").attrs["cfg_id"] = 0b00000010
Op.Get("accel.hbm.feature2weight").attrs["testbench"] = Feature2WeightTestbench


def ActivateDriver(args, output, attrs):
    return FPGA_Run_Activation(args[1], args[0], output, 0)


def ActivateTestbench(args, output, attrs):
    dtensor, btensor = args[0], args[1]
    dshape, bshape = dtensor[0].shape, btensor[0].shape
    daddrs, baddrs, oaddrs = dtensor[1], btensor[1], output[1]
    define = {
        "Height": dshape[1], "Width_in": dshape[2],
        "DAT_IN_BASE_ADDR":  daddrs & 0xffffffff,
        "WT_BASE_ADDR":  baddrs & 0xffffffff,
        "DAT_OUT_BASE_ADDR": oaddrs & 0xffffffff,
    }
    return TestbenchSIM("testbench_ACT", define)


Op.Get("accel.hbm.activate").attrs["cfg_id"] = 0b00000110
Op.Get("accel.hbm.activate").attrs["testbench"] = ActivateTestbench
