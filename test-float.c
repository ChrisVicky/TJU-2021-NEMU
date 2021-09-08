#include <stdio.h>
#define FLOAT int
int f2FF(float a) {
	int tep = ((int *) & a) [0];
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
	unsigned int temp = ((unsigned int *) & a) [0];
	const int BIAS = 127;
	FLOAT ret = temp & 0x7fffff;
	int mark = temp >> 31;
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
*/
	if(temp >> 31) ret = ~ ret + 1;
	return ret;
}
int main(){
	int t = f2F(1.25);
	FLOAT _FLOAT_q = f2F(0.0001);
	FLOAT _FLOAT_p = f2FF(0.0001);
	if(_FLOAT_q==_FLOAT_p) printf("YES");
	else printf("NO");
	 _FLOAT_q = f2F(1e-4);
	 _FLOAT_p = f2FF(1e-4);
	if(_FLOAT_q==_FLOAT_p) printf("YES");
	else printf("NO");
	 _FLOAT_q = f2F(0.551222);
	 _FLOAT_p = f2FF(0.551222);
	if(_FLOAT_q==_FLOAT_p) printf("YES");
	else printf("NO");
	 _FLOAT_q = f2F(1);
	 _FLOAT_p = f2FF(1);
	if(_FLOAT_q==_FLOAT_p) printf("YES");
	else printf("NO");
	 _FLOAT_q = f2F(-1);
	 _FLOAT_p = f2FF(-1);
	if(_FLOAT_q==_FLOAT_p) printf("YES");
	else printf("NO");
	
	return 0;
}