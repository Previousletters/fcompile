#include <iostream>
#include "lib.h"
// #include "chatglm_dynamic_control_1024_0428_1254.h"
// #include "chatglm_dynamic_control_1024_0430_1443.h"
#include "chatglm_dynamic_control_128_0509_1942.h"
#include "chatglm_dynamic_control_128_0509_2007.h"

int main(void) {
    HANDLE device0, device1;
    int token = 19, kvcache = 0;
    chatglm_dynamic_control_128_0509_1942(device0, token, kvcache, 0);
    chatglm_dynamic_control_128_0509_2007(device1, token, kvcache, 0);
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
