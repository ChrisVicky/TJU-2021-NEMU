#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_helper()


#include "cpu/exec/template-end.h"