#include "cpu/exec/template-start.h"

#define instr call

#if DATA_BYTE == 4
make_helper(concat(call_si_, SUFFIX)) {
    int len = concat(decode_si_, SUFFIX) (eip + 1);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip + len + 1);
    print_asm(str(instr) " *%s", op_src->str);
    cpu.eip = cpu.eip + op_src->val - (len + 1);
    return len + 1;
}
#endif

#if DATA_BYTE == 2
make_helper(concat(call_si_, SUFFIX)) {
    Assert(0, "call_si_w");
}
#endif

make_helper(concat(call_rm_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX) (eip + 1);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip + len + 1);
    cpu.eip = op_src->val - (len + 1);
    print_asm(str(instr) " *%s", op_src->str);
    return len + 1;
}

#include "cpu/exec/template-end.h"