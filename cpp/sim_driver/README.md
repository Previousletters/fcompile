## 2024 0129

具体修改情况：

+ 地址生成情况

重新调整了地址生成的情况，一共为4种情况，inputs节点（数据输入节点），outputs节点（结果输出节点），ddrs节点（DDR权重参数节点），hbms节点（HBM权重参数节点），因此有些函数名做了更改。

以上顺序即为最终生成在.h文件中的顺序。除此之外，为了让生成的每个Block更清晰，提供了WeightAddrAnnotate函数以在地址生成的过程中生成对应的注释。

地址分配情况为从start地址开始顺序分配，所以DDR权重参数如果需要分配在固定地址应在程序开始处定义与分配并保证不被清除，HBM与DDR地址管理方案相同。

所以如果需要先存储DDR里的参数，可以在程序开头循环定义每个Block的参数，这样就能保证存储的地址顺序是按照Block划分，并且DDR参数的地址是连续。

+ 生命周期，自动清理功能

对 Mapped_Feature 和 Mapped_Weight 添加了 life_cycle 属性，每次驱动执行完毕之后（conv.h, matrix.h, others.h中的函数）之后会进行 AutoFree的判断和空间清理，如果 life_cycle 为1，则自动清理，否则-1

默认的 Malloc_Feature 和 Malloc_Weight 函数中 life_cycle 处于 DW 后一个参数，默认为0，即永不删除，需手动清理。

函数中还添加了检查机制，防止生命周期结束而不自知，为函数 ObjCheck ,已经添加，如果被清除会自动退出。

所以在使用中，需要手动计算每次该数据被用了几次，以精确进行空间分配。

+ DEBUG 

如果还是需要进行内存DEBUG，在FPGA_MEM.h中实现了FPGA_DDR_show函数，可以查看此时的内存分配情况。

## 2024 0130

驱动已更新为0130版本，并添加新的Split方式，可以根据vector中给定的channel拆分成多组，地址偏移为累加，具体可以直接std::cout << Mapped_Feature 查看。

所有的Mapped_Feature和Mapped_Weight可直接cout出当前状态，分别为Feature(Hin, Win, Chin, addr, bytes), Weight(1, 1, Chin, CHout, addr, bytes)。

所有驱动函数的使用方式未变，ChatGLM_Block代码更新完成，可参考查看。

HBM中的临时数据直接放在HBM权重后，已经考虑自动清除，如果不确定可以通过FPGA_HBM_show()函数查看具体HBM分配情况。
