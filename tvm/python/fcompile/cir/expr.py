from fcompile import ne
import numpy as np
from .driver.basic import Mapped_Weight, Mapped_Feature, Tout
from .driver import get_driver


# AST data structure
class Op:

    op_name = "op"

    def __init__(self, *args):
        self.args = args
        self.index = None


class TupleItem(Op):

    def __init__(self, expr, index):
        self.expr = expr
        self.index = index

    def infer(self, fs):
        self.shape, self.dtype = fs[self.index]
        return fs[self.index]


class Input(Op):

    op_name = "input"

    def __init__(self, name, shape=None, dtype=None):
        self.name = name
        self.index = None
        if shape is not None and dtype is not None:
            self.set_shape(shape, dtype)

    def build(self, shape, dtype):
        self.input_addr = {}
        if dtype == "feature":
            return get_driver("hbm", "Malloc_Feature")(shape[0], shape[1], shape[2], 0, 0, 16)
        elif dtype == "weight":
            return get_driver("hbm", "Malloc_Weight")(shape[0], shape[1], shape[2], shape[3], 0, 4)

    def set_shape(self, shape, dtype):
        self.shape, self.dtype = shape, dtype


class FC(Op):

    op_name = "fc"

    def __init__(self, *args, mode=0b111111):
        self.args = args
        self.mode = mode

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
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        get_driver("hbm", self.op_name)(f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class FC_BN(FC):

    op_name = "fc_bn"

    def infer(self, f, w, b):
        if f[1] != "feature" or w[1] != "weight" or b[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight, b: Mapped_Feature):
        self.reg_ops = []
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        get_driver("hbm", self.op_name)(f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class FC_BN_Res(FC):

    op_name = "fc_bn_res"

    def infer(self, f, w, b, r):
        if f[1] != "feature" or w[1] != "weight" or b[1] != "feature" or r[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight, b: Mapped_Feature, r: Mapped_Feature):
        self.reg_ops = []
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        get_driver("hbm", self.op_name)(f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class MVM(Op):

    op_name = "mvm"

    def __init__(self, *args, mode=0b011111, skip=1):
        self.args = args
        self.mode = mode
        self.skip = skip

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
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        cfg = get_driver("hbm", "mvm_cfg")(f.height, f.width, f.channel, w.out_ch, 16, 4, 16)
        get_driver("hbm", self.op_name)(cfg, 0, f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class MVM_BN(MVM):

    op_name = "mvm_bn"

    def infer(self, f, w, b):
        if f[1] != "feature" or w[1] != "weight" or b[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight, b: Mapped_Feature):
        self.reg_ops = []
        '''
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        '''
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        cfg = get_driver("hbm", "mvm_cfg")(f.height, f.width, f.channel, w.out_ch, 16, 4, 16)
        get_driver("hbm", self.op_name)(cfg, 0, f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class MVM_BN_Res(MVM):

    op_name = "mvm_bn_res"

    def infer(self, f, w, b, r):
        if f[1] != "feature" or w[1] != "weight" or b[1] != "feature" or r[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f[0][0], f[0][1], w[0][3]]
        self.shape = output
        self.dtype = "feature"
        return [output, "feature"]

    def build(self, f: Mapped_Feature, w: Mapped_Weight, b: Mapped_Feature, r: Mapped_Feature):
        self.reg_ops = []
        '''
        if f.channel != w.in_ch:
            raise RuntimeError("Channel not match!")
        '''
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, w.out_ch, 0, 0, 16)
        cfg = get_driver("hbm", "mvm_cfg")(f.height, f.width, f.channel, w.out_ch, 16, 4, 16)
        get_driver("hbm", self.op_name)(cfg, 0, f, w, out, 0, 0, 0, 0, self, 28, 0)
        return out


class LayerNorm(Op):
    op_name = "layer_norm"

    def __init__(self, *args, mode=0b011111):
        self.args = args
        self.mode = mode

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
        out = get_driver("hbm", "Malloc_Feature")(f_d.height, f_d.width, f_d.channel, 0, 0, 16)
        get_driver("hbm", self.op_name)(f_d, f_w, out, 0, 0, self)
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
        out = get_driver("hbm", "Malloc_Feature")(f.height, f.width, f.channel, 0, 0, 16)
        get_driver("hbm", self.op_name)(f, out, 0, self)
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
        out = get_driver("hbm", "Malloc_Weight")(1, f.height, f.channel, f.width, 0)
        get_driver("hbm", self.op_name)(f, out, 0, self)
        return out


class Feature2Weight(Op):
    op_name = "feature2weight"

    def infer(self, f):
        if f[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [1, f[0][0], f[0][1], f[0][2]]
        self.shape = output
        self.dtype = "weight"
        return [output, "weight"]

    def build(self, f: Mapped_Feature):
        self.reg_ops = []
        out = get_driver("hbm", "Malloc_Weight")(1, f.height, f.width, f.channel, 0)
        get_driver("hbm", self.op_name)(f, out, 0, self)
        return out


class ElementWise(Op):
    op_name = "elementwise"

    def __init__(self, *args, mode=0b111111):
        self.args = args
        self.mode = mode

    def infer(self, f_a, f_b):
        if f_a[1] != "feature" or f_b[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = f_a[0]
        self.shape = output
        self.dtype = "feature"
        return [output, self.dtype]

    def build(self, f_a: Mapped_Feature, f_b: Mapped_Feature):
        self.reg_ops = []
        out = get_driver("hbm", "Malloc_Feature")(f_a.height, f_a.width, f_a.channel, 0, 0, 16)
        # get_driver("hbm", "elementwise")(f_a, out, 0, self)
        return out


class PosEmb(Op):
    op_name = "pos_emb"

    def infer(self, f_a):
        if f_a[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = f_a[0]
        self.shape = output
        self.dtype = "feature"
        return [self.shape, self.dtype]


class Reshape(Op):
    op_name = "reshape"

    def __init__(self, *args, new_shape):
        super(Reshape, self).__init__(*args)
        self.new_shape = new_shape

    def infer(self, f):
        def tout_process(data, tout_shape):
            t_data = np.zeros(tout_shape)
            shape = data.shape
            for c in range(tout_shape[0]):
                for h in range(tout_shape[1]):
                    for w in range(tout_shape[2]):
                        for t in range(tout_shape[3]):
                            if c*Tout + t < shape[2]:
                                t_data[c, h, w, t] = data[h, w, c*Tout + t]
            return t_data
        if f[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = self.new_shape
        '''
        t_shape = [(f[0][2] + Tout - 1) // Tout, f[0][0], f[0][1], Tout]
        t_new_shape = [(output[2] + Tout - 1) // Tout, output[0], output[1], Tout]
        data = np.random.randn(*f[0])
        org_data = tout_process(data, t_shape).flatten()
        new_data = tout_process(data.reshape(output), t_new_shape).flatten()
        if np.sum(org_data != new_data):
            raise RuntimeError("Reshape Error! Data after flatten is not same!")
        '''
        self.shape = output
        self.dtype = "feature"
        return [output, self.dtype]

    def build(self, f: Mapped_Feature):
        self.reg_ops = []
        return f


class Split(Op):
    op_name = "split"

    def __init__(self, expr, axis):
        super(Split, self).__init__(expr)
        self.axis = axis

    def infer(self, f):
        if f[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f[0][i]//2 if i == self.axis else f[0][i] for i in range(len(f[0]))]
        self.shape = [output, output]
        self.dtype = "tuple"
        return [[output, "feature"], [output, "feature"]]


class Merge(Op):
    op_name = "merge"

    def __init__(self, *args, axis=0):
        super(Merge, self).__init__(*args)
        self.axis = axis

    def infer(self, f_a, f_b):
        if f_a[1] != "feature" or f_b[1] != "feature":
            raise RuntimeError("Checked Error!")
        output = [f_a[0][i] + f_b[0][i] if i == self.axis else f_a[0][i] for i in range(len(f_a[0]))]
        self.shape = output
        self.dtype = "feature"
        return [self.shape, self.dtype]

    def build(self, f_a: Mapped_Feature, f_b: Mapped_Feature):
        self.reg_ops = []
        tout_shape_a = [((f_a.channel + Tout-1) // Tout), f_a.height, f_a.width, Tout]
        tout_shape_b = [((f_b.channel + Tout-1) // Tout), f_b.height, f_b.width, Tout]
        tout_shape_f = [((self.shape[2] + Tout-1) // Tout), self.shape[0], self.shape[1], Tout]
        out = get_driver("hbm", "Malloc_Feature")(self.shape[0], self.shape[1], self.shape[2], 0, 0, 16)
        if self.axis == 2:
            self.reg_ops.append([10, out.payload, f_a.payload, f_a.height*f_a.width*Tout*2])
            self.reg_ops.append([10, out.payload + f_a.height*f_a.width*Tout*2, f_b.payload, f_b.height*f_b.width*Tout*2])
        else:
            axis = self.axis + 1
            var = ne.Var("loop_1")
            size_a = tout_shape_a[2] * tout_shape_a[3] * 2
            size_b = tout_shape_b[2] * tout_shape_b[3] * 2
            offset_f = tout_shape_f[2] * tout_shape_f[3]
            tout_numb = tout_shape_f[0] * tout_shape_f[1]
            self.reg_ops.append([2, var, 0, tout_numb])
            self.reg_ops.append([10, out.payload + var * offset_f, f_a.payload + var * size_a, size_a])
            self.reg_ops.append([10, out.payload + size_a + var * offset_f, f_b.payload + var * size_b, size_b])
            self.reg_ops.append([-1])
        return out


class Cache:

    op_name = "@create_cache"

    def __init__(self, name, size, dtype="float16"):
        self.name = name
        self.size = size * 2


class LoadCache:

    op_name = "@load_cache"

    def __init__(self, cache, shape, dtype):
        self.cache = cache
        self.shape = shape
        self.dtype = dtype

    def infer(self):
        return [self.shape, self.dtype]

    def build(self):
        out = get_driver("hbm", "Malloc_Feature")(self.shape[0], self.shape[1], self.shape[2], 0, 0, 16)
        return out


class StoreCache:

    op_name = "@store_cache"

    def __init__(self, expr, cache):
        self.expr = expr
        self.cache = cache

    def infer(self, f):
        self.shape = f[0]
        self.dtype = f[1]
        return [self.shape, self.dtype]

    def build(self, f):
        return f
