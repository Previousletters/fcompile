import torch
import torch.nn as nn
import torch.nn.functional as F

from .quant_op import Quant, Dequant, _try_get_del


class Conv2d(nn.Conv2d):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.wscale = _try_get_del(kwargs, "scale_weight", [10])
        self.bscale = _try_get_del(kwargs, "scale_bias", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        super(Conv2d, self).__init__(*args, **kwargs)

    def forward(self, input):
        input_temp = Quant.apply(input.clone(), self.bwidth, self.bscale)
        weight_temp = Quant.apply(self.weight, self.bwidth, self.wscale)
        bias_temp = Quant.apply(self.bias, self.bwidth, self.bscale)
        output = self._conv_forward(input_temp, weight_temp, bias_temp)
        return Dequant.apply(output, self.bwidth, self.oscale)


class Linear(nn.Linear):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.wscale = _try_get_del(kwargs, "scale_weight", [10])
        self.bscale = _try_get_del(kwargs, "scale_bias", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        self.ignore = _try_get_del(kwargs, "ignore", 0)
        super(Linear, self).__init__(*args, **kwargs)

    def forward(self, input):
        input_temp = Quant.apply(input.clone(), self.bwidth, self.dscale)
        weight_temp = Quant.apply(self.weight, self.bwidth, self.wscale)
        bias_temp = Quant.apply(self.bias, self.bwidth, self.bscale)
        output = F.linear(input_temp, weight_temp, bias_temp)
        return Dequant.apply(output, self.bwidth, self.oscale)


class BatchMatMul(nn.Module):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.wscale = _try_get_del(kwargs, "scale_weight", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        super(BatchMatMul, self).__init__(*args, **kwargs)

    def forward(self, input0, input1):
        input_temp = Quant.apply(input0.clone(), self.bwidth, self.dscale)
        weight_temp = Quant.apply(input1.clone(), self.bwidth, self.wscale)
        output = torch.bmm(input_temp, weight_temp)
        return Dequant.apply(output, self.bwidth, self.oscale)


class Softmax(nn.Softmax):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        super(Softmax, self).__init__(*args, **kwargs)

    def forward(self, input):
        input_temp = Quant.apply(input.clone(), self.bwidth, self.dscale)
        output = F.softmax(input_temp, self.dim, _stacklevel=5)
        return Dequant.apply(output, self.bwidth, self.oscale)


class LayerNorm(nn.LayerNorm):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.wscale = _try_get_del(kwargs, "scale_weight", [10])
        self.bscale = _try_get_del(kwargs, "scale_bias", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        super(LayerNorm, self).__init__(*args, **kwargs)

    def forward(self, input):
        input_temp = Quant.apply(input.clone(), self.bwidth, self.dscale)
        weight_temp = Quant.apply(self.weight, self.bwidth, self.wscale)
        bias_temp = Quant.apply(self.bias, self.bwidth, self.bscale)
        output = F.layer_norm(input_temp, self.normalized_shape, weight_temp, bias_temp, self.eps)
        return Dequant.apply(output, self.bwidth, self.oscale)

class MatAdd(nn.Module):
    def __init__(self, *args, **kwargs):
        self.bwidth = _try_get_del(kwargs, "bit_width", 8)
        self.dscale = _try_get_del(kwargs, "scale", [10])
        self.wscale = _try_get_del(kwargs, "scale_weight", [10])
        self.oscale = _try_get_del(kwargs, "scale_out", [10])
        super(MatAdd, self).__init__(*args, **kwargs)

    def forward(self, input0, input1):
        input_temp = Quant.apply(input0.clone(), self.bwidth, self.dscale)
        weight_temp = Quant.apply(input1.clone(), self.bwidth, self.wscale)
        output = input_temp + weight_temp
        return Dequant.apply(output, self.bwidth, self.oscale)


'''
from torch.onnx.symbolic_registry import register_op 
'''
from torch.onnx.symbolic_helper import parse_args 

@parse_args("v", "is", "v", "v", "f", "i") 
def layer_norm(g, input, normalized_shape, weight, bias, eps, cudnn_enable):
    args = [input, weight, bias]
    return g.op("custom::LayerNorm", *args) 

@parse_args("v", "s") 
def gelu(g, input, approximate):
    args = [input]
    return g.op("custom::GELU", *args) 

'''
register_op("layer_norm", layer_norm, "", 13) 
register_op("gelu", gelu, "", 13) 
'''
