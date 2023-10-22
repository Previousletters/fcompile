import os
import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

def main_test_accel_relay():
    x = relay.var("x", shape=(1, 28, 28, 3), dtype="int8")
    w1 = relay.var("w1", shape=(3, 3, 3, 32), dtype="int8")
    w2 = relay.var("w2", shape=(3, 3, 32, 64), dtype="int8")
    out = relay.accel.vit.conv2d(x, w1, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    out = relay.accel.vit.conv2d(out, w2, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    func = relay.Function([x, w1, w2], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)

import numpy as np

from fcompile.fir import FModule
from fcompile.transform import RelayFIR, FPGAParameters, DataMap, FPGAJit
from fcompile.codegen import CCodeGen

def main_accel_ccodegen():
    x = relay.var("x", shape=(1, 28, 28, 3), dtype="int8")
    w1 = relay.var("w1", shape=(3, 3, 3, 32), dtype="int8")
    w2 = relay.var("w2", shape=(3, 3, 32, 64), dtype="int8")
    out = relay.accel.vit.conv2d(x, w1, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    out = relay.accel.vit.conv2d(out, w2, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    func = relay.Function([x, w1, w2], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    params = {
        "w1" : np.random.randint(-10, 10, (3, 3, 3, 32), "int8"),
        "w2" : np.random.randint(-10, 10, (3, 3, 32, 64), "int8"),
    }
    f_mod = FPGAParameters(f_mod, params)
    print(f_mod)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    jit_mod = FPGAJit().Jit(f_mod)
    print(jit_mod)
    c_mod, params, _ = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/source.c", "w") as f:
        f.write(c_mod)

    with open("./test/params.bin", "wb") as f:
        f.write(params)


def main_accel_extern_ccodegen():
    x = relay.var("x", shape=(1, 28, 28, 3), dtype="int8")
    w1 = relay.var("w1", shape=(3, 3, 3, 32), dtype="int8")
    w2 = relay.var("w2", shape=(3, 3, 32, 64), dtype="int8")
    out = relay.accel.vit.conv2d(x, w1, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    out = relay.accel.vit.conv2d(out, w2, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    out = relay.transpose(out, (0, 3, 1, 2))
    func = relay.Function([x, w1, w2], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    params = {
        "w1" : np.random.randint(-10, 10, (3, 3, 3, 32), "int8"),
        "w2" : np.random.randint(-10, 10, (3, 3, 32, 64), "int8"),
    }
    f_mod = FPGAParameters(f_mod, params)
    print(f_mod)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    jit_mod = FPGAJit().Jit(f_mod)
    print(jit_mod)
    c_mod, params, e_mod = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/source_extern.c", "w") as f:
        f.write(c_mod)

    for i in range(len(e_mod)):
        with open(f"./test/source_extern_{i}.c", "w") as f:
            f.write(e_mod[i])

    with open("./test/params.bin", "wb") as f:
        f.write(params)

from fcompile.simulate import modelsim
from fcompile.config import SIM_ROOT
SIM_ROOT = "../sim"

def main_accel_modelsim():
    x = relay.var("x", shape=(1, 28, 28, 3), dtype="int8")
    w1 = relay.var("w1", shape=(3, 3, 3, 32), dtype="int8")
    out = relay.accel.vit.conv2d(x, w1, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    func = relay.Function([x, w1], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "x" : np.random.randint(-10, 10, (1, 28, 28, 3), "int8"),
        "w1" : np.random.randint(-10, 10, (3, 3, 3, 32), "int8"),
    }
    result = modelsim(f_mod, inputs)
    print(result)


def main_accel_extern_modelsim():
    x = relay.var("x", shape=(1, 28, 28, 3), dtype="int8")
    w1 = relay.var("w1", shape=(3, 3, 3, 32), dtype="int8")
    out = relay.accel.vit.conv2d(x, w1, strides=1, padding=0, widths=[8, 8, 8], scales=[5, 5, 4], activate=0)
    out = relay.transpose(out, (0, 3, 1, 2))
    func = relay.Function([x, w1], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    inputs = {
        "x" : np.random.randint(-10, 10, (1, 28, 28, 3), "int8"),
        "w1" : np.random.randint(-10, 10, (3, 3, 3, 32), "int8"),
    }
    result = modelsim(f_mod, inputs)
    print(result)


from fcompile.example import relay_attention

def main_attention_ccodegen():
    qscales = [6, 10, 7]
    kscales = [6, 10, 7]
    vscales = [6, 10, 7]
    oscales = [9, 8, 7, 6, 7]
    mod = relay_attention(8, qscales, kscales, vscales, oscales)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    params = {
        "qweight" : np.random.randint(-10, 10, (1, 1, 64, 64), "int8"),
        "kweight" : np.random.randint(-10, 10, (1, 1, 64, 64), "int8"),
        "vweight" : np.random.randint(-10, 10, (1, 1, 64, 64), "int8"),
        "nweight" : np.random.randint(-10, 10, (1, 1, 64, 64), "int8"),
    }
    f_mod = FPGAParameters(f_mod, params)
    print(f_mod)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    jit_mod = FPGAJit().Jit(f_mod)
    print(jit_mod)
    c_mod, params, _ = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/attention.c", "w") as f:
        f.write(c_mod)

    with open("./test/params.bin", "wb") as f:
        f.write(params)


#main_accel_ccodegen()
#main_accel_extern_ccodegen()
#main_accel_modelsim()
#main_accel_extern_modelsim()
main_attention_ccodegen()
