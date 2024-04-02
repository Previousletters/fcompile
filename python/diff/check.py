import numpy as np
from matplotlib import pyplot as plt


def diff_fp16(name, tresult, fresult, threshold=0.1):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    abs_diff = np.abs(tresult - fresult)
    num_same = np.sum(abs_diff <= threshold)
    num_diff = tresult.size - num_same
    rat_same = num_same / tresult.size
    ft_diff = fresult-tresult
    means_diff = np.mean(ft_diff)
    std_diff = np.std(ft_diff.astype("float64"))
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME  : {name}")
    print(f"* T-F DIFFS : {max_diff}")
    print(f"* F-T DIFFS : {min_diff}")
    print(f"* F-T MEANS : {means_diff}")
    print(f"* F-T STD   : {std_diff}")
    print(f"* NUM DIFF  : {num_diff}")
    print(f"* NUM SAME  : {num_same}")
    print(f"* THRESHOLD : {threshold}")
    print("* SAME%     : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************\n")


def diff_fp16_advanced(name, tresult, fresult, threshold=0.1, thre_relat=0.3):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    abs_diff = np.abs(tresult - fresult)
    rate_diff = abs_diff / (fresult+0.001)
    main_diff_0 = abs_diff > threshold
    main_diff_1 = (abs_diff / (fresult+0.001)) > thre_relat
    main_diff = main_diff_0 | main_diff_1
    num_diff = np.sum(main_diff)
    num_same = tresult.size - num_diff
    rat_same = num_same / tresult.size
    ft_diff = fresult-tresult
    means_diff = np.mean(ft_diff)
    std_diff = np.std(ft_diff.astype("float64"))
    idx_diff = np.argwhere(main_diff)
    for idx in idx_diff:
        print("idx[{0:>4},{1:>4}]\thard: {2:.4f}\tsoft: {3:.4f}\tabs_diff: {4:.4f}\tdeviation rate: {5:.2f}%".format(idx[0], idx[1], tresult[idx[0], idx[1]], fresult[idx[0], idx[1]], abs_diff[idx[0], idx[1]], rate_diff[idx[0], idx[1]]))
    real_diff = np.where(main_diff, 1, 0)
    for n in range(real_diff.shape[0]):
        tp_diff = np.concatenate([real_diff[n].reshape(1, real_diff.shape[1]) for _ in range(4096)], axis=0)
        plt.figure()
        plt.imshow(tp_diff)
        plt.xlabel("CHout from 0 to 4096")
        plt.ylabel(f"Token {n+1} loop 4096 times")
        plt.title(f"Token {n+1}, 0: Right, 1: Error")
        plt.colorbar()
        plt.tight_layout()
        plt.savefig(f"MVMBNRES2_0327_diff_token{n+1}.png")
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME  : {name}")
    print(f"* H-S DIFFS : {max_diff}")
    print(f"* S-H DIFFS : -{min_diff}")
    print(f"* H-S MEANS : {means_diff}")
    print(f"* H-S STD   : {std_diff}")
    print(f"* NUM DIFF  : {num_diff}")
    print(f"* NUM SAME  : {num_same}")
    print(f"* THRESHOLD : {threshold}")
    print("* SAME%     : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************\n")


def result_diff_check(func=diff_fp16):
    def _register_fn(fn):
        def _call():
            n = fn()
            func(fn.__name__, *n)
        return _call
    return _register_fn


@result_diff_check(diff_fp16_advanced)
def check_MVMBNRES2():
    with open("./MVMBNRES2_0327/MVMBNRES2_dat_out_demaped.bin", "rb") as f:
        fpga_out = f.readlines()
    fpga_out = b"".join(fpga_out)
    fpga_data = np.frombuffer(fpga_out, dtype="float16").reshape([19, 4096])
    with open("./MVMBNRES2_0327/Attention_Scores.bin", "rb") as f:
        soft_out = f.readlines()
    soft_out = b"".join(soft_out)
    soft_data = np.frombuffer(soft_out, dtype="float16").reshape([19, 4096])
    return fpga_data, soft_data, 0.03, 0.03  # torch_result, fcompile_verilog_result, threshold


def check_LN_Block09():
    with open("./LN/BLOCK09/MVM_BN_RES_output.bin", "rb") as f:
        block09 = f.readlines()
    block09 = b"".join(block09)
    block09_data = np.frombuffer(block09, dtype="float16").reshape([19, 4096])
    with open("./LN/BLOCK27/MVM_BN_RES_output.bin", "rb") as f:
        block27 = f.readlines()
    block27 = b"".join(block27)
    block27_data = np.frombuffer(block27, dtype="float16").reshape([19, 4096])
    block09_mean = np.mean(block09_data)
    block09_std  = np.std(block09_data.astype("float64"))
    block27_mean = np.mean(block27_data)
    block27_std  = np.std(block27_data.astype("float64"))
    print(f"block09: mean: {block09_mean}, std: {block09_std}, range with 3sigma: [{block09_mean-3*block09_std}, {block09_mean+3*block09_std}]")
    print(f"block27: mean: {block27_mean}, std: {block27_std}, range with 3sigma: [{block27_mean-3*block27_std}, {block27_mean+3*block27_std}]")


def check_LN():
    with open("./LN/BLOCK09/MVM_BN_RES_output.bin", "rb") as f:
        block09 = f.readlines()
    block09 = b"".join(block09)
    block09_data = np.frombuffer(block09, dtype="float16").reshape([19, 4096])
    with open("./LN/BLOCK27/MVM_BN_RES_output.bin", "rb") as f:
        block27 = f.readlines()
    block27 = b"".join(block27)
    block27_data = np.frombuffer(block27, dtype="float16").reshape([19, 4096])
    block09_mean = np.mean(block09_data)
    block09_std  = np.std(block09_data.astype("float64"))
    block27_mean = np.mean(block27_data)
    block27_std  = np.std(block27_data.astype("float64"))
    print(f"block09: mean: {block09_mean}, std: {block09_std}, range with 3sigma: [{block09_mean-3*block09_std}, {block09_mean+3*block09_std}]")
    print(f"block27: mean: {block27_mean}, std: {block27_std}, range with 3sigma: [{block27_mean-3*block27_std}, {block27_mean+3*block27_std}]")
    print(np.max(block09_data), np.min(block09_data), np.mean(np.abs(block09_data)))
    print(np.max(block27_data), np.min(block27_data), np.mean(np.abs(block27_data)))
    print(np.sort(np.abs(block09_data.reshape((19*4096))))[-40])
    print(np.sort(np.abs(block27_data.reshape((19*4096))))[-120])
    print(np.sort(np.abs(block09_data.reshape((19*4096))))[40])
    print(np.sort(np.abs(block27_data.reshape((19*4096))))[40])


# check_MVMBNRES2()
check_LN()
