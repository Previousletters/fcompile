import os
import numpy as np
import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform
from fcompile.fir import FModule
from fcompile.cir import PrintExpr
from fcompile.transform import RelayFIR, FPGAParameters, DataMap, Lower
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
    f_mod = FPGAParameters(f_mod, params)
    print(f_mod)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    c_ir = Lower().build(f_mod)
    PrintExpr.print(c_ir)
    '''
    jit_mod = FPGAJit().Jit(f_mod)
    print(jit_mod)
    c_mod, params, _ = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/source.c", "w") as f:
        f.write(c_mod)

    with open("./test/params.bin", "wb") as f:
        f.write(params)
    '''


main_accel_ccodegen()
