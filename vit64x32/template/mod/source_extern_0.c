// tvm target: c -keys=cpu -link-params=0
#include "tvm/runtime/c_runtime_api.h"
#include "tvm/runtime/c_backend_api.h"
#include <math.h>
#ifdef __cplusplus
#endif
static int32_t fused_transpose(void* args, void* arg_type_ids, int32_t num_args, void* out_ret_value, void* out_ret_tcode, void* resource_handle) {
  void* arg0 = (((TVMValue*)args)[0].v_handle);
  int32_t arg0_code = ((int32_t*)arg_type_ids)[(0)];
  void* arg1 = (((TVMValue*)args)[1].v_handle);
  int32_t arg1_code = ((int32_t*)arg_type_ids)[(1)];
  void* placeholder = (((DLTensor*)arg0)[0].data);
  void* arg0_shape = (((DLTensor*)arg0)[0].shape);
  void* arg0_strides = (((DLTensor*)arg0)[0].strides);
  int32_t dev_id = (((DLTensor*)arg0)[0].device.device_id);
  void* T_transpose = (((DLTensor*)arg1)[0].data);
  void* arg1_shape = (((DLTensor*)arg1)[0].shape);
  void* arg1_strides = (((DLTensor*)arg1)[0].strides);
  if (!(arg0_strides == NULL)) {
  }
  if (!(arg1_strides == NULL)) {
  }
  for (int32_t ax0_ax1_fused = 0; ax0_ax1_fused < 64; ++ax0_ax1_fused) {
    for (int32_t ax2 = 0; ax2 < 24; ++ax2) {
      for (int32_t ax3_outer = 0; ax3_outer < 2; ++ax3_outer) {
        for (int32_t ax3_inner = 0; ax3_inner < 16; ++ax3_inner) {
          if (((ax3_outer * 16) + ax3_inner) < 24) {
            ((int8_t*)T_transpose)[(((((ax0_ax1_fused * 576) + (ax2 * 24)) + (ax3_outer * 16)) + ax3_inner))] = ((int8_t*)placeholder)[(((((ax2 * 1536) + (ax3_outer * 1024)) + (ax3_inner * 64)) + ax0_ax1_fused))];
          }
        }
      }
    }
  }
  return 0;
}


void tvm_extern6(void* args, int32_t* arg_type_ids) {
  fused_transpose(args, arg_type_ids, 3, 0, 0, 0);
}