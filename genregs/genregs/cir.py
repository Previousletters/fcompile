from .driver.conv import FPGA_RunHBM_FC, FPGA_HBM_MVM, Get_MVM_Cfg
from .driver.matrix import FPGA_Run_LN, FPGA_Run_Softmax, FPGA_Run_Transpose
from .driver.basic import Malloc_Weight, Malloc_Feature, Mapped_Weight, Mapped_Feature

# AST data structure
class Op:

    op_name = "op"

    def __init__(self, *args):
        self.args = args


class Input(Op):

    op_name = "input"

    def __init__(self, name, shape=None, dtype=None):
        self.name = name
        if shape != None and dtype != None:
            self.set_shape(shape, dtype)

    def build(self, shape, dtype):
        self.input_addr = {}
        if dtype == "feature":
            return Malloc_Feature(shape[0], shape[1], shape[2], 0, 0, 16)
        elif dtype == "weight":
            return Malloc_Weight(shape[0], shape[1], shape[2], shape[3], 0, 4)

    def set_shape(self, shape, dtype):
        self.shape, self.dtype = shape, dtype


class FC(Op):

    op_name = "fc"

    def infer(self, f, w):
        if f[1] != "feature" or w[1] != "weight":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight):
        self.reg_ops = []
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        out = Malloc_Feature(1, 1, w.out_ch, 0, 0, 16)
        FPGA_RunHBM_FC(f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class MVM(Op):

    op_name = "mvm"

    def infer(self, f, w):
        if f[1] != "feature" or w[1] != "weight":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight):
        self.reg_ops = []
        '''
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        '''
        out = Malloc_Feature(1, f.width, w.out_ch, 0, 0, 16)
        cfg = Get_MVM_Cfg(1, f.width, f.channel, w.out_ch, 16, 4, 16)
        FPGA_HBM_MVM(cfg, 0, f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class LayerNorm(Op):
    op_name = "layer_norm"

    def infer(self, f_d, f_w):
        if f_d[1] != "feature" or f_w[1] != "feature" or f_w[0][2] != f_d[0][2]*2:
            raise RuntimeError("Checked Error!")
        output = f_d[0]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f_d: Mapped_Feature, f_w: Mapped_Feature):
        self.reg_ops = []
        if f_d.channel*2 != f_w.channel:
            raise RuntimeError("Channel not match!")
        out = Malloc_Feature(f_d.height, f_d.width, f_d.channel, 0, 0, 16)
        FPGA_Run_LN(f_d, f_w, out, 0, 0, self)
        return out


class Softmax(Op):
    op_name = "softmax"

    def infer(self, f):
        if f[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = f[0]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature):
        self.reg_ops = []
        out = Malloc_Feature(f.height, f.width, f.channel, 0, 0, 16)
        FPGA_Run_Softmax(f, out, 0, self)
        return out


class Transpose(Op):
    op_name = "transpose"

    def infer(self, f):
        if f[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [1, f[0][0], f[0][2], f[0][1]]
        self.shape = output
        self.dtype = "weight"
        return [output, "weight"]

    def build(self, f: Mapped_Feature):
        self.reg_ops = []
        out = Malloc_Weight(1, f.height, f.channel, f.width, 0)
        FPGA_Run_Transpose(f, out, 0, self)
        return out
