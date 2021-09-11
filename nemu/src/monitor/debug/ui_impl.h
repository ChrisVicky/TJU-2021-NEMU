#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/breakpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

/* IF_DEBUG==1 时启动ui_loop, 否则直接执行cpu_exec(-1); */
#define SUCCESS(format, ...) printf("\33[1;33m" format "\33[0m",## __VA_ARGS__)
#define ERROR(format, ...) printf("\33[1;31m" format "\33[0m", ## __VA_ARGS__)
#define PRINT(format, ...) printf("\33[1;37m" format "\33[0m", ## __VA_ARGS__)
void cpu_exec(uint32_t);
const char *register_name[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *arg);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_b(char *args);
static int cmd_d(char *args);
static int cmd_bt(char *args);
char * get_func_name_by_address(int);

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
	{"si", "Excute 'n' steps (defalt: n = 1)", cmd_si},
	{"info", "Show Registers('r') or Watchpoints('w')", cmd_info},
	{"x", "Scan cache", cmd_x},
	{"p", "Compute an expression", cmd_p},
	{"w", "Make watchpoints", cmd_w},
	{"b", "Make breakpoints", cmd_b},
	{"d", "Delete points", cmd_d},
	{"bt", "Print Stack" , cmd_bt},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

char *rl_gets()
{
	static char *line_read = NULL;

	if (line_read)
	{
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read){
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

static int cmd_q(char *args){
    if(args!=NULL){
        ERROR("Too many arguments!\n");
        return 0;
    }
    return -1;
}

static int cmd_c(char *args){
    if(args!=NULL){
        ERROR("Too many arguments!\n");
        return 0;
    }
    cpu_exec(-1);
    return 0;
}

static int cmd_help(char *args){
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;
	if (arg == NULL)
	{
		/* no argument given */
		for (i = 0; i < NR_CMD; i++)
		{
			printf("\33[1;37m%s\33[0m - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else
	{
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(arg, cmd_table[i].name) == 0)
			{
				printf("\33[1;37m%s\33[0m - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		ERROR("Unknown command '%s'\n" ,arg);
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
				printf("\33[1;37m%s		\33[0m0x%08x		%d\n", register_name[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
			}
		}
		if (strcmp("", temp_cmd) == 0 || strstr(temp_cmd, "eip"))
		{
			printf("\33[1;37meip		\33[0m0x%08x		%d\n", cpu.eip, cpu.eip);
			flag = true;
		}
		if (flag == false)
		{
			printf("\033[1;31mInvalid register '%s'\33[0m\n", temp_cmd);
		}
	}
	else if (strcmp("w", temp_args) == 0)
	{
		WP *head = get_watchpoint_head();
		if (head == NULL)
		{
			printf("\33[1;37mNo watchpoints\33[0m\n");
			return 0;
		}
		printf("\33[1;37mNum	Enb	Expression\33[0m\n");
		while (head != NULL)
		{
			printf("\33[1;36m%d	\33[0m%s	\33[40;36m%s \33[0m(0x%08x)\n", head->NO, head->enable ? "\33[1;32mYes\33[0m" : "\33[1;31mNo\33[0m", head->expressions, head->old_value);
			head = head->next;
		}
	}
	else if(strcmp("b", temp_args) == 0){
		BP *head = get_breakpoint_head();
		if(head==NULL){
			printf("\33[1;37mNo breakpoints\33[0m\n");
			return 0;
		}
		printf("\33[1;37mNum	Enb	eip\33[0m\n");
		while(head!=NULL){
			printf("\33[1;36m%d	\33[0m%s	\33[40;36m%d\33[0m\n" ,head->NO, head->enable ? "\33[1;32mYes\33[0m" : "\33[1;31mNo\33[0m", head->eip);
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
		ERROR("Arguments required\n");
		return 0;
	}
	char *args1 = strtok(args, " ");
	int t1 = strtol(args1, NULL, 10);
	char *args2 = args1 + strlen(args1) + 1;
	if(strlen(args2)==0){
		ERROR("Arguments required\n");
		return 0;
	}
	bool flag = true;
	int t2 = expr(args2, &flag);
	if (!flag)
	{
		return 0;
	}
	int i = 0, j;
	if(t1==0){
		ERROR("Invalid Argument '%s' \n" ,args1);
		return 0;
	}
	while (i < t1)
	{
		printf("\33[1;37m0x%08x: \33[0m", t2);
		for (j = 0; j < 4 && i < t1; i++, j++, t2 += 4)
		{
			printf("0x%08x ", swaddr_read(t2, 4));
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
		ERROR("Arguments required\n");
		return 0;
	}
	int ans = expr(args, &flag);
	if (!flag)
	{
		// Log("Expression Error");
		return 0;
	}
	// printf("NEW E: %s\n" ,args);
	printf("%d	(\033[1;32m0x%08x\33[0m)\n", ans, ans);
	return 0;
}
static int cmd_w(char *args)
{
	bool flag = true;
	if (args == NULL)
	{
		ERROR("Arguments required\n");
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
	printf("\033[1;37mWatchpoint \033[0m\33[1;36m%d\33[0m : \33[40;36m%s\33[0m\n", wp->NO, wp->expressions);
	printf("\033[1;37mCrruent Value: \033[0m0x%08x (%d)\n", ans, ans);
	return 0;
}

static int cmd_b(char *args)
{	
//	bool flag = true;
	if (args == NULL)
	{
		ERROR("Arguments required\n");
		return 0;
	}
	int ans = strtol(args, NULL, 16);
	BP *bp = new_bp();
	if (bp == NULL)
	{
		return 0;
	}
	bp->eip = ans;
	printf("\033[1;37mBreakpoint \033[0m\33[1;36m%d\33[0m : \33[40;36m%d\33[0m\n", bp->NO, bp->eip);
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
		ERROR("Arguments required\n");
		return 0;
	}

	char *args1 = strtok(args, " ");
	
	if (args1 == NULL)
	{
		ERROR("Arguments required\n");
		return 0;
	}
	args = args + strlen(args1) + 1;
	char *args2 = strtok(args, " ");
	if(args2 == NULL){
		ERROR("Arguments required\n");
		return 0;
	}
	if (strcmp("w", args1) == 0){
		int number;
		number = strtol(args1, NULL, 10);
		if (!number && strcmp("0", args1))
		{
			ERROR("Undified info command '%s'\n" ,args1);
			return 0;
		}
		delete_wp(number);
		return 0;
	}else if(strcmp("b", args1) == 0){
		int number;
		number = strtol(args1, NULL, 10);
		if (!number && strcmp("0", args1))
		{
			ERROR("Undified info command '%s'\n" ,args1);
			return 0;
		}
		delete_bp(number);
		return 0;
	}else{
		ERROR("Undified info command '%s'\n" ,args1);
	return 0;
	}
}

static int cmd_bt(char * args){
	if(args!=NULL){
		ERROR("Too many arguments!\n");
		return 0;
	}
	int ebp = cpu.ebp;
	int esp = cpu.esp;
	int eip = cpu.eip;
	if(esp>=HW_MEM_SIZE){
		ERROR("Stack Top (esp=0x%x) is currently outside of the physical memory!\n", esp);
		return 0;
	}
	while(esp!=0){
		char * func_name = get_func_name_by_address(eip);
		SUCCESS("FUNC NAME\n");
		PRINT("%s\n" ,func_name);
		SUCCESS("%-12s%-12s\n" ,"ebp" ,"esp");
		PRINT("0x%-10x0x%-10x\n" ,ebp, esp);
		int i,x, temp_ebp=ebp;
		SUCCESS("%-12s%-12s%-12s%-12s\n" ,"val1","val2","val3","val4");
		for(i=0;i<4&&temp_ebp>=esp;i++,temp_ebp-=4){
			x = swaddr_read(temp_ebp, 4);
			PRINT("0x%-10x" ,x);
		}
		printf("\n\n");
		if(ebp==0){
			return 0;
		}
		esp = ebp;
		ebp = swaddr_read(esp, 4);
		eip = swaddr_read(esp+4, 4);
	}
	return 0;
}
