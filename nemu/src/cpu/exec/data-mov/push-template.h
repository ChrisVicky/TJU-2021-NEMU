#include "cpu/exec/template-start.h"

#define instr push

static void do_execute(){
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, op_src->val);
    Log("cpu.esp = 0x%x" ,cpu.esp);
    Log("REG(R_ESP) = 0x%x" ,REG(R_ESP));
    print_asm_template1();
}

make_instr_helper(i)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(m)
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
