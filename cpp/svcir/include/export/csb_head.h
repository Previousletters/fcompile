#ifndef __EXPORT_CSB_HEAD__
#define __EXPORT_CSB_HEAD__

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <map>
#include <utility>

#include <svir/base.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>

#include <driver/basic.h>

using namespace svir;

namespace backend {

class CSBHead : public Functor<void*> {
  public:
    void Test(SVExpr* expr) {
        void* output = this->Visit(expr);
        std::cout << csb_regs.size() << std::endl;
    }

  private:
    typedef hbm::regops_t (*csb_handle_t)(std::vector<void*>, Attrs*);

    void* Malloc(SVTensor* var) {
        if (var->dtype == "feature") {
            int height = var->shape[1];
            int width = var->shape[2];
            int channel = var->shape[3];
            return static_cast<void*>(hbm::Malloc_Feature(height, width, channel, 0, 0, 16));
        } else if (var->dtype == "weight") {
            int ky = var->shape[0];
            int kx = var->shape[1];
            int chin = var->shape[2];
            int chout = var->shape[3];
            return static_cast<void*>(hbm::Malloc_Weight(ky, kx, chin, chout, 0, 4));
        } else {
            std::cerr << "unknown type!" << std::endl;
            return nullptr;
        }
    }

    void* Visit_(SVVar* var) final {
        void* mapped_ = this->Malloc(var->checked->as<SVTensor>());
        if (var->checked->as<SVTensor>()->dtype == "feature") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Feature*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Feature*)mapped_)->payload_size;
            var_inputs[var->name] = std::make_pair(addr, addr_size);
        } else if (var->checked->as<SVTensor>()->dtype == "weight") {
            uint64_t addr = (uint64_t)(((hbm::Mapped_Weight*)mapped_)->payload);
            int addr_size = ((hbm::Mapped_Weight*)mapped_)->payload_size;
            var_weights[var->name] = std::make_pair(addr, addr_size);
        }
        return mapped_;
    }

    void* Visit_(SVCall* call) final {
        std::vector<void*> new_args;
        for (auto arg : call->args) {
            new_args.push_back(this->Visit(arg));
        }
        new_args.push_back(this->Malloc(call->checked->as<SVTensor>()));
        hbm::regops_t regs = ((csb_handle_t)call->op->attributes["csb_driver"])(new_args, call->attrs);
        csb_regs.push_back(regs);
        return *(new_args.end()-1);
    }

    std::vector<hbm::regops_t> csb_regs;
    std::map<std::string, std::pair<uint64_t, int>> var_inputs;
    std::map<std::string, std::pair<uint64_t, int>> var_weights;

};

};

#endif
