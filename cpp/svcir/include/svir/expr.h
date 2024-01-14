#ifndef __SVIR_EXPR__
#define __SVIR_EXPR__

#include <iostream>
#include <string>
#include <vector>

#include <svir/base.h>

namespace svir {

class SVVar : public SVExpr {
  public:
    SVVar(const std::string& name, const std::vector<int>& shape)
           : name(std::move(name)), shape(std::move(shape)) {
        type_index_ = TypeDefs().Var;
    };

    static constexpr int _type_index = TypeDefs::Var;

    std::string name;
    std::vector<int> shape;
};

class SVConstant : public SVExpr {
  public:
    SVConstant(const std::string& name, const std::vector<int>& shape, Tensor data)
           : name(std::move(name)), shape(std::move(shape)), data(std::move(data)){
        type_index_ = TypeDefs().Constant;
    };

    static constexpr int _type_index = TypeDefs::Constant;

    std::string name;
    std::vector<int> shape;
    Tensor data;
};

class SVCall : public SVExpr {
  public:
    SVCall(Op* op, const std::vector<SVExpr*>& args, Attrs* attrs)
           : args(std::move(args)), op(std::move(op)), attrs(std::move(attrs)) {
        type_index_ = TypeDefs().Call;
    };

    static constexpr int _type_index = TypeDefs::Call;

    std::vector<SVExpr*> args;
    Op* op;
    Attrs* attrs;
};

class SVTensor : public SVExpr {
  public:
    SVTensor(const std::vector<int>& shape, const std::string& dtype)
           : shape(std::move(shape)), dtype(std::move(dtype)) {
        type_index_ = TypeDefs().Tensor;
    };

    static constexpr int _type_index = TypeDefs::Tensor;

    std::vector<int> shape;
    std::string dtype;
};

};

#endif
