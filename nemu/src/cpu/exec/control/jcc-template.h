#include "cpu/exec/template-start.h"

#define instr je
#if instr == je
static void do_execute() {
    if(cpu.eflags.ZF==1){
        cpu.eip += op_src->val;
    }
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}
make_instr_helper(si)
#endif
#undef instr

#include "cpu/exec/template-end.h"
