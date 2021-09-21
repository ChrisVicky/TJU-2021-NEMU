#include "cpu/exec/template-start.h"

#define instr push

#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute(){
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, op_src->val, R_SS);
    print_asm_template1();
}

// make_instr_helper(i)

make_instr_helper(m)
make_instr_helper(r)
#endif

make_helper(concat(push_i_, SUFFIX)){
    int len = concat(decode_i_, SUFFIX)(eip + 1);
    cpu.esp -= 4;
    MEM_W(cpu.esp, op_src->val, R_SS);
    print_asm_template1();
    return len + 1;
}

#include "cpu/exec/template-end.h"
