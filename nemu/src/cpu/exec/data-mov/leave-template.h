#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)){
    cpu.esp = cpu.ebp;
    cpu.ebp = MEM_R(cpu.esp, R_SS);
    cpu.esp += DATA_BYTE;
    print_asm(str(instr));
    return 1;
    /*
        Don't worry about operand-size 16; 
            course it can be settled by adding one.
    */
}

#include "cpu/exec/template-end.h"
