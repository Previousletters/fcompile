import torch as t

from quantizer import Quantizer
import math

def grad_scale(x, scale):
    y = x
    y_grad = x * scale
    return (y - y_grad) + y_grad


def round_pass(x):
    y = x.round()
    y_grad = x
    return (y - y_grad) + y_grad


def floor_pass(x):
    y = x.floor()
    y_grad = x
    return (y - y_grad) + y_grad


class LsqQuan(Quantizer):
    def __init__(self, bit, all_positive=False, symmetric=False, per_channel=True):
        super().__init__(bit)

        if all_positive:
            assert not symmetric, "Positive quantization cannot be symmetric"
            # unsigned activation is quantized to [0, 2^b-1]
            self.thd_neg = 0
            self.thd_pos = 2 ** bit - 1
        else:
            if symmetric:
                # signed weight/activation is quantized to [-2^(b-1)+1, 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1) + 1
                self.thd_pos = 2 ** (bit - 1) - 1
            else:
                # signed weight/activation is quantized to [-2^(b-1), 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1)
                self.thd_pos = 2 ** (bit - 1) - 1

        self.per_channel = per_channel
        self.s = t.nn.Parameter(t.ones([]))

    def init_from(self, x, *args, **kwargs):
        if self.per_channel:
            self.s = t.nn.Parameter(
                x.detach().abs().mean(dim=list(range(1, x.dim())), keepdim=True) * 2 / (self.thd_pos ** 0.5))
        else:
            self.s = t.nn.Parameter(x.detach().abs().mean() * 2 / (self.thd_pos ** 0.5))

    def forward(self, x):
        if self.per_channel:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        else:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(self.s, s_grad_scale)
        final_scale = s_scale.clone()
        final_scale = t.floor(final_scale.log2())
        s_scale = s_scale*0 +  (2 ** final_scale)
        self.final_scale = final_scale
        x = x / s_scale
        x = t.clamp(x, self.thd_neg, self.thd_pos)
        x = round_pass(x)
        x = x * s_scale
        return x


class LsqQuanSparse(Quantizer):
    def __init__(self, bit, sparsity=0.5, weight:t.Tensor=None,all_positive=False, symmetric=False, per_channel=True):
        super().__init__(bit)

        if all_positive:
            assert not symmetric, "Positive quantization cannot be symmetric"
            # unsigned activation is quantized to [0, 2^b-1]
            self.thd_neg = 0
            self.thd_pos = 2 ** bit - 1
        else:
            if symmetric:
                # signed weight/activation is quantized to [-2^(b-1)+1, 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1) + 1
                self.thd_pos = 2 ** (bit - 1) - 1
            else:
                # signed weight/activation is quantized to [-2^(b-1), 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1)
                self.thd_pos = 2 ** (bit - 1) - 1

        self.per_channel = per_channel
        self.s = t.nn.Parameter(t.ones([]))
        self._mask_set_flag_ = False
        self.register_buffer('mask',t.zeros_like(weight))
        self.sparsity = sparsity
        print("sparsity is: ",self.sparsity)

    def init_from(self, x, *args, **kwargs):
        if self.per_channel:
            self.s = t.nn.Parameter(
                x.detach().abs().mean(dim=list(range(1, x.dim())), keepdim=True) * 2 / (self.thd_pos ** 0.5))
        else:
            self.s = t.nn.Parameter(x.detach().abs().mean() * 2 / (self.thd_pos ** 0.5))

    def forward(self, x):
        if self.per_channel:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        else:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(self.s, s_grad_scale)
        final_scale = s_scale.clone()
        self.final_scale = t.floor(final_scale.log2())
        s_scale = s_scale*0 +  (2 ** self.final_scale)
        x = x / s_scale
        x = t.clamp(x, self.thd_neg, self.thd_pos)
        x = round_pass(x)
        # if not self._mask_set_flag_:
        #     p_n = self._calculate_zeros(x)
        #     print("original sparsity is: ",p_n)
        self.mask = channel_fine_grained_prune_mask_TIN8(x, self.sparsity)
        #     self._mask_set_flag_ = True
        x.mul_(self.mask)
        x = x * s_scale
        return x
    def _reset_mask(self):
        self.mask.zero_()
        self._mask_set_flag_ = False
    
    def _calculate_zeros(self,tensor:t.Tensor):
        return (tensor.numel() - tensor.nonzero().shape[0])/tensor.numel()
    
    def _unset_mask(self):
        self._mask_set_flag_ = True

    def _set_mask(self,tensor:t.Tensor):
        ptensor = tensor.clone().detach()
        final_scale = self.s.clone().detach()
        final_scale = t.floor(final_scale.log2())
        s_scale = (2 ** final_scale)
        ptensor = ptensor / s_scale
        ptensor = t.clamp(ptensor, self.thd_neg, self.thd_pos)
        ptensor = round_pass(ptensor)
        self.mask = channel_fine_grained_prune_mask_TIN8(ptensor, self.sparsity)


class LsqQuanSRP(Quantizer):
    def __init__(self, bit, all_positive=False, symmetric=False, per_channel=True):
        super().__init__(bit)

        if all_positive:
            assert not symmetric, "Positive quantization cannot be symmetric"
            # unsigned activation is quantized to [0, 2^b-1]
            self.thd_neg = 0
            self.thd_pos = 2 ** bit - 1
        else:
            if symmetric:
                # signed weight/activation is quantized to [-2^(b-1)+1, 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1) + 1
                self.thd_pos = 2 ** (bit - 1) - 1
            else:
                # signed weight/activation is quantized to [-2^(b-1), 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1)
                self.thd_pos = 2 ** (bit - 1) - 1

        self.per_channel = per_channel
        self.s = t.nn.Parameter(t.ones([]))

    def init_from(self, x, *args, **kwargs):
        if self.per_channel:
            self.s = t.nn.Parameter(
                x.detach().abs().mean(dim=list(range(1, x.dim())), keepdim=True) * 2 / (self.thd_pos ** 0.5))
        else:
            self.s = t.nn.Parameter(x.detach().abs().mean() * 2 / (self.thd_pos ** 0.5))

    def forward(self, x):
        if self.per_channel:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        else:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(self.s, s_grad_scale)
        final_scale = s_scale.clone()
        # final_scale = t.floor(final_scale.log2())
        final_scale = floor_pass(final_scale.log2())
        self.final_scale = final_scale
        s_scale = s_scale*0 +  (2 ** final_scale)
        # s_scale = round_pass(final_scale)
        x = x / s_scale
        x = t.clamp(x, self.thd_neg, self.thd_pos)
        x = round_pass(x)
        x = x * s_scale
        return x

class LsqQuanSparseSRP(Quantizer):
    def __init__(self, bit, sparsity=0.5, weight:t.Tensor=None,all_positive=False, symmetric=False, per_channel=True):
        super().__init__(bit)

        if all_positive:
            assert not symmetric, "Positive quantization cannot be symmetric"
            # unsigned activation is quantized to [0, 2^b-1]
            self.thd_neg = 0
            self.thd_pos = 2 ** bit - 1
        else:
            if symmetric:
                # signed weight/activation is quantized to [-2^(b-1)+1, 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1) + 1
                self.thd_pos = 2 ** (bit - 1) - 1
            else:
                # signed weight/activation is quantized to [-2^(b-1), 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1)
                self.thd_pos = 2 ** (bit - 1) - 1

        self.per_channel = per_channel
        self.s = t.nn.Parameter(t.ones([]))
        self._mask_set_flag_ = False
        self.register_buffer('mask',t.zeros_like(weight))
        self.sparsity = sparsity
        print("sparsity is: ",self.sparsity)

    def init_from(self, x, *args, **kwargs):
        if self.per_channel:
            self.s = t.nn.Parameter(
                x.detach().abs().mean(dim=list(range(1, x.dim())), keepdim=True) * 2 / (self.thd_pos ** 0.5))
        else:
            self.s = t.nn.Parameter(x.detach().abs().mean() * 2 / (self.thd_pos ** 0.5))

    def forward(self, x):
        if self.per_channel:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        else:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(self.s, s_grad_scale)
        final_scale = s_scale.clone()
        # final_scale = t.floor(final_scale.log2())
        final_scale = floor_pass(final_scale.log2())
        self.final_scale = final_scale
        s_scale = s_scale*0 +  (2 ** final_scale)
        # s_scale = round_pass(final_scale)
        x = x / s_scale
        x = t.clamp(x, self.thd_neg, self.thd_pos)
        x = round_pass(x)
        # if not self._mask_set_flag_:
        #     p_n = self._calculate_zeros(x)
        #     print("original sparsity is: ",p_n)
        self.mask = channel_fine_grained_prune_mask_TIN8(x, self.sparsity)
        # self.mask = tensor_fine_grained_prune_mask(x, self.sparsity)
        #     self._mask_set_flag_ = True
        x.mul_(self.mask)
        x = x * s_scale
        return x
    def _reset_mask(self):
        self.mask.zero_()
        self._mask_set_flag_ = False
    
    def _calculate_zeros(self,tensor:t.Tensor):
        return (tensor.numel() - tensor.nonzero().shape[0])/tensor.numel()
    
    def _unset_mask(self):
        self._mask_set_flag_ = True

    def _set_mask(self,tensor:t.Tensor):
        ptensor = tensor.clone().detach()
        final_scale = self.s.clone().detach()
        final_scale = t.floor(final_scale.log2())
        s_scale = (2 ** final_scale)
        ptensor = ptensor / s_scale
        ptensor = t.clamp(ptensor, self.thd_neg, self.thd_pos)
        ptensor = round_pass(ptensor)
        self.mask = channel_fine_grained_prune_mask_TIN8(ptensor, self.sparsity)



class LsqQuanSparseSRP_Kai(Quantizer):
    def __init__(self, bit, sparsity=0.5, weight:t.Tensor=None,all_positive=False, symmetric=False, per_channel=True):
        super().__init__(bit)

        if all_positive:
            assert not symmetric, "Positive quantization cannot be symmetric"
            # unsigned activation is quantized to [0, 2^b-1]
            self.thd_neg = 0
            self.thd_pos = 2 ** bit - 1
        else:
            if symmetric:
                # signed weight/activation is quantized to [-2^(b-1)+1, 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1) + 1
                self.thd_pos = 2 ** (bit - 1) - 1
            else:
                # signed weight/activation is quantized to [-2^(b-1), 2^(b-1)-1]
                self.thd_neg = - 2 ** (bit - 1)
                self.thd_pos = 2 ** (bit - 1) - 1

        self.per_channel = per_channel
        self.s = t.nn.Parameter(t.ones([]))
        self._mask_set_flag_ = False
        self.register_buffer('mask',t.zeros_like(weight))
        self.sparsity = sparsity
        print("sparsity is: ",self.sparsity)

    def init_from(self, x, *args, **kwargs):
        if self.per_channel:
            self.s = t.nn.Parameter(
                x.detach().abs().mean(dim=list(range(1, x.dim())), keepdim=True) * 2 / (self.thd_pos ** 0.5))
        else:
            self.s = t.nn.Parameter(x.detach().abs().mean() * 2 / (self.thd_pos ** 0.5))

    def forward(self, x):
        if self.per_channel:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        else:
            s_grad_scale = 1.0 / ((self.thd_pos * x.numel()) ** 0.5)
        s_scale = grad_scale(self.s, s_grad_scale)
        final_scale = s_scale.clone()
        # final_scale = t.floor(final_scale.log2())
        final_scale = floor_pass(final_scale.log2())
        s_scale = s_scale*0 +  (2 ** final_scale)
        # s_scale = round_pass(final_scale)
        x = x / s_scale
        x = t.clamp(x, self.thd_neg, self.thd_pos)
        x = round_pass(x)
        # if not self._mask_set_flag_:
        #     p_n = self._calculate_zeros(x)
        #     print("original sparsity is: ",p_n)
        self.mask = multi_channel_fine_grained_prune_mask_TIN8(x, self.sparsity)
        # self.mask = tensor_fine_grained_prune_mask(x, self.sparsity)
        #     self._mask_set_flag_ = True
        x.mul_(self.mask)
        x = x * s_scale
        return x
    def _reset_mask(self):
        self.mask.zero_()
        self._mask_set_flag_ = False
    
    def _calculate_zeros(self,tensor:t.Tensor):
        return (tensor.numel() - tensor.nonzero().shape[0])/tensor.numel()
    
    def _unset_mask(self):
        self._mask_set_flag_ = True

    def _set_mask(self,tensor:t.Tensor):
        ptensor = tensor.clone().detach()
        final_scale = self.s.clone().detach()
        final_scale = t.floor(final_scale.log2())
        s_scale = (2 ** final_scale)
        ptensor = ptensor / s_scale
        ptensor = t.clamp(ptensor, self.thd_neg, self.thd_pos)
        ptensor = round_pass(ptensor)
        self.mask = multi_channel_fine_grained_prune_mask_TIN8(ptensor, self.sparsity)





def tensor_fine_grained_prune_mask(tensor: t.Tensor, sparsity : float) -> t.Tensor:
    sparsity = min(max(0.0, sparsity), 1.0)
    if sparsity == 1.0:
        tensor.zero_()
        return t.zeros_like(tensor)
    elif sparsity == 0.0:
        return t.ones_like(tensor)
    assert tensor.dim() >= 2, "Only weights can be pruned"
    ptensor = tensor.data.clone()
    # # ptensor = quantize_for_mask(ptensor)
    # N, C, H, W = 0, 0, 0, 0
    # if tensor.dim() == 4:
    #     N, C, H, W = ptensor.shape
    #     try:
    #         from einops import rearrange
    #         ptensor = rearrange(ptensor, 'N C H W -> (N H W) C')
    #     except(ImportError):
    #         raise ImportError('Please install einops first')
    #         #TODO: add a function to replace einops
    # elif tensor.dim() == 2:
    #     pass
    # else:
    #     print("dim is not 2 or 4")
    #     raise NotImplementedError
    # num_elements = tensor[2].numel()
    # num_elements = ptensor.shape[1]
    num_elements = ptensor.data.numel()
    #-----------------------------------------------
    # dim_less = math.ceil(num_elements/TIN)*TIN-num_elements
    # dim_left = TIN-dim_less
    # ptensor_appended = t.cat([ptensor, t.zeros_like((ptensor.shape[0],dim_less))], dim=1)
    # r_times = math.floor(num_elements/TIN)
    #-----------------------------------------------
    # Step 1: calculate the #zeros (please use round())
    num_zeros = round(num_elements * sparsity)
    if num_zeros == 0:
        return t.ones_like(tensor)
    
    # Step 2: calculate the importance of weight
    importance = ptensor.abs()
    # Step 3: calculate the pruning threshold
    threshold = importance.view(-1).kthvalue(num_zeros).values
    # Step 4: get binary mask (1 for nonzeros, 0 for zeros)
    mask = t.gt(importance, threshold)
    return mask
        #------------------------------------------------
        # Step 5: apply mask to prune the tensor
        # tensor[channel].mul_(mask)
        #------------------------------------------------


# def channel_fine_grained_prune_mask_TIN8(tensor: t.Tensor, sparsity : float,TIN:int=8) -> t.Tensor:
#     # Original version, too slow!
#     # TIN = 8
#     device = tensor.device
#     from einops import rearrange
#     sparsity = min(max(0.0, sparsity), 1.0)
#     if sparsity == 1.0:
#         tensor.zero_()
#         return t.zeros_like(tensor)
#     elif sparsity == 0.0:
#         return t.ones_like(tensor)
#     assert tensor.dim() >= 2, "Only weights can be pruned"
#     ptensor = tensor.data.clone()
#     # ptensor = quantize_for_mask(ptensor)
#     N, C, H, W = 0, 0, 0, 0
#     if tensor.dim() == 4:
#         N, C, H, W = ptensor.shape
#         try:
#             from einops import rearrange
#             ptensor = rearrange(ptensor, 'N C H W -> (N H W) C')
#         except(ImportError):
#             raise ImportError('Please install einops first')
#             #TODO: add a function to replace einops
#     elif tensor.dim() == 2:
#         pass
#     else:
#         print("dim is not 2 or 4")
#         raise NotImplementedError
#     # num_elements = tensor[2].numel()
#     num_elements = ptensor.shape[1]
#     dim_less = math.ceil(num_elements/TIN)*TIN-num_elements
#     # dim_left = TIN-dim_less
#     ptensor_appended = t.cat([ptensor, t.zeros((ptensor.shape[0],dim_less),device=device)], dim=1)
#     r_times = math.ceil(num_elements/TIN)
#     ptensor_appended = rearrange(ptensor_appended, 'N (r c) -> (N r) c', r=r_times, c = TIN)
#     # Step 1: calculate the #zeros (please use round())
#     # num_zeros = round(num_elements * sparsity)
#     num_zeros = round(TIN * sparsity)
#     sub_masks = ()
#     if num_zeros == 0:
#         return t.ones_like(tensor)
    
#     for channel in ptensor_appended:
#         # Step 2: calculate the importance of weight
#         importance = channel.abs()
#         # Step 3: calculate the pruning threshold
#         threshold = importance.view(-1).kthvalue(num_zeros).values
#         # Step 4: get binary mask (1 for nonzeros, 0 for zeros)
#         mask = t.gt(importance, threshold)
#         sub_masks += (mask,)
#     mask = t.stack(sub_masks, dim=0)
#     mask = rearrange(mask, '(N r) c -> N (r c)', r=r_times, c = TIN)[:, :num_elements]
#     if tensor.dim() == 4:
#         try:
#             from einops import rearrange
#             mask = rearrange(mask, '(N H W) C -> N C H W', N=N, H=H, W=W)
#         except(ImportError):
#             raise ImportError('Please install einops first')
#             #TODO: add a function to replace einops
#     return mask
#         #------------------------------------------------
#         # Step 5: apply mask to prune the tensor
#         # tensor[channel].mul_(mask)
#         #------------------------------------------------





def channel_fine_grained_prune_mask_TIN8(tensor: t.Tensor, sparsity : float,TIN:int=8) -> t.Tensor:
    # TIN = 8
    device = tensor.device
    from einops import rearrange
    sparsity = min(max(0.0, sparsity), 1.0)
    if sparsity == 1.0:
        tensor.zero_()
        return t.zeros_like(tensor)
    elif sparsity == 0.0:
        return t.ones_like(tensor)
    assert tensor.dim() >= 2, "Only weights can be pruned"
    ptensor = tensor.data.clone()
    # ptensor = quantize_for_mask(ptensor)
    N, C, H, W = 0, 0, 0, 0
    if tensor.dim() == 4:
        N, C, H, W = ptensor.shape
        try:
            from einops import rearrange
            ptensor = rearrange(ptensor, 'N C H W -> (N H W) C')
        except(ImportError):
            raise ImportError('Please install einops first')
            #TODO: add a function to replace einops
    elif tensor.dim() == 2:
        pass
    else:
        print("dim is not 2 or 4")
        raise NotImplementedError
    # num_elements = tensor[2].numel()
    num_elements = ptensor.shape[1]
    dim_less = math.ceil(num_elements/TIN)*TIN-num_elements
    # dim_left = TIN-dim_less
    ptensor_appended = t.cat([ptensor, t.zeros((ptensor.shape[0],dim_less),device=device)], dim=1)
    r_times = math.ceil(num_elements/TIN)
    ptensor_appended = rearrange(ptensor_appended, 'N (r c) -> (N r) c', N=ptensor_appended.shape[0], r=r_times, c = TIN)
    # Step 1: calculate the #zeros (please use round())
    # num_zeros = round(num_elements * sparsity)
    num_zeros = round(TIN * sparsity)
    # sub_masks = ()
    if num_zeros == 0:
        return t.ones_like(tensor)
    

    ptensor_appended = ptensor_appended.abs()
    mask = t.zeros_like(ptensor_appended)
    thresholds = ptensor_appended.kthvalue(num_zeros,dim=1).values
    mask = ptensor_appended.gt(thresholds.unsqueeze(1))
    # for channel in ptensor_appended:
    #     # Step 2: calculate the importance of weight
    #     importance = channel.abs()
    #     # Step 3: calculate the pruning threshold
    #     threshold = importance.view(-1).kthvalue(num_zeros).values
    #     # Step 4: get binary mask (1 for nonzeros, 0 for zeros)
    #     mask = t.gt(importance, threshold)
    #     sub_masks += (mask,)
    # mask = t.stack(sub_masks, dim=0)
    mask = rearrange(mask, '(N r) c -> N (r c)',N=ptensor.shape[0], r=r_times, c = TIN)[:, :num_elements]
    if tensor.dim() == 4:
        try:
            from einops import rearrange
            mask = rearrange(mask, '(N H W) C -> N C H W', N=N, H=H, W=W)
        except(ImportError):
            raise ImportError('Please install einops first')
            #TODO: add a function to replace einops
    return mask
        #------------------------------------------------
        # Step 5: apply mask to prune the tensor
        # tensor[channel].mul_(mask)
        #------------------------------------------------



def multi_channel_fine_grained_prune_mask_TIN8(tensor: t.Tensor, sparsity : float,TIN:int=8,group_size=2) -> t.Tensor:
    # TIN = 8
    MULT_TIN = TIN*group_size
    device = tensor.device
    from einops import rearrange
    sparsity = min(max(0.0, sparsity), 1.0)
    if sparsity == 1.0:
        tensor.zero_()
        return t.zeros_like(tensor)
    elif sparsity == 0.0:
        return t.ones_like(tensor)
    assert tensor.dim() >= 2, "Only weights can be pruned"
    ptensor = tensor.data.clone()
    # ptensor = quantize_for_mask(ptensor)
    N, C, H, W = 0, 0, 0, 0
    if tensor.dim() == 4:
        N, C, H, W = ptensor.shape
        try:
            from einops import rearrange
            ptensor = rearrange(ptensor, 'N C H W -> (N H W) C')
        except(ImportError):
            raise ImportError('Please install einops first')
            #TODO: add a function to replace einops
    elif tensor.dim() == 2:
        pass
    else:
        print("dim is not 2 or 4")
        raise NotImplementedError
    # num_elements = tensor[2].numel()
    num_elements = ptensor.shape[1]
    dim_less = math.ceil(num_elements/MULT_TIN)*MULT_TIN-num_elements
    # r_times = math.ceil(num_elements/MULT_TIN)
    # dim_left = TIN-dim_less
    ptensor_appended = t.cat([ptensor, t.zeros((ptensor.shape[0],dim_less),device=device)], dim=1)
    r_times = math.ceil(num_elements/MULT_TIN)
    ptensor_appended = rearrange(ptensor_appended, 'N (r g c) -> (N r) g c', N=ptensor_appended.shape[0], r=r_times, c = TIN, g=group_size)
    # Step 1: calculate the #zeros (please use round())
    # num_zeros = round(num_elements * sparsity)
    num_zeros = round(TIN * sparsity)
    # sub_masks = ()
    if num_zeros == 0:
        return t.ones_like(tensor)
    # print('appended tensor is',ptensor_appended)
    ptensor_appended_norm = t.norm(ptensor_appended, p=2,dim=-2)
    # print('normed',ptensor_appended_norm)
    ptensor_appended_norm = ptensor_appended_norm.abs()
    mask = t.zeros_like(ptensor_appended_norm)
    thresholds = ptensor_appended_norm.kthvalue(num_zeros,dim=1).values
    mask = ptensor_appended_norm.gt(thresholds.unsqueeze(1))
    mask = t.cat([mask,mask],dim=-1)
    # for channel in ptensor_appended:
    #     # Step 2: calculate the importance of weight
    #     importance = channel.abs()
    #     # Step 3: calculate the pruning threshold
    #     threshold = importance.view(-1).kthvalue(num_zeros).values
    #     # Step 4: get binary mask (1 for nonzeros, 0 for zeros)
    #     mask = t.gt(importance, threshold)
    #     sub_masks += (mask,)
    # mask = t.stack(sub_masks, dim=0)
    mask = rearrange(mask, '(N r) c -> N (r c)',N=ptensor.shape[0], r=r_times, c = MULT_TIN)[:, :num_elements]
    if tensor.dim() == 4:
        try:
            from einops import rearrange
            mask = rearrange(mask, '(N H W) C -> N C H W', N=N, H=H, W=W)
        except(ImportError):
            raise ImportError('Please install einops first')
            #TODO: add a function to replace einops
    return mask
