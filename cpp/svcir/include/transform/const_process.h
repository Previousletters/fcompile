#ifndef __TRANSFORM_CONST_PROCESS__
#define __TRANSFORM_CONST_PROCESS__

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

#include <svir/base.h>
#include <svir/expr.h>
#include <svir/expr_functor.h>
#include <driver/basic.h>
#include <driver/process.h>

using namespace svir;

namespace transform {

class ConstProcess : public Mutator {
  public:
    void Visit_(Constant* constant) final {
        if (constant->dtype == "feature") {
            int chout = constant->shape[0] / 2;
            DlTensor tensor;
            tensor.size = hbm::GetSizeFeature(1, 1, 2*chout);
            tensor.data = (void*)malloc(tensor.size);
            std::string wfname = constant->fname[0];
            std::string bfname = constant->fname[1];
            uint16_t* bn_wt = get_from_bin(wfname, chout);
            uint16_t* bn_bias = get_from_bin(bfname, chout);
            hbm::BN_TRANS(chout, bn_wt, bn_bias, (int*)tensor.data);
            constant->tensors.push_back(tensor);
        } else if (constant->dtype == "weight") {
            int chin = constant->shape[0];
            int chout = constant->shape[1];
            int* HBM_DDR[32];
            for (int i = 0; i < 32; i++) {
                DlTensor tensor;
                tensor.size = hbm::GetSizeWeight(chin, chout);
                tensor.data = (void*)malloc(tensor.size);
                HBM_DDR[i] = (int*)tensor.data;
                constant->tensors.push_back(tensor);
            }
            std::string wfname = constant->fname[0];
            std::string sfname = constant->fname[1];
            int* wt = get_from_bin(wfname, chin, chout);
            uint16_t* wt_scale = get_from_bin(sfname, chout*chin/128);
            hbm::WT_TRANS(chin, chout, wt, wt_scale, HBM_DDR);
        }
    }

  private:
    uint16_t* get_from_bin(const std::string& name, int chout) {
        uint16_t* data = (uint16_t*)calloc(chout, sizeof(uint16_t));
        if (name == "")
            return data;
        struct stat statbuf;
        stat(name.c_str(), &statbuf);
        size_t filesize = statbuf.st_size;
        std::ifstream inF(name, std::ios::binary);
        if (filesize != chout*sizeof(uint16_t))
            std::cerr << name << " file check error, wanted " << chout*sizeof(uint16_t) << ", got " << filesize << "!\n";
        inF.read((char*)data, filesize);
        inF.close();
        return data;
    }
    int* get_from_bin(const std::string& name, int chin, int chout) {
        int* data = (int*)calloc(chout*chin, sizeof(int));
        if (name == "")
            return data;
        struct stat statbuf;
        stat(name.c_str(), &statbuf);
        size_t filesize = statbuf.st_size;
        std::ifstream inF(name, std::ios::binary);
        if (filesize != chout*chin*sizeof(int))
            std::cerr << name << " file check error, wanted " << chout*chin*sizeof(int) << ", got " << filesize << "!\n";
        inF.read((char*)data, filesize);
        inF.close();
        return data;
    }
};

};


#endif
