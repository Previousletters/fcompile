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
};


#endif
