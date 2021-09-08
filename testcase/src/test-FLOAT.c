#include "trap.h"
#include "FLOAT.h"

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	/* float is a 32-bit 1, 8, 23 bits structure variable */
	unsigned int temp = ((unsigned int *) & a) [0];
    printf("%x\n" ,temp);
	const int BIAS = 127;
	FLOAT ret = temp & 0x7fffff;
	unsigned int mark = temp >> 31;
	ret += (1<<23);
	int exp = ((temp >> 23) & 0xff) - BIAS;
	
	if(exp==0){
		ret = ret >> 7;
	}else if(exp-7 > 0){
		ret = ret << (exp - 7);
	}else{
		ret = ret >> (7 - exp);
	}
/*	switch((exp - 7) > 0){
		case 0:
			ret = ret >> (exp - 7);
			break;
		default:
			ret = ret << (exp - 7);
			break;
	}
*/
	if(temp >> 31) ret = ~ ret + 1;
	return ret;
}
int main()
{
    FLOAT _FLOAT_a = f2F(1.25);
    FLOAT _FLOAT_b = f2F(0.551222);
    const int _int_a = 0x14000;
    if(_FLOAT_a==_int_a){
        nemu_assert(1);
    }else{
        nemu_assert(0);
    }
    return 0;
}