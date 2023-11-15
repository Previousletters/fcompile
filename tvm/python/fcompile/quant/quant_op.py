import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.autograd import Function


def _try_get_del(d, k, default_v=None):
    if k in d:
        ret = d[k]
        del d[k]
        return ret
    else:
        return default_v


class Quant(Function):

    @staticmethod
    def forward(ctx, tensor, bit_width, scale_p):
        if tensor == None:
            return tensor
        ori_data = tensor.data.clone()
        scale = ori_data.contiguous().view((-1,)).abs_()
        scale = scale.sort()[0][int(0.999*scale.shape[-1])]
        scale = (bit_width-1) - torch.ceil_(torch.log2_(scale))
        scale_p[0] = scale.type(torch.int32)
        data_range = torch.pow(torch.tensor(2.0), (bit_width-1)-scale)
        data_step = torch.pow(torch.tensor(2.0), -scale)
        tensor.clamp_(-data_range, data_range-data_step)
        del ori_data
        return torch.round_(tensor/data_step)*data_step

    @staticmethod
    def backward(ctx, grad):
        return grad, None, None

    @staticmethod
    def symbolic(g, tensor, bit_width, scale):
        g_bit_width = g.op("Constant", value_t = torch.tensor((bit_width), dtype=torch.int32))
        g_scale = g.op("Constant", value_t = scale[0])
        return g.op("custom::Quant", tensor, g_bit_width, g_scale)


class Dequant(Function):

    @staticmethod
    def forward(ctx, tensor, bit_width, scale_p):
        ori_data = tensor.data.clone()
        scale = ori_data.contiguous().view((-1,)).abs_()
        scale = scale.sort()[0][int(0.999*scale.shape[-1])]
        scale = (bit_width-1) - torch.ceil_(torch.log2_(scale))
        scale_p[0] = scale.type(torch.int32)
        data_range = torch.pow(torch.tensor(2.0), (bit_width-1)-scale)
        data_step = torch.pow(torch.tensor(2.0), -scale)
        tensor.clamp_(-data_range, data_range-data_step)
        del ori_data
        return torch.round_(tensor/data_step)*data_step

    @staticmethod
    def backward(ctx, grad):
        return grad, None, None

    @staticmethod
    def symbolic(g, tensor, bit_width, scale_p):
        g_bit_width = g.op("Constant", value_t = torch.tensor((bit_width), dtype=torch.int32))
        #g_bit_width = g.op("Attribute", value_t = bit_width)
        g_scale = g.op("Constant", value_t = scale_p[0])
        return g.op("custom::Dequant", tensor, g_bit_width, g_scale)


class Quantize(nn.Module):

    def __init__(self, *args, **kwargs):
        self.bit_width = _try_get_del(kwargs, "bit_width", 8)
        self.scale = _try_get_del(kwargs, "scale", [10])
        super(Quantize, self).__init__()
    
    def forward(self, input):
        return Quant.apply(input, self.bit_width, self.scale)

class Dequantize(nn.Module):

    def __init__(self, *args, **kwargs):
        self.bit_width = _try_get_del(kwargs, "bit_width", 8)
        self.scale = _try_get_del(kwargs, "scale", [10])
        super(Dequantize, self).__init__()
    
    def forward(self, input):
        return Dequant.apply(input, self.bit_width, self.scale)

#from torch.onnx import register_custom_op_symbolic 
#from torch.onnx.symbolic_helper import parse_args 
#
#@parse_args("v", "i", "i") 
#def quant_symbolic(g, input, bit_width, scale): 
#    kwargs = {
#        "bit_width": bit_width,
#        "scale": scale,
#    }
#    return g.op("custom::quant", input, outputs=1, **kwargs) 
#
#@parse_args("v", "i", "i") 
#def dequant_symbolic(g, input, bit_width, scale): 
#    kwargs = {
#        "bit_width": bit_width,
#        "scale": scale,
#    }
#    return g.op("custom::dequant", input, outputs=1, **kwargs) 
# 
# 
#register_custom_op_symbolic("torchcustom::Quant", quant_symbolic, 9) 
#register_custom_op_symbolic("torchcustom::Dequant", dequant_symbolic, 9) 
