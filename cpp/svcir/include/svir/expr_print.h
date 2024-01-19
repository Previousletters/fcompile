#ifndef __IR_EXPR_PRINT__
#define __IR_EXPR_PRINT__

#include "expr.h"
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <svir/expr.h>
#include <svir/expr_functor.h>

namespace svir {

std::string shape_str(std::vector<int> shape, std::string dtype) {
    std::stringstream str;
    str << "/* " << "ret=(";
    for (int i = 0; i < shape.size()-1; i++) {
        str << shape[i] << ", ";
    }
    str << *(shape.end()-1) << "), dtype=" << dtype << " */";
    return str.str();
}

std::string vector_str(std::vector<std::string> var) {
    std::stringstream str;
    for (int i = 0; i < var.size()-1; i++) {
        str << var[i] << ", ";
    }
    str << *(var.end()-1);
    return str.str();
}

class Print : public Functor<std::string> {
  public:
    std::string print(Expr* expr) {
        id_ = 0;
        sstream_.str("");
        svar_ = {};
        std::string output = Functor<std::string>::Visit(expr);
        sstream_ << "  " << "return " << output << ";" << std::endl;
        std::string show = "def main(" + vector_str(svar_) + ") {\n" + sstream_.str() + "}";
        return show;
    }

    std::string Visit_(Constant* constant) final {
        sstream_ << "  "  << constant->name << " = " << "constant(" << constant->dtype << ", ";
        for (int i = 0; i < constant->fname.size()-1; i++) {
            sstream_ << "\"" << constant->fname[i] << "\"" << ", ";
        }
        sstream_ << "\"" << *(constant->fname.end()-1) << "\");";
        if (constant->checked) {
            sstream_ << " " << shape_str(constant->checked->as<Tensor>()->shape, constant->checked->as<Tensor>()->dtype);
        }
        sstream_ << std::endl;
        return constant->name;
    }

    std::string Visit_(Var* var) final {
        sstream_ << "  "  << var->name << " = " << "var();";
        svar_.push_back(var->name);
        if (var->checked) {
            sstream_ << " " << shape_str(var->checked->as<Tensor>()->shape, var->checked->as<Tensor>()->dtype);
        }
        sstream_ << std::endl;
        return var->name;
    }

    std::string Visit_(Call* call) final {
        std::vector<std::string> args;
        for (auto arg : call->args) {
            args.push_back(Functor<std::string>::Visit(arg));
        }
        id_ ++;
        std::string out = "%" + std::to_string(id_);
        sstream_ << "  " << out << " = " << call->op->op_type << "(";
        for (int i = 0; i < args.size()-1; i++) {
            sstream_ << args[i] << ", ";
        }
        sstream_ << *(args.end()-1) << ");";
        if (call->checked) {
            sstream_ << " " << shape_str(call->checked->as<Tensor>()->shape, call->checked->as<Tensor>()->dtype);
        }
        sstream_ << std::endl;
        return out;
    }

  private:
    std::stringstream sstream_;
    std::vector<std::string> svar_;
    int id_;
};

};

#endif
