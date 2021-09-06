#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_, SUFFIX)) {
    int instruct = MEM_R(REG(R_ESP));
    cpu.esp += DATA_BYTE;
    cpu.eip = instruct;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    cpu.eip -= 1;
    print_asm_template1();
    return 1;
}

make_helper(concat(ret_i_, SUFFIX)){
    int len = decode_i_w(eip + 1);
    int instruct = MEM_R(REG(R_ESP));
    REG(R_ESP) += DATA_BYTE;
    cpu.eip = instruct;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    REG(R_ESP) += op_src->val;
    print_asm_template2();
    cpu.eip -= (len + 1);
    return len + 1;
}


#include "cpu/exec/template-end.h"