import tvm.ir

import tvm
from tvm.relay import transform


def partition_for_vsc(mod):
    """ Partition the graph greedily offloading supported operators To FPGA

    Parameters
    ----------
    mod : IR.Module
        The module to run passes on.
    ignored_op : ["nn.max_pool2d"] 
        Ignore max_pool2d op.
    op_num : [2] 
        Ignore the 2nd max_pool2d operator

    Returns
    -------
    mod_and_config : Tuple[Module, Dict[str, Any]]
        A tuple of 1) annotated and partitioned module and 2) "relay.ext.fpga.options"
        configuration which should be given to PassContext when building.
    """

    # Users can use transform.ConvertLayout(op_name, [data_layout, weight_layout]) to transform the layout.
    seq = tvm.transform.Sequential(
        [
            transform.AnnotateTarget("vsc"),
            transform.MergeCompilerRegions(),
            transform.PartitionGraph(),
            transform.InferType(),
        ]
    )
    with tvm.transform.PassContext(opt_level=3):
        mod = seq(mod)

    return mod


def _register_external_op_helper(op_name, supported=True):
    """The helper function to indicate that a given operator can be supported
    by our custom FPGA acceleration.

    Parameters
    ----------
    op_name : Str
        The name of operator that will be registered.

    Returns
    -------
    f : callable
        A function that returns if the operator is supported by FPGA accelerator.
    """

    @tvm.ir.register_op_attr(op_name, "target.vsc")
    def _func_wrapper(expr):
        return supported

    return _func_wrapper


_register_external_op_helper("accel.vsc.conv2d")


def retrieve_kernels(lib):
    submodules = lib._collect_dso_modules()
    # remove the submodule without useful kernel functions
    # happens if the whole computation graph can be delegated to the FPGA (rare case)

    pruned_modules = []
    for module in submodules:
        if module.get_source() == ";":
            continue
        pruned_modules.append(module)

    return pruned_modules
