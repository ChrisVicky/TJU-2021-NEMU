#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
	uint32_t dest = REG(R_EAX);
	uint32_t src = MEM_R(REG(R_EDI), R_ES);
	uint32_t result = dest - src;
	Log("%x - %x = %x" ,dest,src,result);
	update_eflags_pf_zf_sf(result);
	//cpu.eflags.CF = result > dest;
	cpu.eflags.CF = dest > src;
	cpu.eflags.OF = MSB((dest ^ src) & (dest ^ result));

	cpu.edi += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);

	print_asm("scas" str(SUFFIX) " %%es:(%%edi),%%%s", REG_NAME(R_EAX));
	return 1;
}

#include "cpu/exec/template-end.h"
