#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
const char registers[] = "eaxecxedxebxespedpesiedieip";
enum {
	NOTYPE = 256, EQ = 0, HEX, TEN, 
	PLUS, MINUS, TIMES, DIVIDE, FR_BRACKET, BA_BRACKET, 
	REGISTER,
	ADDRESS_SIGN,
	MINUS_SIGN
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},				// spaces		256
	{"==", EQ},						// equal		0
	{"0x[a-f|0-9|A-F]+", HEX},		// HEX			1
	{"[0-9]+", TEN},				// TEN			2
	{"\\+", PLUS},					// plus			3
	{"\\-", MINUS},					// minus		4
	{"\\*", TIMES},					// times		5
	{"\\/", DIVIDE},				// divide		6
	{"\\(", FR_BRACKET},			// for-bracket	7
	{"\\)", BA_BRACKET},			// back-bracket	8
	{"$[eax|ecx|edx|ebx|esp|ebp|esi|edi|eip]", REGISTER}, // register 9
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type){
					case NOTYPE:
						break;
					case MINUS:
						if(i==0 || tokens[i-1].type==PLUS||tokens[i-1].type==MINUS||tokens[i-1].type==TIMES||tokens[i-1].type==DIVIDE)
							tokens[i].type = MINUS_SIGN;
					case TIMES:
						if(i==0 || tokens[i-1].type==PLUS||tokens[i-1].type==MINUS||tokens[i-1].type==TIMES||tokens[i-1].type==DIVIDE)
							tokens[i].type = ADDRESS_SIGN;
					default: 
						strncpy(tokens[++nr_token].str,substr_start,substr_len);
						tokens[nr_token].type = rules[i].token_type;
						//panic("please implement me");
				}
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true; 
}
/*
static bool brackets(int q,int p){
	int i;
	int cnt=0;
	for(i=q;i<p-1;i++){
		if(tokens[i].type==FR_BRACKET) cnt++;
		else if(tokens[i].type==BA_BRACKET) cnt--;
		if(!cnt) return false;
	}
	if(tokens[i].type==FR_BRACKET) cnt++;
	else if(tokens[i].type==BA_BRACKET) cnt--;
	if(cnt) return false;
	return true;
}
*/
/*
static int exp(int q,int p){
	if(q>p){

	}else if(q==p){
 		int data;
		int position;
		switch(tokens[q].type){
			case REGISTER:
				position = strstr(registers,tokens[q].str);
				position /= 3;
				switch(position){
					case 8:
						data = cpu.eip;
						break;
					default:
						data = cpu.gpr[position]._32;
						break;
				}
				break;
			case HEX:
				data = strtol(tokens[q].str,NULL,16);
				break;
			case TEN:
				data = strtol(tokens[q].str,NULL,10);
				break;
			default:
				panic("ERROR %s\n" ,tokens[q].str);
				break;
		}
		return data;
	}else{
		bool flag = brackets(q,p);
		if(flag){
			q++,p--;
			return exp(q,p);
		}else{
			int i=q;
			int cnt = 0;
			int op;
			for(i;i<p;i++){
				if(tokens[i].type==FR_BRACKET) cnt++;
				else if(tokens[i].type==BA_BRACKET) cnt--;
				else if(tokens[i].type==TIMES||tokens[i].type==DIVIDE||tokens[i].type==PLUS||tokens[i].type==MINUS){
					op = i;
					break;
				}
			}
			int val1 = exp(q,op-1);
			int val2 = exp(op+1,p);

		}
	}
	return 0;
}
*/
uint32_t expr(char *e, bool *success) {
	int i;
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	/* IF success == true : tokens shall contain the expression. */
	for(i=1;i<=nr_token;i++) printf("%s\n" ,tokens[i].str);
	panic("please implement me");
	return 0;
}

