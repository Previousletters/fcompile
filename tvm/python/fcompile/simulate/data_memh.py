import struct
import numpy as np


def int_hex(data, max_dt, neg_dt):
    if data >= neg_dt:
        data = neg_dt-1
    if data < 0:
        if data < -neg_dt:
            data = -neg_dt
        data = data + max_dt
    return "{:02x}".format(data) # 8bit


def hex_int(data, max_dt, neg_dt):
    data = int(data, 16)
    if data >= neg_dt:
        data = data - max_dt
    return data # 8bit


def writememh_int(name, data, width):
    max_dt, neg_dt = 2**8, 2**(width-1)
    data = data.reshape((1, -1))
    data_list = []
    for d in data[0]:
        data_list.append(int_hex(d, max_dt, neg_dt))
    data_str = "\n".join(data_list)
    try:
        with open(name, "w") as f:
            f.write(data_str)
    except:
        raise RuntimeError("The path in SIM_ROOT is not correct. " + \
                "Please check your \"sim\" path and make sure " + \
                "you have \"make install\" and \"make compile\".")


def readmemh_int(name, width, shape):
    max_dt, neg_dt = 2**8, 2**(width-1)
    data = np.zeros(shape, dtype="int8").reshape((1, -1))
    with open(name, "r") as f:
        data_list = [d for d in f.readlines() if "//" not in d]
    for n in range(data.size):
        data[0, n] = hex_int(data_list[n], max_dt, neg_dt)
    return data.reshape(shape)


def writememh_fp(name, data, width=16):
    data = data.reshape((1, -1))
    data_list = []
    for d in data[0]:
        d = float(d)
        d = struct.unpack('H', struct.pack('e', d))[0]
        data_list.append("{:04x}".format(d))
    data_str = "\n".join(data_list)
    try:
        with open(name, "w") as f:
            f.write(data_str)
    except:
        raise RuntimeError("The path in SIM_ROOT is not correct. " + \
                "Please check your \"sim\" path and make sure " + \
                "you have \"make install\" and \"make compile\".")


def readmemh_fp(name, shape, width=16):
    data = np.zeros(shape, dtype="float16").reshape((1, -1))
    with open(name, "r") as f:
        data_list = [d for d in f.readlines() if "//" not in d]
    for n in range(data.size):
        d = int(data_list[n], 16)
        d = struct.unpack('e', struct.pack('H', d))[0]
        data[0, n] = d
    return data.reshape(shape)
