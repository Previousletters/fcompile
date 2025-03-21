import tvm

TRANSFORM_FUNC_MAP = {}

def register_transform(op_name):
    def _register_fn(fn):
        global TRANSFORM_FUNC_MAP
        TRANSFORM_FUNC_MAP[op_name] = fn
        def _call(*args):
            return fn(*args)
        return _call
    return _register_fn


def transform(name):
    if isinstance(name, str):
        return TRANSFORM_FUNC_MAP[name]
    elif isinstance(name, list):
        trans_list = [TRANSFORM_FUNC_MAP[n] for n in name]
        def Sequantial(mod, params):
            for trans in trans_list:
                mod, params = trans(mod, params)
                mod = tvm.relay.transform.InferType()(mod)
            return mod, params
        return Sequantial
