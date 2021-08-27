#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <stdlib.h>
#include <regex.h>
const char registers[] = "$eax$ecx$edx$ebx$esp$edp$esi$edi$eip";
enum {
	NOTYPE = 256, 
	HEX = 0, TEN, 
	EQ, NEQ, AND, OR,
	PLUS, MINUS, TIMES, DIVIDE, 
	OR_COMPUTE, AND_COMPUTE, XOR_COMPUTE,
	ADDRESS_SIGN, NEGATIVE_SIGN, REVERSE_SIGN, DENY_SIGN, 
	FR_BRACKET, BA_BRACKET,	REGISTER,
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
	
	{"0x[a-f|0-9|A-F]+", HEX},		// HEX			0
	{"[0-9]+", TEN},				// TEN		

	{"==", EQ},						// equal	
	{"\\!=", NEQ},					// not equal
	{"\\&&", AND},					// and
	{"\\|\\|", OR},					// or	
	{"\\+", PLUS},					// plus			
	{"\\-", MINUS},					// minus		
	{"\\*", TIMES},					// times		
	{"\\/", DIVIDE},				// divide	

	{"\\|", OR_COMPUTE},			// 01 | 10 = 11
	{"\\&", AND_COMPUTE},			// 01 & 10 = 00
	{"\\^", XOR_COMPUTE},			// 10 ^ 01 = 11 	

	{"\\~" ,REVERSE_SIGN},			// Reverse - byte while
	{"\\!" ,DENY_SIGN},				// Logistic

	{"\\(", FR_BRACKET},			// for-bracket	
	{"\\)", BA_BRACKET},			// back-bracket
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", REGISTER}, // register 
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
	int cnt = 0;
	regmatch_t pmatch;
	
	nr_token = 0;
//	memset(tokens,0,sizeof(tokens));

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				// Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type){
					case NOTYPE:
						break;
					case MINUS:
//						if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE){
						if(nr_token==0 || (tokens[nr_token].type <= DENY_SIGN && tokens[nr_token].type >= EQ)){
							tokens[++nr_token].type = NEGATIVE_SIGN;
						}else{
							tokens[++nr_token].type = MINUS;
						}
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						break;
					case TIMES:
//						if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE){
						if(nr_token==0 || (tokens[nr_token].type <= DENY_SIGN && tokens[nr_token].type >= EQ)){
							tokens[++nr_token].type = ADDRESS_SIGN;
						}else{
							tokens[++nr_token].type = TIMES;
						}
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						break;
					default: 
						tokens[++nr_token].type = rules[i].token_type;
						strncpy(tokens[nr_token].str,substr_start,substr_len);
						break;
						//panic("please implement me");
				}
				
				break;
			}
		}
		if(i == NR_REGEX) {
			Log("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
		if(tokens[nr_token].type==FR_BRACKET) cnt++;
		else if(tokens[nr_token].type==BA_BRACKET) cnt--;
		if(cnt<0){
			Log("Error: Wrong Expression: Brackets");
			return false;
		}
	}
	if(cnt==0){
		return true;
	}
	Log("Error: Wrong Expression: Brackets");
	return false;
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
}

static int exe(int q,int p, bool *flag){
	int data = 0;
	switch ((tokens[q].type))
	{
		case NEGATIVE_SIGN:
			data = exe(q+1,p, flag);
			data = ~data + 1;
			break;
		case ADDRESS_SIGN:
			data = exe(q+1,p, flag);
			data = swaddr_read(data, 4);
			break;
		case REVERSE_SIGN:
			data = exe(q+1,p, flag);
			data = ~data;
			break;
		case DENY_SIGN:
			data = exe(q+1,p, flag);
			data = !data;
			break;
		default:			
			if(q>p){
				Log("Error: Wrong Expression\n");
				*flag = false;
				return 0;
			}else if(q==p){
				int position;
				char * temp;
				switch(tokens[p].type){
					case REGISTER:
						temp = strstr(registers,tokens[p].str);
						position = (temp-registers)/4;
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
						Log("A NUMBER OR REGISTER IS NEEDED, BUT %s IS WHAT WE GET.",tokens[p].str);
						*flag = false;
						return 0;
				}
			}else{
				bool bracket_flag = brackets(q,p);
				if(bracket_flag){
					q++,p--;
					return exe(q,p,flag);
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
					if(op==0){
						Log("Wrong Expression:	q=%d	p=%d\n" ,q,p);
						*flag = false;
						return 0;
					}
					int val1 = exe(q,op-1, flag);
					int val2 = exe(op+1,p, flag);
					switch(tokens[op].type){
						case EQ:
							data = (val1==val2);
							break;
						case NEQ:
							data = (val1 != val2);
							break;
						case AND:
							data = (val1 && val2);
							break;
						case OR:
							data = (val1 || val2);
							break;
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
						case OR_COMPUTE:
							data = val1 | val2;
							break;
						case AND_COMPUTE:
							data = val1 & val2;
							break;
						case XOR_COMPUTE:
							data = val1 ^ val2;
							break;
						default:
							Log("Error math type string=%s	type=%d\n" ,tokens[op].str ,tokens[op].type);
							*flag = false;
							return 0;
					}
				}
			}
			break;
	}
	// Log("q=%d p=%d	data=%d\n" ,q,p,data);
	return data;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* 	TODO: Insert codes to evaluate the expression.
		IF success == true : tokens shall contain the expression.
		for(i=1;i<=nr_token;i++) printf("%s	type=%d\n" ,tokens[i].str, tokens[i].type);
	*/
	int ans = exe(1,nr_token, success);
	
		int i;
		for(i=1;i<=nr_token;i++) printf("str=%s	type=%d\n" ,tokens[i].str, tokens[i].type);
	
	strcpy(e,tokens[1].str);
	for(i=2;i<=nr_token;i++){
		strncat(e, tokens[i].str, strlen(tokens[i].str));
	}
	return ans;
}

