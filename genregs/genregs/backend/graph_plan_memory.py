from ..adr import Functor, VM, Tensor, Tuple, DataEnum
from ..driver import get_tensor_size
from .. import ne


class StorageNode:

    def __init__(self, byte_size):
        if isinstance(byte_size, ne.Expr):
            byte_size = byte_size.simplify(1).data
        self.byte_size = byte_size

    def get_address(self, offset):
        return self.address + offset

    def set_address(self, address):
        if isinstance(address, ne.Expr):
            address = address.simplify(1).data
        self.address = address
        return self.address + self.byte_size

    def __str__(self):
        addr_hex = "0x%09x" % (self.address)
        return "|{0:^15}|{1:^12}*".format(addr_hex, str(self.byte_size)+" B")


class Storage:

    def __init__(self):
        self.memo_ = {}
        self.free_ = []

    def malloc(self, prefix, data_byte, auto_check=1):
        if prefix not in self.memo_.keys():
            self.memo_[prefix] = {}
        if auto_check:
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
        self.free_.append(id)

    def set_address(self, base_addr_map):
        for prefix in base_addr_map.keys():
            addr = base_addr_map[prefix]
            for id in self.memo_[prefix].keys():
                addr = self.memo_[prefix][id].set_address(addr)

    def get_address(self, id, offset):
        for memo_ in self.memo_.values():
            if id in memo_.keys():
                storage = memo_[id]
                return storage.get_address(offset)
        print("could not find " + id + " storage!")
        exit(-1)

    def gen_source(self):
        source = ""
        for prefix, memo in self.memo_.items():
            source += f"// {prefix} storage define\n"
            for id, storage in memo.items():
                addr_hex = "0x%09x" % (storage.address)
                source += f"static uint8_t* {id} = (uint8_t*){addr_hex}; "
                source += f"// storage size: {storage.byte_size} B\n"
        return source[:-1]

    def __str__(self):
        ret = "***************************************\n"
        ret += "*{0:^8}|{1:^15}|{2:^12}*\n".format("ID", "Address", "Size")
        for prefix, memo in self.memo_.items():
            ret += "*-------------------------------------*\n"
            for id, storage in memo.items():
                ret += "*{0:^8}".format(id) + str(storage) + "\n"
        ret += "***************************************"
        return ret


class GraphPlanMemory(Functor):

    ddr_str = "ddr"
    hbm_str = "hbm"

    def main(self, expr, ddr_addr, hbm_addr):
        info = Functor()
        info.visit(expr)
        self.info_memo = info.memo
        self.storage = Storage()
        expr = self.visit(expr)
        self.storage.set_address({self.ddr_str: ddr_addr, self.hbm_str: hbm_addr})
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

    def _malloc(self, tensor, auto_check=1):
        if tensor.dtype.mapped == DataEnum.ddr:
            return self.storage.malloc(self.ddr_str, get_tensor_size(tensor), auto_check)
        elif tensor.dtype.mapped == DataEnum.hbm:
            return self.storage.malloc(self.hbm_str, get_tensor_size(tensor), auto_check)
        else:
            print("unknown device, please check!")
            exit(-1)

    def visit_var(self, expr):
        storage_id = self._malloc(expr.checked_type, 0)
        expr.checked_type.storage_id = storage_id
        return expr

    def visit_constant(self, expr):
        storage_id = self._malloc(expr.checked_type, 0)
        expr.checked_type.storage_id = storage_id
        self.info_memo[expr][1] = 0
        return expr

    def visit_call(self, expr):
        new_args = []
        for arg in expr.args:
            new_args.append(self.visit(arg))
        if isinstance(expr.checked_type, Tuple):
            for i in range(len(expr.checked_type.tensors)):
                storage_id = self._malloc(expr.checked_type.tensors[i])
                expr.checked_type.tensors[i].storage_id = storage_id
        elif isinstance(expr.checked_type, Tensor):
            storage_id = self._malloc(expr.checked_type)
            expr.checked_type.storage_id = storage_id
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
