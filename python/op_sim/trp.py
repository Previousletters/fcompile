import numpy as np

def diff(hard_result, soft_result, threshold = 0.01, r_threshold = None, a_threshold = None):
    r_threshold = threshold if r_threshold is None else r_threshold
    a_threshold = threshold if a_threshold is None else a_threshold
    r_diff = np.abs(hard_result - soft_result) / (np.abs(soft_result) + 0.0000001)
    a_diff = np.abs(hard_result - soft_result)
    t_diff = (r_diff > r_threshold) | (a_diff > a_threshold)
    totals = np.sum(np.ones(hard_result.shape), dtype=np.int32)
    diff_ratio = np.sum(t_diff)/totals
    print(f"diff: {np.sum(t_diff):>5}/{totals} = {diff_ratio*100:2.4f}%\tthreshold={r_threshold}")
    return t_diff, diff_ratio

head = 30
token = 20

data0 = np.random.randn(head, 1, 128).astype("float16")
data1 = np.random.randn(head, 128, 2*token).astype("float16")

result_0 = data0 @ data1

data1_0 = data1[:, :, :token]
data1_1 = data1[:, :, token:]

result_1 = np.zeros([head, 1, 2*token], dtype="float16")
result_1[:, :, :token] = data0 @ data1_0
result_1[:, :, token:] = data0 @ data1_1

print(np.sum(result_0 != result_1))

##

data0 = np.random.randn(head, 1, 2*token).astype("float32")
data1 = np.random.randn(head, 2*token, 128).astype("float32")

result_0 = data0 @ data1

data0_0 = data0[:, :, :token]
data0_1 = data0[:, :, token:]
data1_0 = data1[:, :token, :]
data1_1 = data1[:, token:, :]

result_1 = data0_0 @ data1_0 + data0_1 @ data1_1

diff(result_0, result_1, 0.00001)