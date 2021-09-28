#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_n_, SUFFIX)) {
    uint32_t data = swaddr_read(reg_l(R_ESI), DATA_BYTE, R_DS);
    Log("data: %x" ,data);
    swaddr_write(reg_l(R_EDI), DATA_BYTE, data, R_ES);

    if (cpu.eflags.DF == 0) reg_l(R_EDI) += DATA_BYTE, reg_l(R_ESI) += DATA_BYTE;
    else reg_l(R_EDI) -= DATA_BYTE, reg_l(R_ESI) -=DATA_BYTE;
    
    print_asm("movs");
    return 1;
}

#include "cpu/exec/template-end.h"