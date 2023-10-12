import struct

class OpType:

    f_ddr = "struct Mapped_Feature*"
    w_ddr = "struct Mapped_Weight*"
    t_val = "DLTensor"
    c_ptr = "uint8_t*"
    const = "const uint8_t*"


class BaseCodeGen:

    def gen(self, key, value):
        if key == "static":
            return self.gen_static(value)
        elif key == "callop":
            return self.gen_callop(value)
        elif key == "data":
            return self.gen_data(value)
        elif key == "return":
            return self.gen_return(value)
        elif key == "params":
            return self.gen_params(value)


    def gen_static(self, value):
        pass

    def gen_callop(self, value):
        pass

    def gen_data(self, value):
        pass

    def gen_return(self, value):
        pass

    def gen_params(self, value):
        pass

    
def np2bytes(data):
    data = data.reshape((1, -1))
    size = data.size
    params = b""
    for n in range(size):
        params += struct.pack("h", int(data[0, n]))
    return params