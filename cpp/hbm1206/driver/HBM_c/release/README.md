# HBM Driver

conv.cpp, matrix.cpp中的函数：FPGA_MVM, FPGA_Run_Softmax, FPGA_Run_LN, FPGA_Run_Transpose, FPGA_Run_Activation与testbench验证完成。验证方法为：随机数输入尺寸，在cpp和testbench中分别验证对寄存器的输入，验证成功。

conv.cpp中实现了Get_MVM_Cfg函数，此函数对应的python版本为get_hbm_mvm_cfg.py，所需要用到的宏定义在conv.h中定义。FPGA_MVM以及FPGA_MVM_single_time的编写基本实现对tasks的完全对应，包含Left_WT_Base_Addr, log2_WT_base_addr_Bank_Step的参数输入，默认为8和0.

matrix.cpp中所用到的宏定义在matrix.h中定义.

+ MVM函数中，根据testbench，将Height以Tout进行分割，分割后shape以HWC为layout变化为((Height+Tout-1)/Tout, Tout, Width_in)，以此shape作为真正输入进行测试
+ 部分函数中多次用到CHin_Padding_Tout以及对Width的Padding，均在driver中处理，feature数据的输入尺寸以真实尺寸为准而不是padding后的尺寸
+ 在test/main.cpp中编写了对应函数的调用测试，可参考实现。FPGA_Run_Activation需要在调用时同时include approx_pwlf_act.h以直接调用其中的激活函数参数
