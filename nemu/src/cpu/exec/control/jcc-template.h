#include "cpu/exec/template-start.h"

bool is_je(){ return cpu.eflags.ZF == 1; }
bool is

/*
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
*/
static void do_execte() {
    if(concat(is_, instr))()){
        cpu.eip += op_src->val;
    }
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)


#include "cpu/exec/template-end.h"
