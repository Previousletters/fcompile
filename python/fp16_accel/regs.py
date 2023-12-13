import sys
sys.path.append("../../genregs/")

from genregs.cir import Input, FC, MVM, LayerNorm, Softmax, Transpose, Feature2Weight, Reshape, Split, Merge, TupleItem
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
    # x = ne.Var("x", 64)
    data = Input("data", [1, 19, 4096], "feature")
    weight = Input("w1", [1, 1, 4096*2], "feature")
    data = LayerNorm(data, weight)
    qw = [Input(f"q{i}", [1, 1, 4096, 128], "weight") for i in range(32)]
    kw = [Input(f"k{i}", [1, 1, 4096, 128], "weight") for i in range(2)]
    vw = [Input(f"v{i}", [1, 1, 4096, 128], "weight") for i in range(2)]
    q = [MVM(data, qw[i]) for i in range(32)]
    k = [Transpose(MVM(data, kw[i])) for i in range(2)]
    v = [Feature2Weight(MVM(data, vw[i])) for i in range(2)]
    out = []
    for i in range(16):
        tp0 = MVM(Softmax(MVM(q[i+0], k[0])), v[0])
        tp1 = MVM(Softmax(MVM(q[i+1], k[1])), v[1])
        out += [tp0, tp1]
    out = out[0]

    InferType.infer(out)
    PrintExpr.print(out)
    model = Dynamic.JIT(out)
    source = CodeGen(model).gen_head("model_test", prefix="block1_")
    with open("test/source.h", "w") as f:
        f.write(source)


def chatglm_block():
    n = Input("test", [1, 128, 128], "feature")
    out = Split(n, axis=1)
    out0 = TupleItem(out, 0)
    out1 = TupleItem(out, 1)
    out = Merge(out0, out1)

    InferType.infer(out)
    PrintExpr.print(out)
    model = Dynamic.JIT(out)
    source = CodeGen(model).gen_head("model_test")
    print(source)


# main_static_jit()
# main_dynamic_jit()
# chatglm_block()
x = ne.Var("x")
y = ne.Var("y")
n = x + y*5
print(n)
