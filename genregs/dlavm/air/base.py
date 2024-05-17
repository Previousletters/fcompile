class PrimBase:
    pass


class PrimExpr(PrimBase):

    pass


class PrimStmt(PrimBase):

    pass


class Array(PrimExpr):

    def __init__(self, name, shape, dtype):
        super().__init__()
        self.name = name
        self.shape = shape
        self.dtype = dtype


class Var(PrimExpr):

    def __init__(self, name, dtype):
        super().__init__()
        self.name = name
        self.dtype = dtype


class IterVar(PrimExpr):

    def __init__(self, name, dtype):
        super().__init__()
        self.name = name
        self.dtype = dtype


class For(PrimStmt):

    def __init__(
        self, 
        loop_var: Var,
        min: PrimExpr,
        extent: PrimExpr,
        stride: PrimExpr,
        body: PrimStmt
    ):
        super().__init__()
        self.loop_var = loop_var
        self.min = min
        self.extent = extent
        self.stride = stride
        self.body = body


class Allocate(PrimStmt):

    def __init__(
        self,
        buffer: Array,
        body: PrimStmt
    ):
        super().__init__()
        self.buffer = buffer
        self.body = body