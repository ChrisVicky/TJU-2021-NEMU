#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4
make_helper(ret){
    int instruct = swaddr_read(cpu.esp, 4);
    cpu.esp += 4;
    cpu.eip = instruct;
    print_asm(str(instr));
    return 0;
}
#endif

#include "cpu/exec/template-end.h"