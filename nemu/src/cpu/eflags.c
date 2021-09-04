#include "cpu/reg.h"

static const int parity_table [] = {
	0, 1, 0, 1,
	1, 0, 1, 0,
	0, 1, 0, 1,
	1, 0, 1, 0
};

void update_eflags_pf_zf_sf(uint32_t result) {
	uint8_t temp = result & 0xff;
	cpu.eflags.PF = parity_table[temp & 0xf] ^ parity_table[temp >> 4];
	cpu.eflags.ZF = (result == 0);
	cpu.eflags.SF = result >> 31;
}

void update_eflags_cf_of(int32_t result, uint32_t dest, uint32_t src) {
	Log("Watch DEST=%x, SRC=%x, RESULT=%x" ,dest, src, result);
	cpu.eflags.CF = (result < dest && result < src);
	cpu.eflags.OF = ((dest>0 && src>0 && result<0) || (dest<0 && src<0 && result>=0));
}
