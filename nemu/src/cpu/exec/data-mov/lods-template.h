#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat3(instr, _m_, SUFFIX)){
    REG(R_EAX) = MEM_R(cpu.esi);
    REG(R_ESI) += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);
    print_asm("lods" str(SUFFIX) " %%ds:(%%esi),%%%s", REG_NAME(R_EAX));
    return 1;
}

#include "cpu/exec/template-end.h"