import torch
import torch.nn as nn


class KVCache(nn.Module):

    def __init__(self):
        super(KVCache, self).__init__()
        self.q_weight = nn.Parameter(torch.randn(8, 8)/10)
        self.k_weight = nn.Parameter(torch.randn(8, 8)/10)
        self.v_weight = nn.Parameter(torch.randn(8, 8)/10)

    def forward(self, data, k_cache, v_cache, cache=False):
        q_data = data @ self.q_weight
        k_data = data @ self.k_weight
        v_data = data @ self.v_weight
        if cache:
            if k_cache is not None and v_cache is not None:
                k_data = torch.cat((k_data, k_cache), 0)
                v_data = torch.cat((v_data, v_cache), 0)
        atten = q_data @ k_data.transpose(1, 0)
        atten = atten @ v_data
        return atten, k_data, v_data


if __name__ == "__main__":
    model = KVCache()
    model.eval()
    inputs = torch.randn(1, 8)
    k_cache, v_cache = None, None
    for i in range(10):
        output0, k_cache, v_cache = model(inputs[-1].reshape(1, 8), k_cache, v_cache, cache=True)
        output1, _, _ = model(inputs, None, None)
        print(f"token = {i}")
        print("with kvcache")
        print(output0.detach().numpy())
        print("without kvcache")
        print(output1.detach().numpy())
        print("-------------------------------------------------------------")
        inputs = torch.cat((inputs, output1[-1].reshape(1, 8)), 0)
