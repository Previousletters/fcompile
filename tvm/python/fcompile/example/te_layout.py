import tvm
from tvm import te

def get_feature(shape, dtype, tout):
    _, h, w, c = shape
    n = (c + tout - 1) // tout
    new_shape = (n, h, w, tout)
    zero = tvm.tir.const(0).astype("int8")
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    out = te.compute(new_shape, lambda i, j, k, l : 
                te.if_then_else(i*tout+l < c, var[0, j, k, i*tout+l].astype("int8"), zero), 
                name="fout"
    )
    s = te.create_schedule(out.op)

    print(tvm.lower(s, [var, out], simple_mode=True))
    rt = tvm.build(s, [var, out], target="c", name="wrapper_feature")
    print(rt.get_source())

def get_feature_width(shape, dtype, tout, width):
    _, h, w, c = shape
    n = (c + tout - 1) // tout
    new_shape = (n, h, w, tout)
    zero = tvm.tir.const(0).astype("int8")
    opos = tvm.tir.const(2**(width-1)-1).astype("int8")
    oneg = tvm.tir.const(-2**(width-1)).astype("int8")
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    osc = te.compute(shape, lambda i, j, k, l : (var[i, j, k, l]).astype("int16"), name="osc")
    def compute_out(i, j, k, l):
        index = i*tout+l
        tdata = osc[0, j, k, index]
        return te.if_then_else(
                    index < c, 
                    te.if_then_else(
                        tdata > opos, 
                        opos,
                        te.if_then_else(
                            tdata < oneg,
                            oneg,
                            tdata.astype("int8")
                        )
                    ), 
                    zero
                )
    out = te.compute(new_shape, compute_out, name="fout")
    s = te.create_schedule(out.op)
    s[osc].compute_inline()
    print(tvm.lower(s, [var, osc, out], simple_mode=True))
    rt = tvm.build(s, [var, osc, out], target="c", name="wrapper_feature")
    print(rt.get_source())

def get_softmax_feature(shape, dtype, tout, width):
    _, h, w, c = shape
    n = (c + tout - 1) // tout
    nh = (h*w + tout - 1) // tout
    nw = tout
    new_shape = (n, nh, nw, tout)
    zero = tvm.tir.const(0).astype("int8")
    opos = tvm.tir.const(2**(width-1)-1).astype("int8")
    oneg = tvm.tir.const(-2**(width-1)).astype("int8")
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    osc = te.compute(shape, lambda i, j, k, l : (var[i, j, k, l]).astype("int16"), name="osc")
    def compute_out(i, j, k, l):
        cindex = i*tout+l
        hindex = j*tout+k
        tdata = osc[0, j, k, cindex]
        return te.if_then_else(
                    te.all(cindex < c, hindex < h*w),
                    te.if_then_else(
                        tdata > opos, 
                        opos,
                        te.if_then_else(
                            tdata < oneg,
                            oneg,
                            tdata.astype("int8")
                        )
                    ), 
                    zero
                ) 
    out = te.compute(new_shape, compute_out, name="fout")
    s = te.create_schedule(out.op)
    s[osc].compute_inline()
    print(tvm.lower(s, [var, osc, out], simple_mode=True))
    rt = tvm.build(s, [var, osc, out], target="c", name="wrapper_feature")
    print(rt.get_source())


def get_params(wt, shape, tout, tin):
    layout = "OIHW"
    o, i, h, w = shape
    sc_tin = (i + tin - 1) // tin
    max_so_tout = (o + tout - 1) // tout
    st_tout = (tin + tout - 1) // tout
    tp_ = o - (max_so_tout-1)*tout
    if o%tout == 0:
        so_tout = (o + tout - 1) // tout
    else:
        if (o + tout - 1) // tout == 1:
            so_tout = o % tout
        else:
            so_tout = o / tout
    num = int(so_tout*sc_tin*h*w*tout*st_tout*tout)


    new_shape0 = ((max_so_tout-1)*sc_tin*h*w*tout*st_tout*tout,)
    new_shape1 = (sc_tin*h*w*tout*st_tout*tout,)
    new_shape = (num,)
    zero = tvm.tir.const(0).astype("int8")
    var = te.placeholder(name="var", shape=shape, dtype="int8")
    def compute_out0(n):
        tin_out = te.indexmod(n, tout)
        mshwts = te.indexdiv(n, tout)
        kk = te.indexmod(mshwts, st_tout)
        mshwt = te.indexdiv(mshwts, st_tout)
        t_chout = te.indexmod(mshwt, tout)
        mshw = te.indexdiv(mshwt, tout)
        kx = te.indexmod(mshw, h)
        msh = te.indexdiv(mshw, h)
        ky = te.indexmod(msh, w)
        ms = te.indexdiv(msh, w)
        l = te.indexmod(ms, sc_tin)
        m = te.indexdiv(ms, sc_tin)
        cindex = l*tin+kk*tout + tin_out
        tinindex = kk*tout + tin_out
        oindex = m*tout + t_chout
        if layout == "OIHW":
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[oindex, cindex, ky, kx].astype("int8"),
                        zero
                    ) 
        elif layout == "IOHW":
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[cindex, oindex, ky, kx].astype("int8"),
                        zero
                    ) 
    out0 = te.compute(new_shape0, compute_out0, name="fout0")
    def compute_out1(n):
        tin_out = te.indexmod(n, tout)

        mshwts = te.indexdiv(n, tout)
        kk = te.indexmod(mshwts, st_tout)

        mshwt = te.indexdiv(mshwts, st_tout)
        t_chout = te.indexmod(mshwt, tp_)

        mshw = te.indexdiv(mshwt, tp_)
        kx = te.indexmod(mshw, h)

        msh = te.indexdiv(mshw, h)
        ky = te.indexmod(msh, w)

        ms = te.indexdiv(msh, w)
        l = te.indexmod(ms, sc_tin)

        m = te.indexdiv(ms, sc_tin)
        cindex = l*tin+kk*tout + tin_out
        tinindex = kk*tout + tin_out
        oindex = (max_so_tout-1 + m)*tout + t_chout
        if layout == "OIHW":
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[oindex, cindex, ky, kx].astype("int8"),
                        zero
                    ) 
        elif layout == "IOHW":
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[cindex, oindex, ky, kx].astype("int8"),
                        zero
                    ) 
    out1 = te.compute(new_shape1, compute_out1, name="fout1")
    out = te.compute(new_shape, lambda n: te.if_then_else(n >= new_shape0[0], out1[n-new_shape0[0]], out0[n]))
    s = te.create_schedule(out.op)
    rt = tvm.build(s, [var, out], target="llvm")
    result_t = tvm.nd.array(np.zeros(new_shape, dtype="int8"))
    rt(wt, result_t)
    return result_t.asnumpy().flatten()


if __name__ == "__main__":
    import numpy as np
    from fcompile.process import parallel_weight_Tin
    result = 0
    for i in range(100):
        o, i, h, w, tout, tin = 65, 3, 4, 4, 32, 64
        wt = np.random.randint(-10, 10, (o, i, h, w), dtype="int8")
        result_p = parallel_weight_Tin(wt, o, i, h, w, tout, tin)
        in_wt = tvm.nd.array(wt)
        result_t = get_params(in_wt, (o, i, h, w), tout, tin)
        result += sum(result_t-result_p)
    print(result)
