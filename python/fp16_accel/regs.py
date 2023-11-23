import sys
sys.path.append("../../genregs/")

from genregs.cir import Input, FC, MVM, LayerNorm, Softmax, Transpose
from genregs.transform import InferType, PrintExpr
from genregs.export import Static, Dynamic
from genregs.codegen import CodeGen
from genregs import ne


def main_static_jit():
    f = Input("input", [1, 128*2, 32*2], "feature")
    f2 = Input("l_bias", [1, 1, 32*4], "feature")
    out = LayerNorm(f, f2)
    out = Softmax(out)

    InferType.infer(out)
    PrintExpr.print(out)
    model = Static.JIT(out)
    source = CodeGen(model).gen_head("model_test")
    with open("test/source.h", "w") as f:
        f.write(source)


def main_dynamic_jit():
    x = ne.Var("x", 64)
    q = Input("q", [1, x, 512], "feature")
    k = Input("k", [1, x, 512], "feature")
    v = Input("v", [1, 1, x, 512], "weight")
    # f2 = Input("l_bias", [1, 1, 32*2, 32*4], "weight")
    # out = FC(f, f2)
    kt = Transpose(k)
    out = MVM(q, kt)
    out = MVM(out, v)

    InferType.infer(out)
    PrintExpr.print(out)
    model = Dynamic.JIT(out)
    source = CodeGen(model).gen_head("model_test")
    with open("test/source.h", "w") as f:
        f.write(source)


# main_static_jit()
main_dynamic_jit()
