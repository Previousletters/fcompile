import os
import ctypes

class Mapped_Feature(ctypes.Structure):

    _fields_ = [
        ("payload", ctypes.c_uint32), 
        ("payload_prefix", ctypes.c_uint32), 
        ("payload_size", ctypes.c_int32), 
        ("surface_stride", ctypes.c_int32),
        ("line_stride", ctypes.c_int32),
        ("scale", ctypes.c_int32),
        ("conv_out_scale", ctypes.c_int32),
        ("height", ctypes.c_int32),
        ("width", ctypes.c_int32),
        ("channel", ctypes.c_int32),
        ("dat_bit", ctypes.c_int32),
    ]

class Mapped_Weight(ctypes.Structure):

    _fields_ = [
        ("payload", ctypes.c_uint32), 
        ("payload_prefix", ctypes.c_uint32), 
        ("payload_size", ctypes.c_int32), 
        ("scale", ctypes.c_int32),
        ("Ky", ctypes.c_int32),
        ("Kx", ctypes.c_int32),
        ("in_ch", ctypes.c_int32),
        ("out_ch", ctypes.c_int32),
        ("wt_bit", ctypes.c_int32),
    ]

class HBM_MVM_Cfg(ctypes.Structure):

    _fields_ = [
        ("CHin_Padding_with_Tin", ctypes.c_int32),
        ("Hin", ctypes.c_int32),
        ("Win", ctypes.c_int32),
        ("CHout_Padding_with_Tout", ctypes.c_int32),
        ("Hout", ctypes.c_int32),
        ("Wout", ctypes.c_int32),
        ("overlap", ctypes.c_int32),
        ("best_single_dat_banks", ctypes.c_int32),
        ("method", ctypes.c_int32),
        ("dat_num_per_row", ctypes.c_int32),
        ("wt_size_in_bytes", ctypes.c_int32),
        ("wt_num_div_Tin", ctypes.c_int32),
        ("Hout_Split_Times", ctypes.c_int32),
        ("Hout_first", ctypes.c_int32),
        ("Hout_middle", ctypes.c_int32),
        ("Hout_last", ctypes.c_int32),
        ("in_height_first", ctypes.c_int32),
        ("in_height_middle", ctypes.c_int32),
        ("in_height_last", ctypes.c_int32),
        ("CHout_Split_Times", ctypes.c_int32),
        ("out_ch_slice", ctypes.c_int32),
        ("out_ch_slice_last", ctypes.c_int32),
    ]

current_dir = os.path.dirname(os.path.abspath(__file__))
lib = ctypes.CDLL(os.path.join(current_dir, "..", "lib", "libhbm_driver.so"), ctypes.RTLD_GLOBAL)
Malloc_Feature, Malloc_Weight = lib.Malloc_Feature, lib.Malloc_Weight
Malloc_Feature.restype = (ctypes.POINTER(Mapped_Feature))
Malloc_Weight.restype = (ctypes.POINTER(Mapped_Weight))
Get_MVM_Cfg = lib.Get_MVM_Cfg
Get_MVM_Cfg.restype = (HBM_MVM_Cfg)

def FP32_to_FP20(fp32):
	return lib.FP32_to_FP20(ctypes.c_float(fp32))
