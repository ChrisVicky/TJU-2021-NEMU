#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <stdlib.h>
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
						if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE)
							rules[i].token_type = MINUS_SIGN;
						nr_token++;
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						tokens[nr_token].type = rules[i].token_type;
						break;
					case TIMES:
						if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE)
							rules[i].token_type = ADDRESS_SIGN;
						nr_token++;
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						tokens[nr_token].type = rules[i].token_type;
						break;
					default: 
						nr_token++;
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						tokens[nr_token].type = rules[i].token_type;
						break;
						//panic("please implement me");
				}
				//printf("str=%s	type=%d	str=%s	type=%d\n" ,tokens[nr_token].str, tokens[nr_token].type,tokens[nr_token-1].str,tokens[nr_token-1].type);
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

static bool brackets(int q,int p){
	int i;
	int cnt=0;
	for(i=q;i<p;i++){
		if(tokens[i].type==FR_BRACKET) cnt++;
		else if(tokens[i].type==BA_BRACKET) cnt--;
		if(!cnt) return false;
	}
	if(tokens[i].type==FR_BRACKET) cnt++;
	else if(tokens[i].type==BA_BRACKET) cnt--;
	if(cnt) return false;
	return true;
	panic("please implement me");
}


static int exe(int q,int p){
	int data = 0;
	if(q+1==p || q==p){
		int position;
		char * temp;
		switch(tokens[p].type){
			case REGISTER:
				temp = strstr(registers,tokens[p].str);
				position = (temp-registers)/3;
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
				data = strtol(tokens[p].str,NULL,16);
				break;
			case TEN:
				data = strtol(tokens[p].str,NULL,10);
				break;
			default:
				panic("Error math %s\n" ,tokens[p].str);
				break;
		}

		printf("$ q=%d p=%d	data=%d\n",q,p,data);
		if(q<p){
			switch(tokens[q].type){
				case MINUS_SIGN:

					printf("* q=%d p=%d	data=%d\n",q,p,data);
					data = ~data + 1;

					printf("* q=%d p=%d	data=%d\n",q,p,data);
					break;
				case ADDRESS_SIGN:
					data = hwaddr_read(data,4);
					break;
				default:
					panic("Error math type %s\n" ,tokens[q].str);
					break;		
			}
		}
	}else{
		bool flag = brackets(q,p);
		if(flag){
			q++,p--;
			return exe(q,p);
		}else{
			int i;
			int cnt = 0;
			int op=0;
			for(i=q;i<=p;i++){
				if(tokens[i].type==FR_BRACKET) cnt++;
				else if(tokens[i].type==BA_BRACKET) cnt--;
				else if(!cnt&&(tokens[i].type==TIMES||tokens[i].type==DIVIDE||tokens[i].type==PLUS||tokens[i].type==MINUS)){
					op = i;
					break;
				}
			}
			if(op==0) panic("Wrong Expression\n");
			int val1 = exe(q,op-1);
			int val2 = exe(op+1,p);
			switch(tokens[op].type){
				case MINUS:
					data = val1 - val2;
					break;
				case PLUS:
					data = val1 + val2;
					break;
				case TIMES:
					data = val1 * val2;
					break;
				case DIVIDE:
					data = val1 / val2;
					break;
				default:
					panic("Error math type %s\n" ,tokens[op].str);
			}
		}
	}
	printf("q=%d p=%d	data=%d\n" ,q,p,data);
	return data;
}

uint32_t expr(char *e, bool *success) {
	int i;
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	/* IF success == true : tokens shall contain the expression. */
	for(i=1;i<=nr_token;i++) printf("%s	type=%d\n" ,tokens[i].str, tokens[i].type);
	int ans = exe(1,nr_token);
	return ans;
}

