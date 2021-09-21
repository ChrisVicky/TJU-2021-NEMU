#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_, SUFFIX)) {
    cpu.eip = MEM_R(cpu.esp, R_SS);
    cpu.esp += DATA_BYTE;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    cpu.eip -= 1;
    print_asm("ret");
    return 1;
}

make_helper(concat(ret_i_, SUFFIX)){
    int len = decode_i_w(eip + 1);
    cpu.eip = MEM_R(cpu.esp, R_SS);
    cpu.esp += DATA_BYTE;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    cpu.esp += op_src->val;
    print_asm_template1();
    cpu.eip -= (len + 1);
    return len + 1;
}


#include "cpu/exec/template-end.h"