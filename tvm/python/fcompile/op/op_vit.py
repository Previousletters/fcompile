import os
import subprocess
import numpy as np

from .op_base import Op
from .op_fpga import register_fpga_op, make_define
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..utils import list2str
from ..simulate import readmemh, writememh


class AccelOp(Op):

    name = "vit_op"
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

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data, weight = args
        ret_name, cfg_name = "conv_" + name, "cfg_" + name
        dat_name, wet_name = data["name"], weight["name"]
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
        if weight["free"] and weight["static"] == 0:
            ret["callop"].append(FreeWeight(wet_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


@register_fpga_op("accel.vit.conv2d")
class Conv2D(AccelOp):

    name = "vit_conv2d"

    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
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
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, o_bw, self.shape)
        return output


#TODO: this op may should have different dim from Conv2D
@register_fpga_op("accel.vit.mm")
class MM(AccelOp):

    name = "vit_mm"

    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
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
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, o_bw, self.shape)
        return output


@register_fpga_op("accel.vit.softmax")
class Softmax(Op):

    name = "vit_softmax"
    arg_types = [[OpType.f_ddr]]
    ret_type = OpType.f_ddr

    def param_process(self, *largs):
        return [np.array([])]

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


    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_softmax"
        data = args[0]
        _, dh, dw, c = data.shape
        new_shape = ((dh*dw+tout-1)//tout, tout, c)
        new_data = data.reshape((dh*dw, c))
        new_input = np.zeros(new_shape, dtype=data.dtype)
        for dim0 in range(new_shape[0]):
            for dim1 in range(new_shape[1]):
                for dim2 in range(new_shape[2]):
                    if dim0*tout+dim1 < dh*dw:
                        new_input[dim0, dim1, dim2] = new_data[dim0*tout+dim1, dim2]
        d_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        writememh(input_dt, new_input, d_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Height" : dh*dw, 
            "Width_in" : c, "in_scale" : d_sc, "out_scale" : o_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        new_result = readmemh(output_dt, o_bw, new_shape)
        new_output = np.zeros((dh*dw, c), dtype="int8")
        for dim0 in range(new_shape[0]):
            for dim1 in range(new_shape[1]):
                for dim2 in range(new_shape[2]):
                    if dim0*tout+dim1 < dh*dw:
                        new_output[dim0*tout+dim1, dim2] = new_result[dim0, dim1, dim2]
        output = new_output.reshape(data.shape)
        return output    


@register_fpga_op("accel.vit.transpose")
class Transpose(Op):

    name = "vit_transpose"
    arg_types = [[OpType.f_ddr]]
    ret_type = OpType.w_ddr

    def param_process(self, *largs):
        return [np.array([])]

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = "tp_" + name
        dat_name = data["name"]
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        _, oh, ow, o = self.shape
        d_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, o_sc = [int(i) for i in attrs["scales"]]
        tin, tout = attrs["tin"], attrs["tout"]
        # TODO: finish relu, gelu, mode in attrs
        mode = 0
        mal_str = MallocFeature(ret_name, [oh, ow, o, o_sc, o_sc, o_bw])
        fun_str = f"FPGA_Run_Transpose({dat_name}, {ret_name}, {mode});"
        ret["callop"] = [mal_str, fun_str]
        if data["free"]:
            ret["callop"].append(FreeFeature(dat_name))
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape, "static" : 0}
        return ret


    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_TRANSPOSE"
        data = args[0]
        _, _, dh, dw = data.shape
        nw = int(((dw + tin - 1)//tin) * tin)
        new_shape = (dh, nw)
        new_input = np.zeros(new_shape, dtype=data.dtype)
        new_input[:, :dw] = data[0, 0, :, :]
        d_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        writememh(input_dt, new_input, d_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Height" : dh, "Width_in" : nw
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        out_shape = (nw, dh)
        new_result = readmemh(output_dt, o_bw, out_shape)
        new_output = new_result[:dw, :]
        output = new_output.reshape(self.shape)
        return output


#TODO: how to make k_factor and bias together into w_ddr
@register_fpga_op("accel.vit.layer_norm")
class LayerNorm(Op):

    name = "vit_layer_norm"
    arg_types = [[OpType.f_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

    def param_process(self, *largs):
        return [np.array([])]

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


    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_LN"
        data, k_bias = args
        _, _, dh, dw = data.shape
        d_bw, w_bw, b_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, b_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        writememh(input_dt, data, d_bw)
        writememh(input_wt, k_bias, w_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Height" : dh, "Width_in" : dw,
            "in_scale" : d_sc, "wt_scale" : w_sc, "bias_scale" : b_sc, "out_scale" : o_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, o_bw, self.shape)
        return output


#TODO: the fpga function may not in lib
@register_fpga_op("accel.vit.conv2d_add")
class Conv2DResAdd(Op):

    name = "vit_conv2d_add"
    arg_types = [[OpType.f_ddr], [OpType.w_ddr], [OpType.f_ddr]]
    ret_type = OpType.f_ddr

    #TODO args should be flexible for multi-input
    def param_process(self, args, attrs, tin, tout):
        _, weight = args
        if weight.size == 0:
            return [np.array([]), np.array([]), np.array([])]
        _, wt_width, _ = attrs["widths"]
        H, W, I, O = weight.shape
        new_weight = HWIO2OIHW(weight)
        new_weight = parallel_weight_Tin(new_weight, O, I, H, W, tout, tin)
        new_weight = np_to_bin(new_weight, int(wt_width))
        return [np.array([]), new_weight, np.array([])]


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


    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_mp_conv_res"
        data, weight, res_add = args
        _, dh, dw, c = data.shape
        kh, kw, i, o = weight.shape
        _, oh, ow, _ = self.shape
        ph, pw = [int(i) for i in attrs["padding"]]
        sh, sw = [int(i) for i in attrs["strides"]]
        d_bw, w_bw, r_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, r_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        input_ad = os.path.join(SIM_ROOT, "run", "input_ad")
        writememh(input_dt, data, d_bw)
        writememh(input_wt, weight, w_bw)
        writememh(input_ad, res_add, r_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Hin_L0" : dh, "Win_L0" : dw, "CHin_L0" : c, "CHout_L0" : o,
            "Kx_L0" : kw, "Ky_L0" : kh, "Sx_L0" : sw, "Sy_L0" : sh, "Px_L0" : pw, "Py_L0" : ph, 
            "DAT_IN_scale_L0" : d_sc, "WT_scale_L0" : w_sc, "Conv_out_scale_L0" : o_sc, "Res_Add_scale" : r_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, o_bw, self.shape)
        return output


@register_fpga_op("accel.vit.activate")
class Activate(Op):

    name = "vit_activate"
    arg_types = [[OpType.f_ddr]]
    ret_type = OpType.f_ddr

    #TODO args should be flexible for multi-input
    def param_process(self, args, attrs, tin, tout):
        return [np.array([])]

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


    def modelsim(self, args, attrs, tin, tout):
        from ..config import SIM_ROOT, SIM_HIDE_STDOUT
        tb_name = "testbench_mp_conv_res"
        data, weight, res_add = args
        _, dh, dw, c = data.shape
        kh, kw, i, o = weight.shape
        _, oh, ow, _ = self.shape
        ph, pw = [int(i) for i in attrs["padding"]]
        sh, sw = [int(i) for i in attrs["strides"]]
        d_bw, w_bw, r_bw, o_bw = [int(i) for i in attrs["widths"]]
        d_sc, w_sc, r_sc, o_sc = [int(i) for i in attrs["scales"]]
        input_dt = os.path.join(SIM_ROOT, "run", "input_dt")
        input_wt = os.path.join(SIM_ROOT, "run", "input_wt")
        input_ad = os.path.join(SIM_ROOT, "run", "input_ad")
        writememh(input_dt, data, d_bw)
        writememh(input_wt, weight, w_bw)
        writememh(input_ad, res_add, r_bw)
        define = {
            "DAT_DW_L0" : d_bw, "DAT_DW_L1" : o_bw, "Hin_L0" : dh, "Win_L0" : dw, "CHin_L0" : c, "CHout_L0" : o,
            "Kx_L0" : kw, "Ky_L0" : kh, "Sx_L0" : sw, "Sy_L0" : sh, "Px_L0" : pw, "Py_L0" : ph, 
            "DAT_IN_scale_L0" : d_sc, "WT_scale_L0" : w_sc, "Conv_out_scale_L0" : o_sc, "Res_Add_scale" : r_sc
        }
        define_str = make_define(define)
        cmd = f"make sim DEFINES={define_str} TB_NAME={tb_name}"
        if SIM_HIDE_STDOUT:
            subprocess.run(cmd, stdout=subprocess.DEVNULL, shell=True, cwd=SIM_ROOT)
        else:
            subprocess.run(cmd, shell=True, cwd=SIM_ROOT)
        output_dt = os.path.join(SIM_ROOT, "run", "output_dt")
        output = readmemh(output_dt, o_bw, self.shape)
        return output
