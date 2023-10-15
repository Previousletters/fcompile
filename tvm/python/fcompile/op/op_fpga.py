import os
import subprocess
import numpy as np

from .op_base import Op
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..utils import list2str
from ..simulate import readmemh, writememh

FPGA_OP_MAP = {}

def register_fpga_op(op_name, level=10):
    def _register_fn(fn):
        global FPGA_OP_MAP
        FPGA_OP_MAP[op_name] = fn

        def _call(*args, **kwargs):
            return fn(*args, **kwargs)

        return _call

    return _register_fn

def make_define(define:dict) -> str:
    define_list = []
    for key, value in define.items():
        tp_str = f"+define+{key}={value}"
        define_list.append(tp_str)
    define_str = "".join(define_list)  
    return "\"" + define_str + "\""


class AccelOp(Op):

    name = "accel_op"
    arg_types = [[OpType.f_ddr], [OpType.w_ddr]]
    ret_type = OpType.f_ddr

    #TODO args should be flexible for multi-input
    def param_process(self, args, attrs, tin, tout):
        _, weight = args
        if weight.size == 0:
            return [np.array([]), np.array([])]
        _, wt_width, _ = attrs["widths"]
        H, W, I, O = weight.shape
        new_weight = HWIO2OIHW(weight)
        new_weight = parallel_weight_Tin(new_weight, O, I, H, W, tout, tin)
        new_weight = np_to_bin(new_weight, int(wt_width))
        return [np.array([]), new_weight]


@register_fpga_op("accel.vit.conv2d")
class Conv2D(AccelOp):

    name = "accel_conv2d"

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data, weight = args
        ret_name, cfg_name = "conv_" + name, "cfg_" + name
        dat_name, wet_name = data["name"], weight["name"]
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        kh, kw, i, o = weight["shape"]
        _, oh, ow, _ = self.shape
        ph, pw = [int(i) for i in attrs["padding"]]
        sh, sw = [int(i) for i in attrs["strides"]]
        d_bw, w_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        # TODO: finish relu, gelu, mode in attrs
        relu, gelu, mode = 0, 0, 0
        cfg_list = get_conv_cfg("conv", dh, dw, c, o, kh, kw, sh, sw, ph, pw, d_bw, tin, tout)
        cfg_str = f"struct Conv_Cfg {cfg_name} = " + "{" + list2str(cfg_list) + "};"
        mal_str = MallocFeature(ret_name, [oh, ow, o, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Conv({cfg_name}, {relu}, {gelu}, {mode}, {dat_name}, {wet_name}, {ret_name});"
        ret["callop"] = [cfg_str, mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    
    def modelsim(self, args, attrs):
        from ..config import SIM_ROOT, SIM_HINT_STDOUT
        tb_name = "testbench_conv_mp"
        data, weight = args
        _, dh, dw, c = data.shape
        kh, kw, i, o = weight.shape
        _, oh, ow, _ = self.shape
        ph, pw = [int(i) for i in attrs["padding"]]
        sh, sw = [int(i) for i in attrs["strides"]]
        d_bw, w_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data, d_bw)
        writememh(input_wt, weight, w_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Hin_L0" : dh, "Win_L0" : dw, "CHin_L0" : c, "CHout" : o,
            "Kx_L0" : kw, "Ky_L0" : kh, "Sx_L0" : sw, "Sy_L0" : sh, "Px_L0" : pw, "Py_L0" : ph, 
            "DAT_IN_scale_L0" : d_sc, "WT_scale_L0" : w_sc, "Conv_out_scale_L0" : o_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HINT_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, d_bw, self.shape)
        return output

    
@register_fpga_op("accel.vit.mm")
class MM(AccelOp):

    name = "accel_mm"

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data, weight = args
        ret_name, cfg_name = "conv_" + name, "cfg_" + name
        dat_name, wet_name = data["name"], weight["name"]
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        kh, kw, i, o = weight["shape"]
        _, oh, ow, _ = self.shape
        ph, pw = [int(i) for i in attrs["padding"]]
        sh, sw = [int(i) for i in attrs["strides"]]
        d_bw, w_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        # TODO: finish relu, gelu, mode in attrs
        relu, gelu, mode = 0, 0, 0
        cfg_list = get_conv_cfg("conv", dh, dw, c, o, kh, kw, sh, sw, ph, pw, d_bw, tin, tout)
        cfg_str = f"struct Conv_Cfg {cfg_name} = " + "{" + list2str(cfg_list) + "};"
        mal_str = MallocFeature(ret_name, [oh, ow, o, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Conv({cfg_name}, {relu}, {gelu}, {mode}, {dat_name}, {wet_name}, {ret_name});"
        ret["callop"] = [cfg_str, mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    
    def modelsim(self, args, attrs):
        from ..config import SIM_ROOT, SIM_HINT_STDOUT
        tb_name = "testbench_MVM"
        data, weight = args
        _, dh, dw, c = data.shape
        kh, kw, i, o = weight.shape
        d_bw, w_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data, d_bw)
        writememh(input_wt, weight, w_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Hin_L0" : dh, "Win_L0" : dw, "CHin_L0" : c, "CHout_L0" : o,
            "DAT_IN_scale_L0" : d_sc, "WT_scale_L0" : w_sc, "Conv_out_scale_L0" : o_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HINT_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, d_bw, self.shape)
        return output