 #include <string.h>
 #include <stdint.h>

void FC(float* data, float* weight, float* result, int H, int C, int O);

// static cache define
static uint8_t k_cache [32768];
static uint8_t v_cache [32768];

// "kvcache" layer wrapper function
void mod_mvm_0(float* in0, float* in1, float* out0) {
    FC(in0, in1, out0, 1, 128, 128);
}
void mod_load_1(float* out0, int seq) {
  memcpy((void*)8704, (void*)k_cache, (((1 * seq) * 128) * 2));
}
void mod_mvm_2(float* in0, float* in1, float* out0) {
    FC(in0, in1, out0, 1, 128, 128);
}
void mod_merge_3(float*in0, float* in1, int seq) {
  for (int loop_1 = 0; loop_1 < 4; loop_1++) {
    memcpy((void*)(49920 + (loop_1 * 4128)), (void*)(8704 + (loop_1 * ((seq * 32) * 2))), ((seq * 32) * 2));
    memcpy((void*)((49920 + ((seq * 32) * 2)) + (loop_1 * 4128)), (void*)(49664 + (loop_1 * 64)), 64);
  }
}
void mod_store_4() {
  memcpy((void*)k_cache, (void*)49920, 33024);
}
void mod_transpose_5() {

}
void mod_mvm_6() {

}
void mod_load_7(int seq) {
  memcpy((void*)91520, (void*)v_cache, (((1 * seq) * 128) * 2));
}
void mod_mvm_8() {

}
void mod_merge_9(int seq) {
  for (int loop_1 = 0; loop_1 < 4; loop_1++) {
    memcpy((void*)(132736 + (loop_1 * 4128)), (void*)(91520 + (loop_1 * ((seq * 32) * 2))), ((seq * 32) * 2));
    memcpy((void*)((132736 + ((seq * 32) * 2)) + (loop_1 * 4128)), (void*)(132480 + (loop_1 * 64)), 64);
  }
}
void mod_store_10() {
  memcpy((void*)v_cache, (void*)132736, 33024);
}
void mod_feature2weight_11() {

}
void mod_mvm_12() {

}

// "kvcache" model wrapper function
void kvcache(, int seq) {
  mod_mvm_0(d, qw, qd);
  mod_load_1(kc, seq);
  mod_mvm_2(d, kw, kd);
  mod_merge_3(kc, kd, seq);
  mod_store_4(device);
  mod_transpose_5(device);
  mod_mvm_6(device);
  mod_load_7(device, seq);
  mod_mvm_8(device);
  mod_merge_9(device, seq);
  mod_store_10(device);
  mod_feature2weight_11(device);
  mod_mvm_12(device);
}
