## tasks_driver_HBM_MVM

#### conv.cpp翻译说明：

1. 在49寄存器中，Left_WT_Base_Addr被赋值为0，log2_WT_base_addr_Bank_Step在testbench中直接被赋值为8，具体含义不太清楚，在cpp中直接简化为写入8.

```systemverilog
AXI_Lite_Write(49, {Left_WT_Base_Addr,log2_WT_base_addr_Bank_Step});// tasks_driver_HBM_MVM.vh 313行
```

2. 在修改时保证了函数名和输入参数相同，因此有些输入参数没有被用到，但是依旧做了保留。

```cpp
// in_feature_dw, wt_scale_dw在函数中没有被用到，理论来说 wt_dw 参数应该恒定为 MAX_WT_DW，即4
struct MVM_Cfg Get_MVM_Cfg(int Hin, int Win, int CHin, int CHout, int in_feature_dw, int wt_dw, int wt_scale_dw);
```

3. 新修改的conv.cpp中由于引用了许多有关HBM的宏，所以需要额外include TOP_defines.h，由于没有conv.h的情况，所以默认做了引用，没有在conv.cpp中添加include.

4. 在Get_MVM_Cfg函数中，计算total_bytes_if_reuse_wt和total_bytes_if_reuse_dat时，以当前的代码（包括tasks.vh）都存在先引用计算再赋值的问题，主要是CHout_Split_Times和Hout_Split_Times两个参数，最终会导致best_method计算出错，目前已移至最后。

5. 相比MVM参考版本，HBM版本主要修改的地方为:

    Get_MVM_Cfg函数中 WT_scale_bits 参数的计算，引入了 WT_scale_group_nums.

    FPGA_MVM函数中，在调用 FPGA_MVM_single_time 函数时 wt_base_addr 的计算需要额外除以 HBM_Port

    添加了对49寄存器的配置

    对4中提到的问题的修改，主要为 total_bytes_if_reuse_wt 和 total_bytes_if_reuse_dat 的计算，引用了未赋值的 CHout_Split_Times 和 Hout_Split_Times 参数，望检查确认。

#### get_hbm_mvm_cfg.py翻译说明：

+ 已与CPP代码对齐，主要翻译了Get_MVM_Cfg函数，此代码中只保留了get_hbm_mvm_cfg函数，其他无用函数均已去除，无依赖，可直接移植

## tasks_SOFTMAX

matrix.cpp 翻译说明：

1. 此代码翻译参考原VIT加速器中的matrix.cpp，删除了pixel_out输入，由CHin代替

2. 删除了13，16号寄存器对scale信息的描述。尽管tasks文件中并对26号写入Out_and_In_Mode参数，但在CPP依然做了保留

## tasks_ACT

matrix.cpp 翻译说明：

1. 根据tasks_ACT，添加了三个新数组的输入，wt, bias, x_region，此数据的生成由python提供，目前已生成[-4, 4]的激活函数，存储在approx_pwlf_act.h，可直接引用。

act_data.py 说明：

1. 此文件的核心拟合过程由黄思晓提供，在此基础上重新做了封装和输出。approx_pwlf_act.h文件为生成好的激活函数[-4, 4]，可直接使用，中间拟合图片和数据也会保存

## tasks_LN

matrix.cpp 翻译说明：

1. 参照vit版本，做了比较大的更改，并且在matrix.cpp中添加了FP32_to_FP20函数，以int数据类型表示。由于此数据似乎可以离线计算，所以目前以驱动函数参数的形式给定，而非tasks_LN中写在驱动函数里面。目前python版本还没更新。

## tasks_TRANSPOSE

matrix.cpp 翻译说明：

1. 函数名和输入参数并未做修改，内容上与tasks做了对应

