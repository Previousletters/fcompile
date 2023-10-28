import tvm
from tvm import relay
from tvm.relay import ExprFunctor, ExprMutator, ExprVisitor
from tvm.topi.utils import get_const_tuple
import numpy as np

from .register import register_transform

CONVERT_OP_MAP = {}

def register_op_map(op_name, target):
    def _register_fn(fn):
        global CONVERT_OP_MAP
        if op_name not in CONVERT_OP_MAP:
            CONVERT_OP_MAP[op_name] = {}
        CONVERT_OP_MAP[op_name][target] = fn().convert
        def _call(*args):
            return fn(*args)
        return _call
    return _register_fn


def convert_op(name, target):
    if name not in CONVERT_OP_MAP:
        def _fn(*args):
            return relay.accel.base.dequantize(args[0], args[1], args[2])
        return _fn
    return CONVERT_OP_MAP[name][target]

def get_tuple_4d(shape):
    shape = list(shape)
    new_shape = [1 for i in range(4-len(shape))]
    new_shape += shape
    return tuple(new_shape)


@register_op_map("nn.conv2d", "vit")
class Conv2dVIT(ExprVisitor):

    def convert(self, expr, owidth, oscale, oshape):
        self.widths = []
        self.scales = []
        self.args = []
        strides = int(expr.attrs["strides"][0])
        padding = int(expr.attrs["padding"][0])
        for arg in expr.args:
            self.visit(arg)
        if hasattr(self, "failure"):
            return relay.accel.base.dequantize(expr, owidth, oscale)
        else:
            data, weight = self.args
            return relay.accel.vit.conv2d(data, weight, strides, padding, widths=self.widths, scales=self.scales)

    def visit_call(self, call):
        if str(call.op) == "accel.quantize":
            self.widths.append(call.attrs["bwidth"])
            self.scales.append(call.attrs["dscale"])
            self.args.append(call.args[0])
        else:
            self.failure = 1


@register_op_map("nn.batch_matmul", "vit")
class MMVIT(ExprVisitor):

    enable_ops = ["transpose", "reshape"]

    def convert(self, expr, owidth, oscale, oshape):
        self.widths = []
        self.scales = []
        self.args = []
        self.oshape = oshape
        for arg in expr.args:
            self.visit(arg)
        if hasattr(self, "failure"):
            return relay.accel.base.dequantize(expr, owidth, oscale)
        else:
            return self.gen_expr(owidth, oscale)

    def visit_call(self, call):
        if str(call.op) == "accel.quantize":
            self.widths.append(call.attrs["bwidth"])
            self.scales.append(call.attrs["dscale"])
            self.args.append(call.args[0])
        elif str(call.op) in self.enable_ops:
            self.visit_call(call.args[0])
        else:
            self.failure = 1

    def gen_expr(self, owidth, oscale):
        self.widths.append(owidth)
        self.scales.append(oscale)
        data, weight = self.args
        dshape = data.checked_type.shape
        wshape = weight.checked_type.shape
        new_dshape = get_tuple_4d(dshape)
        new_data = relay.reshape(data, new_dshape)
        if isinstance(weight, relay.Var):
            new_wshape = get_tuple_4d((wshape[1], wshape[0]))
            new_weight = relay.transpose(weight, [1, 0])
            new_weight = relay.reshape(new_weight, new_wshape)
        elif isinstance(weight, relay.Call):
            new_wshape = get_tuple_4d(wshape)
            new_weight = relay.reshape(weight, new_wshape)
        output = relay.accel.vit.mm(new_data, new_weight, self.widths, self.scales, 0)
        output = relay.reshape(output, self.oshape)
        return output


@register_op_map("nn.softmax", "vit")
class SoftmaxVIT(ExprVisitor):

    def convert(self, expr, owidth, oscale, oshape):
        self.widths = []
        self.scales = []
        self.args = []
        self.oshape = oshape
        for arg in expr.args:
            self.visit(arg)
        if hasattr(self, "failure"):
            return relay.accel.base.dequantize(expr, owidth, oscale)
        else:
            return self.gen_expr(owidth, oscale)

    def visit_call(self, call):
        if str(call.op) == "accel.quantize":
            self.widths.append(call.attrs["bwidth"])
            self.scales.append(call.attrs["dscale"])
            self.args.append(call.args[0])
        else:
            self.failure = 1

    def gen_expr(self, owidth, oscale):
        self.widths.append(owidth)
        self.scales.append(oscale)
        data = self.args[0]
        dshape = data.checked_type.shape
        new_dshape = get_tuple_4d(dshape)
        new_data = relay.reshape(data, new_dshape)
        output = relay.accel.vit.softmax(new_data, self.widths, self.scales, 0)
        output = relay.reshape(output, self.oshape)
        return output


@register_op_map("multiply", "vit")
class MultiplyVIT(ExprVisitor):

    def convert(self, expr, owidth, oscale, oshape):
        self.widths = []
        self.scales = []
        self.args = []
        self.oshape = oshape
        for arg in expr.args:
            self.visit(arg)
        if hasattr(self, "failure"):
            return relay.accel.base.dequantize(expr, owidth, oscale)
        else:
            return self.gen_expr(owidth, oscale)

    def visit_call(self, call):
        if str(call.op) == "accel.quantize":
            self.widths.append(call.attrs["bwidth"])
            self.scales.append(call.attrs["dscale"])
            self.args.append(call.args[0])
        else:
            self.failure = 1

    def gen_expr(self, owidth, oscale):
        self.widths.append(owidth)
        self.scales.append(oscale)
        data, weight = self.args
        dshape = data.checked_type.shape
        n = weight.data.asnumpy()
        new_dshape = get_tuple_4d(dshape)
        new_data = relay.reshape(data, new_dshape)
        chout = int(new_dshape[3])
        new_wshape = get_tuple_4d((chout, chout))
        new_weight = np.zeros(shape=new_wshape, dtype="int8")
        for i in range(chout):
            new_weight[0, 0, i, i] = n
        new_weight = tvm.nd.array(new_weight)
        new_weight = relay.Constant(new_weight)
        output = relay.accel.vit.mm(new_data, new_weight, self.widths, self.scales, 0)
        output = relay.reshape(output, self.oshape)
        return output


@register_op_map("transpose", "vit")
class TransposeVIT(ExprVisitor):

    def convert(self, expr, owidth, oscale, oshape):
        self.widths = []
        self.scales = []
        self.args = []
        self.oshape = oshape
        for arg in expr.args:
            self.visit(arg)
        if hasattr(self, "failure"):
            return relay.accel.base.dequantize(expr, owidth, oscale)
        else:
            return self.gen_expr(owidth, oscale)

    def visit_call(self, call):
        if str(call.op) == "accel.quantize":
            self.widths.append(call.attrs["bwidth"])
            self.scales.append(call.attrs["dscale"])
            self.args.append(call.args[0])
        else:
            self.failure = 1

    def gen_expr(self, owidth, oscale):
        self.widths.append(owidth)
        self.scales.append(oscale)
        data = self.args[0]
        dshape = data.checked_type.shape
        new_dshape = get_tuple_4d(dshape)
        new_data = relay.reshape(data, new_dshape)
        output = relay.accel.vit.transpose(new_data, self.widths, self.scales)
        output = relay.reshape(output, self.oshape)
        return output


class ConvertVIT(ExprMutator):

    def convert(self, mod):
        self.memo = {}
        self.new_vars = []
        expr = mod["main"].body
        new_expr = self.visit(expr)
        func = relay.Function(self.new_vars, new_expr)
        new_mod = tvm.IRModule.from_expr(func)
        return new_mod

    def visit_var(self, var):
        var = super().visit_var(var)
        self.new_vars.append(var)
        return var

    def visit_call(self, call):
        if hasattr(self, "bwidth") and hasattr(self, "dscale"):
            op_name = call.op
            call_op = call
            oshape = call.checked_type.shape
            if str(call.op) == "reshape":
                call_op = call.args[0]
                op_name = call_op.op
            new_call = convert_op(str(op_name), "vit")(call_op, self.bwidth, self.dscale, oshape)
            del self.bwidth, self.dscale
            return super().visit_call(new_call)
        elif str(call.op) == "accel.dequantize":
            self.bwidth = int(call.attrs["bwidth"])
            self.dscale = int(call.attrs["dscale"])
            new_call = call.args[0]
            return super().visit(new_call)
        else:
            new_call = call
            return super().visit_call(new_call)


@register_transform("convert_vit")
def transform(mod, params):
    mod = ConvertVIT().convert(mod)
    return mod, params
