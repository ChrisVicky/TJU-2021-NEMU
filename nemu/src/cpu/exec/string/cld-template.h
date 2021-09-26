#include "cpu/exec/template-start.h"
make_helper(cld){
    cpu.eflags.DF = 0;
    return 1;
}
#include "cpu/exec/template-end.h"