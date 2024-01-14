import onnx
import tvm
from tvm.relay import frontend
from tvm.relay import transform
from tvm.relay.op.contrib import partition_for_vsc


def load():
    # model = onnx.load("/home/previous/work/tensor-dnn/script/models/lenet.onnx")
    precision = {
        "features.0.weight": [8, 5, 8, 5, 8, 5],
        "features.2.weight": [4, 2, 4, 2, 4, 2],
        "features.4.weight": [2, 0, 2, 1, 2, 0],
        "17": [4, 2, 4, 2, 4, 2],
        "classifier.1.weight": [8, 5, 8, 5, 8, 5],
    }
    model = onnx.load("./onnx/vgg16.onnx")
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

    pass_1 = tvm.transform.Sequential(
        [
            transform.DenseToConv2D(),
            transform.InferType(),
        ]
    )
    with tvm.transform.PassContext(opt_level=3):
        mod = pass_1(mod)
    print("---------------- Pass 1 ----------------")
    print(mod)

    pass_2 = tvm.transform.Sequential(
        [
            transform.EleminateShapeTrans(),
            transform.InferType(),
        ]
    )
    with tvm.transform.PassContext(opt_level=3):
        mod = pass_2(mod)
    print("---------------- Pass 2 ----------------")
    print(mod)

    pass_3 = tvm.transform.Sequential(
        [
            transform.ConvertVSCAccel(precision, sim_8bit=1),
            transform.InferType(),
        ]
    )
    print("---------------- Pass 3 ----------------")
    with tvm.transform.PassContext(opt_level=3):
        mod = pass_3(mod)
    print(mod)

    pass_4 = tvm.transform.Sequential(
        [
            transform.FusedVSCOps(),
            transform.InferType(),
        ]
    )
    print("---------------- Pass 4 ----------------")
    with tvm.transform.PassContext(opt_level=3):
        mod = pass_4(mod)
    print(mod)

    print("---------------- Pass 5 ----------------")
    print("weight offline process, waiting...\n")

    mod = partition_for_vsc(mod)
    print("---------------- Pass 6 ----------------")
    print(mod)


if __name__ == "__main__":
    load()
