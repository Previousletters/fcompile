from ..adr import Op, Tensor
from .basic import get_tensor_size, Tout
from .conv import *
from .matrix import *
import subprocess


def TestbenchSIM(tb_name: str, define: dict) -> list:
    from .config import template_rtl, tb_sim_path
    csb_rtl = []
    cmd_rtl = list(template_rtl) + [tb_sim_path]
    define_cfg = [f"+define+{k}={v}" for k, v in define.items()]
    cmd_rtl.append("TOP_MODULE=" + tb_name)
    cmd_rtl.append("SIM_DEFINE=\"" + "".join(define_cfg) + "\"")
    p_rtl = subprocess.Popen(cmd_rtl, stdout=subprocess.PIPE)
    out_rtl, rtl_err = p_rtl.communicate()
    saved_out_rtl = out_rtl.decode("utf-8")
    out_rtl = out_rtl.decode("utf-8").replace("# ", "").split("\n")
    for out in out_rtl:
        if "csb_rtl" in out:
            eval(out)
    if len(csb_rtl) == 0:
        raise RuntimeError(saved_out_rtl)
    return csb_rtl


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
        oshape, odtype, oaddrs = output[0].shape, output[0].dtype, output[1]
        output_offset = [output[0], (oaddrs & 0xffffffff) + oshape[1] * (oshape[2] + Tout - 1) // Tout * odtype.get_bytes()]
        regs += FPGA_RunHBM_MVM_F2W(0, 0b00110011111, args[0], args[1], output, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        regs += FPGA_RunHBM_MVM_F2W(0, 0b00110011111, args[0], args[2], output_offset, 2, 0, 0, 0, 0, attrs["log2_step"], 0)
        return regs
    else:
        print("Error! Not support skip > 2")
        exit(-1)


Op.Get("accel.hbm.mvm").attrs["cfg_id"] = 0b00001000
Op.Get("accel.hbm.mvm").attrs["driver"] = MVMDriver
Op.Get("accel.hbm.mvm").attrs["testbench"] = MVMTestbench


def MVMBNDriver(args, output, attrs):
    if attrs["skip"] == 1:
        return FPGA_RunHBM_MVM(0, 0b01100011111, args[0], args[1], args[2], [None, None], output, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    else:
        print("Error! Not support skip > 1")
        exit(-1)


Op.Get("accel.hbm.mvm_bn").attrs["cfg_id"] = 0b01001000
Op.Get("accel.hbm.mvm_bn").attrs["driver"] = MVMBNDriver


def MVMBNResDriver(args, output, attrs):
    if attrs["arg_max"] and attrs["skip"] == 1:
        output0, output1 = output
        return FPGA_RunHBM_MVM_BN_Res_ArgMax(attrs["res_mode"], 0b111100011111, args[0], args[1], args[2], args[3], output0, output1, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    elif attrs["arg_max"] == 0 and attrs["skip"] == 1:
        return FPGA_RunHBM_MVM(attrs["res_mode"], 0b11100011111, args[0], args[1], args[2], args[3], output, attrs["skip"], 0, 0, 0, 0, attrs["log2_step"], 0)
    elif attrs["skip"] == 2:
        regs = []
        oshape, odtype, oaddrs = output[0].shape, output[0].dtype, output[1]
        output_offset = [output[0], (oaddrs & 0xffffffff) + oshape[1] * (oshape[2] + Tout - 1) // Tout * odtype.get_bytes()]
        regs += FPGA_RunHBM_MVM_BN_Res_afterTRP(attrs["res_mode"], 0b11101011111, args[0], args[1], args[3], args[4], output, 1, 0, 0, 0, 0, attrs["log2_step"], 0)
        regs += FPGA_RunHBM_MVM_BN_Res_afterTRP(attrs["res_mode"], 0b11101011111, args[0], args[2], args[3], args[4], output_offset, 1, 0, 0, 0, 0, attrs["log2_step"], 0)
        return regs
    else:
        print("Error! Not support this attrs: ")
        print(attrs)
        exit(-1)


Op.Get("accel.hbm.mvm_bn_res").attrs["cfg_id"] = 0b11001000
Op.Get("accel.hbm.mvm_bn_res").attrs["driver"] = MVMBNResDriver


def AddDriver(args, output, attrs):
    return []

Op.Get("accel.hbm.add").attrs["cfg_id"] = 0b00000001
Op.Get("accel.hbm.add").attrs["driver"] = AddDriver


def SoftmaxDriver(args, output, attrs):
    return FPGA_Run_Softmax(args[0], output, 0)


Op.Get("accel.hbm.softmax").attrs["cfg_id"] = 0b00000101
Op.Get("accel.hbm.softmax").attrs["driver"] = SoftmaxDriver


def LayerNormDriver(args, output, attrs):
    return FPGA_Run_LN(args[0], args[1], output, attrs["rms"], 0)


Op.Get("accel.hbm.layer_norm").attrs["cfg_id"] = 0b00000111
Op.Get("accel.hbm.layer_norm").attrs["driver"] = LayerNormDriver


def PosEmbDriver(args, output, attrs):
    return FPGA_Run_PosEmb(args[0], args[1], output, 0)


Op.Get("accel.hbm.pos_emb").attrs["cfg_id"] = 0b00000100
Op.Get("accel.hbm.pos_emb").attrs["driver"] = PosEmbDriver


def TransposeDriver(args, output, attrs):
    return FPGA_Run_Transpose(args[0], output, attrs["out_and_in_mode"], attrs["log2_step"], 0)


Op.Get("accel.hbm.transpose").attrs["cfg_id"] = 0b00000011
Op.Get("accel.hbm.transpose").attrs["driver"] = TransposeDriver


def Feature2WeightDriver(args, output, attrs):
    return FPGA_Run_Feature2Weight(args[0], output, attrs["out_and_in_mode"], attrs["log2_step"], 0)


Op.Get("accel.hbm.feature2weight").attrs["cfg_id"] = 0b00000010
Op.Get("accel.hbm.feature2weight").attrs["driver"] = Feature2WeightDriver


def ActivateDriver(args, output, attrs):
    return FPGA_Run_Activation(args[1], args[0], output, 0)


Op.Get("accel.hbm.activate").attrs["cfg_id"] = 0b00000110
Op.Get("accel.hbm.activate").attrs["driver"] = ActivateDriver