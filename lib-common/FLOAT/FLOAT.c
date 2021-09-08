#include "FLOAT.h"
#include <string.h>


FLOAT F_mul_F(FLOAT a, FLOAT b) {
//	nemu_assert(0);
return (FLOAT)(((long long)a * (long long)b) >> 16);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 * asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */
	/*
	asm volatile ("??? %2" 
				: "=a"(???), "=d"(???) 
				: "r"(???), "a"(???), "d"(???)
				);
				a->%eax, d->%edx, r->registers
				divl %1 %0 : %0 = %0 / %1
	*/
	
	int sign=1;
	if((a>>31)^(b>>31)){
		sign=-1;
	}
	a = Fabs(a);
	b = Fabs(b);
	FLOAT ans=a/b;
	int i;
	a=a%b;
	for(i=1;i<=16;i++)
	{
		a<<=1;
		ans<<=1;
		if(a>=b)
		{
			a-=b;
			ans++;
		}
	}
	return ans*sign;	

}

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
	int u[100];
	memcpy((void *)u,(void *)&a,4);
	int temp=u[0];
	const int BIAS = 150;
	FLOAT ret = temp & 0x7fffff;
	ret += (1<<23);
	int exp = ((temp >> 23) & 0xff) - BIAS;
	if(exp<-16)
		ret >>= -16 - exp;
	if(exp>-16)
		ret <<= 16+exp;
	if(temp >> 31) ret = ~ ret + 1;
	return ret;
}

FLOAT Fabs(FLOAT a) {

	if(a >> 31) a = ~ a + 1;
//	nemu_assert(0);
	return a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

