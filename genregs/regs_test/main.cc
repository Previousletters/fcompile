#include <iostream>
#include "lib.h"
// #include "chatglm_dynamic_control_1024_0428_1254.h"
// #include "chatglm_dynamic_control_1024_0430_1443.h"
// #include "chatglm_dynamic_control_128_0509_1942.h"
// #include "chatglm_dynamic_control_128_0509_2007.h"
#include "chatglm2_2048_0719_2217.h"
#include "chatglm2_2048_0720_0013.h"

int main(void) {
    HANDLE device0, device1;
    int token = 18, kvcache = 1, last_token = 0;
    step2(device0, kvcache, token, last_token);
    target2(device1, kvcache, 17, last_token);
    for (int i = 0; i < device1.size(); i++) {
        int same = int(device1[i].second == device0[i].second);
        std::cout << device1[i].first << ": " << device1[i].second << " : " << device0[i].second << " < " << same << " > " << std::endl;
    }
    std::cout << int(device0.size() == device1.size()) << std::endl;
    return 0;
}
