import torch
from torch.nn import functional as F

import numpy as np

import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

from fcompile.fir import FModule
from fcompile.simulate import modelsim
from fcompile.transform import RelayFIR
from fcompile.quant import Quantize, Dequantize
from fcompile import config

config.SIM_HINT_STDOUT = True

def result_diff_check(func):
    def _register_fn(fn):
        def _call():
            r_torch, r_fcompile = fn()
            func(fn.__name__, r_torch, r_fcompile)
        return _call
    return _register_fn

def process(data, scale):
    data = data.numpy() * (2**scale)
    return data.astype("int8")

def diff(name, tresult, fresult):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    num_same = np.sum(tresult == fresult)
    num_diff = tresult.size - num_same
    rat_same = num_same / tresult.size
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME : {name}")
    print(f"* T-F DIFF : {max_diff}")
    print(f"* F-T DIFF : {min_diff}")
    print(f"* NUM DIFF : {num_diff}")
    print(f"* NUM SAME : {num_same}")
    print("* SAME%    : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************")

@result_diff_check(diff)
def check_conv2d():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(1, 3, 28, 28)) / 2
    weight = torch.randn(size=(32, 3, 3, 3)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    wquanted = quantize(weight)
    wscale = int(quantize.scale[0])

    tp_out = F.conv2d(dquanted, wquanted, None, stride=1, padding=0)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).transpose((0, 2, 3, 1))

    widths, scales = [dat_bw_l0, dat_bw_l0, dat_bw_l1], [dscale, wscale, oscale]
    dvar = relay.var("data", shape=(1, 28, 28, 3), dtype="int8")
    wvar = relay.var("weight", shape=(3, 3, 3, 32), dtype="int8")
    fout = relay.accel.vit.conv2d(dvar, wvar, strides=1, padding=0, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    inputs = {
        "data" : process(dquanted, dscale).transpose((0, 2, 3, 1)),
        "weight" : process(wquanted, wscale).transpose((2, 3, 1, 0)),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


@result_diff_check(diff)
def check_mm():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(64, 64)) / 2
    weight = torch.randn(size=(64, 32)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    wquanted = quantize(weight)
    wscale = int(quantize.scale[0])

    tp_out = torch.mm(dquanted, wquanted)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).reshape(1, 1, 64, 32)

    widths, scales = [dat_bw_l0, dat_bw_l0, dat_bw_l1], [dscale, wscale, oscale]
    dvar = relay.var("data", shape=(1, 1, 64, 64), dtype="int8")
    wvar = relay.var("weight", shape=(1, 1, 64, 32), dtype="int8")
    fout = relay.accel.vit.mm(dvar, wvar, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 64, 64),
        "weight" : process(wquanted, wscale).reshape(1, 1, 64, 32),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


check_conv2d()
check_mm()