#ifndef __OP_BASE__
#define __OP_BASE__

#include <vector>

#include <svir/expr.h>
#include <svir/base.h>

#define MERGE_BODY(b) static bool __state##b

#define MERGE(b) MERGE_BODY(b)

#define OP_REG(OpName, inum, onum, rel)  \
      MERGE(__COUNTER__) = Op::Register(OpName, inum, onum, rel)

namespace svir {
};


#endif
