import numpy as np

def process(data, scale):
    data = data.numpy() * (2**scale)
    return data.astype("int8")

def diff(name, tresult, fresult):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    num_same = np.sum(tresult == fresult)
    num_diff = tresult.size - num_same
    rat_same = num_same / tresult.size
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME : {name}")
    print(f"* T-F DIFF : {max_diff}")
    print(f"* F-T DIFF : {min_diff}")
    print(f"* NUM DIFF : {num_diff}")
    print(f"* NUM SAME : {num_same}")
    print("* SAME%    : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************\n")

def diff_scale(name, tresult, fresult, scale, threshold):
    max_diff = np.max(tresult-fresult)
    min_diff = np.max(fresult-tresult)
    abs_diff = np.abs(tresult - fresult)
    num_same = np.sum(abs_diff<=threshold)
    num_diff = tresult.size - num_same
    rat_same = num_same / tresult.size
    ft_diff = fresult-tresult
    means_diff = np.mean(ft_diff)
    std_diff = np.std(ft_diff)
    print("**************** DIFFERENCE ****************")
    print(f"* JOB NAME  : {name}")
    print(f"* OUT SCALE : {scale}")
    print(f"* T-F DIFFS : {max_diff}, {max_diff/(2**scale)}")
    print(f"* F-T DIFFS : {min_diff}, {min_diff/(2**scale)}")
    print(f"* F-T MEANS : {means_diff}, {means_diff/(2**scale)}")
    print(f"* F-T STD   : {std_diff}, {std_diff/(2**scale)}")
    print(f"* NUM DIFF  : {num_diff}")
    print(f"* NUM SAME  : {num_same}")
    print(f"* THRESHOLD : {threshold}")
    print("* SAME%     : {:.02f}%".format(rat_same*100))
    print("**************** DIFFERENCE ****************\n")

def result_diff_check(func=diff):
    def _register_fn(fn):
        def _call():
            n = fn()
            func(fn.__name__, *n)
        return _call
    return _register_fn