import subprocess

testbench = {
    "mvm": "testbench_HBM_MVM",
    "mvm_bn": "testbench_HBM_MVM_bn",
    "mvm_bn_res": "testbench_HBM_MVM_bn_res",
    "fc": "testbench_HBM_FC",
    "fc_bn": "testbench_HBM_FC_BN",
    "fc_bn_res": "testbench_HBM_FC_BN_RES",
    "ln": "testbench_LN",
    "act": "testbench_ACT",
    "softmax": "testbench_SOFTMAX",
    "transpose": "testbench_TRANSPOSE_HBM",
    "feature2weight": "testbench_Feature2Weight_HBM",
    "elementwise": "testbench_ElementWise",
    "pos_emb": "testbench_EMB",
}

define_v1 = ["Height", "Width_in", "Width_out"]
define_v2 = ["Hin", "Win", "CHin"]
define_v3 = ["Head", "Height", "Width_in"]

template_cpp = (
    "./HBM_c/test",
)

template_rtl = (
    "make",
    "run",
    "-C",
    "HBM_sv",
)


def main(name, mat_cfg, define=define_v1):
    print("(" + ", ".join(mat_cfg) + ")")
    csb_cpp = []
    csb_rtl = []
    cmd_cpp, cmd_rtl = list(template_cpp), list(template_rtl)
    cmd_cpp.append(name)
    cmd_cpp += mat_cfg
    define_cfg = zip(define, mat_cfg)
    define_cfg = ["+define+" + i[0] + "=" + i[1] for i in define_cfg]
    cmd_rtl.append("TOP_MODULE=" + testbench[name])
    cmd_rtl.append("SIM_DEFINE=\"" + "".join(define_cfg) + "\"")
    p_cpp = subprocess.Popen(cmd_cpp, stdout=subprocess.PIPE)
    p_rtl = subprocess.Popen(cmd_rtl, stdout=subprocess.PIPE)
    out_cpp, cpp_err = p_cpp.communicate()
    out_rtl, rtl_err = p_rtl.communicate()
    saved_out_rtl = out_rtl.decode("utf-8")
    out_cpp = out_cpp.decode("utf-8").split("\n")
    out_rtl = out_rtl.decode("utf-8").replace("# ", "").split("\n")
    for out in out_cpp:
        if "csb_cpp" in out:
            eval(out)
    for out in out_rtl:
        if "csb_rtl" in out:
            eval(out)
    print("--------------------------------------------------------------")
    if csb_cpp == csb_rtl:
        print(name + ": Success!")
    else:
        print(name + ": Failure!")
        print(saved_out_rtl)
        print("")
        for i in range(len(csb_rtl)):
            if csb_cpp[i] != csb_rtl[i]:
                print(f"diff {i}: ", end="")
                print("csb_cpp: ", csb_cpp[i], end=", ")
                print("csb_rtl: ", csb_rtl[i])
    print("--------------------------------------------------------------")

import random

num = 1

for i in range(num):
    hin, win, wout = random.randint(1, 40), random.randint(10, 2000), random.randint(10, 2000)
    # hin, win, wout = 32, 128*2, 128
    hin_s, win_s, wout_s = f"{hin}", f"{win}", f"{wout}"
    main("pos_emb", [hin_s, win_s, wout_s], define_v3)
    main("ln", [hin_s, win_s])
    main("mvm", [hin_s, win_s, wout_s])
    main("mvm_bn", [hin_s, win_s, wout_s])
    main("mvm_bn_res", [hin_s, win_s, wout_s])
    main("fc", ["1", win_s, wout_s])
    main("fc_bn", ["1", win_s, wout_s])
    main("fc_bn_res", ["1", win_s, wout_s])
    main("softmax", [hin_s, win_s])
    main("transpose", [hin_s, win_s])
    main("feature2weight", [hin_s, win_s])
    main("elementwise", [hin_s, win_s, wout_s], define_v2)
    main("act", [hin_s, win_s])
