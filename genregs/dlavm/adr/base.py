# AST data structure
class DataEnum:
    hbm = "hbm"
    ddr = "ddr"
    fp16 = "float16"
    int8 = "int8"
    int4 = "int4"
    int2 = "int2"

    type_bytes = {fp16: 2, int2: 0.25, int4: 0.5, int8: 1}


class DataType:

    def __init__(self, dtype, mapped):
        self.dtype = dtype
        self.mapped = mapped

    def get_bytes(self):
        return DataEnum.type_bytes[self.dtype]

    def __str__(self):
        return f"({self.dtype}, {self.mapped})"

    def __eq__(self, __value: object) -> bool:
        return self.dtype == __value.dtype and self.mapped == __value.mapped


class Attrs:

    default = {}

    def __init__(self, attrs):
        self._attrs = attrs

    def __str__(self) -> str:
        str_out = ""
        for k, v in self._attrs:
            if k in self.default and self.default[k] != v:
                str_out += f"{k}={v} "
        return str_out[:-1]

    def __getitem__(self, key):
        if key not in self._attrs:
            return self.default[key]
        return self._attrs[key]

    def __len__(self):
        return len(self._attrs)


class Base:

    def __init__(self, args, prefix="runtime"):
        self.args = args
        self.prefix = prefix

    def __str__(self):
        return PrintExpr.print(self)

    def __getitem__(self, index):
        return TupleItem(self, index)


class TupleItem(Base):

    def __init__(self, expr, index):
        self.expr = expr
        self.index = index
        self.checked_type = None


class Tuple(Base):

    def __init__(self, tensors):
        self.tensors = tensors

    def __str__(self):
        return f"/* Tuple({len(self.tensors)}) */"


class Tensor(Base):

    def __init__(self, shape, dtype, device):
        self.shape = shape
        self.dtype = dtype
        self.device = device
        self.bytesize = None
        self.storage_id = None
        self.offset = 0

    def get_bytesize(self):
        if self.bytesize is None:
            self.bytesize = self.device.malloc_bytes(self.shape, self.dtype)
        return self.bytesize

    def __str__(self):
        ret = "/* ret=(" + ", ".join([str(i) for i in self.shape]) + "), "
        ret += "dtype=" + str(self.dtype)
        if self.device is not None:
            ret += ", device=" + self.device.name
        if self.storage_id is not None:
            ret += ", id=" + self.storage_id + ", offset=" + str(self.offset)
        ret += " */"
        return ret

    def __eq__(self, __value) -> bool:
        if self.storage_id is None:
            return self.shape == __value.shape and self.dtype == __value.dtype


class Var(Base):

    def __init__(self, name, shape, dtype, device, prefix="runtime"):
        self.name = name
        self.shape = shape
        self.dtype = dtype
        self.device = device
        self.checked_type = None
        self.prefix = prefix


class Constant(Base):

    def __init__(self, name, data, shape=None, dtype=None, device=None, prefix="weight"):
        self.name = name
        self.data = data
        self.shape = shape
        self.dtype = dtype
        self.device = device
        self.checked_type = None
        self.prefix = prefix


class Call(Base):

    def __init__(self, op, args, attrs, prefix="runtime"):
        self.op = op
        self.args = args
        self.attrs = attrs
        self.checked_type = None
        self.prefix = prefix


class VM(Base):

    def __init__(self, op, args, attrs):
        self.op = op
        self.args = args
        self.attrs = attrs
        self.checked_type = None


class Op:

    memo = {}

    def __init__(self, name):
        self.name = name
        self.attrs = {}

    @classmethod
    def Register(cls, op_name, rel_func):
        cls.memo[op_name] = Op(op_name)
        cls.memo[op_name].attrs["rel"] = rel_func

    @classmethod
    def Get(cls, op_name):
        if op_name not in cls.memo.keys():
            msg = f"no found op \"{op_name}\", please register first"
            raise RuntimeError(msg)
        return cls.memo[op_name]


# Deep First Search with memory
class Functor:

    def __init__(self):
        self.memo = {}

    def visit(self, expr):
        if expr in list(self.memo.keys()):
            self.memo[expr][1] += 1
            return self.memo[expr][0]
        if isinstance(expr, Var):
            ret = self.visit_var(expr)
        if isinstance(expr, Constant):
            ret = self.visit_constant(expr)
        elif isinstance(expr, TupleItem):
            ret = self.visit_tupleitem(expr)
        elif isinstance(expr, Call):
            ret = self.visit_call(expr)
        elif isinstance(expr, VM):
            ret = self.visit_vm(expr)
        self.memo[expr] = [ret, 1]
        return ret

    def visit_var(self, expr):
        return expr

    def visit_constant(self, expr):
        return expr

    def visit_tupleitem(self, expr):
        new_expr = self.visit(expr.expr)
        expr.expr = new_expr
        return expr

    def visit_call(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        expr.args = new_args
        return expr

    def visit_vm(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        expr.args = new_args
        return expr


# Print Expr Pass
class PrintExpr(Functor):

    @classmethod
    def print(cls, expr):
        return cls().show_main(expr)

    def show_main(self, expr):
        self.id = 0
        self.args = []
        self.body = []
        ret_name = self.visit(expr)
        arg_str = ", ".join(self.args)
        result = f"def main({arg_str}) " + "{\n"
        result += "\n".join(self.body)
        result += f"\n  return {ret_name}\n" + "}"
        return result

    def visit_var(self, expr):
        self.args.append(expr.name)
        func = f"  {expr.name} = var()"
        if expr.checked_type:
            func += " " + str(expr.checked_type)
        self.body.append(func)
        return expr.name

    def visit_constant(self, expr):
        func = f"  {expr.name} = constant()"
        if expr.checked_type:
            func += " " + str(expr.checked_type)
        self.body.append(func)
        return expr.name

    def visit_tupleitem(self, expr):
        arg_str = self.visit(expr.expr)
        ret_name = "%" + str(self.id)
        self.id += 1
        func = f"  {ret_name} = {arg_str}.{expr.index}"
        if expr.checked_type:
            func += " " + str(expr.checked_type)
        self.body.append(func)
        return ret_name

    def visit_call(self, expr):
        arg_str = ", ".join([self.visit(arg) for arg in expr.args])
        ret_name = "%" + str(self.id)
        self.id += 1
        func = f"  {ret_name} = {expr.op.name}({arg_str})"
        if expr.checked_type:
            func += " " + str(expr.checked_type)
        self.body.append(func)
        return ret_name

    def visit_vm(self, expr):
        arg_str = ", ".join([self.visit(arg) for arg in expr.args])
        ret_name = "%" + str(self.id)
        self.id += 1
        func = f"  {ret_name} = {expr.op.name}({arg_str})"
        if expr.checked_type:
            func += " " + str(expr.checked_type)
        self.body.append(func)
        return ret_name
