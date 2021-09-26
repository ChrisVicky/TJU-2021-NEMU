#include "cpu/exec/template-start.h"

make_helper(std){
    cpu.eflags.DF = 1;
    return 0;
}
#include "cpu/exec/template-end.h"