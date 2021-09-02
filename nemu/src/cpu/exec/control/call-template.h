#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip + op_src->size);
    

}

make_instr_helper(si)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"