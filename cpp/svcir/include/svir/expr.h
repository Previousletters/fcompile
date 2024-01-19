#ifndef __IR_EXPR__
#define __IR_EXPR__

#include <iostream>
#include <string>
#include <vector>

#include <svir/base.h>

namespace svir {

class Var : public Expr {
  public:
    Var(const std::string& name, const std::vector<int>& shape)
           : name(std::move(name)), shape(std::move(shape)) {
        type_index_ = TypeDefs().Var;
    };

    static constexpr int _type_index = TypeDefs::Var;

    std::string name;
    std::vector<int> shape;
};

class Constant : public Expr {
  public:
    Constant(const std::string& name, const std::vector<int>& shape, const std::string& dtype, const std::vector<std::string>& fname)
           : name(std::move(name)), shape(std::move(shape)), dtype(std::move(dtype)), fname(std::move(fname)) {
        type_index_ = TypeDefs().Constant;
    };

    static constexpr int _type_index = TypeDefs::Constant;

    std::vector<DlTensor> tensors;
    std::string name;
    std::vector<int> shape;
    std::string dtype;
    std::vector<std::string> fname;
};

class Call : public Expr {
  public:
    Call(Op* op, const std::vector<Expr*>& args, Attrs* attrs)
           : args(std::move(args)), op(std::move(op)), attrs(std::move(attrs)) {
        type_index_ = TypeDefs().Call;
    };

    static constexpr int _type_index = TypeDefs::Call;

    std::vector<Expr*> args;
    Op* op;
    Attrs* attrs;
};

class Tensor : public Expr {
  public:
    Tensor(const std::vector<int>& shape, const std::string& dtype)
           : shape(std::move(shape)), dtype(std::move(dtype)) {
        type_index_ = TypeDefs().Tensor;
    };

    static constexpr int _type_index = TypeDefs::Tensor;

    std::vector<int> shape;
    std::string dtype;
};

};

#endif
