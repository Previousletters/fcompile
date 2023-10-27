from ..op import *
from ..fir import FCall, FExtern, Mutator
from ..utils import shape2str

map_ops = [AccelFMap, AccelWMap, TVMMap]

def get_map_op(ret_type, arg_types):
    if ret_type in arg_types:
        return None
    else:
        for op in map_ops:
            if ret_type in op.arg_types[0]:
                if op.ret_type in arg_types:
                    return op
        arg_str = shape2str(arg_types)
        raise RuntimeError("No map operator found between " + ret_type + " with " + arg_str)


class DataMap(Mutator):

    def transform(self, fmod):
        new_op = Output(fmod.fexpr.op.shape, fmod.fexpr.op.dtype)
        new_fexpr = FCall([fmod.fexpr], new_op, {}, 0)
        fmod.fexpr = self.visit(new_fexpr)
        return fmod
    
    def get_id(self):
        if hasattr(self, "id"):
            self.id += 1
            return self.id
        else:
            self.id = 0
            return self.id

    def visit_call(self, call):
        ret_types = [arg.op.ret_type for arg in call.args]
        new_args = []
        call.args = [self.visit(arg) for arg in call.args]
        arg_types = call.op.arg_types
        index_list = range(len(call.args))
        zipped_data = zip(ret_types, arg_types, call.args, index_list)
        offset = 0
        for ret, intype, arg, index in zipped_data:
            new_op = get_map_op(ret, intype)
            if new_op != None:
                width = call.attrs["widths"][index+offset]
                scale = call.attrs["scales"][index+offset]
                new_attrs = {"widths" : [width], "scales" : [scale]}
                if len(call.attrs["scales"]) != len(call.args) + 1:
                    offset += 1
                    new_attrs["widths"].append(call.attrs["widths"][index+offset])
                    new_attrs["scales"].append(call.attrs["scales"][index+offset])
                new_op = new_op(arg.op.shape, arg.op.dtype)
                new_id = self.get_id()
                new_arg = FCall([arg], new_op, new_attrs, new_id)
                new_args.append(new_arg)
            else:
                new_args.append(arg)
        new_id = self.get_id()
        return FCall(new_args, call.op, call.attrs, new_id)

    def visit_extern(self, call):
        ret_types = [arg.op.ret_type for arg in call.args]
        new_args = []
        call.args = [self.visit(arg) for arg in call.args]
        arg_types = call.op.arg_types
        index_list = range(len(call.args))
        zipped_data = zip(ret_types, arg_types, call.args, index_list)
        for ret, intype, arg, index in zipped_data:
            new_op = get_map_op(ret, intype)
            if new_op != None:
                width = arg.attrs["widths"][-1]
                scale = arg.attrs["scales"][-1]
                new_attrs = {"widths" : [width], "scales" : [scale]}
                new_op = new_op(arg.op.shape, arg.op.dtype)
                new_id = self.get_id()
                new_arg = FCall([arg], new_op, new_attrs, new_id)
                new_args.append(new_arg)
            else:
                new_args.append(arg)
        new_id = self.get_id()
        new_op = TVMOp(call.op.var_, call.op.expr, call.op.shape, call.op.dtype, new_id)
        return FExtern(new_args, new_op, call.attrs, new_id)
