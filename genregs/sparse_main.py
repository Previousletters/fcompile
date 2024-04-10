from dlavm import adr
from dlavm import transform
from dlavm import backend


def model():
    data = adr.sparse.var_ddr("data", [32, 32, 3])
    weight = adr.sparse.const_ddr("weight", None, [64, 3, 3, 3])
    output = adr.sparse.conv2d(data, weight, [1, 1], [0, 0], [8, 8], [4, 4], [8, 8, 8], 0)
    output = transform.infer_type(output)
    print(output)
    expr, source, storage, _ = backend.csb_test_head(output, "main_test", 0x40000000, 0x00)
    print(expr)
    print(source)
    print(storage)


model()
