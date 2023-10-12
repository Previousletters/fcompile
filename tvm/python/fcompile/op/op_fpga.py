import numpy as np

from .op_base import Op
from ..codegen import OpType
from ..codegen.function import *
from ..process import *
from ..utils import list2str

FPGA_OP_MAP = {}

def register_fpga_op(op_name, level=10):
    def _register_fn(fn):
        global FPGA_OP_MAP
        FPGA_OP_MAP[op_name] = fn

        def _call(*args, **kwargs):
            return fn(*args, **kwargs)

        return _call

    return _register_fn


@register_fpga_op("accel.vit.conv2d")
class Conv2D(Op):

    name = "accel_conv2d"
    arg_types = [[OpType.f_ddr], [OpType.w_ddr]]
    ret_type = OpType.f_ddr

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
        tin, tout = attrs["tin"], attrs["tout"]
        _, dh, dw, c = data["shape"]
        kh, kw, i, o = weight["shape"]
        _, oh, ow, _ = self.shape
        ph, _, pw, _ = [int(i) for i in attrs["padding"]]
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