#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum
{
	R_EAX,
	R_ECX,
	R_EDX,
	R_EBX,
	R_ESP,
	R_EBP,
	R_ESI,
	R_EDI
};
enum
{
	R_AX,
	R_CX,
	R_DX,
	R_BX,
	R_SP,
	R_BP,
	R_SI,
	R_DI
};
enum
{
	R_AL,
	R_CL,
	R_DL,
	R_BL,
	R_AH,
	R_CH,
	R_DH,
	R_BH
};
enum
{
	R_ES,
	R_CS,
	R_SS,
	R_DS
};

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct
{
	union
	{
		union
		{
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];
		/* Do NOT change the order of the GPRs' definitions. */
		/* Without struct, every register shall use the same 32B mem.  */
		struct
		{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
	swaddr_t eip;

	union
	{
		struct
		{
			uint32_t CF : 1;
			uint32_t pad0 : 1;
			uint32_t PF : 1;
			uint32_t pad1 : 1;
			uint32_t AF : 1;
			uint32_t pad2 : 1;
			uint32_t ZF : 1;
			uint32_t SF : 1;
			uint32_t TF : 1;
			uint32_t IF : 1;
			uint32_t DF : 1;
			uint32_t OF : 1;
			uint32_t IOPL : 2;
			uint32_t NT : 1;
			uint32_t pad3 : 1;
			uint16_t pad4;
		};
		uint32_t val;
	} eflags;

	union
	{
		struct
		{
			uint32_t Base : 32;
			uint32_t Limit : 16;
		};
	} GDTR;

	union
	{
		struct
		{
			uint32_t protect_enable : 1;
			uint32_t monitor_coprocessor : 1;
			uint32_t emulation : 1;
			uint32_t task_switched : 1;
			uint32_t extension_type : 1;
			uint32_t numeric_error : 1;
			uint32_t pad0 : 10;
			uint32_t write_protect : 1;
			uint32_t pad1 : 1;
			uint32_t alignment_mask : 1;
			uint32_t pad2 : 10;
			uint32_t no_write_through : 1;
			uint32_t cache_disable : 1;
			uint32_t paging : 1;
		};
		uint32_t val;
	} CR0;

	struct
	{
		union
		{
			struct
			{
				uint32_t RPL : 2;	 // requestor's privilege level
				uint32_t TI : 1;	 // table indicator
				uint32_t index : 13; // index in gdtr
			};
			uint32_t val;
		} visible;
		union
		{
			struct{
				uint32_t base 	: 32;
				uint32_t limit 	: 20;
				uint32_t type : 4;
				uint32_t segment_type : 1;
				uint32_t privilege_level : 2;
				uint32_t present : 1;
				uint32_t soft_use : 1;
				uint32_t operation_size : 1;
				uint32_t pad0 : 1;
				uint32_t granularity : 1;
			}cache;
			unsigned long long value : 64;
		} invisible;
	} sreg[4];

} CPU_state;

typedef struct SegmentDescriptor {
	uint32_t limit_15_0          : 16;
	uint32_t base_15_0           : 16;
	uint32_t base_23_16          : 8;
	uint32_t type                : 4;
	uint32_t segment_type        : 1;
	uint32_t privilege_level     : 2;
	uint32_t present             : 1;
	uint32_t limit_19_16         : 4;
	uint32_t soft_use            : 1;
	uint32_t operation_size      : 1;
	uint32_t pad0                : 1;
	uint32_t granularity         : 1;
	uint32_t base_31_24          : 8;
} SegDesc;

extern CPU_state cpu;

static inline int check_reg_index(int index)
{
	assert(index >= 0 && index < 8);
	return index;
}

void load_sreg(int sreg_index);

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char *regsl[];
extern const char *regsw[];
extern const char *regsb[];

#endif
