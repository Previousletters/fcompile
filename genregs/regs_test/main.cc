#include <iostream>
#include "lib.h"
//#include "chatglm_dynamic_control_0418_1636.h"
#include "chatglm_with_kvcache_28_0422_2029.h"
#include "chatglm_dynamic_control_0422_2029.h"

int main(void) {
    HANDLE device0, device1;
    chatglm_dynamic_control_0422_2029(device0, 20, 1);
    chatglm_with_kvcache_28_0422_2029(device1);
    if (device0.size() != device1.size()) {
        return 1;
    } else {
        for (int i = 0; i < device0.size(); i++) {
            if (device0[i].first != device1[i].first || device0[i].second != device1[i].second) {
                printf("*Error* : %d, dynamic_control: %d, %d; without_kvcache: %d, %d\n", i, device0[i].first, device0[i].second, device1[i].first, device1[i].second);
            }
        }
        std::cout << "Check Finish! Total " << device0.size() << " regs!" << std::endl;
    }
    return 0;
}
