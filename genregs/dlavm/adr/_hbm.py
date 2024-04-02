from .base import Op, Tensor, Tuple, DataEnum, DataType
from functools import reduce


def CHECK_ERROR(judge, error_str):
    if judge:
        print(error_str)
        exit(-1)


def MVMRel(args, attrs):
    if len(args) != attrs["skip"]+1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype  # Cin, Cout
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.hbm or wtype.dtype != DataEnum.int4:
        return False, []
    if dshape[-1] != wshape[0]:
        return False, []
    if len(args) > 2:
        for n in range(len(args)-2):
            if args[1] != args[2+n]:
                return False, []
    oshape = [i for i in dshape]
    oshape[-1] = wshape[-1]
    return True, Tensor(oshape, dtype, device)


Op.Register("accel.hbm.mvm", MVMRel)


def MVMafterTRPRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # FH, H, W, C
    wshape, wtype = args[1].shape, args[1].dtype  # WH, O, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, []
    if dshape[-1] != wshape[-1] or dshape[-2] != wshape[-2]:
        return False, []
    oshape = [i for i in dshape]
    return True, Tensor(oshape, dtype, device)


Op.Register("accel.hbm.mvm_afterTRP", MVMafterTRPRel)


def MVMafterF2WRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # FH, H, W, C
    wshape, wtype = args[1].shape, args[1].dtype  # WH, O, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "accel.hbm.mvm_afterF2W gets wrong data type"
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, "accel.hbm.mvm_afterF2W gets wrong weight type"
    if dshape[-1] != wshape[-1]:
        return False, "accel.hbm.mvm_afterF2W needs same chout"
    oshape = [i for i in dshape]
    return True, Tensor(oshape, dtype, device)


Op.Register("accel.hbm.mvm_afterF2W", MVMafterF2WRel)


def MVMBNRel(args, attrs):
    if len(args) != attrs["skip"]+2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype  # Cin, Cout
    bshape, btype = args[-1].shape, args[-1].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.hbm or wtype.dtype != DataEnum.int4:
        return False, []
    if btype.mapped != DataEnum.ddr or btype.dtype != DataEnum.fp16:
        return False, []
    if dshape[-1] != wshape[0]:
        return False, []
    if bshape[-1] != wshape[1]*2:
        return False, []
    if bshape[:-1] != [1 for i in range(len(bshape)-1)]:
        return False, []
    oshape = [i for i in dshape]
    oshape[-1] = wshape[1]
    return True, Tensor(oshape, dtype, device)


Op.Register("accel.hbm.mvm_bn", MVMBNRel)


def MVMBNResRel(args, attrs):
    if len(args) != attrs["skip"]+3:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype  # Cin, Cout
    bshape, btype = args[-2].shape, args[-2].dtype  # H, W, C
    rshape, rtype = args[-1].shape, args[-1].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "data check error: " + str(args[0])
    if wtype.mapped != DataEnum.hbm or wtype.dtype != DataEnum.int4:
        return False, "weight check error: " + str(args[1])
    if btype.mapped != DataEnum.ddr or btype.dtype != DataEnum.fp16:
        return False, "bn check error: " + str(args[-2])
    if rtype.mapped != DataEnum.ddr or rtype.dtype != DataEnum.fp16:
        return False, "res check error: " + str(args[-1])
    if dshape[-1] != wshape[0]:
        return False, f"input channel check error: {dshape[2]} and {wshape[0]}" 
    if bshape[-1] != wshape[1]*2:
        return False, f"bn channel check error: {bshape[-1]} and {wshape[1]}"
    if bshape[:-1] != [1 for i in range(len(bshape)-1)]:
        return False, "bn shape is error!"
    if len(args) > 4:
        for n in range(len(args)-4):
            if args[1] != args[2+n]:
                return False, "weights do not match!"
    oshape = [dshape[0], dshape[1], wshape[1]]
    # if rshape[0] != oshape[0] or rshape[1] != oshape[1] or rshape[2] != oshape[2]:
    if rshape[2] != oshape[2]:
        rshape_str = "(" + ", ".join([str(n) for n in rshape]) + ")"
        oshape_str = "(" + ", ".join([str(n) for n in oshape]) + ")"
        return False, f"res add data{rshape_str} is not same with output data{oshape_str}"
    if attrs["arg_max"]:
        return True, Tuple([Tensor(oshape, dtype, device), Tensor(oshape, dtype, device)])
    else:
        return True, Tensor(oshape, dtype, device)


Op.Register("accel.hbm.mvm_bn_res", MVMBNResRel)


def AddRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, []
    if dshape != wshape and dshape[-1] != wshape[-1]:
        return False, "two data shapes not match"
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.add", AddRel)


def MulRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, []
    if dshape != wshape:
        return False, []
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.mul", MulRel)


def LayerNormRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, []
    if dshape[-1]*2 != wshape[-1]:
        return False, "input channels not match"
    if wshape[:-1] != [1 for i in range(len(wshape)-1)]:
        return False, "weight first ndim should be 1"
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.layer_norm", LayerNormRel)


def SoftmaxRel(args, attrs):
    if len(args) != 1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, []
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.softmax", SoftmaxRel)


def PosEmbRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "input data type is error"
    if wtype.mapped != DataEnum.ddr or wtype.dtype != DataEnum.fp16:
        return False, "weight data type is error"
    if dshape[-1] != wshape[-1]*2:
        return False, "input channel not match"
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.pos_emb", PosEmbRel)


def TransposeRel(args, attrs):
    if len(args) != 1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "input data type is error"
    if dshape[:-2] != [1 for i in range(len(dshape)-2)]:
        return False, "the first ndim should be 1"
    oshape = [dshape[-1], dshape[-2]]
    return True, Tensor(oshape, DataType(DataEnum.int4, DataEnum.hbm), device)


Op.Register("accel.hbm.transpose", TransposeRel)


def Feature2WeightRel(args, attrs):
    if len(args) != 1:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "input data type is error"
    if dshape[:-2] != [1 for i in range(len(dshape)-2)]:
        return False, "the first ndim should be 1"
    oshape = [dshape[-2], dshape[-1]]
    return True, Tensor(oshape, DataType(DataEnum.int4, DataEnum.hbm), device)


Op.Register("accel.hbm.feature2weight", Feature2WeightRel)


def ActivateRel(args, attrs):
    if len(args) != 2:
        return False, []
    device = args[0].device
    dshape, dtype = args[0].shape, args[0].dtype  # H, W, C
    wshape, wtype = args[1].shape, args[1].dtype
    if dtype.mapped != DataEnum.ddr or dtype.dtype != DataEnum.fp16:
        return False, "input data type is error"
    if wtype.mapped != DataEnum.ddr:
        return False, "Activate weight should be DDR data"
    if reduce(lambda x, y: x * y, wshape) < 48:
        return False, "Maybe activate weight is too small"
    return True, Tensor(dshape, dtype, device)


Op.Register("accel.hbm.activate", ActivateRel)