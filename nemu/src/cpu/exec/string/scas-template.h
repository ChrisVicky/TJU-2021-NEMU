#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
	
	// uint32_t dest = REG(R_EAX);
	// uint32_t src = swaddr_read(reg_l(R_EDI), DATA_BYTE, R_ES);
	// uint32_t result = dest - src;
	// update_eflags_pf_zf_sf(result);
	// cpu.eflags.CF = dest > src;
	// cpu.eflags.OF = MSB((dest ^ src) & (dest ^ result));

	// cpu.edi += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);

	// print_asm("scas" str(SUFFIX) " %%es:(%%edi),%%%s", REG_NAME(R_EAX));
	// return 1;

    swaddr_t s1 = REG(R_EAX), s2 = swaddr_read(reg_l(R_EDI), DATA_BYTE, R_ES);
    uint32_t res = s1- s2;
	//Log("%x - %x = %x" ,s1,s2,res);
    if (cpu.eflags.DF == 0)
    {
        reg_l(R_EDI) += DATA_BYTE;
    } else {
        reg_l(R_EDI) -= DATA_BYTE;
    }
    //concat(update_, SUFFIX) (res);
    update_eflags_pf_zf_sf(res);
	int len = (DATA_BYTE << 3) - 1;
    cpu.eflags.CF = s1<s2;
    cpu.eflags.OF = (((s1>>len) != (s2>>len)) && ((s2>>len) == cpu.eflags.SF));
    print_asm("scas%s", str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
