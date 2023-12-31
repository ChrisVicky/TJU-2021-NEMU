#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <stdlib.h>
#include <regex.h>
bool is_variable(char * );
int get_address_by_name(char * );

const char registers[] = "$eax$ecx$edx$ebx$esp$ebp$esi$edi$eip";
enum
{
	NOTYPE = 256,
	HEX = 0,
	TEN,

	//18
	EQ,
	NEQ,
	ADDRESS_SIGN,
	NEGATIVE_SIGN,
	REVERSE_SIGN,
	DENY_SIGN,
	AND_COMPUTE,
	XOR_COMPUTE,
	OR_COMPUTE,
	AND,
	OR,
	SMALLER,
	BIGGER,
	SMALLER_EQ,
	BIGGER_EQ,
	LEFT,
	RIGHT,
	PLUS,
	MINUS,
	TIMES,
	DIVIDE,
	MODE,

	FR_BRACKET,
	BA_BRACKET,
	REGISTER,
	VARIABLE,
	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +", NOTYPE, 0}, // spaces		256

	{"0x[a-f|0-9|A-F]+", HEX, 0}, // HEX			0
	{"[0-9]+", TEN, 0},			  // TEN

	{"==", EQ, 7},			 // equal
	{"!=", NEQ, 7},			 // not equal
	{"&&", AND, 11},		 // and
	{"\\|\\|", OR, 12},		 // or
	{"&", AND_COMPUTE, 8},	 // 01 & 10 = 00
	{"\\^", XOR_COMPUTE, 9}, // 10 ^ 01 = 11
	{"\\|", OR_COMPUTE, 10}, // 01 | 10 = 11
	{"<<", LEFT, 5},		 // left_move
	{">>", RIGHT, 5},		 // right_move
	{"<=", SMALLER_EQ, 6},	 // smaller or equal
	{">=", BIGGER_EQ, 6},	 // bigger or qual

	{"<", SMALLER, 6}, //smaller
	{">", BIGGER, 6},  // bigger

	{"\\+", PLUS, 4},  // plus
	{"\\-", MINUS, 4}, // minus

	{"\\*", TIMES, 3},	// times
	{"\\/", DIVIDE, 3}, // divide
	{"\\%", MODE, 3},	// mode

	{"\\~", REVERSE_SIGN, 2}, // Reverse - byte while
	{"\\!", DENY_SIGN, 2},	  // Logistic

	{"\\(", FR_BRACKET, 1},									   // for-bracket
	{"\\)", BA_BRACKET, 1},									   // back-bracket
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", REGISTER, 1}, // register
	{"\\$(EAX|ECX|EDX|EBX|ESP|EBP|ESI|EDI|EIP)", REGISTER, 1}, // register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
	int priority;
} Token;

Token tokens[32];
int nr_token;
bool vst[32];

static bool is_sign(Token t)
{
	return (t.type <= MODE && t.type >= EQ);
}

static bool make_token(char *e)
{
	int position = 0;
	int cnt = 0;
	regmatch_t pmatch;

	nr_token = 0;
	memset(tokens, 0, sizeof(tokens));
	memset(vst, 0, sizeof(vst));

	while (e[position] != '\0')
	{
		int i;
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch (rules[i].token_type)
				{
				case NOTYPE:
					break;
				case MINUS:
					// if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE){
					if (nr_token == 0 || is_sign(tokens[nr_token]))
					{
						tokens[++nr_token].type = NEGATIVE_SIGN;
						tokens[nr_token].priority = 2;
					}
					else
					{
						tokens[++nr_token].type = MINUS;
						tokens[nr_token].priority = rules[i].priority;
					}
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					break;
				case TIMES:
					// if(nr_token==0 || tokens[nr_token].type==PLUS||tokens[nr_token].type==MINUS||tokens[nr_token].type==TIMES||tokens[nr_token].type==DIVIDE){
					if (nr_token == 0 || is_sign(tokens[nr_token]))
					{
						tokens[++nr_token].type = ADDRESS_SIGN;
						tokens[nr_token].priority = 2;
					}
					else
					{
						tokens[++nr_token].type = TIMES;
						tokens[nr_token].priority = rules[i].priority;
					}
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					break;
				default:
					tokens[++nr_token].type = rules[i].token_type;
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].priority = rules[i].priority;
					break;
					//panic("please implement me");
				}

				break;
			}
		}
		// Log("e='%s' i=%d" ,e ,i);
		if (i == NR_REGEX)
		{
			int len = is_variable(e + position);
			if(len){
				tokens[++nr_token].type = VARIABLE;
				strncpy(tokens[nr_token].str, e + position, len);
				e += len;
			}else{
				// Log("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
				printf("\33[1;34mNo match at position %d\n%s\n%*.s^\33[0m\n", position, e, position, "");
				return false;
			}
		}
		//Log("tokens[%d] = %s" ,nr_token, tokens[nr_token].str);
		if (tokens[nr_token].type == FR_BRACKET && !vst[nr_token])
		{
		//	Log("FRONT %s" ,tokens[nr_token].str);
			cnt++;
			vst[nr_token] = 1;
		}
		else if (tokens[nr_token].type == BA_BRACKET && !vst[nr_token])
		{
		//	Log("BACK %s" ,tokens[nr_token].str);
			cnt--;
			vst[nr_token] = 1;
		}
		if (cnt < 0)
		{
			printf("\33[1;34mError: Wrong Expression: Brackets\33[0m\n");
			return false;
		}
	}
	if (cnt == 0)
	{
		return true;
	}
	printf("\33[1;34mError: Wrong Expression: Brackets\33[0m\n");
	return false;
}

static bool brackets(int q, int p)
{
	int i;
	int cnt = 0;
	for (i = q; i < p; i++)
	{
		if (tokens[i].type == FR_BRACKET)
			cnt++;
		else if (tokens[i].type == BA_BRACKET)
			cnt--;
		if (!cnt)
			return false;
	}
	if (tokens[i].type == FR_BRACKET)
		cnt++;
	else if (tokens[i].type == BA_BRACKET)
		cnt--;
	if (cnt)
		return false;
	return true;
}

static int getOp(int q, int p, bool *flag)
{
	int cnt = 0, priority = -1;
	int i, op = 0;
	for (i = q; i <= p; i++)
	{
		if (tokens[i].type == FR_BRACKET)
			cnt++;
		else if (tokens[i].type == BA_BRACKET)
			cnt--;
		else if (!cnt && (is_sign(tokens[i]) && tokens[i].priority > priority))
		{
			op = i;
			priority = tokens[i].priority;
		}
	}
	return op;
}

static uint32_t exe(int q, int p, bool *flag)
{
	int data = 0;
	if (q > p)
	{
		if (*flag == true)
			printf("\33[1;34mError: Wrong Expression\33[0m\n");
		*flag = false;
		return 0;
	}
	else if (q == p)
	{
		int position;
		char *temp;
		switch (tokens[p].type)
		{
		case REGISTER:
			temp = strstr(registers, tokens[p].str);
			position = (temp - registers) / 4;
			switch (position)
			{
			case 8:
				data = cpu.eip;
				break;
			default:
				data = cpu.gpr[position]._32;
				break;
			}

			//	Log("Reg %s; data=%x; temp=%s;p osition=%d" ,tokens[p].str ,data ,temp ,position);
			break;
		case HEX:
			data = strtol(tokens[p].str, NULL, 16);
			break;
		case TEN:
			data = strtol(tokens[p].str, NULL, 10);
			break;
		case VARIABLE:
			data = get_address_by_name(tokens[p].str);
			break;
		default:
			//Log("q=%d	p=%d" ,q,p);
			printf("\33[1;34mA NUMBER or REGISTER is needed, BUT %s is what we get\33[0m\n", tokens[p].str);
			*flag = false;
			return 0;
		}
	}
	else
	{
		bool bracket_flag = brackets(q, p);
		if (bracket_flag)
		{
			q++, p--;
			return exe(q, p, flag);
		}
		else
		{
			int op = getOp(q, p, flag);
			//Log("%s" ,tokens[q].str);
			if (op == 0)
			{
				char *temp_str = tokens[q].str;
				int i;
				for (i = q + 1; i <= p; i++)
					strcat(temp_str, tokens[i].str);
				printf("\33[1;34mWrong Expression: '%s'\33[0m\n", temp_str);
				*flag = false;
				return 0;
			}
			//Log("q=%d	op=%d	p=%d" ,q,op,p);
			int val1 = 0;
			if(op>q) val1 = exe(q, op - 1, flag);
			int val2 = exe(op + 1, p, flag);
			switch (tokens[op].type)
			{
			case NEGATIVE_SIGN: // "-"
				data = ~val2 + 1;
				break;
			case ADDRESS_SIGN: // "*"
				data = swaddr_read(val2, 4, R_DS);
				break;
			case REVERSE_SIGN: // "~"
				data = ~val2;
				break;
			case DENY_SIGN: // "!"
				data = !val2;
				break;
			case EQ:
				data = (val1 == val2);
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
			case MODE:
				data = val1 % val2;
				break;
			case LEFT:
				data = val1 << val2;
				break;
			case RIGHT:
				data = val1 >> val2;
				break;
			case BIGGER:
				data = val1 > val2;
				break;
			case SMALLER:
				data = val1 < val2;
				break;
			case BIGGER_EQ:
				data = val1 <= val2;
				break;
			case SMALLER_EQ:
				data = val1 >= val2;
				break;
			default:
				printf("\33[1;34mError math type string=%s	type=%d\33[0m\n", tokens[op].str, tokens[op].type);
				*flag = false;
				return 0;
			}
		}
	}
	// Log("q=%d p=%d	data=%d\n" ,q,p,data);
	return data;
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
	/* 	TODO: Insert codes to evaluate the expression.
		IF success == true : tokens shall contain the expression.
		for(i=1;i<=nr_token;i++) printf("%s	type=%d\n" ,tokens[i].str, tokens[i].type);
	*/
	int i;
	//for(i=1;i<=nr_token;i++) printf("\033[40;31mstr=%s	type=%d	priority=%d\033[0m\n" ,tokens[i].str, tokens[i].type, tokens[i].priority);
	int ans = exe(1, nr_token, success);
	strcpy(e, tokens[1].str);
	for (i = 2; i <= nr_token; i++)
	{
		strncat(e, tokens[i].str, strlen(tokens[i].str));
	}
	return ans;
}
