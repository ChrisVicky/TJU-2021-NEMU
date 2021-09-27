#include "cpu/exec/template-start.h"
#include "memory/tlb.h"
#include "cpu/decode/modrm.h"
#define instr mov



static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX), R_DS);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr, R_DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}
/* Not fully functioned */
make_helper(concat(mov_r2cr_, SUFFIX)){
	
	ModR_M m;
	m.val = instr_fetch(eip+1,1);
	uint32_t src = REG(R_EAX);
#if DATA_BYTE == 2
	src = src & 0xfff;
#endif
	if (m.val == 0xc0)
	{
		cr0.val = src;
		print_asm("mov %%%s,%%cr0", REG_NAME(R_EAX));
	} else if(m.val == 0xd8) {
		cr3.val = src;
		resetTLB();
		print_asm("mov %%%s,%%cr3", REG_NAME(R_EAX));
	}
	return 2;
}

make_helper(concat(mov_cr2r_, SUFFIX)){
	ModR_M m;
	m.val = instr_fetch(eip+1, 1);
	if (m.val == 0xc0)
	{
		REG(R_EAX) = cr0.val;
		print_asm("mov %%cr0,%%%s", REG_NAME(m.R_M));
	} else if(m.val == 0xd8) {
		REG(R_EAX) = cr3.val;
		print_asm("mov %%cr3,%%%s", REG_NAME(m.R_M));
	}
	return 2;
}
#if DATA_BYTE == 2
make_helper(mov_seg){
	ModR_M m;
	m.val = instr_fetch(eip+1, 1);
	cpu.sreg[m.reg].visible.val = REG(m.R_M);
	if (m.val == 0xd8)
	{
		cpu.sreg[R_DS].visible.val = REG(R_EAX);
		load_sreg(R_DS);
		print_asm("mov %%%s, ds", REG_NAME(R_EAX));
	} else if(m.val == 0xc0) {
		cpu.sreg[R_ES].visible.val = REG(R_EAX);
		load_sreg(R_ES);
		print_asm("mov %%%s, es", REG_NAME(R_EAX));
	} else if(m.val == 0xd0) {
		cpu.sreg[R_SS].visible.val = REG(R_EAX);
		load_sreg(R_SS);
		print_asm("mov %%%s, ss", REG_NAME(R_EAX));
	}
	return 2;
}
#endif
#include "cpu/exec/template-end.h"
