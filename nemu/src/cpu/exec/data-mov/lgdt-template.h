#include "cpu/exec/template-start.h"

#define instr lgdt

make_helper(concat(lgdt_, SUFFIX)) {
    uint16_t limit = instr_fetch(eip, 2);
    uint32_t base = instr_fetch(eip+2, 4);
#if DATA_BYTE == 2
    base = base & 0xfff;
#endif
    cpu.GDTR.Base = base;
    cpu.GDTR.Limit = limit;
    print_asm_template1();
    print_asm(str(instr) " 0x%08x, 0x%08x", limit, base);
    return 6;
}

#include "cpu/exec/template-end.h"
