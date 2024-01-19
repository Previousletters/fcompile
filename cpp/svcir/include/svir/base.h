#ifndef __IR_BASE__
#define __IR_BASE__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#define MERGE_BODY(a, b) static bool a##b

#define MERGE(a, b) MERGE_BODY(a, b)

namespace svir {

struct DlTensor {
    void* data;
    uint64_t size;
};

struct TypeDefs {
    enum TypeDef {
        Expr,
        Var,
        Constant,
        Call,
        Tensor,
        Tuple
    };
};

class Expr {
  public:
    Expr() = default;
    Expr(int type_index) : type_index_(std::move(type_index)) {};

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
    Expr* checked = nullptr; 

  protected:
    int type_index_;
};

struct Attrs {
};

struct Report {
    std::vector<Expr*> args;
    Expr* out;
};

typedef bool (*InferRel_t)(std::vector<Expr*>, Attrs*, Report*);

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

    static bool RegisterAttrs(const std::string& op_name, const std::string& attr_name, void* attr_func) {
        memo_[op_name].attributes[attr_name] = attr_func;
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
    std::unordered_map<std::string, void*> attributes;
};

};

#endif
