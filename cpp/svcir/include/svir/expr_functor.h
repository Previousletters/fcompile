#ifndef __IR_EXPR_FUNCTOR__
#define __IR_EXPR_FUNCTOR__

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <svir/expr.h>

namespace svir {

template<typename T>
class Functor {
  public:
    T Visit(Expr* expr) {
        if (memo_.find(expr) != memo_.end()) {
            return memo_[expr];
        }
        if (expr->IsInstance<svir::Var>()) {
            T new_expr = Visit_(expr->as<svir::Var>());
            memo_[expr] = new_expr;
            return new_expr;
        } else if (expr->IsInstance<svir::Call>()) {
            T new_expr = Visit_(expr->as<svir::Call>());
            memo_[expr] = new_expr;
            return new_expr;
        } else if (expr->IsInstance<svir::Constant>()) {
            T new_expr = Visit_(expr->as<svir::Constant>());
            memo_[expr] = new_expr;
            return new_expr;
        }
        return T();
    }

    virtual T Visit_(Var* var) {
        return T();
    }
    virtual T Visit_(Call* call) {
        return T();
    }
    virtual T Visit_(Constant* constant) {
        return T();
    }

  private:
    std::unordered_map<Expr*, T> memo_;
};


class Mutator {
  public:
    void Visit(Expr* expr) {
        if (memo_.find(expr) != memo_.end()) {
            memo_[expr] ++;
        } else {
            if (expr->IsInstance<svir::Var>()) {
                Visit_(expr->as<svir::Var>());
                memo_[expr] = 1;
            } else if (expr->IsInstance<svir::Call>()) {
                Visit_(expr->as<svir::Call>());
                memo_[expr] = 1;
            } else if (expr->IsInstance<svir::Constant>()) {
                Visit_(expr->as<svir::Constant>());
                memo_[expr] = 1;
            }
        }
    }

    virtual void Visit_(Var* var) {
    }
    virtual void Visit_(Constant* constant) {
    }
    virtual void Visit_(Call* call) {
        for (auto arg : call->args) {
            Mutator::Visit(arg);
        }
    }

  private:
    std::unordered_map<Expr*, int> memo_;
};

};

#endif
