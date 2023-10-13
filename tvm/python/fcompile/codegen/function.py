from ..utils import *

def MallocFeature(name, attrs, prefix=1):
    str_attrs = list2str(attrs)
    if prefix:
        return f"struct Mapped_Feature* {name} = Malloc_Feature({str_attrs});"
    else:
        return f"{name} = Malloc_Feature({str_attrs});"


def MallocWeight(name, attrs, prefix=1):
    str_attrs = list2str(attrs)
    if prefix:
        return f"struct Mapped_Weight* {name} = Malloc_Weight({str_attrs});"
    else:
        return f"{name} = Malloc_Weight({str_attrs});"


def CPtr2Feature(in_name, out_name):
    return f"Map_Feature((char*){in_name}, {out_name});"


def CPtr2Weight(in_name, out_name):
    return f"Map_Weight((char*){in_name}, {out_name});"


def DLTensor2Feature(in_name, out_name):
    return f"Map_Feature((char*){in_name}.data, {out_name});"


def FreeFeature(name):
    return f"Free_Feature({name});"


def Feature2CPtr(in_name, out_name):
    return f"DeMap_Feature({in_name}, (char*){out_name});"


def Feature2DLTensor(in_name, out_name):
    return f"DeMap_Feature({in_name}, (char*){out_name}.data);"


def CreateDLTensor(name, ptr_name, shape):
    stride = shape2stride(shape)
    dltensor = f"DLTensor {name};"
    dldata   = f"{name}.data = (void*){ptr_name};"
    dlndim   = f"{name}.ndim = {len(shape)};"
    dloffset = f"{name}.byte_offset = 0;"
    dldtype  = f"{name}.dtype = " + "(DLDataType){.code = 2, .bits = 32, .lanes = 1};"
    dldevice = f"{name}.device = " + "(DLDevice){.device_type = kDLCPU, .device_id = 0};"
    tpshape  = f"int64_t shape_{name}[] = " + "{" + list2str(shape) + "};"
    dlshape  = f"{name}.shape = shape_{name};"
    tpstride = f"int64_t stride_{name}[] = " + "{" + list2str(stride) + "};"
    dlstride = f"{name}.strides = stride_{name};"
    return [dltensor, dldata, dlndim, dloffset, dldtype, dldevice, tpshape, dlshape, tpstride, dlstride]
