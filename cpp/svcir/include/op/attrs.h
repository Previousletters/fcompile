#ifndef __OP_ATTRS__
#define __OP_ATTRS__

#include <svir/base.h>

namespace svir {

struct ActivateAttrs : public Attrs {
    int out_and_in_mode;
};

struct MVMAttrs : public Attrs {
    int relu;
    int skip;
    int out_to_BRAM;
    int in_from_BRAM;
    int out_base_addr;
    int in_base_addr;
    int log2_bank_step;
    int left_wt_base_addr;
};

struct ElementAttrs : public Attrs {
    int element_mode;
    int out_and_in_mode;
};

struct TransposeAttrs : public Attrs {
    int out_and_in_mode;
    int log2_bank_step;
    int left_wt_base_addr;
};

struct LayerNormAttrs : public Attrs {
    int out_and_in_mode;
};

struct SoftmaxAttrs : public Attrs {
    int out_and_in_mode;
};

struct F2WeightAttrs : public Attrs {
    int out_and_in_mode;
    int log2_bank_step;
    int left_wt_base_addr;
};

struct PosEmbAttrs : public Attrs {
    int out_and_in_mode;
    int pos_numb;
};

};

#endif
