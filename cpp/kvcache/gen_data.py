import os
import torch
import numpy as np
import math

def out_txt_1(data_pt, txt_name):
    data = data_pt.detach().cpu().numpy()
    shape = data.shape

    filename = os.path.join('out_txt', txt_name)
    with open(filename, 'w') as file:
        shape_line = ' '.join(map(str, data.shape))
        file.write(f'{shape_line}\n')
        np.savetxt(file, data.flatten(), delimiter=',', newline='\n')

def out_bin(data_pt, bin_name):
    if data_pt.dtype == torch.float32:
        data_pt = data_pt.to(torch.float16)

    with open(os.path.join('out_txt', bin_name), 'wb') as f:
        f.write(data_pt.detach().cpu().numpy().flatten().tobytes())

def head_chout_chin_2_chout_head_chin(data_pt_src, head, chout, chin):
    data_pt = data_pt_src.reshape((head, chout, chin))
    data_pt = data_pt.permute(1, 0, 2)
    return data_pt

def out_txt(data_pt_src, txt_name, is_out=False, is_transpose=False):
    if is_transpose:
        data_pt = data_pt_src.transpose(0, 1)
    else:
        data_pt = data_pt_src

    data = data_pt.detach().cpu().numpy()
    shape = data.shape

    # print(txt_name, shape, data.mean(), data_pt.dtype)
    if False:
        out_txt_1(data_pt, txt_name)
        out_bin(data_pt, txt_name.replace('.txt', '.bin'))


def scaled_dot_product_attention(query, key, value, attn_mask=None, dropout_p=0.0, is_causal=True, scale=None) -> torch.Tensor:
    # Efficient implementation equivalent to the following:
    L, S = query.size(-2), key.size(-2)
    scale_factor = 1 / math.sqrt(query.size(-1)) if scale is None else scale
    attn_bias = torch.zeros(L, S, dtype=query.dtype).to('cuda')
    if is_causal:
        assert attn_mask is None
        temp_mask = torch.ones(L, S, dtype=torch.bool).tril(diagonal=0).to('cuda')
        attn_bias.masked_fill_(temp_mask.logical_not(), float("-inf"))
        attn_bias.to(query.dtype)

    if attn_mask is not None:
        if attn_mask.dtype == torch.bool:
            attn_mask.masked_fill_(attn_mask.logical_not(), float("-inf"))
        else:
            attn_bias += attn_mask
    attn_weight = query @ key.transpose(-2, -1)

    scores0 = attn_weight.clone()

    attn_weight = attn_weight * scale_factor

    scores1 = attn_weight.clone()
    Mask = attn_bias.clone()
    rsqrt_dk = scale_factor

    attn_weight += attn_bias

    scores1_5 = attn_weight.clone()

    attn_weight = torch.softmax(attn_weight, dim=-1)

    scores2 = attn_weight.clone()

    return attn_weight @ value, scores0, scores1, Mask, rsqrt_dk, scores2, scores1_5


query = torch.randn(19, 1, 32, 128).to(torch.float16)
key = torch.randn(19, 1, 2, 128).to(torch.float16)
value = torch.randn(19, 1, 2, 128).to(torch.float16)

key = key.unsqueeze(-2)
key = key.expand(-1, -1, -1, 32//2, -1)
key = key.contiguous().view(key.size()[:2] + (32, 128))
value = value.unsqueeze(-2)
value = value.expand(-1, -1, -1, 32//2, -1)
value = value.contiguous().view(value.size()[:2] + (32, 128))
print(value.shape)
query_layer, key_layer, value_layer = [k.permute(1, 2, 0, 3) for k in [query, key, value]]
is_causal_bool = (query_layer.shape[2] == key_layer.shape[2])
context_layer, scores0, scores1, Mask, rsqrt_dk, scores2, scores1_5 = scaled_dot_product_attention(query_layer, key_layer, value_layer)
blockname = "test"
out_txt(scores0, blockname + f"scores0.txt", is_out=True)
out_txt(scores1_5, blockname + f"scores1_5.txt", is_out=True)
out_txt(scores1_5[0][0], blockname + f"scores1_5_index0.txt", is_out=True)
out_txt(scores1, blockname + f"scores1.txt", is_out=True)

# print('rsqrt_dk', rsqrt_dk)
out_txt(scores2, blockname + f"scores2.txt", is_out=True)
out_txt(context_layer, blockname + f"Attention_output.txt", is_out=True)