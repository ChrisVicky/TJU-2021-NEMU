#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#define instr je
#include "jcc-template.h"
#undef instr
#undef DATA_BYTE

#define DATA_BYTE 2
#define instr je
#include "jcc-template.h"
#undef instr
#undef DATA_BYTE

#define DATA_BYTE 4
#define instr je
#include "jcc-template.h"
#undef instr
#undef DATA_BYTE


