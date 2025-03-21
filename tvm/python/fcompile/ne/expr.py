class Op:

    add = {"py": " + ", "cpp": " + "}
    sub = {"py": " - ", "cpp": " - "}
    mul = {"py": " * ", "cpp": " * "}
    div = {"py": " / ", "cpp": " / "}
    mod = {"py": " % ", "cpp": " % "}
    fdiv = {"py": " // ", "cpp": " / "}
    lshift = {"py": " << ", "cpp": " << "}
    rshift = {"py": " >> ", "cpp": " >> "}


class Expr:

    def __init__(self, args, op):
        self.args = args
        self.op = op

    def get_vars(self):
        vars = []
        for arg in self.args:
            vars += arg.get_vars()
        return vars

    def simplify(self, max_numb=0):
        new_args = [arg.simplify(max_numb) for arg in self.args]
        self.args = new_args
        if isinstance(new_args[0], Numb) and isinstance(new_args[1], Numb):
            return Numb(eval(str(self)))
        else:
            return self

    def __add__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.add)

    def __sub__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.sub)

    def __mul__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.mul)

    def __truediv__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.div)

    def __floordiv__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.fdiv)

    def __mod__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.mod)

    def __lshift__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.lshift)

    def __rshift__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [self, new_expr]
        return Expr(new_args, Op.rshift)

    def __radd__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.add)

    def __rsub__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.sub)

    def __rmul__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.mul)

    def __rtruediv__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.div)

    def __rfloordiv__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.fdiv)

    def __rmod__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        new_args = [new_expr, self]
        return Expr(new_args, Op.mod)

    def __gt__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        return self.simplify(max_numb=1) > new_expr

    def __ge__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        return self.simplify(max_numb=1) >= new_expr

    def __lt__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        return self.simplify(max_numb=1) < new_expr

    def __le__(self, data):
        new_expr = data
        if isinstance(data, str):
            new_expr = Var(data)
        elif isinstance(data, (int, float)):
            new_expr = Numb(data)
        return self.simplify(max_numb=1) <= new_expr

    def __eq__(self, new_expr):
        if isinstance(new_expr, Expr):
            if self.op == new_expr.op and len(self.args) == len(new_expr.args):
                for i in range(len(self.args)):
                    if self.args[i] != self.new_expr.args[i]:
                        return False
                return True
        return False

    def export(self, tag):
        return "(" + self.args[0].export(tag) + \
            self.op[tag] + \
            self.args[1].export(tag) + ")"

    def __str__(self):
        return self.export(tag="py")


class Var(Expr):

    def __init__(self, name, max_data=128):
        self.name = name
        self.max_data = max_data

    def __eq__(self, new_expr):
        if isinstance(new_expr, Var):
            return self.name == new_expr.name
        return False

    def get_vars(self):
        vars = [self.name]
        return vars

    def simplify(self, max_numb=0):
        if max_numb:
            return Numb(self.max_data)
        else:
            return self

    def export(self, tag):
        return self.name


class Numb(Expr):

    def __init__(self, data):
        self.data = data

    def __eq__(self, new_expr):
        if isinstance(new_expr, Numb):
            return self.data == new_expr.data
        return False

    def get_vars(self):
        vars = []
        return vars

    def simplify(self, max_numb=0):
        return self

    def export(self, tag):
        if tag == "cpp":
            self.data = self.data % (1 << 32)
            return str(self.data)
        else:
            return str(self.data)

    def __gt__(self, data):
        if isinstance(data, str):
            return self.data > Var(data).max_data
        elif isinstance(data, (int, float)):
            return self.data > data
        else:
            return self.data > data.simplify(1).data

    def __ge__(self, data):
        if isinstance(data, str):
            return self.data >= Var(data).max_data
        elif isinstance(data, (int, float)):
            return self.data >= data
        else:
            return self.data >= data.simplify(1).data

    def __lt__(self, data):
        if isinstance(data, str):
            return self.data < Var(data).max_data
        elif isinstance(data, (int, float)):
            return self.data < data
        else:
            return self.data < data.simplify(1).data

    def __le__(self, data):
        if isinstance(data, str):
            return self.data <= Var(data).max_data
        elif isinstance(data, (int, float)):
            return self.data <= data
        else:
            return self.data <= data.simplify(1).data


class If(Expr):

    def __init__(self, judge_expr, then_expr, else_expr):
        self.judge_expr = judge_expr
        self.then_expr = then_expr
        self.else_expr = else_expr
        if isinstance(judge_expr, str):
            self.judge_expr = Var(judge_expr)
        elif isinstance(judge_expr, (int, float)):
            self.judge_expr = Numb(judge_expr)
        if isinstance(then_expr, str):
            self.then_expr = Var(then_expr)
        elif isinstance(then_expr, (int, float)):
            self.then_expr = Numb(then_expr)
        if isinstance(else_expr, str):
            self.else_expr = Var(else_expr)
        elif isinstance(else_expr, (int, float)):
            self.else_expr = Numb(else_expr)

    def get_vars(self):
        vars = []
        for arg in [self.judge_expr, self.then_expr, self.else_expr]:
            vars += arg.get_vars()
        return vars

    def simplify(self, max_numb=0):
        bool_expr = self.judge_expr
        if isinstance(self.then_expr, Expr):
            self.then_expr = self.then_expr.simplify(max_numb)
        if isinstance(self.else_expr, Expr):
            self.else_expr = self.else_expr.simplify(max_numb)
        if isinstance(self.judge_expr, Expr):
            self.judge_expr = self.judge_expr.simplify(max_numb)
            if isinstance(self.judge_expr, Numb):
                if self.judge_expr.data:
                    return self.then_expr
                else:
                    return self.else_expr
            else:
                return self
        if bool_expr:
            return self.then_expr
        else:
            return self.else_expr

    def export(self, tag):
        return "(" + self.judge_expr.export(tag) + " ? " + \
               self.then_expr.export(tag) + " : " + \
               self.else_expr.export(tag) + ")"


if __name__ == "__main__":
    a = Var("x") * Numb(1)
    a = a // 2
    a = 1 - a
    a = If(a, Numb(1), Numb(3))
    print(a)
    a = a.simplify()
    print(a.export("cpp"))
