import os
import numpy as np
import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform
from fcompile.fir import FModule
from fcompile.cir import PrintExpr
from fcompile.transform import RelayFIR, Ftransform
from fcompile.codegen import CCodeGen


def main_accel_ccodegen():
    x = relay.var("x", shape=(1, 28, 28, 32), dtype="float16")
    w1 = relay.var("w1", shape=(1, 1, 1, 64), dtype="float16")
    out = relay.accel.hbm.rms_norm(x, w1, widths=[16, 16, 16], scales=[0, 0, 0])
    out = relay.accel.hbm.softmax(out, widths=[16, 16], scales=[0, 0])
    func = relay.Function([x, w1], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    params = {
        "w1": np.random.randn(1, 1, 1, 64).astype("float16"),
    }
    f_mod = Ftransform("param_const")(f_mod, params)
    print(f_mod)
    f_mod = Ftransform("data_map")(f_mod)
    print(f_mod)
    c_mod = Ftransform("cir_lower")(f_mod)
    for mod in c_mod:
        if mod["node"] == "cir":
            PrintExpr.print(mod["source"], mod["name"])
        else:
            print(mod)
    runtime, params, source = CCodeGen().build(c_mod)
    os.makedirs("test", exist_ok=True)
    with open("./test/runtime.c", "w") as f:
        f.write(runtime)

    for n, src in enumerate(source):
        with open(f"./test/source_{n}.c", "w") as f:
            f.write(src)

    with open("./test/params.bin", "wb") as f:
        f.write(params)


def main_atten_ccodegen():
    x = relay.var("x", shape=(1, 1, 28, 32), dtype="float16")
    x2 = relay.var("x2", shape=(1, 1, 64, 32), dtype="float16")
    w1 = relay.var("w1", shape=(1, 1, 32, 64), dtype="float16")
    w2 = relay.var("w2", shape=(1, 1, 32, 64), dtype="float16")
    x3 = relay.var("x3", shape=(1, 1, 28, 64), dtype="float16")
    out1 = relay.accel.hbm.matmul(x, w1, widths=[16, 4, 16], scales=[0, 0, 0])
    out2 = relay.accel.hbm.matmul(x2, w2, widths=[16, 4, 16], scales=[0, 0, 0])
    out = relay.accel.hbm.matmul(out1, out2, widths=[16, 4, 16], scales=[0, 0, 0])
    out = relay.accel.hbm.add(out, x3, widths=[16, 16, 16], scales=[0, 0, 0])
    func = relay.Function([x, w1, w2, x2, x3], out)
    mod = IRModule.from_expr(func)
    print(mod)
    mod = transform.InferType()(mod)
    print(mod)
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    params = {
        "w1": np.random.randn(1, 1, 32, 64).astype("float16"),
    }
    f_mod = Ftransform("param_const")(f_mod, params)
    #f_mod = Ftransform("fusion_ops")(f_mod)
    f_mod = Ftransform("data_map")(f_mod)
    print(f_mod)
    c_mod = Ftransform("cir_lower")(f_mod)
    '''
    for mod in c_mod:
        if mod["node"] == "cir":
            PrintExpr.print(mod["source"], mod["name"])
        else:
            print(mod)
    '''
    runtime, params, source = CCodeGen().build(c_mod)
    os.makedirs("test", exist_ok=True)
    with open("./test/runtime.c", "w") as f:
        f.write(runtime)

    for n, src in enumerate(source):
        with open(f"./test/source_{n}.c", "w") as f:
            f.write(src)

    with open("./test/params.bin", "wb") as f:
        f.write(params)


# main_accel_ccodegen()
main_atten_ccodegen()
