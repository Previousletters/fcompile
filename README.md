# FPGA-Backend Compile

编译流程:

    PyTorch(quantized model) --> TVM Relay IR --> Convert Pass --> 
    TVM Relay Accel IR --> FIR --> FPGA / RTL Sim

## Development

__2023.10.12__

1. 在tvm/src中注册算子，先添加相应的attrs在include中，然后在src/relay/op中添加make方法，注册算子

2. 在tvm/python中添加对应算子的make函数和attrs的数据类型注册

3. 开发后端编译器的AST，命名为FIR，其中包含FVar，FCVar，FCall，FExtern，FModule等类型，FTuple暂未支持

4. 对于FIR开发遍历方法，可以通过继承Funtor或Mutator并重载相应的visit函数实现

5. 在后端编译器中添加对应的算子，主要包含仿真函数和fpga_jit静态化函数

6. 开发相应的优化方法，使得计算图更连续，可生成更直接的jit_mod

7. 开发CodeGen对象，目前主要开发了C语言对象方法
