#include <stdio.h>
#define FLOAT int
int f2F(float a) {
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
/*
	unsigned int temp = ((unsigned int *) & a) [0];
	const int BIAS = 127;
	FLOAT ret = temp & 0x7fffff;
	unsigned int mark = temp >> 31;
	ret += (1<<23);
	int exp = ((temp >> 23) & 0xff) - BIAS;
	printf("temp=%x	exp=%d	ret=%x	mark=%d	\n"  ,temp,exp, ret, mark);
	
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

	if(temp >> 31) ret = ~ ret + 1;
	return ret;*/
	int u[100];
	memcpy((void *)u,(void *)&a,4);
	int tep=u[0];
	int sign = tep >> 31;
  int exp = (tep >> 23) & 0xff;
  int x = tep & 0x7fffff;
  if (exp != 0)
    x += 1 << 23;
  exp -= 150;
  if (exp < -16)
    x >>= -16 - exp;
  if (exp > -16)
    x <<= 16 + exp;
  return sign == 0 ? x : -x;
}
int main(){
	int t = f2F(1.25);
	FLOAT _FLOAT_q = f2F(0.0001);
	printf("%x\n" ,_FLOAT_q);
	FLOAT _FLOAT_a = f2F(1e-4);
	printf("%x\n" ,_FLOAT_a);
    FLOAT _FLOAT_b = f2F(0.551222);
	printf("%x\n" ,_FLOAT_b);
	FLOAT _FLOAT_c = f2F(-1.0);
	printf("%x\n" ,_FLOAT_c);
	FLOAT _FLOAT_d = f2F(1.0);
	printf("%x\n" ,_FLOAT_d);
	return 0;
}
