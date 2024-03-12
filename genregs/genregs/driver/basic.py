from functools import reduce
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