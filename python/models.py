import torch
from torch import nn
from math import sqrt

from fcompile.quant import nn as fqnn

class PatchEmbed(nn.Module):
    """ 2D Image to Patch Embedding
    """
    def __init__(self, img_size=224, patch_size=16, in_chans=3, embed_dim=768, norm_layer=None, flatten=True):
        super().__init__()
        img_size = (img_size, img_size)
        patch_size = (patch_size, patch_size)
        self.img_size = img_size
        self.patch_size = patch_size
        self.grid_size = (img_size[0] // patch_size[0], img_size[1] // patch_size[1])
        self.num_patches = self.grid_size[0] * self.grid_size[1]
        self.flatten = flatten

        self.proj = fqnn.Conv2d(in_chans, embed_dim, kernel_size=patch_size, stride=patch_size, bias=False)
        #self.norm = norm_layer(embed_dim) if norm_layer else nn.Identity()

    def forward(self, x):
        x = self.proj(x)
        x = x.reshape(x.shape[0], x.shape[1], -1).transpose(1, 2)  # BCHW -> BNC
        return x

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

    def __init__(self, dim, mlp_ratio=4., qkv_bias=False, drop=0., attn_drop=0.):
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
        x = self.res1(x, self.attn(self.norm1(x)))
        x = self.res2(x, self.mlp(self.norm2(x)))
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

class VisionTransformer(nn.Module):

    def __init__(self, img_size=224, patch_size=16, in_chans=3, num_classes=10, embed_dim=768, depth=2):
        super().__init__()
        self.patch_embed = PatchEmbed(
            img_size=img_size, patch_size=patch_size, in_chans=in_chans, embed_dim=embed_dim)
        self.num_classes = num_classes
        self.num_features = self.embed_dim = embed_dim  # num_features for consistency with other models
        self.blocks = nn.Sequential(*[
            Block(dim=embed_dim)
            for i in range(depth)])
        self.head = fqnn.Linear(self.num_features, num_classes)

    def forward(self, x):
        x = self.patch_embed(x)
        x = self.blocks(x)
        return x
        #cls_token = self.cls_token.expand(x.shape[0], -1, -1)  # stole cls_tokens impl from Phil Wang, thanks

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

def vit_export():
    name = "./test/vit_test.onnx"
    width = 8
    data = torch.randn(size=(1, 3, 224, 224))
    model = VisionTransformer()
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
    mod = transform.InferType()(mod)
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


def compile_vit():
    onnx_model = onnx.load("./test/vit_test.onnx")
    shape_dict = {"data" : (1, 3, 224, 224)}
    # return = mod, params
    mod, params = frontend.contrib.onnx.from_onnx(onnx_model, shape_dict)
    mod = transform.InferType()(mod)
    print(mod)
    mod = transform.ConvertLayout({"nn.conv2d" : ["NHWC", "HWIO"]})(mod)
    mod = transform.InferType()(mod)
    pass_list = ["convert_type", "infer_precision", "convert_vit", "eliminate"]
    mod, params = ftransform(pass_list)(mod, params)
    print(mod)
    np_params = {}
    for name, data in params.items():
        np_params[name] = data.asnumpy()
    f_mod = FModule(RelayFIR().convert(mod), tin=64, tout=32)
    print(f_mod)
    f_mod = FPGAParameters(f_mod, np_params)
    f_mod = DataMap().transform(f_mod)
    print(f_mod)
    jit_mod = FPGAJit().Jit(f_mod)
    c_mod, params, _ = CCodeGen().build(jit_mod)

    os.makedirs("test", exist_ok=True)
    with open("./test/vit_block.c", "w") as f:
        f.write(c_mod)

    with open("./test/vit_block.bin", "wb") as f:
        f.write(params)

#onnx_export()
#vit_export()
#compile()
compile_vit()
