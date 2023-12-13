from .m_utils import PrintFir, GetFirVar
from ..utils import shape2str


class Base:

    Type = "Base"

    def __init__(self, args, op, attrs):
        self.args = args
        self.op = op
        self.attrs = attrs

    def __str__(self):
        str_shape = f"ret={shape2str(self.op.shape)}, dtype={self.op.dtype}"
        if hasattr(self, "data"):
            dshape = self.data.shape
            str_shape = str_shape + " : " + f"const={shape2str(dshape)}"
        return f"/* {str_shape} */"


class FVar(Base):

    Type = "Var"

    def __init__(self, name, op):
        super().__init__([], op, {})
        self.expr_name = name


class FCVar(Base):

    Type = "CVar"

    def __init__(self, name, op, data):
        super().__init__([], op, {})
        self.data = data
        self.expr_name = name


class FCall(Base):

    Type = "FCall"

    def __init__(self, args, op, attrs):
        super().__init__(args, op, attrs)


class FExtern(Base):

    Type = "FExtern"

    def __init__(self, args, op, attrs):
        super().__init__(args, op, attrs)


class FModule:

    def __init__(self, fexpr, tin=64, tout=32):
        self.fexpr = fexpr
        self.tin = tin
        self.tout = tout

    def __str__(self):
        vars = GetFirVar().get(self.fexpr)
        body, extern = PrintFir().fir_body(self.fexpr)
        str_var = ", ".join([var.expr_name for var in vars])
        str_body = "\n".join(["  " + b for b in body])
        str_extern = "\n".join(extern)
        str_main = f"def main({str_var}, tin={self.tin}, tout={self.tout})" + " {\n" + str_body + "\n}\n"
        return str_main + str_extern
