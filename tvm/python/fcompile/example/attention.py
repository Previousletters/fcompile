import tvm
from tvm import relay
from tvm import IRModule
from tvm.relay import transform

def relay_attention(width, qscales, kscales, vscales, oscales):
    qdscale, qwscale, qoscale = qscales
    kdscale, kwscale, koscale = kscales
    vdscale, vwscale, voscale = vscales
    nscale, a0scale, a1scale, a2scale, oscale = oscales
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
    return mod
