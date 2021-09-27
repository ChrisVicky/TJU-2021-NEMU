#include "nemu.h"

// #define ENTRY_START 0x100000
// #define ENTRY_START 0x800000
#define ENTRY_START 0x100000

extern uint8_t entry [];
extern uint32_t entry_len;
extern char *exec_file;

void load_elf_tables(int, char *[]);
//void load_elf_variables();
void init_regex();
void init_wp_pool();
void init_bp_pool();
void init_ddr3();
void initialize_cache();

FILE *log_fp = NULL;

static void init_log() {
	log_fp = fopen("log.txt", "w");
	Assert(log_fp, "Can not open 'log.txt'");
}

static void welcome() {
	printf("Welcome to NEMU!\nThe executable is %s.\nFor help, type \"help\"\n",
			exec_file);
}

void init_monitor(int argc, char *argv[]) {
	/* Perform some global initialization */

	/* Open the log file. */
	init_log();

	/* Load the string table and symbol table from the ELF file for future use. */
	load_elf_tables(argc, argv);
	
	//load_elf_variables();
	

	/* Compile the regular expressions. */
	init_regex();
	

	/* Initialize the watchpoint pool. */
	init_wp_pool();
	
	/* Initialize the breakpoint pool*/
	init_bp_pool();
	

	/* Display welcome message. */
	welcome();
	
}

#ifdef USE_RAMDISK
static void init_ramdisk() {
	int ret;
	const int ramdisk_max_size = 0xa0000;
	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(0), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
#endif

static void load_entry() {
	int ret;
	FILE *fp = fopen("entry", "rb");
	Assert(fp, "Can not open 'entry'");

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}

static void init_eflags(){
	cpu.eflags.val = 2;
}

static void init_CR0(){
	cr0.val = 0;
	cr0.paging = 1;
	/* 0: Real Mode */
}
static void init_cs(){
	cpu.sreg[R_CS].visible.val = 0;
	cpu.sreg[R_CS].invisible.value = 0;
	cpu.sreg[R_CS].invisible.cache.limit = 0xffffffff;
}

void restart() {
	/* Perform some initialization to restart a program */
#ifdef USE_RAMDISK
	/* Read the file with name `argv[1]' into ramdisk. */
	init_ramdisk();
#endif

	/* Initialize cache */
	initialize_cache();
	
	/* Initialize e-flags */
	init_eflags();

	/* init CR0 -> control register 0 */
	init_CR0();

	init_cs();

	/* Read the entry code into memory. */
	load_entry();

	/* Set the initial instruction pointer. */
	cpu.eip = ENTRY_START;

	/* Initialize DRAM. */
	init_ddr3();
}
