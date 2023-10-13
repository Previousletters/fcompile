# FPGA-Backend Compile

开发环境:

    tvm, numpy=1.20, modelsim

编译流程:

    PyTorch(quantized model) --> TVM Relay IR --> Convert Pass --> 
    TVM Relay Accel IR --> FIR --> FPGA / RTL Sim

文件说明:

    fcompile: 软链接，实际地址为tvm/python/fcompile，与TVM共用PYTHONPATH变量

    include/rtl/tb: RTL源码和Testbench等

    python: main函数脚本，主要测试与控制目录

    sim: RTL仿真目录

    template: FPGA部署例子，其中包括加速器驱动等等

    tvm: fpga-accel版本的tvm源码

## TVM Relay Accel IR --> FIR --> RTL Sim

本项目开发环境为WSL+Windows(ModelSIM)，如果为Linux系统则需要添加ModelSIM环境变量并修改sim/bin/Makefile的Tools，目前Windows系统由于路径问题暂不支持，其他仿真器暂无支持。

具体参考函数如下：

```python
# python/main.py
main_accel_modelsim()

# python/main.py
main_accel_extern_modelsim()
```

0. 仿真准备

    首先需要将rtl文件进行编译，tb文件在仿真时编译

```shell
cd sim
make install
make compile
```

1. 搭建计算图

    当前暂无Relay Convert Relay Accel的Pass方法，所以直接通过Relay Accel IR搭建计算图，并生成IRModule。其中，InferType优化方法是必须的。

2. FIR转换

    将tvm的relay计算结构转换为fcompile中提供的fir计算结构。此计算结构可以直接打印查看结果。

3. 仿真执行

    在仿真阶段，FIR计算结构不需要进行优化，数据以numpy.array类型流通。在新的环境下，fcompile.config.SIM_ROOT需要根据sim文件夹的路径进行更改。

## TVM Relay Accel IR --> FIR --> FPGA

FPGA代码的生成不需要特殊环境。由于FPGA代码涉及C语言，其开发较为精细，所以后续需要根据实际情况调整达到更好的效果。

具体参考函数如下：

```python
# python/main.py
main_accel_ccodegen()

# python/main.py
main_accel_extern_ccodegen()
```

### FPGA Jit

首先需要将FIR进行优化，主要包括权重加载和离线处理，数据Map以及运行时不同数据类型的转换。 其次，将模型静态化导出也是需要的。其导出的格式为python中的dict数据类型，其概括与说明如下：

```python
[
    {
        "node_type" : "call", # var, const, call, extern, output
        "static" : {
            "name" : name,
            "type" : OpType.xxx,
            "attrs" : [xxx]
        }, # static global variable
        "callop" : [
            op1,
            op2,
        ], # string op function list in run module
        "params" : [
            op1,
            op2,
        ], # op function in param module
        "return" : {
            "name" : name,
            "type" : OpType.xxx,
            "shape" : shape,
            "free" : free
        } # used as arg for the next operator
          # most of the time not used in codegen
    }
]
```

后续的代码或模型的生成均依赖此结构。

### Target C Code Generator

C语言版本的代码生成器，即将Accel算子和整体计算图全部转换为C语言模型文件，并依赖同时生成的权重bin文件和扩展算子对应的C语言文件。后续编写对应的main.c文件即可。

此FPGA部署代码在template中。

## Development

__2023.10.12__

1. 在tvm/src中注册算子，先添加相应的attrs在include中，然后在src/relay/op中添加make方法，注册算子

2. 在tvm/python中添加对应算子的make函数和attrs的数据类型注册

3. 开发后端编译器的AST，命名为FIR，其中包含FVar，FCVar，FCall，FExtern，FModule等类型，FTuple暂未支持

4. 对于FIR开发遍历方法，可以通过继承Funtor或Mutator并重载相应的visit函数实现

5. 在后端编译器中添加对应的算子，主要包含仿真函数和fpga_jit静态化函数

6. 开发相应的优化方法，使得计算图更连续，可生成更直接的jit_mod

7. 开发CodeGen对象，目前主要开发了C语言对象方法

__2023.10.14__

1. 对于modelsim的仿真，需要先建立sim仿真项目，由Makefile编写控制，然后由python控制Makefile完成

2. 数据的输入以memh进行，在testbench中使用readmemh读取数据，writememh写出结果

3. extern算子中直接通过调用TVM Graph Execution计算完成