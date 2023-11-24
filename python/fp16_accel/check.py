import torch
from torch.nn import functional as F

import numpy as np

import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

from fcompile.fir import FModule
from fcompile.transform import RelayFIR
from fcompile.simulate import rtl_simulate, result_diff_check, diff_fp16
from fcompile import config

config.SIM_HIDE_STDOUT = True
config.SIM_ROOT = "/home/previous/accel/hbm1110/sim"


class RMSNorm(torch.nn.Module):
    def __init__(self, normalized_shape, eps=1e-5, dtype=torch.float16, **kwargs):
        super().__init__()
        # self.weight = torch.nn.Parameter(torch.empty(normalized_shape, dtype=dtype))
        self.weight = torch.randn(*normalized_shape).to(dtype) + 1.0
        self.eps = eps

    def forward(self, hidden_states: torch.Tensor):
        input_dtype = hidden_states.dtype
        variance = hidden_states.to(torch.float32).pow(2).mean(-1, keepdim=True)
        hidden_states = hidden_states * torch.rsqrt(variance + self.eps)

        return (self.weight * hidden_states).to(input_dtype)

    def _initialize_weights(self):
        for m in self.modules():
            torch.nn.init.normal_(m.weight, 0, 0.01)


@result_diff_check(diff_fp16)
def check_rms_norm():
    data = torch.randn(size=(128, 32)).to(torch.float16)
    rms = RMSNorm(normalized_shape=(32,))
    result = rms(data)
    t_out = result.detach().numpy().reshape(1, 1, 128, 32)
    weight = rms.weight.detach().numpy()
    bias = np.zeros((32,)).astype("float16")
    weight = np.concatenate((weight, bias))

    widths, scales = [16, 16, 16], [0, 0, 0]
    dvar = relay.var("data", shape=(1, 1, 128, 32), dtype="float16")
    wvar = relay.var("weight", shape=(1, 1, 1, 64), dtype="float16")
    fout = relay.accel.hbm.rms_norm(dvar, wvar, widths=widths, scales=scales)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=128, tout=32)
    print(f_mod)
    inputs = {
        "data": data.numpy().reshape(1, 1, 128, 32),
        "weight": weight.reshape(1, 1, 1, 64),
    }
    f_out = rtl_simulate(f_mod, inputs)
    return t_out, f_out, 0.4  # torch_result, fcompile_verilog_result, threshold


@result_diff_check(diff_fp16)
def check_softmax():
    data = torch.randn(size=(128, 32)).to(torch.float32)
    result = F.softmax(data, dim=1)
    t_out = result.detach().numpy().reshape(1, 1, 128, 32).astype("float16")

    widths, scales = [16, 16], [0, 0]
    dvar = relay.var("data", shape=(1, 1, 128, 32), dtype="float16")
    fout = relay.accel.hbm.softmax(dvar, widths=widths, scales=scales)
    func = relay.Function([dvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=128, tout=32)
    print(f_mod)
    inputs = {
        "data": data.numpy().reshape(1, 1, 128, 32).astype("float16"),
    }
    f_out = rtl_simulate(f_mod, inputs)
    return t_out, f_out, 0.1  # torch_result, fcompile_verilog_result, threshold


@result_diff_check(diff_fp16)
def check_exp():
    from approx_pwlf_act import exp_wt, exp_bias, exp_x_region
    data = np.random.randn(128, 32).astype("float16")
    result = np.exp(data)
    t_out = result.reshape(1, 1, 128, 32).astype("float16")

    widths, scales = [16, 16], [0, 0]
    dvar = relay.var("data", shape=(1, 1, 128, 32), dtype="float16")
    wvar = relay.var("weight", shape=(48,), dtype="float16")
    fout = relay.accel.hbm.activation(dvar, wvar, widths=widths, scales=scales)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=128, tout=32)
    print(f_mod)
    inputs = {
        "data": data.reshape(1, 1, 128, 32).astype("float16"),
        "weight": np.array(exp_wt + exp_bias + exp_x_region).astype("float16")
    }
    f_out = rtl_simulate(f_mod, inputs)
    return t_out, f_out, 0.1  # torch_result, fcompile_verilog_result, threshold


@result_diff_check(diff_fp16)
def check_tanh():
    from approx_pwlf_act import tanh_wt, tanh_bias, tanh_x_region
    data = torch.randn(size=(128, 32)).to(torch.float32)
    result = torch.nn.Tanh()(data)
    t_out = result.detach().numpy().reshape(1, 1, 128, 32).astype("float16")

    widths, scales = [16, 16], [0, 0]
    dvar = relay.var("data", shape=(1, 1, 128, 32), dtype="float16")
    wvar = relay.var("weight", shape=(48,), dtype="float16")
    fout = relay.accel.hbm.activation(dvar, wvar, widths=widths, scales=scales)
    func = relay.Function([dvar, wvar], fout)
    mod = IRModule.from_expr(func)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=128, tout=32)
    print(f_mod)
    inputs = {
        "data": data.numpy().reshape(1, 1, 128, 32).astype("float16"),
        "weight": np.array(tanh_wt + tanh_bias + tanh_x_region).astype("float16")
    }
    f_out = rtl_simulate(f_mod, inputs)
    return t_out, f_out, 0.1  # torch_result, fcompile_verilog_result, threshold


check_rms_norm()
check_softmax()
check_exp()
check_tanh()
