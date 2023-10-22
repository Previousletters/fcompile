import tvm
from tvm import te

from ..codegen import Layout

LAYOUT_TRANS_MAP = {}

def register_layout_trans(org_layout, tag_layout):
    def _register_fn(fn):
        global LAYOUT_TRANS_MAP
        if org_layout in LAYOUT_TRANS_MAP.keys():
            LAYOUT_TRANS_MAP[org_layout][tag_layout] = fn
        else:
            LAYOUT_TRANS_MAP[org_layout] = {tag_layout : fn}
        def _call(*args, **kwargs):
            return fn(*args, **kwargs)
        return _call
    return _register_fn


@register_layout_trans(Layout.BHWC, Layout.NHWT)
def pad_split_C_tout(shape, tag_shape, dtype, tin, tout, width):
    _, h, w, c = shape
    n = (c + tout - 1) // tout
    new_shape = (n, h, w, tout)
    zero = tvm.tir.const(0).astype("int8")
    if dtype == "int8":
        var = te.placeholder(name="var", shape=shape, dtype=dtype)
        def compute_out(i, j, k, l):
            index = i*tout+l
            tdata = var[0, j, k, index]
            return te.if_then_else(
                        index < c, 
                        tdata.astype("int8"),
                        zero
                    )
        out = te.compute(new_shape, compute_out, name="fout")
        s = te.create_schedule(out.op)
        return tvm.lower(s, [var, out], simple_mode=True)
    else:
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
        return tvm.lower(s, [var, osc, out], simple_mode=True)


@register_layout_trans(Layout.NHWT, Layout.BHWC)
def unpad_C(shape, tag_shape, dtype, tin, tout, width):
    n, h, w, _ = shape
    _, _, _, c = tag_shape
    new_shape = (1, h, w, c)
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    def compute_out(i, j, k, l):
        nindex = te.indexdiv(l, tout)
        tindex = te.indexmod(l, tout)
        tdata = var[nindex, j, k, tindex].astype(dtype)
        return tdata
    out = te.compute(new_shape, compute_out, name="fout")
    s = te.create_schedule(out.op)
    return tvm.lower(s, [var, out], simple_mode=True)


@register_layout_trans(Layout.BHWC, Layout.NHTT)
def pad_split_HWC_tout(shape, tag_shape, dtype, tin, tout, width):
    _, h, w, c = shape
    n = (c + tout - 1) // tout
    nh = (h*w + tout - 1) // tout
    nw = tout
    new_shape = (n, nh, nw, tout)
    zero = tvm.tir.const(0).astype("int8")
    if dtype == "int8":
        var = te.placeholder(name="var", shape=shape, dtype=dtype)
        def compute_out(i, j, k, l):
            cindex = i*tout+l
            hwindex = j*tout+k
            nhindex = te.indexdiv(hwindex, w)
            nwindex = te.indexmod(hwindex, w)
            tdata = var[0, nhindex, nwindex, cindex]
            return te.if_then_else(
                        te.all(cindex < c, hwindex < h*w),
                        tdata, 
                        zero
                    ) 
        out = te.compute(new_shape, compute_out, name="fout")
        s = te.create_schedule(out.op)
        return tvm.lower(s, [var, out], simple_mode=True)
    else:
        opos = tvm.tir.const(2**(width-1)-1).astype("int8")
        oneg = tvm.tir.const(-2**(width-1)).astype("int8")
        var = te.placeholder(name="var", shape=shape, dtype=dtype)
        osc = te.compute(shape, lambda i, j, k, l : (var[i, j, k, l]).astype("int16"), name="osc")
        def compute_out(i, j, k, l):
            cindex = i*tout+l
            hwindex = j*tout+k
            nhindex = te.indexdiv(hwindex, w)
            nwindex = te.indexmod(hwindex, w)
            tdata = osc[0, nhindex, nwindex, cindex]
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
        return tvm.lower(s, [var, osc, out], simple_mode=True)


@register_layout_trans(Layout.NHTT, Layout.BHWC)
def unpad_HWC(shape, tag_shape, dtype, tin, tout, width):
    n, h, _, _ = shape
    _, nh, nw, c = tag_shape
    new_shape = (1, nh, nw, c)
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    def compute_out(i, j, k, l):
        nbindex = te.indexdiv(l, tout)
        ncindex = te.indexmod(l, tout)
        tpindex = j*nw + k
        nhindex = te.indexdiv(tpindex, tout)
        nwindex = te.indexmod(tpindex, tout)
        tdata = var[nbindex, nhindex, nwindex, tindex].astype(dtype)
        return tdata
    out = te.compute(new_shape, compute_out, name="fout")
    s = te.create_schedule(out.op)
    return tvm.lower(s, [var, out], simple_mode=True)


@register_layout_trans(Layout.NHTT, Layout.NHWT)
def unpad_HW(shape, tag_shape, dtype, tin, tout, width):
    n, h, _, t = shape
    _, nh, nw, _ = tag_shape
    new_shape = (n, nh, nw, t)
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
    def compute_out(i, j, k, l):
        tpindex = j*nw + k
        nhindex = te.indexdiv(tpindex, tout)
        nwindex = te.indexmod(tpindex, tout)
        tdata = var[i, nhindex, nwindex, l].astype(dtype)
        return tdata
    out = te.compute(new_shape, compute_out, name="fout")
    s = te.create_schedule(out.op)
    return tvm.lower(s, [var, out], simple_mode=True)


def pad_reshape(shape, tag_shape, dtype, tin, tout, width, layout):
    opos = tvm.tir.const(2**(width-1)-1).astype("int8")
    oneg = tvm.tir.const(-2**(width-1)).astype("int8")
    bit_num = 8//width
    if layout = Layout.OIHW:
        o, i, h, w = shape
    elif layout = Layout.HWIO:
        h, w, i, o = shape
    elif layout = Layout.NHWT:
        tn, w, i, _ = shape
        _, _, _, o = tag_shape
        h = 1
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
    zero = tvm.tir.const(0)
    var = te.placeholder(name="var", shape=shape, dtype=dtype)
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
        if layout == Layout.OIHW:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[oindex, cindex, ky, kx],
                        zero
                    ) 
        elif layout == Layout.HWIO:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[ky, kx, cindex, oindex],
                        zero
                    ) 
        elif layout == Layout.NHWT:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[m, ky, cindex, t_chout],
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
        if layout == Layout.OIHW:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[oindex, cindex, ky, kx],
                        zero
                    ) 
        elif layout == Layout.HWIO:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[ky, kx, cindex, oindex],
                        zero
                    ) 
        elif layout == Layout.NHWT:
            return te.if_then_else(
                        te.all(cindex < i, tinindex < tin, oindex < o),
                        var[(max_so_tout-1 + m), ky, cindex, t_chout],
                        zero
                    ) 
    out1 = te.compute(new_shape1, compute_out1, name="fout1")
    def overflow(n):
        tdata = te.if_then_else(n >= new_shape0[0], out1[n-new_shape0[0]], out0[n])
        tdata = te.if_then_else(
            tdata > opos, 
            opos,
            te.if_then_else(
                tdata < oneg,
                oneg,
                tdata.astype("int8")
            )
        )
        tdata = te.if_then_else(tdata > 0, tdata, tdata + (2**width))
        return tdata
    outf = te.compute(new_shape, overflow, name="overflow")
    def combine_data(n):
        if width == 4:
            data_0 = (outf[n*2]).astype("int8")
            data_1 = (outf[n*2+1] * (1 << width)).astype("int8")
            return data_0 + data_1
        elif width == 8:
            data_0 = (outf[n*4]).astype("int8")
            data_1 = (outf[n*4+1] * (1 << (width*1))).astype("int8")
            data_1 = (outf[n*4+2] * (1 << (width*2))).astype("int8")
            data_1 = (outf[n*4+3] * (1 << (width*3))).astype("int8")
            return data_0 + data_1 + data_2 + data_3
        elif bit_num == 1:
            return outf[n]
    out = te.compute((num//bit_num), combine_data, name="combine_data")
    s = te.create_schedule(out.op)
    return tvm.lower(s, [var, out], simple_mode=True)


@register_layout_trans(Layout.OIHW, Layout.OCHWTST)
def pad_reshape_OI_OIHW(shape, tag_shape, dtype, tin, tout, width):
    return pad_reshape(shape, tag_shape, dtype, tin, tout, width, Layout.OIHW)


@register_layout_trans(Layout.HWIO, Layout.OCHWTST)
def pad_reshape_OI_HWIO(shape, tag_shape, dtype, tin, tout, width):
    return pad_reshape(shape, tag_shape, dtype, tin, tout, width, Layout.HWIO)


@register_layout_trans(Layout.NHWT, Layout.OCHWTST)
def pad_reshape_OI_NHWT(shape, tag_shape, dtype, tin, tout, width):
    return pad_reshape(shape, tag_shape, dtype, tin, tout, width, Layout.NHWT)

