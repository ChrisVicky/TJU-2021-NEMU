#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)){
    // MEM_W(cpu.esp, cpu.ebp);
    REG(R_ESP) = REG(R_EBP);
    REG(R_EBP) = MEM_R(REG(R_ESP));
    REG(R_ESP) += DATA_BYTE;
    print_asm(str(instr));
    return 1;
    /*
        Don't worry about operand-size 16; 
            course it can be settled by adding one.
    */
}

#include "cpu/exec/template-end.h"
