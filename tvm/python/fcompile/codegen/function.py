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