import torch
from torch.nn import functional as F

import numpy as np

import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

from fcompile.fir import FModule
from fcompile.transform import RelayFIR
from fcompile.quant import Quantize, Dequantize
from fcompile.simulate import modelsim, result_diff_check, diff, diff_scale, process
from fcompile import config

config.SIM_HIDE_STDOUT = False

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
    return t_out, f_out, oscale, 5 # torch_result, fcompile_verilog_result, out_scale, same_threshold


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
    wvar = relay.var("k_bias", shape=(1, 1, 1, 128), dtype="int8")
    fout = relay.accel.vit.layer_norm(dvar, wvar, widths=widths, scales=scales)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 128, 64),
        "k_bias" : np.concatenate((process(wquanted, wscale), process(bquanted, bscale)), axis=0).reshape(1, 1, 1, 128),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out, oscale, 1 # torch_result, fcompile_verilog_result


@result_diff_check(diff_scale)
def check_conv2d_add():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(1, 64, 7, 7)) / 2
    weight = torch.randn(size=(32, 64, 3, 3)) / 2
    res_add = torch.randn(size=(1, 32, 7, 7)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    wquanted = quantize(weight)
    wscale = int(quantize.scale[0])
    rquanted = quantize(res_add)
    rscale = int(quantize.scale[0])

    tp_out = F.conv2d(dquanted, wquanted, None, stride=1, padding=1)
    tp_out = torch.add(tp_out, rquanted)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale).transpose((0, 2, 3, 1))

    widths, scales = [dat_bw_l0, dat_bw_l0, dat_bw_l1, dat_bw_l1], [dscale, wscale, rscale, oscale]
    dvar = relay.var("data", shape=(1, 7, 7, 64), dtype="int8")
    wvar = relay.var("weight", shape=(3, 3, 64, 32), dtype="int8")
    rvar = relay.var("res", shape=(1, 7, 7, 32), dtype="int8")
    fout = relay.accel.vit.conv2d_add(dvar, wvar, rvar, strides=1, padding=1, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar, wvar, rvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).transpose((0, 2, 3, 1)),
        "weight" : process(wquanted, wscale).transpose((2, 3, 1, 0)),
        "res" : process(rquanted, rscale).transpose((0, 2, 3, 1)),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out, oscale, 1 # torch_result, fcompile_verilog_result


@result_diff_check(diff)
def check_matmul():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(1, 28, 28, 32)) / 2
    weight = torch.zeros((32, 32, 1, 1))
    n_factor = 0.6
    for i in range(32):
        weight[i, i, 0, 0] = n_factor

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    wquanted = quantize(weight)
    wscale = int(quantize.scale[0])

    tp_out = dquanted * wquanted[0, 0, 0, 0]
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale)

    widths, scales = [dat_bw_l0, dat_bw_l0, dat_bw_l1], [dscale, wscale, oscale]
    dvar = relay.var("data", shape=(1, 28, 28, 32), dtype="int8")
    wvar = relay.var("weight", shape=(1, 1, 32, 32), dtype="int8")
    fout = relay.accel.vit.conv2d(dvar, wvar, strides=1, padding=0, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale),
        "weight" : process(wquanted, wscale).transpose((2, 3, 1, 0)),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


@result_diff_check(diff)
def check_gelu():
    dat_bw_l0, dat_bw_l1 = 8, 8
    data = torch.randn(size=(1, 1, 64, 64)) / 2

    quantize = Quantize(bit_width=dat_bw_l0)
    dequantize = Dequantize(bit_width=dat_bw_l1)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])

    tp_out = torch.nn.GELU()(dquanted)
    
    oquanted = dequantize(tp_out)
    oscale = int(dequantize.scale[0])
    t_out = process(oquanted, oscale)

    widths, scales = [dat_bw_l0, dat_bw_l1], [dscale, oscale]
    dvar = relay.var("data", shape=(1, 1, 64, 64), dtype="int8")
    fout = relay.accel.vit.activation(dvar, widths=widths, scales=scales, activate=0)
    func = relay.Function([dvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out # torch_result, fcompile_verilog_result


#check_conv2d()
#check_mm()
#check_softmax()
#check_transpose()
#check_layernorm()
#check_conv2d_add()
#check_matmul()
check_gelu()
