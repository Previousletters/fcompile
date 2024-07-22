from .base import Op, Call, Var, Constant, DataEnum, DataType
from ..device import HBM


def var_ddr(name, shape, dtype=DataEnum.fp16, device=HBM):
    dtype = DataType(dtype, DataEnum.ddr)
    expr = Var(name, shape, dtype, device)
    expr.prefix = "runtime"
    return expr


def const_ddr(name, data, shape=None, dtype=DataEnum.fp16, device=HBM):
    dtype = DataType(dtype, DataEnum.ddr)
    expr = Constant(name, data, shape, dtype, device)
    expr.prefix = "weight"
    return expr


def const_hbm(name, data, shape=None, dtype=DataEnum.int4, device=HBM):
    dtype = DataType(dtype, DataEnum.hbm)
    expr = Constant(name, data, shape, dtype, device)
    expr.prefix = "hbm"
    return expr


def mvm(*args, skip=1, log2_step=28, **kwattrs):
    if skip == 1:
        attrs = {
            "skip": skip,
            "log2_step": log2_step,
            **kwattrs
        }
        return Call(Op.Get("accel.hbm.mvm"), args[:2], attrs)
    elif skip == 2:
        attrs = {
            "skip": skip,
            "log2_step": log2_step,
            **kwattrs
        }
        return Call(Op.Get("accel.hbm.mvm"), args[:3], attrs)
    else:
        print("Error! make mvm")
        exit(-1)


def mvm_bn(data, weight, wt_and_bias, padding=0, skip=1, log2_step=28, **kwattrs):
    attrs = {
        "skip": skip,
        "padding": padding,
        "log2_step": log2_step,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.mvm_bn"), [data, weight, wt_and_bias], attrs)


def mvm_bn_res(*args, skip=1, res_mul=0, arg_max=0, relu=0, log2_step=28, **kwattrs):
    if skip == 1:
        attrs = {
            "skip": skip,
            "res_mode": (res_mul << 1) | relu,
            "mul_mode": res_mul,
            "log2_step": log2_step,
            "arg_max": arg_max,
            **kwattrs
        }
        return Call(Op.Get("accel.hbm.mvm_bn_res"), args[:4], attrs)
    elif skip == 2:
        attrs = {
            "skip": skip,
            "res_mode": (res_mul << 1) | relu,
            "mul_mode": res_mul,
            "arg_max": arg_max,
            "log2_step": log2_step,
            **kwattrs
        }
        return Call(Op.Get("accel.hbm.mvm_bn_res"), args[:5], attrs)
    else:
        print("Error! make mvm_bn_res")
        exit(-1)


def mvm_afterTRP(data, weight, padding=0, kvcache=1, **kwattrs):
    attrs = {
        "kvcache": kvcache,
        "padding": padding,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.mvm_afterTRP"), [data, weight], attrs)


def mvm_afterF2W(data, weight, padding=0, kvcache=1, **kwattrs):
    attrs = {
        "kvcache": kvcache,
        "padding": padding,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.mvm_afterF2W"), [data, weight], attrs)


def add(data0, data1, **kwattrs):
    attrs = {**kwattrs}
    return Call(Op.Get("accel.hbm.add"), [data0, data1], attrs)


def mul(data0, data1, **kwattrs):
    attrs = {**kwattrs}
    return Call(Op.Get("accel.hbm.mul"), [data0, data1], attrs)


def layer_norm(data, weight, rms=0, **kwattrs):
    attrs = {
        "rms": rms,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.layer_norm"), [data, weight], attrs)


def rms_norm(data, weight, rms=1, **kwattrs):
    attrs = {
        "rms": rms,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.layer_norm"), [data, weight], attrs)


def softmax(data, padding=0, kvcache=1, **kwattrs):
    attrs = {
        "kvcache": kvcache,
        "padding": padding,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.softmax"), [data], attrs)


def pos_emb(data, weight, padding=0, kvcache=1, out_and_in_mode=0, **kwattrs):
    attrs = {
        "kvcache": kvcache,
        "padding": padding,
        "out_and_in_mode": out_and_in_mode,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.pos_emb"), [data, weight], attrs)


def transpose(data, out_and_in_mode=0, log2_step=28):
    attrs = {
        "out_and_in_mode": out_and_in_mode,
        "log2_step": log2_step
    }
    return Call(Op.Get("accel.hbm.transpose"), [data], attrs)


def feature2weight(data, out_and_in_mode=0, log2_step=28):
    attrs = {
        "out_and_in_mode": out_and_in_mode,
        "log2_step": log2_step
    }
    return Call(Op.Get("accel.hbm.feature2weight"), [data], attrs)


def activate(data, weight, out_and_in_mode=0, **kwattrs):
    attrs = {
        "out_and_in_mode": out_and_in_mode,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.activate"), [data, weight], attrs)


def silu(data, out_and_in_mode=0):
    import numpy as np
    silu_weight = const_ddr("global::silu_weight", np.zeros([32*3], dtype="uint8"), [32*3], DataEnum.int8)
    return activate(data, silu_weight, out_and_in_mode=out_and_in_mode)


def cache(expr):
    expr.prefix = "cache"
    expr.attrs["padding"] = 1
    return expr


def conv2d(data, weight, strides=[1, 1], padding=[0, 0], **kwattrs):
    attrs = {
        "strides": strides,
        "padding": padding,
        **kwattrs
    }
    return Call(Op.Get("accel.hbm.conv2d"), [data, weight], attrs)

