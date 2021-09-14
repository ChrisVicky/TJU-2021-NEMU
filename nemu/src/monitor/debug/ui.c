#include "ui_impl.h"

/* IF_DEBUG==1 时启动ui_loop, 否则直接执行cpu_exec(-1); */
#define IF_DEBUG 0

void ui_mainloop()
{
#if IF_DEBUG == 0
	SUCCESS("NO %s" ,"DEBUG");
	cpu_exec(10);
	return ;
#endif
	SUCCESS("DEBUG\n");
	while (1)
	{
		int i;
		char *str = rl_gets();
		char *str_end = str + strlen(str);
		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL)
		{
			continue;
		}
		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end)
		{
			args = NULL;
		}
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
			ERROR("Unknown command '%s'\n" ,cmd);
		}
	}
}
#undef IF_DEBUG
