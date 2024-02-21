import numpy as np
from .codegen_csb_head import CodeGenCSBHead
from ..adr import Op
from ..driver.basic import AXI_DAT_WIDTH
from .. import ne


class CodeGenCFGHead(CodeGenCSBHead):

    def ext_define(self):
        self.cfg_numb = 0
        self.task_cfg = []
        self.func_cfg = []
        self.func_init = []

    def to_string(self):
        super().to_string()
        self.func_init_str = f"void {self.mod_name}Init(FILE* f_cfg) " + "{\n" + \
                             f"{self.tab}rewind(f_cfg);\n" + \
                             "\n".join([self.tab + i for i in self.func_init]) + "\n}"

    def gen_source(self):
        self.gen_cfg()
        cfg_params = b"".join([cfg.tobytes() for cfg in self.func_cfg])
        return super().gen_source(), cfg_params

    def gen_cfg(self):
        byte_size = AXI_DAT_WIDTH
        task_cfg = np.zeros(AXI_DAT_WIDTH, dtype="uint32")
        for num, task in enumerate(self.task_cfg):
            new_task = []
            for csb in task[1]:
                if csb[1] in [33, 214]:
                    break
                new_task.append(csb[2])
            task_size = 0
            if len(new_task) > 16:
                task_size += AXI_DAT_WIDTH
            task_size += AXI_DAT_WIDTH
            task_cfg[num // 4] += task[0] << (8*(num%4))
            task_cfg = np.pad(task_cfg, ((0, task_size)), constant_values=0)
            task_cfg[byte_size:byte_size+len(new_task)] = np.array(new_task)
            byte_size += task_size
        if len(self.task_cfg):
            id = self.storage.malloc("ddr", byte_size, 0)
            self.storage.set_address()
            self.func_cfg.append(task_cfg)
            max_tasks_num, aug_group_num = len(self.task_cfg), byte_size // AXI_DAT_WIDTH
            self.cfg_numb = 0
            self.task_cfg = []
            self.func_init.append(f"fread((void*){id}, 1, {byte_size}, f_cfg);")
            self.func_init.append(f"(uint8_t*){id} += {byte_size};")
            self.func_body.append("""#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_run);
#endif
%(tab)sCSB_Write(64+1, (uint64_t)%(storage)s);
%(tab)sCSB_Write(64+2, %(aug_group_num)d);
%(tab)sCSB_Write(64+3, %(max_tasks_num)d);
%(tab)sCSB_Write(64+4, 1);//start
%(tab)swhile(CSB_Read(device, 64) != 1) {}
#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("cfg run time     = %%fs \\n",time_sec1);
#endif""" % {"tab": self.tab, "storage": id, "aug_group_num": aug_group_num, "max_tasks_num": max_tasks_num})

    def gen_accel(self, node):
        op_name = node["op_name"]
        task_id = Op.Get(op_name).attrs["cfg_id"]

        state_max_depth = self.cfg_numb == 63
        state_op_split = self.cfg_numb == 0 and len(self.func_cfg) != 0
        state_none_task_id = task_id is None
        state_dynamic = sum([reg[3] for reg in node["csb_regs"]])

        if state_max_depth or state_none_task_id or state_op_split or state_dynamic:
            self.gen_cfg()

        if state_none_task_id or state_dynamic:
            self.func_body.append(f"// {op_name} accel operator node")
            cfg_start = 1
            for reg in node["csb_regs"]:
                if reg[0] == 1:
                    if cfg_start:
                        self.func_body.append("""#ifdef REGS_DEBUG
QueryPerformanceCounter(&start_cfg);
#endif""")
                        cfg_start = 0
                    elif reg[1] in [33, 214]:
                        self.func_body.append("""#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_cfg);
QueryPerformanceCounter(&start_run);
#endif""")
                        cfg_start = 1
                    self.func_body.append(f"{self.tab}CSB_Write(device, {reg[1]}, {reg[2]});")
                elif reg[0] == 0:
                    self.func_body.append(f"{self.tab}While(device, CSB_Read({reg[1]}) != {reg[2]}) " + "{}")
                    self.func_body.append("""#ifdef REGS_DEBUG
QueryPerformanceCounter(&stop_run);
time_sec0 = (unsigned long long)(stop_cfg.QuadPart - start_cfg.QuadPart) / (double)freq.QuadPart;
time_sec1 = (unsigned long long)(stop_run.QuadPart - start_run.QuadPart) / (double)freq.QuadPart;
printf("%(op_name)s cfg reg time = %%fs \\n",time_sec0);
printf("%(op_name)s run time     = %%fs \\n",time_sec1);
#endif""" % {"op_name": op_name})
        else:
            self.cfg_numb += 1
            self.task_cfg.append([task_id, node["csb_regs"]])

    def gen_cpu(self, node):
        self.cfg_numb = 0
        return super().gen_cpu(node)
