#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    DATA_TYPE_S result = op_dest->val - op_src->val;
    update_eflags_pf_zf_sf(result);
    if((op_dest->val > 0 && op_src->val < 0 && result < 0) || (op_dest->val < 0 && op_src->val > 0 && result > 0)){
        cpu.eflags.OF = 1;
    }else{
        cpu.eflags.OF = 0;
    }
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