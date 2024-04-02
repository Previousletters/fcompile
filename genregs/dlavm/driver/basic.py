from functools import reduce
import subprocess
from .. import ne
from ..adr import DataEnum, DataType


def CSB_For(expr, tag):
    tag.reg_ops.append(expr)


def CSB_End(expr, tag):
    tag.reg_ops.append([-1, 0, 0])


def CSB_Write(regs, addr, data):
    if data is None:
        regs.append([1, addr, 0])
    elif isinstance(data, ne.Expr):
        regs.append([1, addr, data.simplify().export("cpp")])
    else:
        regs.append([1, addr, data & 0xffffffff])


def CSB_Read(regs, addr, data):
    if data is None:
        regs.append([0, addr, 0])
    elif isinstance(data, ne.Expr):
        regs.append([0, addr, data.simplify().export("cpp")])
    else:
        regs.append([0, addr, data & 0xffffffff])


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