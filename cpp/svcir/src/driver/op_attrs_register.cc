#include <iostream>
#include <vector>

#include <op/base.h>
#include <op/attrs.h>
#include <driver/basic.h>
#include <driver/conv.h>
#include <driver/matrix.h>

namespace svir {

hbm::regops_t csb_mvm(std::vector<void*> args, Attrs* attrs) {
    hbm::Mapped_Feature* input = static_cast<hbm::Mapped_Feature*>(args[0]);
    hbm::Mapped_Weight* weight = static_cast<hbm::Mapped_Weight*>(args[1]);
    hbm::Mapped_Feature* output = static_cast<hbm::Mapped_Feature*>(args[2]);
    MVMAttrs* mvm_attr = static_cast<MVMAttrs*>(attrs);
    hbm::FPGA_RunHBM_MVM(mvm_attr->relu, hbm::mode_mvm, input, weight, output, mvm_attr->skip, mvm_attr->out_to_BRAM, mvm_attr->in_from_BRAM, mvm_attr->out_base_addr, mvm_attr->in_base_addr, 0, mvm_attr->log2_bank_step, mvm_attr->left_wt_base_addr);
    return hbm::CSB_Finish();
}

OP_ATTRS_REG("mvm", "csb_driver", (void*)(&csb_mvm));

hbm::regops_t csb_mvm_bn(std::vector<void*> args, Attrs* attrs) {
    hbm::Mapped_Feature* input = static_cast<hbm::Mapped_Feature*>(args[0]);
    hbm::Mapped_Weight* weight = static_cast<hbm::Mapped_Weight*>(args[1]);
    hbm::Mapped_Feature* bn = static_cast<hbm::Mapped_Feature*>(args[2]);
    hbm::Mapped_Feature* output = static_cast<hbm::Mapped_Feature*>(args[3]);
    MVMAttrs* mvm_attr = static_cast<MVMAttrs*>(attrs);
    hbm::FPGA_RunHBM_MVM_BN(mvm_attr->relu, hbm::mode_mvm_bn, input, weight, bn, output, mvm_attr->skip, mvm_attr->out_to_BRAM, mvm_attr->in_from_BRAM, mvm_attr->out_base_addr, mvm_attr->in_base_addr, 0, mvm_attr->log2_bank_step, mvm_attr->left_wt_base_addr);
    return hbm::CSB_Finish();
}

OP_ATTRS_REG("mvm_bn", "csb_driver", (void*)(&csb_mvm_bn));

hbm::regops_t csb_transpose(std::vector<void*> args, Attrs* attrs) {
    hbm::Mapped_Feature* input = static_cast<hbm::Mapped_Feature*>(args[0]);
    hbm::Mapped_Weight* weight = static_cast<hbm::Mapped_Weight*>(args[1]);
    TransposeAttrs* new_attr = static_cast<TransposeAttrs*>(attrs);
    hbm::FPGA_Run_Transpose(input, weight, new_attr->out_and_in_mode, 0, new_attr->log2_bank_step, new_attr->left_wt_base_addr);
    return hbm::CSB_Finish();
}

OP_ATTRS_REG("transpose", "csb_driver", (void*)(&csb_transpose));

hbm::regops_t csb_layer_norm(std::vector<void*> args, Attrs* attrs) {
    hbm::Mapped_Feature* input = static_cast<hbm::Mapped_Feature*>(args[0]);
    hbm::Mapped_Feature* weight = static_cast<hbm::Mapped_Feature*>(args[1]);
    hbm::Mapped_Feature* output = static_cast<hbm::Mapped_Feature*>(args[2]);
    LayerNormAttrs* new_attr = static_cast<LayerNormAttrs*>(attrs);
    hbm::FPGA_Run_LN(input, weight, output, 0, new_attr->out_and_in_mode);
    return hbm::CSB_Finish();
}

OP_ATTRS_REG("layer_norm", "csb_driver", (void*)(&csb_layer_norm));

};
