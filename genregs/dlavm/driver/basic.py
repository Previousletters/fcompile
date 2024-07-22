from functools import reduce
import subprocess
from .. import ne
from ..adr import DataEnum, DataType


def CSB_For(expr, tag):
    tag.append([-2, expr])


def CSB_End(expr, tag):
    tag.append([-1, 0, 0])


def CSB_Write(regs, addr, data):
    if data is None:
        regs.append([1, addr, 0])
    elif isinstance(data, ne.Expr):
        regs.append([1, addr, data.simplify()])
    else:
        regs.append([1, addr, data & 0xffffffff])


def CSB_Read(regs, addr, data):
    if data is None:
        regs.append([0, addr, 0])
    elif isinstance(data, ne.Expr):
        regs.append([0, addr, data.simplify()])
    else:
        regs.append([0, addr, data & 0xffffffff])


def Ceil(data0, data1):
    return (data0 + data1 - 1) // data1


def Ceil_Padding(data0, data1):
    return ((data0 + data1 - 1) // data1) * data1


def TestbenchSIM(tb_name: str, define: dict) -> list:
    from .config import template_rtl, tb_sim_path, tb_debug, tb_macro_log
    if tb_debug:
        tb_macro_log.append({"name": tb_name, "testbench": define})
    csb_rtl = []
    cmd_rtl = list(template_rtl) + [tb_sim_path]
    define_cfg = [f"+define+{k}={v}" for k, v in define.items()]
    cmd_rtl.append("TOP_MODULE=" + tb_name)
    cmd_rtl.append("SIM_DEFINE=\"" + "".join(define_cfg) + "\"")
    p_rtl = subprocess.Popen(cmd_rtl, stdout=subprocess.PIPE)
    out_rtl, rtl_err = p_rtl.communicate()
    saved_out_rtl = out_rtl.decode("utf-8")
    out_rtl = out_rtl.decode("utf-8").replace("# ", "").split("\n")
    for out in out_rtl:
        if "csb_rtl" in out:
            eval(out)
    if len(csb_rtl) == 0:
        raise RuntimeError(saved_out_rtl)
    return csb_rtl


class Tasks:

    memo = {}

    @classmethod
    def Register(cls, op_name, device):
        def _register_task(task):
            if cls.memo.get(device.name) is None:
                cls.memo[device.name] = {}
            if cls.memo[device.name].get(op_name) is None:
                cls.memo[device.name][op_name] = [[task, device.version]]
            elif len(cls.memo[device.name][op_name]) == 1:
                if device.version > cls.memo[device.name][op_name][0][1]:
                    cls.memo[device.name][op_name].append([task, device.version])
                else:
                    cls.memo[device.name][op_name].insert(0, [task, device.version])
            else:
                for i in range(len(cls.memo[device.name][op_name]) - 1):
                    if cls.memo[device.name][op_name][i][1] < device.version:
                        if cls.memo[device.name][op_name][i+1][1] > device.version:
                            break
                cls.memo[device.name][op_name].insert(i+1, [task, device.version])
        return _register_task


    @classmethod
    def Get(cls, op_name, device):
        # if op_name not in cls.memo.keys():
        #     msg = f"no found op \"{op_name}\", please register first"
        #     raise RuntimeError(msg)
        for i in range(len(cls.memo[device.name][op_name]) - 1):
            if cls.memo[device.name][op_name][i][1] < device.version:
                if cls.memo[device.name][op_name][i+1][1] > device.version:
                    return cls.memo[device.name][op_name][i][0]
                elif cls.memo[device.name][op_name][i+1][1] == device.version:
                    return cls.memo[device.name][op_name][i+1][0]
            elif cls.memo[device.name][op_name][i][1] == device.version:
                return cls.memo[device.name][op_name][i][0]
        return cls.memo[device.name][op_name][i+1][0]


