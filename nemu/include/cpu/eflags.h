#ifndef __EFLAGS_H__
#define __EFLAGS_H__

#include "common.h"

void update_eflags_pf_zf_sf(uint32_t);
void update_eflags_cf_of(int32_t, int32_t, int32_t);

#endif
