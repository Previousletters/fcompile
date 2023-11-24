import os
import subprocess
import numpy as np

from .op_base import Op
from .op_fpga import register_fpga_op, make_define
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..utils import list2str
from ..simulate import readmemh_fp as readmemh
from ..simulate import writememh_fp as writememh


# TODO: how to make k_factor and bias together into w_ddr
@register_fpga_op("accel.hbm.layer_norm")
class LayerNorm(Op):

    name = "hbm_layer_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

    def param_process(self, args, attrs, tin, tout):
        _, weight = args
        if weight.size == 0:
            return [np.array([]), np.array([])]
        if len(weight.shape) == 1:
            return [np.array([]), weight]
        _, wt_width, _, _ = attrs["widths"]
        H, W, I, O = weight.shape
        new_weight = HWIO2OIHW(weight)
        new_weight = parallel_weight_Tin(new_weight, O, I, H, W, tout, tin)
        new_weight = np_to_bin(new_weight, int(wt_width))
        return [np.array([]), new_weight]

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data, weight = args
        ret_name = "ln_" + name
        dat_name, wet_name = data["name"], weight["name"]
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        d_bw, _, _, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, _, _, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        # TODO: finish relu, gelu, mode in attrs
        relu, gelu, mode = 0, 0, 0
        mal_str = MallocFeature(ret_name, [dh, dw, c, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Run_LN({dat_name}, {wet_name}, {ret_name}, {mode});"
        ret["callop"] = [mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    def tb_simulate(self, args, attrs, tin, tout, simulator):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_LN"
        data, k_bias = args
        _, _, dh, dw = data.shape
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data)
        writememh(input_wt, k_bias)
        define = {
            "Height": dh, "Width_in": dw, "RMS_Norm": 0
        }
        define_str = make_define(define, simulator)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, self.shape)
        return output


# TODO: how to make k_factor and bias together into w_ddr
@register_fpga_op("accel.hbm.rms_norm")
class RMSNorm(Op):

    name = "hbm_rms_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

    def param_process(self, args, attrs, tin, tout):
        _, weight = args
        if weight.size == 0:
            return [np.array([]), np.array([])]
        if len(weight.shape) == 1:
            return [np.array([]), weight]
        _, wt_width, _, _ = attrs["widths"]
        H, W, I, O = weight.shape
        new_weight = HWIO2OIHW(weight)
        new_weight = parallel_weight_Tin(new_weight, O, I, H, W, tout, tin)
        new_weight = np_to_bin(new_weight, int(wt_width))
        return [np.array([]), new_weight]

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data, weight = args
        ret_name = "ln_" + name
        dat_name, wet_name = data["name"], weight["name"]
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        d_bw, _, _, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, _, _, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        # TODO: finish relu, gelu, mode in attrs
        relu, gelu, mode = 0, 0, 0
        mal_str = MallocFeature(ret_name, [dh, dw, c, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Run_LN({dat_name}, {wet_name}, {ret_name}, {mode});"
        ret["callop"] = [mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    def tb_simulate(self, args, attrs, tin, tout, simulator):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_LN"
        data, k_bias = args
        _, _, dh, dw = data.shape
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data)
        writememh(input_wt, k_bias)
        define = {
            "Height": dh, "Width_in": dw, "RMS_Norm": 1
        }
        define_str = make_define(define, simulator)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, self.shape)
        return output


@register_fpga_op("accel.hbm.softmax")
class Softmax(Op):

    name = "hbm_softmax"
    arg_types = [[OpType.f_ddr]]
    ret_type = OpType.f_ddr

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = "softmax_" + name
        dat_name = data["name"]
        _, dh, dw, c = data["shape"]
        d_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        pixel_in, pixel_out, mode = dh*dw, c, 0
        mal_str = MallocFeature(ret_name, [dh, dw, c, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Run_Softmax({dat_name}, {ret_name}, {pixel_in}, {pixel_out}, {mode});"
        ret["callop"] = [mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    def tb_simulate(self, args, attrs, tin, tout, simulator):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_SOFTMAX"
        data = args[0]
        _, _, h, w = data.shape
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        writememh(input_dt, data)
        define = {
            "Height": h, "Width_in": w
        }
        define_str = make_define(define, simulator)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, self.shape)
        return output


@register_fpga_op("accel.hbm.activation")
class Activation(Op):

    name = "hbm_activation"
    arg_types = [[OpType.f_ddr], [OpType.c_ptr]]
    ret_type = OpType.f_ddr

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = "softmax_" + name
        dat_name = data["name"]
        _, dh, dw, c = data["shape"]
        d_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        pixel_in, pixel_out, mode = dh*dw, c, 0
        mal_str = MallocFeature(ret_name, [dh, dw, c, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Run_Softmax({dat_name}, {ret_name}, {pixel_in}, {pixel_out}, {mode});"
        ret["callop"] = [mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    def tb_simulate(self, args, attrs, tin, tout, simulator):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_ACT"
        data, weight = args
        _, _, h, w = data.shape
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data)
        writememh(input_wt, weight)
        define = {
            "Height": h, "Width_in": w
        }
        define_str = make_define(define, simulator)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, self.shape)
        return output


