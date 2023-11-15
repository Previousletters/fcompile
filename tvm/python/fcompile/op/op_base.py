from ..utils import shape2byte
from ..codegen import OpType, Layout
from ..codegen.function import *

class Op:

    name = "op"
    arg_types = []
    ret_type = None

    def __init__(self, shape, dtype):
        self.shape = shape
        self.dtype = dtype

    def fpga_jit(self, name):
        pass

    def param_process(self, args, attrs, tin, tout):
        import numpy as np
        return [np.array([]) for arg in args]

    def infer_rt(self, args, attrs, tin, tout):
        self.rt_shape = self.shape
        return True


class Input(Op):

    name = "input"
    ret_type = OpType.c_ptr

    def fpga_jit(self, name):
        ret = {}
        var_name = self.name + "_" + name
        byte = shape2byte(self.shape, self.dtype)
        ret["static"] = {"name" : var_name, "type" : self.ret_type, "attrs" : [byte]}
        ret["return"] = {"name" : var_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


class Const(Op):

    name = "const"
    ret_type = OpType.const

    def fpga_jit(self, name, byte):
        ret = {}
        var_name = self.name + "_" + name
        ret["static"] = {"name" : var_name, "type" : self.ret_type, "attrs" : [byte]}
        ret["return"] = {"name" : var_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


class Output(Op):

    name = "output"
    arg_types = [[OpType.f_ddr, OpType.t_val]]
    ret_type = OpType.c_ptr

    def fpga_jit(self, name, args, _):
        ret = {}
        data = args[0]
        byte = shape2byte(self.shape, self.dtype)
        if data["type"] == OpType.f_ddr:
            ret_name = self.name + "_" + name
            dat_name = data["name"]
            demap_str = Feature2CPtr(dat_name, ret_name)
            ret["static"] = {"name" : ret_name, "type" : self.ret_type, "attrs" : [byte]}
            ret["callop"] = [demap_str]
            if data["free"]:
                ret["callop"].append(FreeFeature(dat_name))
        elif data["type"] == OpType.t_val:
            ret_name = data["pointer"]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


class AccelFMap(Op):

    name = "accel_fmap"
    arg_types = [[OpType.c_ptr, OpType.t_val, OpType.w_ddr]]
    ret_type = OpType.f_ddr
    map_func = {OpType.c_ptr : CPtr2Feature, OpType.t_val : DLTensor2Feature, OpType.w_ddr : Weight2Feature}

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = self.name + "_" + name
        dat_name = data["name"]
        _, h, w, c = data["shape"]
        if len(attrs["scales"]) == 1:
            width, scale = int(attrs["widths"][0]), int(attrs["scales"][0])
            mal_str = MallocFeature(ret_name, [h, w, c, scale, scale, width])
        elif len(attrs["scales"]) == 2:
            width, scale0, scale1 = int(attrs["widths"][0]), int(attrs["scales"][0]), int(attrs["scales"][1])
            mal_str = MallocFeature(ret_name, [h, w, c, scale0, scale1, width])
        map_str = self.map_func[data["type"]](dat_name, ret_name)
        ret["callop"] = [mal_str, map_str]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret

    def infer_rt(self, attrs, tin, tout):
        _, h, w, c = self.shape
        n = (c + tout - 1) // tout
        new_shape = (n, h, w, tout)
        self.rt_shape = new_shape


class AccelWMap(Op):

    name = "accel_wmap"
    arg_types = [[OpType.c_ptr, OpType.t_val, OpType.const, OpType.f_ddr]]
    ret_type = OpType.w_ddr

    def fpga_jit(self, name, args, attrs):
        ret = {}
        static = 0
        data = args[0]
        ret_name = self.name + "_" + name
        dat_name = data["name"]
        h, w, i, o = data["shape"]
        width, scale = int(attrs["widths"][0]), int(attrs["scales"][0])
        if data["type"] == OpType.const:
            static = 1
            ret["static"] = {"name" : ret_name, "type" : self.ret_type, "attrs" : [h, w, i, o, scale, width]}
        if data["type"] == OpType.f_ddr:
            mal_str = MallocWeight(ret_name, [h, w, i, o, scale, width])
            map_str = Feature2Weight(dat_name, ret_name)
            ret["callop"] = [mal_str, map_str]
        else:
            map_str = CPtr2Weight(dat_name, ret_name)
            ret["params"] = [map_str]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape, "static" : static}
        return ret

    def infer_rt(self, args, attrs, tin, tout):
        self.rt_shape = self.shape
        return True


class TVMMap(Op):

    name = "tvm_map"
    arg_types = [[OpType.c_ptr, OpType.f_ddr]]
    ret_type = OpType.t_val

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = self.name + "_" + name
        dat_name = data["name"]
        if data["type"] == OpType.c_ptr:
            dlt_str = CreateDLTensor(ret_name, dat_name, self.shape)
            ret["callop"] = [dlt_str]
        elif data["type"] == OpType.f_ddr:
            ptr_name = "op_" + name
            byte = shape2byte(self.shape, self.dtype)
            dlt_str = CreateDLTensor(ret_name, ptr_name, self.shape)
            map_str = Feature2DLTensor(dat_name, ret_name)
            ret["static"] = {"name" : ptr_name, "type" : OpType.c_ptr, "attrs" : [byte]}
            ret["callop"] = dlt_str + [map_str]
            if data["free"]:
                ret["callop"].append(FreeFeature(dat_name))
        else:
            raise RuntimeError("Haven't support infer_rt parameter process!")
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape, "pointer" : dat_name}
        return ret
