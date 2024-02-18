#ifndef __EXPORT_CSB_HEAD__
#define __EXPORT_CSB_HEAD__

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include <svir/base.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>
#include <driver/basic.h>

#include "csb_jit.h"
#include "utils.h"

using namespace svir;

namespace backend {

void* Malloc(Tensor* var) {
    if (var->dtype == "feature") {
        if (var->shape.size() == 1) {
            return static_cast<void*>(hbm::Malloc_Feature(1, 1, var->shape[0], 0, 0, 16));
        } else {
            int height = var->shape[0];
            int width = var->shape[1];
            int channel = var->shape[2];
            return static_cast<void*>(hbm::Malloc_Feature(height, width, channel, 0, 0, 16));
        }
    } else if (var->dtype == "weight") {
        int chin = var->shape[0];
        int chout = var->shape[1];
        return static_cast<void*>(hbm::Malloc_Weight(1, 1, chin, chout, 0, 4));
    } else {
        std::cerr << "unknown type!" << std::endl;
        return nullptr;
    }
}

class ConstMapping : public Mutator {
  public:
    std::map<Constant*, void*> Test(Expr* expr) {
        this->Visit(expr);
        return const_mapped;
    }

  private:
    void Visit_(Constant* constant) final {
        void* mapped_ = Malloc(constant->checked->as<Tensor>());
        const_mapped[constant] = mapped_;
    }

    std::map<Constant*, void*> const_mapped;
};


class CSBHead : public Functor<void*> {
  public:
    void Build(Expr* expr, const std::string& out_name) {
        const_mapped = ConstMapping().Test(expr);
        void* output = this->Visit(expr);
        uint64_t addr = (uint64_t)(((hbm::Mapped_Weight*)output)->payload);
        int addr_size = ((hbm::Mapped_Weight*)output)->payload_size;
        outputs.push_back({out_name, addr, addr_size});
    }

    void GetSource(const std::string& fname, const std::string& call_name) {
        std::string source = CSBCodeGen(csb_regs, var_ddr, var_hbm, const_ddr, const_hbm, outputs).JITSource(call_name);
        std::ofstream srcF(fname);
        srcF << source;
    }

    void SaveParams(const std::string& name) {
        std::ofstream ddrF(name + "_ddr.bin", std::ios::binary);
        if (!ddrF) {
            std::cout << "Error! Could not open file " << name + "_ddr.bin!" << std::endl;
            return;
        }
        for (auto& tensor : tensor_ddr) {
            ddrF.write(static_cast<char*>(tensor.data), tensor.size);
        }
        ddrF.close();
        for (int n = 0; n < 32; n++) {
            std::ofstream hbmF(name + "_hbm" + std::to_string(n) + ".bin", std::ios::binary);
            for (auto& tensors : tensor_hbm) {
                hbmF.write(static_cast<char*>(tensors[n].data), tensors[n].size);
            }
            hbmF.close();
        }
    }

  private:
    typedef hbm::regops_t (*csb_handle_t)(std::vector<void*>&, Expr*, Attrs*);

    void* Visit_(Constant* constant) final {
        void* mapped_ = const_mapped[constant];
        if (constant->checked->as<Tensor>()->dtype == "feature") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Feature*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Feature*)mapped_)->payload_size;
            const_ddr.push_back({constant->name, addr, addr_size});
            if (constant->tensors.size()) {
                tensor_ddr.push_back(constant->tensors[0]);
                if (constant->tensors[0].size != addr_size) 
                    std::cerr << "Error! Size not match!" << std::endl;
            }
        } else if (constant->checked->as<Tensor>()->dtype == "weight") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Weight*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Weight*)mapped_)->payload_size;
            const_hbm.push_back({constant->name, addr, addr_size});
            if (constant->tensors.size()) {
                tensor_hbm.push_back(constant->tensors);
                if (constant->tensors[0].size != addr_size) 
                    std::cerr << "Error! Size not match!" << std::endl;
            }
        }
        return mapped_;
    }

    void* Visit_(Var* var) final {
        void* mapped_ = Malloc(var->checked->as<Tensor>());
        if (var->checked->as<Tensor>()->dtype == "feature") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Feature*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Feature*)mapped_)->payload_size;
            var_ddr.push_back({var->name, addr, addr_size});
        } else if (var->checked->as<Tensor>()->dtype == "weight") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Weight*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Weight*)mapped_)->payload_size;
            var_hbm.push_back({var->name, addr, addr_size});
        }
        return mapped_;
    }

    void* Visit_(Call* call) final {
        std::vector<void*> new_args;
        for (auto arg : call->args) {
            new_args.push_back(this->Visit(arg));
        }
        hbm::regops_t regs = ((csb_handle_t)call->op->attributes["csb_driver"])(new_args, call->checked, call->attrs);
        if (regs.size())
            csb_regs.push_back(regs);
        return *(new_args.end()-1);
    }

    std::vector<hbm::regops_t> csb_regs;
    std::vector<PtrInfo> outputs;
    std::vector<PtrInfo> var_ddr;
    std::vector<PtrInfo> var_hbm;
    std::vector<PtrInfo> const_ddr;
    std::vector<PtrInfo> const_hbm;
    std::map<Constant*, void*> const_mapped;
    std::vector<DlTensor> tensor_ddr;
    std::vector<std::vector<DlTensor>> tensor_hbm;

};

};

#endif
