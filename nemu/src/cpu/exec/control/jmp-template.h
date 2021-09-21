#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	cpu.eip += op_src->val;
#if DATA_BYTE == 2
	cpu.eip = cpu.eip & 0x0000ffff;
#endif
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

#if DATA_BYTE == 1 || DATA_BYTE == 4
make_instr_helper(si)
#endif

#if DATA_BYTE == 2
make_helper(jmp_si_w){
	int len = decode_si_l(eip + 1);
	do_execute();
	return len + 1;
}

make_helper(jmp_rm_w) {
	int len = decode_rm_w(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}

make_helper(ljmp_w) {
	op_src->val = instr_fetch(eip+1, 2);
	op_src->type = OP_TYPE_IMM;
	op_dest->val = instr_fetch(eip+5, 2);
	op_dest->type = OP_TYPE_IMM;
	cpu.sreg[R_CS].visible.val = op_dest->val;
	cpu.sreg[R_CS].invisible.cache.base_15_0 = 0;
	cpu.sreg[R_CS].invisible.cache.base_23_16 = 0;
	cpu.sreg[R_CS].invisible.cache.limit_15_0 = 0xffff;
	cpu.sreg[R_CS].invisible.cache.limit_19_16 = 0xf;
	cpu.eip = op_src->val - 5;
	return 5;
}
#endif

#if DATA_BYTE == 4
make_helper(jmp_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}

make_helper(ljmp_l) {
	op_src->val = instr_fetch(eip+1, 4);
	op_src->type = OP_TYPE_IMM;
	op_dest->val = instr_fetch(eip+5, 2);
	op_dest->type = OP_TYPE_IMM;
	cpu.sreg[R_CS].visible.val = op_dest->val;
	cpu.sreg[R_CS].invisible.cache.base_15_0 = 0;
	cpu.sreg[R_CS].invisible.cache.base_23_16 = 0;
	cpu.sreg[R_CS].invisible.cache.base_31_24 = 0;
	cpu.sreg[R_CS].invisible.cache.limit_15_0 = 0xffff;
	cpu.sreg[R_CS].invisible.cache.limit_19_16 = 0xf;
	cpu.eip = op_src->val - 7;
	print_asm("ljmp" " %x: %x" ,op_src->val, op_dest->val);
	return 7;
}
#endif
#include "cpu/exec/template-end.h"
