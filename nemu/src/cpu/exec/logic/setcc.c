#include "cpu/exec/helper.h"

#define DATA_BYTE 1

#define instr seta
#include "setcc-template.h"
#undef instr

#define instr setae
#include "setcc-template.h"
#undef instr

#define instr setb
#include "setcc-template.h"
#undef instr

#define instr setbe
#include "setcc-template.h"
#undef instr

#define instr sete
#include "setcc-template.h"
#undef instr

#define instr setg
#include "setcc-template.h"
#undef instr

#define instr setge
#include "setcc-template.h"
#undef instr

#define instr setl
#include "setcc-template.h"
#undef instr

#define instr setle
#include "setcc-template.h"
#undef instr

#define instr setne
#include "setcc-template.h"
#undef instr

#define instr setno
#include "setcc-template.h"
#undef instr

#define instr setnp
#include "setcc-template.h"
#undef instr

#define instr setns
#include "setcc-template.h"
#undef instr

#define instr seto
#include "setcc-template.h"
#undef instr

#define instr setp
#include "setcc-template.h"
#undef instr

#define instr sets
#include "setcc-template.h"
#undef instr

#undef DATA_BYTE