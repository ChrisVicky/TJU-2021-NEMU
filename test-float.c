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
int main(){
	int t = f2F(1.25);
	if(t == 0x140){
		printf("YES");
	}
	printf("%x\n" ,t);
	return 0;
}
