#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#include "common.h"

#define NR_BP 32

typedef struct breakpoint {
	int NO;
	struct breakpoint *next;
	/* TODO: Add more members if necessary */
	int eip;
    bool enable;
	int cnt;

} BP;
BP * new_bp();
void delete_bp(int number);
BP * get_breakpoint_head();
#endif
