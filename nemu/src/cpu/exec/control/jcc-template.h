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

#ifndef __IS_H__
#define __IS_H__
bool is_ja() { return (!CF) && (!ZF); }
bool is_jae() { return (!CF); }
bool is_jb() { return CF==1; }
bool is_jbe() { return CF==1 || ZF == 1; }
bool is_jc() { return CF == 1; }
bool is_jcxz() { return !CX; }
bool is_jecxz() { return !ECX; }
bool is_je() { return ZF == 1; }
bool is_jz() { return ZF == 1; }
bool is_jg() { return !ZF && SF==OF; }
bool is_jge() { return SF==OF; }
bool is_jl() { return SF!=OF; }
bool is_jle() { return ZF==1 && SF!=OF; }
bool is_jna() { return CF==1 && ZF==1; }
bool is_jnae() { return CF==1; }
bool is_jnb() { return !CF; }
bool is_jnbe() { return CF==0 && ZF==0; }
bool is_jnc() { return !CF; }
bool is_jne() { return !ZF; }
bool is_jng() { return ZF==1 || SF!=OF; }
bool is_jnge() { return SF!=OF; }
bool is_jnl() { return SF==OF; }
bool is_jnle() { return !ZF && SF==OF; }
bool is_jno() { return !OF; }
bool is_jnp() { return !PF; }
bool is_jns() { return !SF; }
bool is_jnz() { return !ZF; }
bool is_jo() { return OF==1; }
bool is_jp() { return PF==1; }
bool is_jpe() { return PF==1; }
bool is_jpo() { return !PF; }
bool is_js() { return SF==1; }
// bool is_jz() { return ZF==1; }
#endif

static void do_execute(){
    cpu.eip = op_src->val;
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x" ,cpu.eip + 1 + DATA_BYTE);
}

make_helper(concat3(instr, _si_, SUFFIX)) {
    int len = 0;
#if DATA_BYTE == 2 || DATA_BYTE == 4
    len = decode_si_l(eip + 1);
#else
    len = decode_si_b(eip + 1);
#endif
    if(concat(is_, instr)()){
        cpu.eip += op_src->val;
    }
#if DATA_BYTE == 2
    cpu.eip = cpu.eip & 0x0000ffff;
#endif
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
    return len + 1;
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
/*
#undef __IS_H__
#undef CF
#undef PF
#undef AF
#undef ZF
#undef SF
#undef TF
#undef IF
#undef DF
#undef OF 
#undef IOPF 
#undef NT
*/
