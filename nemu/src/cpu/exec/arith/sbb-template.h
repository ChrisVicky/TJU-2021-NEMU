#include "cpu/exec/template-start.h"

#define instr sbb

static void do_execute () {
	    DATA_TYPE src = op_src->val;
    if (op_src->size == 1 && op_dest->size !=1)
    {
        op_src->val = (int8_t)op_src->val;
    }
    src += cpu.eflags.CF;
    DATA_TYPE result = op_dest->val - src;
    int len = (DATA_BYTE << 3) - 1;
    cpu.eflags.CF = op_dest->val < src;
    int s1, s2;
    cpu.eflags.SF = result >> len;
    s1 = op_dest->val >> len;
    s2 = src >> len;
    cpu.eflags.OF = (s1!=s2 && s2==cpu.eflags.SF);
    cpu.eflags.ZF = !result;
    OPERAND_W(op_dest, result);
    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    cpu.eflags.PF = !(result&1);
    print_asm_template2();
	
	// DATA_TYPE result = op_dest->val - (op_src->val + cpu.eflags.CF);
	// OPERAND_W(op_dest, result);

	// /* TODO: Update EFLAGS. */
	// update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	// cpu.eflags.CF = result < op_dest->val;
	// cpu.eflags.OF = MSB(~(op_dest->val ^ op_src->val) & (op_dest->val ^ result));

	// print_asm_template2();
	
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
