import ctypes
import numpy as np

NUM_OF_RW_CH = 4
lib = ctypes.CDLL("D:/FPGA_ChatGLM2_0509/clib/xdma/xdma_rw.dll", ctypes.RTLD_GLOBAL)

_open_device, _CSB_Write, _CSB_Read = lib.open_device, lib.CSB_Write, lib.CSB_Read
_init, _DDR_Read, _DDR_Write = lib.init, lib.DDR_Read, lib.DDR_Write
_open_device.restype = ctypes.POINTER(ctypes.c_uint64)
_DDR_Read.restype = ctypes.POINTER(ctypes.c_uint8)

devices = _open_device()
accel, c2hx, h2cx = devices[0], devices[2], devices[6]

def CSB_Write(address, data):
    _CSB_Write(accel, address, data)

def CSB_Read(address):
    return _CSB_Read(accel, address)

def DDR_Write(np_data, address):
    bdata = np_data.tobytes()
    data_size = len(bdata)
    brpt = (ctypes.c_byte * data_size).from_buffer(bytearray(bdata))
    trpt = ctypes.cast(brpt, ctypes.POINTER(ctypes.c_uint8))
    tp_r0 = _DDR_Write(h2cx, trpt, ctypes.c_uint64(address), data_size)
    if tp_r0 == data_size:
        return 1
    else:
        return 0
    
def DDR_Read(address, shape, dtype):
    data_size = len(np.zeros(shape=shape, dtype=dtype).tobytes())
    res = bytearray(data_size)
    uint8_handle = _DDR_Read(c2hx, ctypes.c_uint64(address), data_size)
    rptr = (ctypes.c_byte * data_size).from_buffer(res)
    if not ctypes.memmove(rptr, uint8_handle, data_size):
        raise RuntimeError("memmove faild")
    data = np.frombuffer(res, dtype=dtype).reshape(shape)
    return data

address = 0x250000000
CSB_Write(2, 10)
print(CSB_Read(2))
data = np.random.randn(3, 3).astype("float16")
DDR_Write(data, address)
result = DDR_Read(address, [3, 3], "float16")
print(data)
print(result)

# test = np.array([0.1, 0.2, 0.3, 0.4], dtype="float16")
# address = 0x250000000
# btest = test.tobytes()
# data_size = len(btest)
# brpt = (ctypes.c_byte * data_size).from_buffer(bytearray(btest))
# trpt = ctypes.cast(brpt, ctypes.POINTER(ctypes.c_uint8))
# tp_r0 = _DDR_Write(h2cx, trpt, ctypes.c_uint64(address), data_size)
# print(tp_r0)

# res = bytearray(data_size)
# uint8_handle = DDR_Read(c2hx, ctypes.c_uint64(address), data_size)
# rptr = (ctypes.c_byte * data_size).from_buffer(res)
# if not ctypes.memmove(rptr, uint8_handle, data_size):
#     raise RuntimeError("memmove faild")
# data = np.frombuffer(res, dtype="float16")
# print(data)