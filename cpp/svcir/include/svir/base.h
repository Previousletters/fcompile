#ifndef __SVIR_BASE__
#define __SVIR_BASE__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace svir {

struct TypeDefs {
    enum TypeDef {
        Expr,
        Var,
        Call,
        Tensor,
        Tuple
    };
};

class SVExpr {
  public:
    SVExpr() = default;
    SVExpr(int type_index) : type_index_(std::move(type_index)) {};

    template<typename TargetObj>
    bool IsInstance() const{
        return type_index_ == TargetObj::_type_index;
    };

    template<typename TargetObj>
    TargetObj* as(){
        if (this->type_index_ == TargetObj::_type_index) {
            return static_cast<TargetObj*>(this);
        }
        else {
            return nullptr;
        }
    };

    static constexpr int _type_index = TypeDefs::Expr;
    SVExpr* checked = nullptr; 

  protected:
    int type_index_;
};

struct Attrs {
};

struct Report {
    std::vector<SVExpr*> args;
    SVExpr* out;
};

typedef bool (*InferRel_t)(std::vector<SVExpr*>, Attrs, Report*);

class Op {
  public:
    static std::unordered_map<std::string, Op> memo_;
    Op() = default;
    Op(const std::string& name, int in_num, int out_num, InferRel_t infer)
      : op_type(std::move(name)), in_num(std::move(in_num)), out_num(std::move(out_num)), infer(std::move(infer)) {};

    static bool Register(const std::string& name, int in_num, int out_num, InferRel_t infer) {
        memo_[name] = Op(name, in_num, out_num, infer);
        return true;
    }

    static Op* Get(const std::string& name) {
        if (memo_.find(name) != memo_.end())
            return &memo_[name];
        else {
            std::cerr << "No found operator " << name << ", please register!" << std::endl;
            return nullptr;
        }
    }
    
    std::string op_type;
    int in_num;
    int out_num;
    InferRel_t infer;
};


};

#endif
