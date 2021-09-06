#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
//    DATA_TYPE_S result = (DATA_TYPE_S) op_dest->val - (DATA_TYPE_S) op_src->val;
    // Log("%x - %x = %d" ,op_dest->val, op_src->val, result);
//    update_eflags_pf_zf_sf(result);
//    cpu.eflags.CF = result > dest;
//	cpu.eflags.OF = MSB((dest ^ src) & (dest ^ result));

    DATA_TYPE dest = op_dest->val;
	DATA_TYPE src = op_src->val;
	DATA_TYPE result = dest - src;

	update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = result > dest;
	cpu.eflags.OF = MSB((dest ^ src) & (dest ^ result));
    // Log("Show Flags : ZF = %x, OF = %x, SF = %x" ,cpu.eflags.ZF, cpu.eflags.OF, cpu.eflags.SF);
    Log("Compare 0x%d and 0x%d, RESULT=0x%d\n" ,op_dest->val, op_src->val, result);
    print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"