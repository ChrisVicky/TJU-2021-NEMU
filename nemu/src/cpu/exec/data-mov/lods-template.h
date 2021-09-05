#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat3(instr, _m_, SUFFIX)){
    uint32_t address = cpu.esi;
    REG(R_EAX) = swaddr_read(address, 4);
    if(cpu.eflags.DF){
        cpu.esi += DATA_BYTE;
    }else{
        cpu.esi -= DATA_BYTE;
    }
    print_asm_template1();
    return 1;
}

#include "cpu/exec/template-end.h"