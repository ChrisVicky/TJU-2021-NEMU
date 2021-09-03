#include "cpu/exec/template-start.h"
#if DATA_BYTE == 1
bool is_je() { return cpu.eflags.ZF == 1; }
#endif

#if DATA_BYTE == 4 || DATA_BYTE == 1
static void do_execute() {
    if(concat(is_, instr)()){
        cpu.eip += op_src->val;
    }
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}
make_instr_helper(si)

#elif DATA_BYTE == 2
make_helper(concat(instr, _si_w)){
    int len = decode_si_l(eip + 1);
    if(concat(is_, instr)()){
        cpu.eip += op_src->val;
    }
    cpu.eip = cpu.eip & 0x0000ffff;
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
    return len + 1;
}
#endif
#include "cpu/exec/template-end.h"


/*

*/