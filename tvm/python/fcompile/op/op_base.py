from ..utils import shape2byte
from ..codegen import OpType
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
    arg_types = [[OpType.c_ptr, OpType.t_val]]
    ret_type = OpType.f_ddr
    map_func = {OpType.c_ptr : CPtr2Feature, OpType.t_val : DLTensor2Feature}

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = self.name + "_" + name
        dat_name = data["name"]
        _, h, w, c = data["shape"]
        width, scale = int(attrs["widths"][0]), int(attrs["scales"][0])
        mal_str = MallocFeature(ret_name, [h, w, c, scale, scale, width])
        map_str = self.map_func[data["type"]](dat_name, ret_name)
        ret["callop"] = [mal_str, map_str]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


class AccelWMap(Op):

    name = "accel_wmap"
    arg_types = [[OpType.c_ptr, OpType.t_val, OpType.const]]
    ret_type = OpType.w_ddr

    def fpga_jit(self, name, args, attrs):
        ret = {}
        data = args[0]
        ret_name = self.name + "_" + name
        dat_name = data["name"]
        h, w, i, o = data["shape"]
        width, scale = int(attrs["widths"][0]), int(attrs["scales"][0])
        if data["type"] == OpType.const:
            ret["static"] = {"name" : ret_name, "type" : self.ret_type, "attrs" : [h, w, i, o, scale, width]}
        else:
            raise RuntimeError("Haven't support runtime parameter process!")
        map_str = CPtr2Weight(dat_name, ret_name)
        ret["params"] = [map_str]
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape}
        return ret


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
            raise RuntimeError("Haven't support runtime parameter process!")
        ret["return"] = {"name" : ret_name, "type" : self.ret_type, "shape" : self.shape, "pointer" : dat_name}
        return ret