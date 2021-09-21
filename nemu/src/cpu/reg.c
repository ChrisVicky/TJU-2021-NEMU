#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);
}

void load_sreg(int sreg_index){
	SegDesc * gdt = (SegDesc *) (uint64_t)cpu.GDTR.Base;
	int index = cpu.sreg[sreg_index].visible.index;
	Assert(index>=cpu.GDTR.Limit, "Invalid index '%x'" ,index);
	cpu.sreg[sreg_index].invisible.cache.base = gdt[index].base_15_0 + (gdt[index].base_23_16<<16) + (gdt[index].base_31_24<<24);
	cpu.sreg[sreg_index].invisible.cache.limit = gdt[index].limit_15_0 + (gdt[index].limit_19_16<<16);
	cpu.sreg[sreg_index].invisible.cache.granularity = gdt[index].granularity;
	cpu.sreg[sreg_index].invisible.cache.operation_size = gdt[index].operation_size;
	cpu.sreg[sreg_index].invisible.cache.pad0 = gdt[index].pad0;
	cpu.sreg[sreg_index].invisible.cache.present = gdt[index].present;
	cpu.sreg[sreg_index].invisible.cache.privilege_level = gdt[index].privilege_level;
	cpu.sreg[sreg_index].invisible.cache.segment_type = gdt[index].segment_type;
	cpu.sreg[sreg_index].invisible.cache.soft_use = gdt[index].soft_use;
	cpu.sreg[sreg_index].invisible.cache.type = gdt[index].type;
	Log("Value: %llx " ,cpu.sreg[sreg_index].invisible.value);
}
