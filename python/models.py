import torch
from torch import nn
from math import sqrt

from fcompile.quant import nn as fqnn

class Attention(nn.Module):

    def __init__(self, dim, proj_drop=0.):
        super(Attention, self).__init__()
        self.q = fqnn.Linear(dim, dim, bias=False)
        self.k = fqnn.Linear(dim, dim, bias=False)
        self.v = fqnn.Linear(dim, dim, bias=False)
        self._norm_fact = 10
        self.q_kt = fqnn.BatchMatMul()
        self.softmax = fqnn.Softmax(dim=-1)
        self.qk_v = fqnn.BatchMatMul()
        self.proj_drop = nn.Dropout(proj_drop)

    def forward(self, x):
        Q = self.q(x)
        K = self.k(x)
        V = self.v(x)
        KT = torch.transpose(K, 1, 2)
        atten = self.q_kt(Q, KT) * self._norm_fact
        atten = self.softmax(atten)
        atten = self.qk_v(atten, V)
        atten = self.proj_drop(atten)
        return atten


class Mlp(nn.Module):
    """ MLP as used in Vision Transformer, MLP-Mixer and related networks
    """
    def __init__(self, in_features, hidden_features=None, out_features=None,  drop=0.):
        super().__init__()
        out_features = out_features or in_features
        hidden_features = hidden_features or in_features
        self.fc1 = fqnn.Linear(in_features, hidden_features, bias=False)
        self.act = nn.GELU()
        self.fc2 = fqnn.Linear(hidden_features, out_features, bias=False)
        self.drop = nn.Dropout(drop)

    def forward(self, x):
        x = self.fc1(x)
        x = self.act(x)
        x = self.drop(x)
        x = self.fc2(x)
        x = self.drop(x)
        return x


class Block(nn.Module):

    def __init__(self, dim, mlp_ratio=4., qkv_bias=False, drop=0., attn_drop=0., drop_path=0.):
        super().__init__()
        self.norm1 = fqnn.LayerNorm(dim)
        self.attn = Attention(dim, proj_drop=drop)
        self.res1 = fqnn.MatAdd()
        self.norm2 = fqnn.LayerNorm(dim)
        mlp_hidden_dim = int(dim * mlp_ratio)
        self.mlp = Mlp(in_features=dim, hidden_features=mlp_hidden_dim, drop=drop)
        self.res2 = fqnn.MatAdd()
        self._initialize_weights()

    def forward(self, x):
        x = self.attn(x)
        #x = self.res1(x, self.attn(self.norm1(x)))
        #x = self.res2(x, self.mlp(self.norm2(x)))
        return x

    def _initialize_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.normal_(m.weight, 0, 0.01)
                if m.bias is not None:
                    nn.init.normal_(m.bias, 0, 0.01)
            elif isinstance(m, nn.LayerNorm):
                nn.init.normal_(m.weight, 0, 0.01)
                nn.init.normal_(m.bias, 0, 0.01)
            elif isinstance(m, nn.Linear):
                nn.init.normal_(m.weight, 0, 0.01)
                if m.bias is not None:
                    nn.init.normal_(m.bias, 0, 0.01)

def onnx_export():
    name = "./test/vit_block.onnx"
    width = 8
    data = torch.randn(size=(1, 64, 64))
    model = Block(64)
    model.eval()
    torch.onnx.export(model, data, name, input_names=["data"], output_names=["output"])

import tvm
from tvm.relay import frontend
from tvm.relay import transform
import onnx
from fcompile.transform import transform as ftransform
from fcompile.fir import FModule
from fcompile.transform import RelayFIR, FPGAParameters, DataMap, FPGAJit
from fcompile.codegen import CCodeGen
import os


def compile():
    onnx_model = onnx.load("./test/vit_block.onnx")
    shape_dict = {"data" : (1, 64, 64)}
    # return = mod, params
    mod, params = frontend.contrib.onnx.from_onnx(onnx_model, shape_dict)
    print(mod)
    pass_list = ["convert_type", "infer_precision", "convert_vit", "eliminate"]
    mod, params = ftransform(pass_list)(mod, params)
    print(mod)
    np_params = {}
    for name, data in params.items():
        np_params[name] = data.asnumpy()
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    f_mod = FPGAParameters(f_mod, np_params)
    print(f_mod)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    jit_mod = FPGAJit().Jit(f_mod)
    print(jit_mod)
    c_mod, params, _ = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/vit_block.c", "w") as f:
        f.write(c_mod)

    with open("./test/vit_block.bin", "wb") as f:
        f.write(params)


#onnx_export()
compile()
