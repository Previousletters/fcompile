import torch as t
import torch.nn.functional as F
from lsq import LsqQuan,LsqQuanSRP,LsqQuanSparse,LsqQuanSparseSRP, grad_scale, floor_pass, round_pass, channel_fine_grained_prune_mask_TIN8


class WrapQuanSparseConv2d(t.autograd.Function):
    @staticmethod
    def forward(ctx, x, weight, quan_w_fn, quan_a_fn, stride, padding, dilation, groups):
        s_grad_scale = 1.0 / ((quan_a_fn.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(quan_a_fn.s.data, s_grad_scale)
        final_scale = s_scale.clone()
        final_scale = floor_pass(final_scale.log2())
        quan_a_fn.final_scale = final_scale

        s_grad_scale = 1.0 / ((quan_w_fn.thd_pos * weight.numel()) ** 0.5)
        s_scale = grad_scale(quan_w_fn.s.data, s_grad_scale)
        final_scale = s_scale.clone()
        final_scale = floor_pass(final_scale.log2())
        quan_w_fn.final_scale = final_scale
        s_scale = s_scale*0 + (2 ** final_scale)
        # s_scale = round_pass(final_scale)
        weight = weight / s_scale
        weight = t.clamp(weight, quan_w_fn.thd_neg, quan_w_fn.thd_pos)
        weight = round_pass(weight)

        quan_w_fn.mask = channel_fine_grained_prune_mask_TIN8(weight, quan_w_fn.sparsity)
        # self.mask = tensor_fine_grained_prune_mask(x, self.sparsity)
        #     self._mask_set_flag_ = True
        return F.conv2d(input=x, weight=weight, bias=None, stride=stride, padding=padding, dilation=dilation, groups=groups)

    @staticmethod
    def symbolic(g, x, weight, quan_w_fn, quan_a_fn, stride, padding, dilation, groups):
        mask = t.where(quan_w_fn.mask, 1, 0)
        return g.op("custom::QuanSparseConv2d", x, weight, mask_t=mask, sparsity_f=quan_w_fn.sparsity, bit_i=quan_w_fn.bit, scales_i=[-int(quan_w_fn.final_scale), -int(quan_a_fn.final_scale)], strides_i=stride, paddings_i=padding)


class WrapQuanSparseLinear(t.autograd.Function):
    @staticmethod
    def forward(ctx, x, weight, quan_w_fn, quan_a_fn):
        s_grad_scale = 1.0 / ((quan_a_fn.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(quan_a_fn.s.data, s_grad_scale)
        final_scale = s_scale.clone()
        final_scale = floor_pass(final_scale.log2())
        quan_a_fn.final_scale = final_scale

        s_grad_scale = 1.0 / ((quan_w_fn.thd_pos * weight.numel()) ** 0.5)
        s_scale = grad_scale(quan_w_fn.s.data, s_grad_scale)
        final_scale = s_scale.clone()
        final_scale = floor_pass(final_scale.log2())
        quan_w_fn.final_scale = final_scale
        s_scale = s_scale*0 + (2 ** final_scale)
        # s_scale = round_pass(final_scale)
        weight = weight / s_scale
        weight = t.clamp(weight, quan_w_fn.thd_neg, quan_w_fn.thd_pos)
        weight = round_pass(weight)

        quan_w_fn.mask = channel_fine_grained_prune_mask_TIN8(weight, quan_w_fn.sparsity)
        # self.mask = tensor_fine_grained_prune_mask(x, self.sparsity)
        #     self._mask_set_flag_ = True
        return t.nn.functional.linear(x, weight, None)

    @staticmethod
    def symbolic(g, x, weight, quan_w_fn, quan_a_fn):
        mask = t.where(quan_w_fn.mask, 1, 0)
        return g.op("custom::QuanSparseConv2d", x, weight, mask_t=mask, sparsity_f=quan_w_fn.sparsity, bit_i=quan_w_fn.bit, scales_i=[-int(quan_w_fn.final_scale), -int(quan_a_fn.final_scale)])


class QuanSparseConv2d(t.nn.Conv2d):
    def __init__(self, in_channels, out_channels, kernel_size, sparsity=0.0,stride=1,padding=0,dilation=1,groups=1,bias=False,padding_mode='zeros',weight_bit_width=8,act_bit_width=8,bias_bit_width=8, export_onnx=False):
        super().__init__(in_channels, out_channels, kernel_size,
                         stride=stride,
                         padding=padding,
                         dilation=dilation,
                         groups=groups,
                         bias=bias,
                         padding_mode=padding_mode)
        self.quan_w_fn = LsqQuanSparse(bit=weight_bit_width,weight= self.weight,sparsity=sparsity, per_channel=False)
        self.quan_a_fn = LsqQuan(bit=act_bit_width, per_channel=False)
        self.use_bias = bias
        self.sparsity = sparsity
        self.weight_bit_width = weight_bit_width

        # self.weight = t.nn.Parameter(m.weight.detach())
        self.quan_w_fn.init_from(self.weight)
        if bias:
            self.quan_b_fn = LsqQuan(bit=bias_bit_width, per_channel=False)
            self.bias = t.nn.Parameter(self.bias)
        else:
            self.bias = None
            self.quan_b_fn = None
            # self.bias = t.nn.Parameter(bias)
        self.size = self._calculate_coded_complexity_MB()
        self.export_onnx = export_onnx

    def forward(self, x):
        quantized_weight = self.quan_w_fn(self.weight)
        quantized_act = self.quan_a_fn(x)
        if(self.use_bias):
            quantized_bias = self.quan_b_fn(self.bias)
        else:
            quantized_bias = None
        return self._conv_forward(quantized_act, quantized_weight,quantized_bias)

    def _init_weight(self):
        self.quan_w_fn.init_from(self.weight)
        if self.use_bias:
            self.quan_b_fn.init_from(self.bias)
    def _reset_mask(self):
        self.quan_w_fn._reset_mask()
    def _unset_mask(self):
        self.quan_w_fn._unset_mask()
    def _set_mask(self):
        self.quan_w_fn._set_mask(self.weight)
    def _calculate_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        return self.weight.numel() * self.weight_bit_width/8*(1-self.sparsity)
    def _calculate_coded_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        if(self.sparsity==0):
            return self._calculate_complexity()
        else:
            ch = self.weight.shape[1]
            if(ch%8!=0):
                ch = ch + 8 - ch%8
                print(ch)
            return self._calculate_complexity()+self.weight.numel()/self.weight.shape[1]*ch /8
    def _calculate_coded_complexity_MB(self):
        return self._calculate_coded_complexity()/1024/1024

class QuanSparseConv2dSRP(t.nn.Conv2d):
    def __init__(self, in_channels, out_channels, kernel_size, sparsity=0.0,stride=1,padding=0,dilation=1,groups=1,bias=True,padding_mode='zeros',bit_width=8, export_onnx=False):
        super().__init__(in_channels, out_channels, kernel_size,
                         stride=stride,
                         padding=padding,
                         dilation=dilation,
                         groups=groups,
                         bias=bias,
                         padding_mode=padding_mode)
        self.quan_w_fn = LsqQuanSparseSRP(bit=bit_width,weight= self.weight,sparsity=sparsity, per_channel=False)
        self.quan_a_fn = LsqQuanSRP(bit=bit_width, per_channel=False)
        self.use_bias = bias
        self.sparsity = sparsity
        self.weight_bit_width = bit_width
        self.export_onnx = export_onnx
        print("bit_width: ",self.weight_bit_width,", sparsity",self.sparsity)

        # self.weight = t.nn.Parameter(m.weight.detach())
        self.quan_w_fn.init_from(self.weight)
        if bias:
            self.quan_b_fn = LsqQuanSRP(bit=bit_width, per_channel=False)
            self.bias = t.nn.Parameter(self.bias)
        else:
            self.bias = None
            self.quan_b_fn = None
            # self.bias = t.nn.Parameter(bias)
        self.size = self._calculate_coded_complexity_MB()

    def forward(self, x):
        return WrapQuanSparseConv2d.apply(x, self.weight, self.quan_w_fn, self.quan_a_fn, self.stride, self.padding, self.dilation, self.groups)
        '''
        if self.export_onnx:
            return WrapQuanSparseConv2d.apply(x, self.weight, self.quan_w_fn, self.quan_a_fn, self.stride, self.padding, self.dilation, self.groups)
        else:
            quantized_weight = self.quan_w_fn(self.weight)
            quantized_act = self.quan_a_fn(x)
            if(self.use_bias):
                quantized_bias = self.quan_b_fn(self.bias)
            else:
                quantized_bias = None
            return F.conv2d(input=quantized_act, weight=quantized_weight, bias=quantized_bias, stride=self.stride, padding=self.padding, dilation=self.dilation, groups=self.groups)
        '''
    def _init_weight(self):
        self.quan_w_fn.init_from(self.weight)
        if self.use_bias:
            self.quan_b_fn.init_from(self.bias)
    def _reset_mask(self):
        self.quan_w_fn._reset_mask()
    def _unset_mask(self):
        self.quan_w_fn._unset_mask()
    def _set_mask(self):
        self.quan_w_fn._set_mask(self.weight)
    def _calculate_complexity(self):
        # print(self.weight_bit_width,self.sparsity,self.weight.numel())
        return self.weight.numel() * self.weight_bit_width/8*(1-self.sparsity)
    def _calculate_coded_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        if(self.sparsity==0):
            return self._calculate_complexity()
        else:
            ch = self.weight.shape[1]
            if(ch%8!=0):
                ch = ch + 8 - ch%8
                print(ch)
            return self._calculate_complexity()+self.weight.numel()/self.weight.shape[1]*ch /8
    def _calculate_coded_complexity_MB(self):
        return self._calculate_coded_complexity()/1024/1024
    def _calculate_param_size_M(self):
        if self.use_bias:
            return self.weight.numel()*(1-self.sparsity)/1024/1024+self.bias.numel()/1024/1024
        else:
            return self.weight.numel()*(1-self.sparsity)/1024/1024

class QuanSparseLinear(t.nn.Linear):
    def __init__(self, in_features, out_features, sparsity=0.0, bias=True,weight_bit_width=8,act_bit_width=8,bias_bit_width=8):
        super().__init__(in_features, out_features, bias)
        self.quan_w_fn = LsqQuanSparse(bit=weight_bit_width, weight= self.weight,sparsity= sparsity, per_channel=False)
        self.quan_a_fn = LsqQuan(bit=act_bit_width, per_channel=False)
        self.use_bias = bias
        self.quan_w_fn.init_from(self.weight)
        self.sparsity = sparsity
        self.weight_bit_width = weight_bit_width

        if bias:
            self.quan_b_fn = LsqQuan(bit=bias_bit_width, per_channel=False)
            self.quan_b_fn.init_from(self.bias)
        else:
            self.bias = None
            self.quan_b_fn = None
        self.size = self._calculate_coded_complexity_MB()

    def forward(self, x):
        quantized_weight = self.quan_w_fn(self.weight)
        quantized_act = self.quan_a_fn(x)
        if(self.use_bias):
            quantized_bias = self.quan_b_fn(self.bias)
        else:
            quantized_bias = None
        return t.nn.functional.linear(quantized_act, quantized_weight, quantized_bias)

    def _init_weight(self):
        self.quan_w_fn.init_from(self.weight)
        if self.use_bias:
            self.quan_b_fn.init_from(self.bias)
    def _reset_mask(self):
        self.quan_w_fn._reset_mask()
    def _unset_mask(self):
        self.quan_w_fn._unset_mask()
    def _set_mask(self):
        self.quan_w_fn._set_mask(self.weight)
    def _calculate_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        return self.weight.numel() * self.weight_bit_width/8*(1-self.sparsity)
    def _calculate_coded_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        if(self.sparsity==0):
            return self._calculate_complexity()
        else:
            ch = self.weight.shape[1]
            if(ch%8!=0):
                ch = ch + 8 - ch%8
                print(ch)
            return self._calculate_complexity()+self.weight.numel()/self.weight.shape[1]*ch /8
    def _calculate_coded_complexity_MB(self):
        return self._calculate_coded_complexity()/1024/1024

class QuanSparseLinearSRP(t.nn.Linear):
    def __init__(self, in_features, out_features, sparsity=0.0, bias=True,bit_width=8, export_onnx=False):
        super().__init__(in_features, out_features, bias)
        self.quan_w_fn = LsqQuanSparseSRP(bit=bit_width, weight= self.weight,sparsity= sparsity, per_channel=False)
        self.quan_a_fn = LsqQuanSRP(bit=bit_width, per_channel=False)
        self.use_bias = bias
        self.quan_w_fn.init_from(self.weight)
        self.sparsity = sparsity
        self.weight_bit_width = bit_width
        print("bit_width: ",self.weight_bit_width,", sparsity",self.sparsity)

        if bias:
            self.quan_b_fn = LsqQuanSRP(bit=bit_width, per_channel=False)
            self.quan_b_fn.init_from(self.bias)
        else:
            self.bias = None
            self.quan_b_fn = None
        self.size = self._calculate_coded_complexity_MB()

    def forward(self, x):
        return WrapQuanSparseLinear.apply(x, self.weight, self.quan_w_fn, self.quan_a_fn)
        # return t.nn.functional.linear(x, self.weight, None)
        '''
            quantized_weight = self.quan_w_fn(self.weight)
            quantized_act = self.quan_a_fn(x)
            if(self.use_bias):
                quantized_bias = self.quan_b_fn(self.bias)
            else:
                quantized_bias = None
            return t.nn.functional.linear(quantized_act, quantized_weight, quantized_bias)
        '''

    def _init_weight(self):
        self.quan_w_fn.init_from(self.weight)
        if self.use_bias:
            self.quan_b_fn.init_from(self.bias)
    def _reset_mask(self):
        self.quan_w_fn._reset_mask()
    def _unset_mask(self):
        self.quan_w_fn._unset_mask()
    def _set_mask(self):
        self.quan_w_fn._set_mask(self.weight)
    def _calculate_complexity(self):
        # print(self.weight_bit_width,self.sparsity,self.weight.numel())
        return self.weight.numel() * self.weight_bit_width/8*(1-self.sparsity)
    def _calculate_coded_complexity(self):
        print(self.weight_bit_width,self.sparsity,self.weight.numel())
        if(self.sparsity==0):
            return self._calculate_complexity()
        else:
            ch = self.weight.shape[1]
            if(ch%8!=0):
                ch = ch + 8 - ch%8
                print(ch)
            return self._calculate_complexity()+self.weight.numel()/self.weight.shape[1]*ch /8
    def _calculate_coded_complexity_MB(self):
        return self._calculate_coded_complexity()/1024/1024
    def _calculate_param_size_M(self):
        if self.use_bias:
            return self.weight.numel()*(1-self.sparsity)/1024/1024+self.bias.numel()/1024/1024
        else:
            return self.weight.numel()*(1-self.sparsity)/1024/1024

# QuanModuleMapping = {
#     t.nn.Conv2d: QuanConv2d,
#     t.nn.Linear: QuanLinear
# }
