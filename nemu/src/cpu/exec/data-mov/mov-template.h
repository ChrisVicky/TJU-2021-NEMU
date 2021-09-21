#include "cpu/exec/template-start.h"

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
	uint32_t src = cpu.eax;
#if DATA_BYTE == 2
	src = src & 0xfff;
#endif
	cpu.CR0.val = src;
	print_asm_template1();
	return 2;
}

make_helper(concat(mov_cr2r_, SUFFIX)){
	REG(R_EAX) = cpu.CR0.val;
	print_asm_template1();
	return 2;
}
#if DATA_BYTE == 1
make_helper(mov_sr2r_b){
	read_ModR_M(eip+1, op_src, op_dest);
	ModR_M m;
	m.val = instr_fetch(eip+1, 1);
	cpu.sreg.sg[m.reg].visible.val = REG(m.R_M);
	return 2;
}
#endif
#include "cpu/exec/template-end.h"
