#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
const char *register_name[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
	static char *line_read = NULL;

	if (line_read)
	{
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read)
	{
		add_history(line_read);
	}
	else
	{
		int cnt = where_history();
		if (!cnt)
			return line_read;
		HIST_ENTRY **list = history_list();
		strcpy(line_read, list[cnt - 1]->line);
	}
	return line_read;
}
static int cmd_c(char *args)
{
	cpu_exec(-1);
	return 0;
}
static int cmd_q(char *args) { return -1; }
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *arg);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct
{
	char *name;
	char *description;
	int (*handler)(char *);
} cmd_table[] = {
	{"help", "Display informations about all supported commands", cmd_help},
	{"c", "Continue the execution of the program", cmd_c},
	{"q", "Exit NEMU", cmd_q},
	/* TODO: Add more commands */
	{"si", "Continue the execution for n steps", cmd_si},
	{"info", "Show registers", cmd_info},
	{"x", "Scan the cache", cmd_x},
	{"p", "Expression", cmd_p},
	{"w", "Watch Points", cmd_w},
	{"d", "Delete Points", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;
	if (arg == NULL)
	{
		/* no argument given */
		for (i = 0; i < NR_CMD; i++)
		{
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else
	{
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(arg, cmd_table[i].name) == 0)
			{
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("\033[1;31mUnknown command '%s'\33[0m\n", arg);
	}
	return 0;
}
static int cmd_si(char *args)
{
	int n;
	char *temp_args = strtok(args, " ");
	if (temp_args == NULL)
		n = 1;
	else
		n = atoi(temp_args);
	cpu_exec(n);
	return 0;
}
static int cmd_info(char *args)
{
	int i;
	bool flag = false;
	char *temp_args = strtok(args, " ");
	if (temp_args == NULL)
	{
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	char *temp_cmd = temp_args + strlen(temp_args) + 1;
	if (strcmp("r", temp_args) == 0)
	{
		for (i = 0; i < 8; i++)
		{
			if (strcmp("", temp_cmd) == 0 || strstr(temp_cmd, register_name[i]))
			{
				flag = true;
				printf("\33[40;33m%s		0x%08x		%d\33[0m\n", register_name[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
			}
		}
		if (strcmp("", temp_cmd) == 0 || strstr(temp_cmd, "eip"))
		{
			printf("\33[40;33meip		0x%08x		%d\33[0m\n", cpu.eip, cpu.eip);
			flag = true;
		}
		if (flag == false)
		{
			printf("\033[1;31mInvalid register '%s'\33[0m\n", temp_cmd);
		}
	}
	else if (strcmp("w", temp_args) == 0)
	{
		WP *head = get_head();
		if (head == NULL)
		{
			printf("\33[1;31mNo watchpoints.\33[0m\n");
			return 0;
		}
		printf("\33[1;31mNum	Enb	Expression\33[0m\n");
		while (head != NULL)
		{
			printf("\33[40;33m%d	%s	%s (0x%08x)\33[0m\n", head->NO, head->enable ? "Yes" : "No", head->expressions, head->old_value);
			head = head->next;
		}
	}
	else
	{
		printf("\033[1;31mUndified info command '%s'\33[0m\n", temp_args);
	}
	return 0;
}
static int cmd_x(char *args)
{
	if (args == NULL)
	{
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	char *args1 = strtok(args, " ");
	int t1 = strtol(args1, NULL, 10);
	char *args2 = args1 + strlen(args1) + 1;
	if(strlen(args2)==0){
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	bool flag = true;
	int t2 = expr(args2, &flag);
	if (!flag)
	{
		// printf("Wrong Expression %s\n", args2);
		return 0;
	}
	int i = 0, j;
	while (i < t1)
	{
		printf("\33[1;37m0x%08x: \33[0m", t2);
		for (j = 0; j < 4 && i < t1; i++, j++, t2 += 4)
		{
			printf("\33[40;33m0x%08x \33[0m", swaddr_read(t2, 4));
		}
		printf("\n");
	}
	return 0;
}
static int cmd_p(char *args)
{
	bool flag = true;
	if (args == NULL)
	{
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	int ans = expr(args, &flag);
	if (!flag)
	{
		// Log("Expression Error");
		return 0;
	}
	// printf("NEW E: %s\n" ,args);
	printf("\033[1;37mDEC: \033[0m\33[40;33m%d	\033[0m\033[1;37mHEX: \033[0m\33[40;33m0x%08x\\33[0mn", ans, ans);
	return 0;
}
static int cmd_w(char *args)
{
	bool flag = true;
	if (args == NULL)
	{
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	int ans = expr(args, &flag);
	if (!flag)
	{
		// Log("Expression Error");
		return 0;
	}
	// Log("Expression: %s	Ans: 0x%08x\n" ,args ,ans);
	
	// Log("args: %s\n" ,args);
	WP *wp = new_wp();
	if (wp == NULL)
	{
		return 0;
	}
	strcpy(wp->expressions, args);
	wp->old_value = ans;
	printf("\033[1;37mWatchpoint \033[0m\33[40;33m%d: %s\33[0m\n", wp->NO, wp->expressions);
	printf("\033[1;37mCrruent Value: \033[0m\33[40;33m0x%08x (%d)\33[0m\n", ans, ans);
	return 0;
}

static int cmd_d(char *args)
{
	if (args == NULL)
	{
		/*
		printf("\033[47;30mArguments required\033[0m");
		printf("\n");
		*/
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}

	char *args1 = strtok(args, " ");
	
	if (args1 == NULL)
	{
		printf("\33[1;31mArguments required\33[0m\n");
		return 0;
	}
	int number;
	number = strtol(args1, NULL, 10);
	if(!number && strcmp("0", args1)){
		printf("\033[1;31mInvalid Argument '%s'\033[0m\n" ,args1);
		return 0;
	}
	free_wp(number);
	return 0;
}

void ui_mainloop()
{
	while (1)
	{
		int i;
		/*HIST_ENTRY **list = history_list();
		for (i = 0; i < history_length; i++)
		{
			printf("\033[40;34m history_list[%d]: line: '%s' timestap: '%s' \n\033[0m", i, list[i]->line, list[i]->timestamp);
		}*/
		char *str = rl_gets();
		char *str_end = str + strlen(str);
		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL)
		{
			continue;
		}
		/*Log("str: '%s'", str);
		Log("str_end: '%s'", str_end);
		Log("cmd: '%s'", cmd);*/
		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end)
		{
			args = NULL;
		}
		/* Log("args: %s", args); */

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(cmd, cmd_table[i].name) == 0)
			{
				if (cmd_table[i].handler(args) < 0)
				{
					return;
				}
				break;
			}
		}
		if (i == NR_CMD)
		{
			printf("\033[1;31mUnknown command '%s'\033[0m\n", cmd);
		}
	}
}
/*
static bool is_number(char *arg){
	int i;
	char temp_arg = 'a';
	char HEX[] = "abcdef0123456789";
	if(arg[0]=='0' && arg[1]=='x') arg = arg+2;
	for(i=0;i<strlen(arg);i++){
		temp_arg = arg[i];
		if(strchr(HEX, temp_arg)==NULL) return false;
	}
	return true;
}
static int cmd_x_MORE(char* arg){
	char *temp_arg = arg;
	int cnt = 0;
	char *temp = strtok(temp_arg, " ");
	char *args[5];
	int t[5];
	while(temp!=NULL){
		if(cnt == 3) break;
		temp_arg = temp + strlen(temp) + 1;
		args[++cnt] = temp;
		temp = strtok(temp_arg, " ");
	}
	switch(cnt){
		case 0:
			printf("Argument required (starting display address).\n");
			return 0;
			break;
		case 1:
			if(!is_number(args[1])){
				printf("Unkown command argument '%s'\n" ,args[1]);
				break;
			}
			t[1] = strtol(args[1],NULL,16);
			printf("0x%08x:	0x%08x\n" ,t[1],swaddr_read(t[1],4));
			break;
		case 2:
			if(is_number(args[1])){
				if(is_number(args[2])){
					t[1] = strtol(args[1],NULL,10);
					t[2] = strtol(args[2],NULL,16);
					int i = 0,j;
					while(i<t[1]){
						printf("0x%08x:" ,t[2]);
						for(j=0;j<4&&i<t[1];j++,i++,t[2]+=4){
							printf("	0x%08x" ,swaddr_read(t[2],4));
						}
						printf("\n");
					}
				}else{
					printf("Invalid argument '%s'\n" ,args[2]);
				}
			}else if(is_number(args[2])){
				printf("Invalid argument '%s'\n" ,args[1]);
			}else{
				printf("Invalid argument '%s' '%s'\n" ,args[1] ,args[2]);
			}
			break;
		default:
			printf("Too many arguments\n");
			break;
	}
	return 0;
}
*/