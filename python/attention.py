import torch
from torch import nn

import numpy as np
from math import sqrt

import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

from fcompile.fir import FModule
from fcompile.transform import RelayFIR
from fcompile.quant import Quantize, Dequantize, QuantifiedLinear
from fcompile.simulate import modelsim, result_diff_check, diff, diff_scale, process
from fcompile import config

config.SIM_HIDE_STDOUT = False

class SelfAttention(nn.Module):
    # input : batch_size * seq_len * input_dim
    # q : batch_size * input_dim * dim_k
    # k : batch_size * input_dim * dim_k
    # v : batch_size * input_dim * dim_v
    def __init__(self, input_dim, dim_k, dim_v, bit_width):
        super(SelfAttention, self).__init__()
        self.quantize = Quantize(bit_width=bit_width)
        self.q = QuantifiedLinear(input_dim, dim_k, bias=False, bit_width=bit_width)
        self.k = QuantifiedLinear(input_dim, dim_k, bias=False, bit_width=bit_width)
        self.v = QuantifiedLinear(input_dim, dim_v, bias=False, bit_width=bit_width)
        self._norm_fact = 1 / sqrt(dim_k)
        
    
    def forward(self, x):
        Q = self.q(x) # Q: batch_size * seq_len * dim_k
        K = self.k(x) # K: batch_size * seq_len * dim_k
        V = self.v(x) # V: batch_size * seq_len * dim_v
         
        norm_weight = torch.zeros((1, 1, x.shape[1], x.shape[1]))
        for i in range(x.shape[1]):
            norm_weight[0, 0, i, i] = self._norm_fact
        self.nquanted = self.quantize(norm_weight)
        self.nscale = int(self.quantize.scale[0])

        atten = torch.bmm(Q, K.permute(0,2,1))

        aquanted = self.quantize(atten)
        self.a0scale = int(self.quantize.scale[0])

        atten =  aquanted * self.nquanted[0, 0, 0, 0]

        aquanted = self.quantize(atten)
        self.a1scale = int(self.quantize.scale[0])

        atten = nn.Softmax(dim=-1)(aquanted) # Q * K.T() # batch_size * seq_len * seq_len

        aquanted = self.quantize(atten)
        self.a2scale = int(self.quantize.scale[0])
        
        output = torch.bmm(aquanted, V) # Q * K.T() * V # batch_size * seq_len * dim_v

        oquanted = self.quantize(output)
        self.oscale = int(self.quantize.scale[0])
        
        return oquanted


@result_diff_check(diff_scale)
def check_attention():
    width = 8
    data = torch.randn(size=(1, 64, 64)) / 2

    atten = SelfAttention(64, 64, 64, width)
    quantize = Quantize(bit_width=width)

    oquanted = atten(data)

    dquanted = quantize(data)
    dscale = int(quantize.scale[0])
    qwidth, qdscale = atten.q.bit_width, int(atten.q.scale[0])
    qwscale, qoscale = int(atten.q.scale_weight[0]), int(atten.q.scale_out[0])
    kwidth, kdscale = atten.k.bit_width, int(atten.k.scale[0])
    kwscale, koscale = int(atten.k.scale_weight[0]), int(atten.k.scale_out[0])
    vwidth, vdscale = atten.v.bit_width, int(atten.v.scale[0])
    vwscale, voscale = int(atten.v.scale_weight[0]), int(atten.v.scale_out[0])

    qweight_ = quantize(atten.q.weight)
    kweight_ = quantize(atten.k.weight)
    vweight_ = quantize(atten.v.weight)

    nweight_, nscale = atten.nquanted, atten.nscale
    a0scale, a1scale, a2scale, oscale = atten.a0scale, atten.a1scale, atten.a2scale, atten.oscale

    t_out = process(oquanted.detach(), oscale).reshape(1, 1, 64, 64)

    data = relay.var("data", shape=(1, 1, 64, 64), dtype="int8")
    qweight = relay.var("qweight", shape=(1, 1, 64, 64), dtype="int8")
    kweight = relay.var("kweight", shape=(1, 1, 64, 64), dtype="int8")
    vweight = relay.var("vweight", shape=(1, 1, 64, 64), dtype="int8")
    nweight = relay.var("nweight", shape=(1, 1, 64, 64), dtype="int8")
    q = relay.accel.vit.conv2d(data, qweight, strides=1, padding=0, widths=[width, width, width], scales=[qdscale, qwscale, qoscale])
    k = relay.accel.vit.conv2d(data, kweight, strides=1, padding=0, widths=[width, width, width], scales=[kdscale, kwscale, koscale])
    v = relay.accel.vit.conv2d(data, vweight, strides=1, padding=0, widths=[width, width, width], scales=[vdscale, vwscale, voscale])
    kt = relay.accel.vit.transpose(k, widths=[width, width], scales=[koscale, koscale])
    atten = relay.accel.vit.mm(q, kt, widths=[width, width, width], scales=[qoscale, koscale, a0scale])
    atten = relay.accel.vit.conv2d(atten, nweight, strides=1, padding=0, widths=[width, width, width], scales=[a0scale, nscale, a1scale])
    atten = relay.accel.vit.softmax(atten, widths=[width, width], scales=[a1scale, a2scale])
    out = relay.accel.vit.mm(atten, v, widths=[width, width, width], scales=[a2scale, voscale, oscale])
    func = relay.Function([data, qweight, kweight, vweight, nweight], out)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "data" : process(dquanted, dscale).reshape(1, 1, 64, 64),
        "qweight" : process(qweight_.detach(), qwscale).transpose((1, 0)).reshape(1, 1, 64, 64),
        "kweight" : process(kweight_.detach(), kwscale).transpose((1, 0)).reshape(1, 1, 64, 64),
        "vweight" : process(vweight_.detach(), vwscale).transpose((1, 0)).reshape(1, 1, 64, 64),
        "nweight" : process(nweight_.detach(), nscale),
    }
    f_out = modelsim(f_mod, inputs)
    return t_out, f_out, oscale, 5 # torch_result, fcompile_verilog_result, out_scale, same_threshold


check_attention()