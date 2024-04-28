from ..adr import Functor, VM, Tensor, Tuple, DataEnum
from .. import ne


class StorageNode:

    def __init__(self, byte_size):
        if isinstance(byte_size, ne.Expr):
            byte_size = byte_size.simplify(1).data
        self.byte_size = byte_size

    def get_address(self, offset):
        if offset == -1:
            return self.address + self.byte_size
        return self.address + offset

    def set_address(self, address=None):
        if address is None:
            return self.address + self.byte_size
        if isinstance(address, ne.Expr):
            address = address.simplify(1).data
        if hasattr(self, "address"):
            if self.address != address:
                print("*WARNING* : StorageNode new address dosen't match: ", self.address, address)
        self.address = address
        return self.address + self.byte_size

    def __str__(self):
        addr_hex = "0x%09x" % (self.address)
        return "|{0:^15}|{1:^12}*".format(addr_hex, str(self.byte_size)+" B")


class Storage:

    def __init__(self):
        self.memo_ = {}
        self.free_ = []

    def malloc(self, prefix, data_byte):
        if prefix not in self.memo_.keys():
            self.memo_[prefix] = {}
        if prefix == "runtime":
            min_id, min_diff, found = None, None, 0
            for id in self.free_:
                if id not in self.memo_[prefix].keys():
                    continue
                storage = self.memo_[prefix][id]
                new_diff = storage.byte_size - data_byte
                if min_id is None:
                    min_id, min_diff = id, new_diff
                else:
                    if found:
                        if new_diff >= 0:
                            min_id = min_id if new_diff > min_diff else id
                            min_diff = min_diff if new_diff > min_diff else new_diff
                    else:
                        if new_diff >= 0:
                            min_id, min_diff = id, new_diff
                        else:
                            min_id = min_id if new_diff < min_diff else id
                            min_diff = min_diff if new_diff < min_diff else new_diff
                found = min_diff >= 0
            if min_id:
                if found:
                    self.free_.remove(min_id)
                    return min_id
                else:
                    self.memo_[prefix][min_id].byte_size -= min_diff
                    self.free_.remove(min_id)
                    return min_id
        id = f"{prefix}{len(self.memo_[prefix])}"
        self.memo_[prefix][id] = StorageNode(data_byte)
        return id

    def free(self, id):
        if "runtime" in id:
            self.free_.append(id)

    def set_address(self, base_addr_map=None):
        if base_addr_map is None:
            for prefix in self.memo_.keys():
                addr = None
                for id in self.memo_[prefix].keys():
                    addr = self.memo_[prefix][id].set_address(addr)
        else:
            saved_prefix_addr = {}
            self.sort_keys = base_addr_map.keys()
            for prefix, addr in base_addr_map.items():
                if prefix not in self.memo_.keys():
                    continue
                if isinstance(addr, str):
                    addr = saved_prefix_addr[addr]
                for id in self.memo_[prefix].keys():
                    addr = self.memo_[prefix][id].set_address(addr)
                saved_prefix_addr[prefix] = addr

    def get_address(self, id, offset):
        for memo_ in self.memo_.values():
            if id in memo_.keys():
                storage = memo_[id]
                return storage.get_address(offset)
        print("could not find " + id + " storage!")
        exit(-1)

    def gen_source(self):
        source = ""
        for prefix in self.sort_keys:
            memo = self.memo_[prefix]
            source += f"// {prefix} storage define\n"
            for id, storage in memo.items():
                addr_hex = "0x%09x" % (storage.address)
                source += f"static uint8_t* {id} = (uint8_t*){addr_hex}; "
                source += f"// storage size: {storage.byte_size} B\n"
        return source[:-1]

    def __str__(self):
        ret = "*******************************************\n"
        ret += "*{0:^12}|{1:^15}|{2:^12}*\n".format("Storage ID", "Address", "Size")
        if hasattr(self, "sort_keys"):
            for prefix in self.sort_keys:
                memo = self.memo_[prefix]
                ret += "*-----------------------------------------*\n"
                for id, storage in memo.items():
                    ret += "*{0:^12}".format(id) + str(storage) + "\n"
            ret += "*******************************************"
            return ret
        else:
            for prefix, memo in self.memo_.items():
                ret += "*-----------------------------------------*\n"
                for id, storage in memo.items():
                    ret += "*{0:^12}".format(id) + str(storage) + "\n"
            ret += "*******************************************"
            return ret


class GraphPlanMemory(Functor):

    def main(self, expr, init_addr, debug=1):
        self.debug = debug
        info = Functor()
        info.visit(expr)
        self.info_memo = info.memo
        self.storage = Storage()
        expr = self.visit(expr)
        self.storage.set_address(init_addr)
        return expr, self.storage

    def _link_storage(self, arg, expr):
        if not hasattr(self, "linked"):
            self.linked = [[arg, expr]]
            return
        for index in range(len(self.linked)):
            if arg in self.linked[index]:
                self.linked[index].append(expr)
                return
        self.linked.append([arg, expr])

    def _check_free(self, arg):
        if hasattr(self, "linked"):
            for index, ll in enumerate(self.linked):
                if arg in ll:
                    for a in ll:
                        if self.info_memo[a][1] != 1:
                            self.info_memo[a][1] -= 1
                            return
                    self.storage.free(arg.checked_type.storage_id)
                    del self.linked[index]
                    return
        if self.info_memo[arg][1] == 1:
            self.storage.free(arg.checked_type.storage_id)
        else:
            self.info_memo[arg][1] -= 1

    def _malloc(self, tensor, prefix):
        if tensor.dtype.mapped == DataEnum.ddr:
            return self.storage.malloc(prefix, tensor.get_bytesize())
        elif tensor.dtype.mapped == DataEnum.hbm:
            return self.storage.malloc(prefix, tensor.get_bytesize())
        else:
            print("unknown device, please check!")
            exit(-1)

    def visit_var(self, expr):
        storage_id = self._malloc(expr.checked_type, expr.prefix)
        expr.checked_type.storage_id = storage_id
        return expr

    def visit_constant(self, expr):
        storage_id = self._malloc(expr.checked_type, expr.prefix)
        expr.checked_type.storage_id = storage_id
        self.info_memo[expr][1] = 0
        return expr

    def visit_call(self, expr):
        new_args = []
        for arg in expr.args:
            new_args.append(self.visit(arg))
        if isinstance(expr.checked_type, Tuple):
            for i in range(len(expr.checked_type.tensors)):
                storage_id = self._malloc(expr.checked_type.tensors[i], expr.prefix)
                expr.checked_type.tensors[i].storage_id = storage_id
            if self.debug:
                print(expr.op.name, [i.checked_type.storage_id for i in new_args], "->", [i.storage_id for i in expr.checked_type.tensors])
        elif isinstance(expr.checked_type, Tensor):
            storage_id = self._malloc(expr.checked_type, expr.prefix)
            expr.checked_type.storage_id = storage_id
            if self.debug:
                print(expr.op.name, [i.checked_type.storage_id for i in new_args], "->", expr.checked_type.storage_id)
        else:
            print("infer_type first!")
            exit(-1)
        expr.args = new_args
        for arg in new_args:
            self._check_free(arg)
        return expr

    def visit_tupleitem(self, expr):
        arg = self.visit(expr.expr)
        expr.expr = arg
        expr.checked_type = arg.checked_type.tensors[expr.index]
        if isinstance(expr.expr, VM):
            self._link_storage(expr.expr, expr)
        return expr

    def visit_vm(self, expr):
        new_args = [self.visit(arg) for arg in expr.args]
        storage_id = new_args[0].checked_type.storage_id
        if isinstance(expr.checked_type, Tuple):
            for i in range(len(expr.checked_type.tensors)):
                expr.checked_type.tensors[i].storage_id = storage_id
        elif isinstance(expr.checked_type, Tensor):
            expr.checked_type.storage_id = storage_id
        else:
            print("infer_type first!")
            exit(-1)
        self._link_storage(new_args[0], expr)
        expr.args = new_args
        return expr


def graph_plan_memory(expr, init_addr):
    return GraphPlanMemory().main(expr, init_addr)
