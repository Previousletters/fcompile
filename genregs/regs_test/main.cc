#include <iostream>
#include "lib.h"
// #include "chatglm_dynamic_control_1024_0428_1254.h"
// #include "chatglm_dynamic_control_1024_0430_1443.h"
// #include "chatglm_dynamic_control_128_0509_1942.h"
// #include "chatglm_dynamic_control_128_0509_2007.h"
#include "kvmvm_token_0709_2032.h"

int main(void) {
    HANDLE device0;
    int token = 1;
    kvmvm_token_0709_2032(device0, token);
    for (auto i : device0) {
        std::cout << i.first << ": " << i.second << std::endl;
    }
    std::cout << 32 % 32 << std::endl;
    return 0;
}
