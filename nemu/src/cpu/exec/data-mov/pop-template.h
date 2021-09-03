#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    op_src->val = swaddr_read(cpu.esp, DATA_BYTE);
    cpu.esp += DATA_BYTE;
    print_asm_template1();
}

// make_instr_helper(m)
make_instr_helper(r)

#include "cpu/exec/template-end.h"
