#include "cpu/exec/template-start.h"
#ifndef __IS_H__
#define __IS_H__
bool is_je() { return cpu.eflags.ZF == 1; }

#endif
static void do_execute(){
    cpu.eip = op_src->val;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x" ,cpu.eip + 1 + DATA_BYTE);
}

make_helper(concat3(instr, _si_, SUFFIX)) {
    int len = 0;
#if DATA_BYTE == 2 || DATA_BYTE == 4
    len = decode_si_l(eip + 1);
#else
    len = decode_si_b(eip + 1);
#endif
    if(concat(is_, instr)()){
        cpu.eip += op_src->val;
    }
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
    return len + 1;
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
