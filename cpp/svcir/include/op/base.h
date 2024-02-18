#ifndef __OP_BASE__
#define __OP_BASE__

#include <vector>

#include <svir/expr.h>
#include <svir/base.h>

#define OP_REG(OpName, inum, onum, rel)  \
      MERGE(__op_reg, __COUNTER__) = Op::Register(OpName, inum, onum, rel)

#define OP_ATTRS_REG(OpName, AttrName, AttrFunc)  \
      MERGE(__op_attrs_reg, __COUNTER__) = Op::RegisterAttrs(OpName, AttrName, AttrFunc)

namespace svir {

Expr* mvm(Expr* data, Expr* weight, int relu, int skip, int log2_bank_step=28, int left_wt_base_addr=0);

Expr* mvm_bn(Expr* data, Expr* weight, Expr* bn, int relu, int skip, int log2_bank_step=28, int left_wt_base_addr=0);

Expr* layer_norm(Expr* data, Expr* weight, int out_and_in_mode=0);

Expr* transpose(Expr* input, int out_and_in_mode=0, int log2_bank_step=28, int left_wt_base_addr=0);

Expr* softmax(Expr* input, int out_and_in_mode=0);

Expr* feature2weight(Expr* input, int out_and_in_mode=0, int log2_bank_step=28, int left_wt_base_addr=0);

Expr* rotary_pos_emb(Expr* input, Expr* pos_in, int pos_numb, int out_and_in_mode);

Expr* add(Expr* data_a, Expr* data_b, int out_and_in_mode=0);

Expr* mul(Expr* data_a, Expr* data_b, int out_and_in_mode=0);

Expr* minus(Expr* data_a, Expr* data_b, int out_and_in_mode=0);

Expr* silu(Expr* data, Expr* approx, int out_and_in_mode=0);

};


#endif
