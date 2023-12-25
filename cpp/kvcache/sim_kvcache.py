import torch
import torch.nn as nn


class KVCache(nn.Module):

    def __init__(self):
        super(KVCache, self).__init__()
        self.q_weight = nn.Parameter(torch.randn(128, 128))
        self.k_weight = nn.Parameter(torch.randn(128, 128))
        self.v_weight = nn.Parameter(torch.randn(128, 128))

    def forward(self, data, k_cache, v_cache, cache=False):
        q_data = data @ self.q_weight
        k_data = data @ self.k_weight
        v_data = data @ self.v_weight
        if cache:
            if k_cache is not None and v_cache is not None:
                k_data = torch.cat((k_data, k_cache), 0)
                v_data = torch.cat((v_data, v_cache), 0)
        print(k_data.shape)
        atten = q_data @ k_data.transpose(1, 0)
        atten = atten @ v_data
        return atten, k_data, v_data


if __name__ == "__main__":
    model = KVCache()
    model.eval()
    inputs = torch.randn(3, 128)
    output = model(inputs, None, None)
    print(output[0])
    output0 = model(inputs[0].reshape(1, 128), None, None, cache=True)
    output1 = model(inputs[1].reshape(1, 128), output0[1], output0[2], cache=True)
    output2 = model(inputs[2].reshape(1, 128), output1[1], output1[2], cache=True)
    print(output0[0])
    print(output1[0])
    print(output2[0])
