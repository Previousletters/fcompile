#include <cstdint>
#include <iostream>
#include <dlfcn.h>

class Module {
  public:
    typedef void (*(*pfunc_t)(int));
    typedef void (*func_t)();

    Module() : loaded(0) {}

    int load_model(const std::string& name) {
        handle = dlopen(name.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (handle == nullptr) 
            return 0;
        loaded = 1;
        run_ = (func_t)dlsym(handle, "TVMWrapRun");
        get_input_ptr = (pfunc_t)dlsym(handle, "TVMWrapGetInputPtr");
        get_output_ptr = (pfunc_t)dlsym(handle, "TVMWrapGetOutputPtr");
        std::cout << "Load model success: " << name << std::endl;
        return 1;
    }
    void* input_ptr(int index) {
        if (loaded) {
            return (void*)get_input_ptr(index);
        } else {
            return nullptr;
        }
    }
    void* output_ptr(int index) {
        if (loaded) { 
            return (void*)get_output_ptr(index);
        } else {
            return nullptr;
        }
    }
    int run() {
        if (loaded)
            run_();
        return loaded;
    }
  private:
    int loaded;
    void* handle;
    func_t run_;
    pfunc_t get_input_ptr;
    pfunc_t get_output_ptr;
};
