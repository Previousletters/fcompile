# generate static jit model
class CodeGen:

    def __init__(self, model):
        self.model = model

    def gen_head(self, model_name, prefix="", tab="  "):
        cache_src = "// static cache define\n" + "\n".join(self.model["cache"])
        attr_str = ", ".join([f"int {attr}" for attr in self.model["attrs"]])
        if len(attr_str) > 0:
            attr_str = ", " + attr_str
        run_model = [f"void {model_name}(HANDLE device{attr_str}) " + "{"]
        run_body = []
        for key, value in self.model["regops"].items():
            attr_str = ", ".join([f"int {attr}" for attr in value["attrs"]])
            call_str = ", ".join(value["attrs"])
            if len(attr_str) > 0 and len(call_str) > 0:
                attr_str = ", " + attr_str
                call_str = ", " + call_str
            key = prefix + key
            run_model.append(tab + key + f"(device{call_str});")
            run_body_tp = [f"void {key}(HANDLE device{attr_str}) " + "{"]
            tab_block = 1
            for csb in value["body"]:
                run_body_tp_src = ""
                if csb[0] == -1:
                    tab_block -= 1
                for i in range(tab_block):
                    run_body_tp_src += tab
                if csb[0] == 1:
                    run_body_tp_src += f"CSB_Write(device, {csb[1]}, {csb[2]});"
                elif csb[0] == 0:
                    run_body_tp_src += f"while (CSB_Read(device, {csb[1]}) != {csb[2]}) " + "{ }"
                elif csb[0] == -1:
                    run_body_tp_src += "}"
                elif csb[0] == 10:
                    run_body_tp_src += f"memcpy((void*){csb[1]}, (void*){csb[2]}, {csb[3]});"
                elif csb[0] == 2:
                    run_body_tp_src += f"for (int {csb[1]} = {csb[2]}; {csb[1]} < {csb[3]}; {csb[1]}++) " + "{"
                    tab_block += 1
                else:
                    raise RuntimeError("Not found!!!")
                run_body_tp.append(run_body_tp_src)
            run_body_tp.append("}")
            run_body.append("\n".join(run_body_tp))
        run_model.append("}")
        run_body_src = "\n".join(run_body)
        run_model_src = "\n".join(run_model)
        source_map = {
            "model_name" : model_name, "run_body_src" : run_body_src, "run_model_src" : run_model_src, "cache": cache_src
        }
        source = ''' #include <string.h>
#include "accel_api.h"

%(cache)s

// \"%(model_name)s\" layer wrapper function
%(run_body_src)s

// \"%(model_name)s\" model wrapper function
%(run_model_src)s
''' % source_map
        return source
