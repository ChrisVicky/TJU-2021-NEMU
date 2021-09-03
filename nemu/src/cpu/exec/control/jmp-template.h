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
#endif

#if DATA_BYTE == 4
make_helper(jmp_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}
#endif
#include "cpu/exec/template-end.h"
