import sys
sys.path.append("../../genregs/")

from genregs import cir
from genregs import ne
from genregs.transform import InferType, PrintExpr


def block():
    seq = ne.Var("seq")
    data = cir.Input("data", [seq, 1, 4096], "feature")
    ln_weight = cir.Input("ln_weight", [1, 1, 4096*2], "feature")
    q_weight = cir.Input("q_weight", [1, 1, 4096, 128*32], "weight")
    q_bn = cir.Input("q_bn", [1, 1, 128*32*2], "feature")
    k_weight = cir.Input("k_weight", [1, 1, 4096, 128*2], "weight")
    k_bn = cir.Input("k_bn", [1, 1, 128*2*2], "feature")
    v_weight = cir.Input("v_weight", [1, 1, 4096, 128*2], "weight")
    v_bn = cir.Input("v_bn", [1, 1, 128*2*2], "feature")
    rsqrt = cir.Input("rsqrt", [32, seq, seq], "feature")

    out = cir.LayerNorm(data, ln_weight)
    q_data = cir.MVM_BN(out, q_weight, q_bn)
    k_data = cir.MVM_BN(out, k_weight, k_bn)
    v_data = cir.MVM_BN(out, v_weight, v_bn)

    q_data = cir.PosEmb(q_data)
    k_data = cir.PosEmb(k_data)

    q_data = cir.Reshape(q_data, new_shape=[32, seq, 128])
    k_data = cir.Reshape(k_data, new_shape=[2, seq, 128])
    q_tuple = cir.Split(q_data, axis=0)
    k_tuple = cir.Split(k_data, axis=0)

    qk_tuple = []
    for i in range(2):
        q_data_i = cir.TupleItem(q_tuple, i)
        k_data_i = cir.TupleItem(k_tuple, i)
        k_data_i = cir.Transpose(k_data_i)
        qk_tuple.append(cir.MVM(q_data_i, k_data_i, skip=2))

    qk_out = cir.Merge(*qk_tuple)
    qk_out = cir.ElementWise(qk_out, rsqrt)
    qk_out = cir.Softmax(qk_out)

    qk_tuple = cir.Split(qk_out, axis=0)
    v_data = cir.Reshape(v_data, new_shape=[2, seq, 128])
    v_tuple = cir.Split(v_data, axis=0)

    atten_tuple = []
    for i in range(2):
        qk_data_i = cir.TupleItem(qk_tuple, i)
        v_data_i = cir.TupleItem(v_tuple, i)
        v_data_i = cir.Feature2Weight(v_data_i)
        atten_tuple.append(cir.MVM(qk_data_i, v_data_i, skip=2))

    atten_out = cir.Merge(*atten_tuple)

    return atten_out


def kv_cache():
    seq = ne.Var("seq")
    data = cir.Input("data", [1, 1, 4096], "feature")
    q_weight = cir.Input("q_weight", [1, 1, 4096, 128*2], "weight")
    k_weight = cir.Input("k_weight", [1, 1, 4096, 128*2], "weight")
    v_weight = cir.Input("v_weight", [1, 1, 4096, 128*2], "weight")

    k_cache = cir.Cache("k_cache", 64*256)
    v_cache = cir.Cache("v_cache", 64*256)
    q_data = cir.MVM(data, q_weight)
    k_data = cir.MVM(data, k_weight)
    v_data = cir.MVM(data, v_weight)

    kc_data = cir.LoadCache(k_cache, [1, seq, 256], "feature")
    k_data = cir.Merge(kc_data, k_data, axis=1)
    k_data = cir.StoreCache(k_data, k_cache)

    k_data = cir.Transpose(k_data)
    scores = cir.MVM(q_data, k_data)
    scores = cir.Softmax(scores)

    vc_data = cir.LoadCache(v_cache, [1, seq, 256], "feature")
    v_data = cir.Merge(vc_data, v_data, axis=1)
    v_data = cir.StoreCache(v_data, v_cache)
    v_data = cir.Feature2Weight(v_data)
    scores = cir.MVM(scores, v_data)

    InferType.infer(scores)
    PrintExpr.print(scores)


if __name__ == "__main__":
    '''
    expr = block()
    InferType.infer(expr)
    PrintExpr.print(expr)
    '''
    kv_cache()
