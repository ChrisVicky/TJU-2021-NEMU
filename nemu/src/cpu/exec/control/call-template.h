#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_si_, SUFFIX)) {
    int len = decode_si_l(eip + 1);
    cpu.eip = cpu.eip + len + 1;
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, cpu.eip);
    cpu.eip = cpu.eip + op_src->val;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x", cpu.eip);
    cpu.eip = cpu.eip - (len + 1);
    return len + 1;
}

make_helper(concat(call_rm_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX) (eip + 1);
    cpu.eip = cpu.eip + len + 1;
    cpu.esp = DATA_BYTE;
    MEM_W(cpu.esp, cpu.eip);
    cpu.eip = op_src->val;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x", cpu.eip);
    cpu.eip = cpu.eip - (len + 1);
    return len + 1;
}

#include "cpu/exec/template-end.h"