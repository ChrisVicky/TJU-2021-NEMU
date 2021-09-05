#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "cpu/helper.h"
#include "monitor/watchpoint.h"
#include "monitor/breakpoint.h"
#include <setjmp.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = STOP;

int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

/* Used with exception handling. */
jmp_buf jbuf;

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
}

/* Simulate how the CPU works. */
void cpu_exec(volatile uint32_t n) {
	if(nemu_state == END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	volatile uint32_t n_temp = n;
#endif

	setjmp(jbuf);

	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			fputc('.', stderr);
		}
#endif

		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		int instr_len = exec(cpu.eip);

		cpu.eip += instr_len;

#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);
		if(n_temp < MAX_INSTR_TO_PRINT) {
			printf("%s\n", asm_buf);
		}else if(n==n_temp){
			printf("%s\n", asm_buf);
		}else if(n==1){
			printf("......\n");
			printf("%s\n", asm_buf);		
		}
#endif

		/* TODO: check watchpoints here. */
		WP * head = get_watchpoint_head();
		while(head!=NULL){
			if(head->enable==false){
				head = head->next;
				continue;
			}
			char *temp_e = (head)->expressions;
			bool flag = true;
			int value = expr(temp_e, &flag);
			if(value == head->old_value){
				head = head->next;
				continue;
			}
			nemu_state = STOP;
			head->cnt ++;
			printf("\033[1;36mHint Watchpoint %d at address 0x%08x, expr = \033[0m" ,head->NO ,cpu.eip);
			printf("\033[40;36m%s\033[0m\n" ,head->expressions);
			
			printf("\033[1;37mOld value = \033[0m0x%08x\n" ,head->old_value);
			printf("\033[1;37mNew value = \033[0m0x%08x\n" ,value);
			printf("\033[1;37mHit it \33[0m\33[1;36m%d\33[0m\33[1;37m Times\33[0m\n" ,head->cnt);
			//printf("Value: 0x%08x ->" ,head->old_value);
			//printf(" 0x%08x\n\n" ,value);
			head->old_value = value;
			head = head->next;
		}

		/* TODO: Check breakpoints here. */
		BP * b_head = get_breakpoint_head();
		while(b_head!=NULL){
			if(b_head->enable==false){
				b_head = b_head->next;
				continue;
			}
			int value = cpu.eip;
			if(value != b_head->eip){
				b_head = b_head->next;
				continue;
			}
			nemu_state = STOP;
			b_head->cnt ++;
			printf("\033[1;36mHint Watchpoint %d at address 0x%08x, expr = \033[0m\n" ,b_head->NO ,cpu.eip);
			printf("\033[1;37mHit it \33[0m\33[1;36m%d\33[0m\33[1;37m Times\33[0m\n" ,b_head->cnt);
			//printf("Value: 0x%08x ->" ,b_head->old_value);
			//printf(" 0x%08x\n\n" ,value);
			b_head = b_head->next;
		}


#ifdef HAS_DEVICE
		extern void device_update();
		device_update();
#endif

		if(nemu_state != RUNNING) { return; }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
}
