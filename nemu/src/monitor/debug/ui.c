#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}
static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}
static int cmd_q(char *args) {
	return -1;
}
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{ "si","Continue the execution for n step", cmd_si},
	{"info", "Show all registers", cmd_info},
	{"x", "Scan the cache", cmd_x},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}
static int cmd_si(char* args){
	int n;
	char * temp_args = strtok(args," ");
	if(temp_args == NULL) n = 1;
	else n = atoi(temp_args);
	cpu_exec(n);
	return 0;
}
static int cmd_info(char* args){
	int i;
	char *cpu_name[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
	char *temp_args = strtok(args, " ");
	if(temp_args == NULL){
		printf("Invalid argument \n");
		return 0;
	}
	char *temp_cmd = temp_args + strlen(temp_args) + 1;
	if(strcmp("r",temp_args)==0){
		printf("register	value\n");
		for(i=0;i<8;i++){
			if(strcmp("",temp_cmd)==0 || strstr(temp_cmd, cpu_name[i])){
				printf("%s		0x%x\n" ,cpu_name[i],cpu.gpr[i]._32);
			}
		}
		if(strcmp("",temp_cmd)==0 || strstr(temp_cmd, "eip")){
			printf("eip		0x%x\n" ,cpu.eip);
		}
	}else{
		printf("Unknown command argument '%s'\n" ,temp_args);
	}
	return 0;
}
static int cmd_x(char* args){
	char *arg1 = strtok(args, " ");
	int t,i;
	if(arg1==NULL){
		printf("Invalid argument\n");
		return 0;
	}
	char *temp_args = args + strlen(args) + 1;
	if(strcmp("",temp_args)==0){
		if(strstr(arg1,"0x")){
			t = strtol(arg1,NULL,16);
			printf("0x%x : %x\n" ,t,swaddr_read(t,4));
		}else{
			printf("Unkown command argument '%s'\n" ,arg1);
		}
		return 0;
	}
	char *arg2 = strtok(temp_args, " ");
	if(strstr(arg2, "0x")){
		int t = strtol(arg1,NULL,16);
		int w = strtol(arg2,NULL,16);
		for(i=0;i<t;i+=4){
			printf("0x%x : %x\n" ,w,swaddr_read(w,4));
			w += 4;
		}
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
