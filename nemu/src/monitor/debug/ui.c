#include "ui_impl.h"

/* IF_DEBUG==1 时启动ui_loop, 否则直接执行cpu_exec(-1); */
#define IF_DEBUG 1

void ui_mainloop()
{
#if IF_DEBUG == 0
	SUCCESS("NO %s" ,"DEBUG");
	cpu_exec(5);
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

		Log("INSTR");
					return;
				}
				break;
			}
		}
		if (i == NR_CMD)
		{
			ERROR("Unknown command '%s'\n" ,cmd);
		}

/* Print out the SEGMENT_REG */
		int ii = 0;
		char *SN[] = {"ES","CS","SS","DS"};
		Log("Current eip = %x" ,cpu.eip);
		Log("%-10s%-10s%-10s" ,"Name","visible","limit");
		for(ii=0;ii<4;ii++){
			Log("%-10s%-10x%-16x" ,SN[ii],cpu.sreg[ii].visible.val,cpu.sreg[ii].invisible.cache.limit);
		}
	}
}
#undef IF_DEBUG
