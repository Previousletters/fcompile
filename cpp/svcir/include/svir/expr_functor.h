#ifndef __SVIR_EXPR_FUNCTOR__
#define __SVIR_EXPR_FUNCTOR__

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <svir/expr.h>

namespace svir {

template<typename T>
class Functor {
  public:
    T Visit(SVExpr* expr) {
        if (memo_.find(expr) != memo_.end()) {
            return memo_[expr];
        }
        if (expr->IsInstance<svir::SVVar>()) {
            T new_expr = Visit_(expr->as<svir::SVVar>());
            memo_[expr] = new_expr;
            return new_expr;
        } else if (expr->IsInstance<svir::SVCall>()) {
            T new_expr = Visit_(expr->as<svir::SVCall>());
            memo_[expr] = new_expr;
            return new_expr;
        } else if (expr->IsInstance<svir::SVConstant>()) {
            T new_expr = Visit_(expr->as<svir::SVConstant>());
            memo_[expr] = new_expr;
            return new_expr;
        }
        return T();
    }

    virtual T Visit_(SVVar* var) {
        return T();
    }
    virtual T Visit_(SVCall* call) {
        return T();
    }
    virtual T Visit_(SVConstant* constant) {
        return T();
    }

  private:
    std::unordered_map<SVExpr*, T> memo_;
};


class Mutator {
  public:
    void Visit(SVExpr* expr) {
        if (memo_.find(expr) != memo_.end()) {
            memo_[expr] ++;
        } else {
            if (expr->IsInstance<svir::SVVar>()) {
                Visit_(expr->as<svir::SVVar>());
                memo_[expr] = 1;
            } else if (expr->IsInstance<svir::SVCall>()) {
                Visit_(expr->as<svir::SVCall>());
                memo_[expr] = 1;
            }
        }
    }

    virtual void Visit_(SVVar* var) {
    }
    virtual void Visit_(SVCall* call) {
    }

  private:
    std::unordered_map<SVExpr*, int> memo_;
};

};

#endif
