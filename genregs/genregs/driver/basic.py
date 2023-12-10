from .fpga_ddr import FPGA_Malloc
from ..ne import Expr
from .register import register_driver

Tb = 1
HBM_Port = 32
base_Tin = 128
Tout = 32
log2_Tout = 5
MAX_DAT_DW = 16
MAX_WT_DW = 4
MAX_BN_DW = 16
T_quant_block = 128
HBM_AXI_DATA_WIDTH = 256
WT_quant_scale_DW = 16
DAT_BRAM_NUM = 1
HBM_Port = 32
WT_BRAM_NUM = HBM_Port
log2_AXI_BURST_LEN = log2_Tout
WT_CH_Tgroup = T_quant_block*HBM_AXI_DATA_WIDTH//WT_quant_scale_DW
DAT_BRAM_DEPTH = (1<<20)//base_Tin//MAX_DAT_DW//DAT_BRAM_NUM
WT_BRAM_DEPTH = (1<<25)//HBM_AXI_DATA_WIDTH//WT_BRAM_NUM
AXI_DAT_WIDTH = MAX_DAT_DW*Tout*Tb
Pixel_Data_Width = AXI_DAT_WIDTH
Pixel_Data_Bytes = int((AXI_DAT_WIDTH)>>3)
AXI_BURST_LEN_SOFTMAX = 4


def CSB_Write(tag, addr, value):
    if not isinstance(value, Expr):
        value = value % (1 << 32)
    tag.reg_ops.append([1, addr, value])


def CSB_Read(tag, addr, target):
    tag.reg_ops.append([0, addr, target])


def CSB_For(expr, tag):
    tag.reg_ops.append(expr)


def CSB_End(expr, tag):
    tag.reg_ops.append([-1, 0, 0])


class Mapped_Feature:

    def __init__(self):
        self.payload = None
        self.payload_size = None
        self.surface_stride = None
        self.line_stride = None
        self.scale = None
        self.conv_out_scale = None
        self.height = None
        self.width = None
        self.channel = None
        self.dat_bit = None


class Mapped_Weight:

    def __init__(self):
        self.payload = None
        self.payload_size = None
        self.scale = None
        self.Ky = None
        self.Kx = None
        self.in_ch = None
        self.out_ch = None
        self.wt_bit = None


@register_driver("hbm", "Malloc_Feature", "1126")
def Malloc_Feature(height, width, ch, scale=0, conv_out_scale=0, dat_bit=16):
    ret = Mapped_Feature()
    ret.scale = scale
    ret.height = height
    ret.width = width
    ret.channel = ch
    ret.dat_bit = dat_bit
    ret.conv_out_scale = conv_out_scale

    ret.line_stride = width * (MAX_DAT_DW // 8) * Tout
    ret.surface_stride = ret.line_stride * height

    require_bytes = ret.surface_stride * ((ch + Tout - 1) // Tout)

    if isinstance(require_bytes, Expr):
        require_bytes = require_bytes.simplify(1).data
    ret.payload_size = require_bytes
    ret.payload = FPGA_Malloc(require_bytes)
    return ret


@register_driver("hbm", "Malloc_Weight", "1126")
def Malloc_Weight(Ky, Kx, in_ch, out_ch, scale=0, wt_bit=4):
    ret = Mapped_Weight()
    ret.scale = scale
    ret.Ky = Ky
    ret.Kx = Kx
    ret.in_ch = in_ch
    ret.out_ch = out_ch
    ret.wt_bit = wt_bit

    require_bytes = ((base_Tin * wt_bit) // 8) * Kx * Ky * out_ch * ((in_ch + base_Tin - 1) // base_Tin)

    if isinstance(require_bytes, Expr):
        require_bytes = require_bytes.simplify(1).data
    ret.payload_size = require_bytes
    ret.payload = FPGA_Malloc(require_bytes)
    return ret
