#include "cpu/exec/template-start.h"

#define instr ret

make_helper(ret){
    int instruct = swaddr_read(cpu.esp, 4);
    cpu.esp += 4;
    cpu.eip = instruct;
    cpu.eip -= 1;
    print_asm_template1();
    return 1;
}

make_helper(concat(instr, _i)){
    int len = decode_i_w(eip + 1);
    int instruct = swaddr_read(REG(R_ESP), 4);
    REG(R_ESP) += 4;
    cpu.eip = instruct;
    cpu.eip -= (len + 1);
    REG(R_ESP) += op_src->val;
    print_asm_template2();
    return len + 1;
}

#include "cpu/exec/template-end.h"