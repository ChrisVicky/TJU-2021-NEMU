#ifndef __JCC_H__
#define __JCC_H__

#define instr je
make_helper(concat(instr, _si_b));
make_helper(concat(instr, _si_v));
#undef instr

#endif