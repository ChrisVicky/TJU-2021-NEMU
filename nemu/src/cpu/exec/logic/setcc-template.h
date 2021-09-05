#include "cpu/exec/template-start.h"

#define CF cpu.eflags.CF
#define PF cpu.eflags.PF
#define AF cpu.eflags.AF
#define ZF cpu.eflags.ZF
#define SF cpu.eflags.SF
#define TF cpu.eflags.TF
#define IF cpu.eflags.IF
#define DF cpu.eflags.DF
#define OF cpu.eflags.OF
#define IOPF cpu.eflags.IOPL
#define NT cpu.eflags.NT
#define CX cpu.gpr[1]._16
#define ECX cpu.gpr[1]._32


#ifndef __SET_TABLE__
#define __SET_TABLE__
bool is_seta() { return !CF && !ZF; }
bool is_setae() { return !CF; }
bool is_setb() { return CF==1; }
bool is_setbe() { return CF==1 || ZF==1; }
bool is_sete() { return ZF==1; }
bool is_setg() { return !ZF && SF==OF; }
bool is_setge() { return SF==OF; }
bool is_setl() { return SF!=OF; }
bool is_setle() { return ZF==1 || SF!=OF; }
bool is_setne() { return !ZF; }
bool is_setno() { return !OF; }
bool is_setnp() { return !PF; }
bool is_setns() { return !SF; }
bool is_seto() { return OF==1; }
bool is_setp() { return PF==1; }
bool is_sets() { return SF==1; }
#endif


static void do_execute() {
    if(concat(is_, instr)()){
       	OPERAND_W(op_src, 1);
    }else OPERAND_W(op_src, 0);
    print_asm_template1();
}
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
