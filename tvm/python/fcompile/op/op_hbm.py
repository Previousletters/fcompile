import os
import subprocess

from .op_base import Op
from .op_fpga import register_fpga_op, make_define
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..simulate import readmemh_fp as readmemh
from ..simulate import writememh_fp as writememh


@register_fpga_op("accel.hbm.matmul")
class MatMul(Op):

    name = "hbm_matmul"
    arg_types = [[OpType.f_ddr], [OpType.w_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.hbm.linear")
class Linear(Op):

    name = "hbm_linear"
    arg_types = [[OpType.f_ddr], [OpType.w_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.transpose")
class Transpose(Op):

    name = "hbm_transpose"
    arg_types = [[OpType.f_ddr]]
    ret_type = OpType.w_ddr

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


@register_fpga_op("accel.hbm.add")
class Add(Op):

    name = "hbm_add"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.hbm.batch_norm")
class BatchNorm(Op):

    name = "hbm_batch_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.hbm.bias")
class Bias(Op):

    name = "hbm_bias"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

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


# TODO: how to make k_factor and bias together into f_ddr
@register_fpga_op("accel.hbm.layer_norm")
class LayerNorm(Op):

    name = "hbm_layer_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.hbm.rms_norm")
class RMSNorm(Op):

    name = "hbm_rms_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

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


@register_fpga_op("accel.hbm.activation")
class Activation(Op):

    name = "hbm_activation"
    arg_types = [[OpType.f_ddr], [OpType.c_ptr]]
    ret_type = OpType.f_ddr

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
