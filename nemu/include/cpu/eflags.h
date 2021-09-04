#ifndef __EFLAGS_H__
#define __EFLAGS_H__

#include "common.h"

void update_eflags_pf_zf_sf(uint32_t);
void update_eflags_cf_of_MINUS(int32_t, uint32_t, uint32_t);
void update_eflags_cf_of_PLUS(int32_t, uint32_t, uint32_t);
#endif
