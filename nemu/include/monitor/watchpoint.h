#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define NR_WP 32

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expressions[32*32];
	int old_value;
	bool enable;

} WP;
WP * new_wp();
void delete_wp(int number);
WP * get_head();
#endif
