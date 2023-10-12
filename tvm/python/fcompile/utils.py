def list2str(shape):
    str_list = ", ".join([str(i) for i in shape])
    return str_list

def shape2str(shape):
    str_shape = list2str(shape)
    return f"({str_shape})"
    
def shape2byte(shape, dtype):
    if dtype in ["int8", "uint8"]:
        byte = 1
    else:
        byte = 0
    for s in shape:
        byte = byte * s
    return byte