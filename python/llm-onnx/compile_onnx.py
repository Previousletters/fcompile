import onnx
import tvm
from tvm.relay import frontend
from tvm.relay import transform
from tvm.relay.op.contrib import partition_for_vsc


def load():
    # model = onnx.load("/home/previous/work/tensor-dnn/script/models/lenet.onnx")

    model = onnx.load("./onnx/block_0.onnx")
    mod, params = frontend.from_onnx(model)
    print("---------------- Origin ----------------")
    print(mod)

    pass_0 = tvm.transform.Sequential(
        [
            transform.InferType(),
            transform.RemoveUnusedFunctions(),
            transform.SimplifyInference(),
            transform.SimplifyExpr(),
            transform.FoldScaleAxis(),
            transform.ConvertLayout({"nn.conv2d": ["NHWC", "OIHW"]}),
        ]
    )
    with tvm.transform.PassContext(opt_level=3):
        mod = pass_0(mod)
    print("---------------- Pass 0 ----------------")
    print(mod)


if __name__ == "__main__":
    load()
