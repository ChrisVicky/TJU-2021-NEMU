#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    DATA_TYPE_S result = op_dest->val - op_src->val;
    // Log("%x - %x = %d" ,op_dest->val, op_src->val, result);
    update_eflags_pf_zf_sf(result);
    update_eflags_cf_of_MINUS(result, op_dest->val, op_src->val);
    // Log("Show Flags : ZF = %x, OF = %x, SF = %x" ,cpu.eflags.ZF, cpu.eflags.OF, cpu.eflags.SF);
    Log("Compare 0x%x and 0x%x, RESULT=0x%x\n" ,op_dest->val, op_src->val, result);
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