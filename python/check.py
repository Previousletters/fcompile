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
            n = fn()
            func(fn.__name__, *n)
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
    print("**************** DIFFERENCE ****************\n")

def diff_scale(name, tresult, fresult, scale, threshold):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    abs_diff = np.abs(tresult - fresult)
    num_same = np.sum(abs_diff<=threshold)
    num_diff = tresult.size - num_same
    rat_same = num_same / tresult.size
    ft_diff = fresult-tresult
    means_diff = np.mean(ft_diff)
    std_diff = np.std(ft_diff)
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME  : {name}")
    print(f"* OUT SCALE : {scale}")
    print(f"* T-F DIFFS : {max_diff}, {max_diff/(2**scale)}")
    print(f"* F-T DIFFS : {min_diff}, {min_diff/(2**scale)}")
    print(f"* F-T MEANS : {means_diff}, {means_diff/(2**scale)}")
    print(f"* F-T STD   : {std_diff}, {std_diff/(2**scale)}")
    print(f"* NUM DIFF  : {num_diff}")
    print(f"* NUM SAME  : {num_same}")
    print(f"* THRESHOLD : {threshold}")
    print("* SAME%     : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************\n")


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
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
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
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 64, 64),
        "weight" : process(wquanted, wscale).reshape(1, 1, 64, 32),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


@result_diff_check(diff_scale)
def check_softmax():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(64, 64)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])

    tp_out = F.softmax(dquanted, dim=1)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).reshape(1, 1, 64, 64)

    widths, scales = [dat_bw_l0, dat_bw_l1], [dscale, oscale]
    dvar = relay.var("data", shape=(1, 1, 64, 64), dtype="int8")
    fout = relay.accel.vit.softmax(dvar, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 64, 64),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out, oscale, 6 # torch_result, fcompile_verilog_result, out_scale, same_threshold


@result_diff_check(diff)
def check_transpose():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(128, 64)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])

    tp_out = torch.transpose(dquanted, 0, 1).contiguous()
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).reshape(1, 1, 64, 128)

    widths, scales = [dat_bw_l0, dat_bw_l1], [dscale, oscale]
    dvar = relay.var("data", shape=(1, 1, 128, 64), dtype="int8")
    fout = relay.accel.vit.transpose(dvar, widths=widths, scales=scales)
    func = relay.Function([dvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 128, 64),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


@result_diff_check(diff_scale)
def check_layernorm():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(128, 64)) / 2
    k_factor = torch.randn(size=(64,)) / 2
    bias = torch.randn(size=(64,)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    wquanted = quantize(k_factor)
    wscale = int(quantize.scale[0])
    bquanted = quantize(bias)
    bscale = int(quantize.scale[0])

    tp_out = F.layer_norm(dquanted, (64,), wquanted, bquanted)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).reshape(1, 1, 128, 64)

    widths, scales = [dat_bw_l0, dat_bw_l0, dat_bw_l0, dat_bw_l1], [dscale, wscale, bscale, oscale]
    dvar = relay.var("data", shape=(1, 1, 128, 64), dtype="int8")
    wvar = relay.var("k_factor", shape=(64,), dtype="int8")
    bvar = relay.var("bias", shape=(64,), dtype="int8")
    fout = relay.accel.vit.layer_norm(dvar, wvar, bvar, widths=widths, scales=scales)
    func = relay.Function([dvar, wvar, bvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 128, 64),
        "k_factor" : process(wquanted, wscale),
        "bias" : process(bquanted, bscale),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out, oscale, 1 # torch_result, fcompile_verilog_result


check_conv2d()
check_mm()
check_softmax()
check_transpose()
check_layernorm()