#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}

void load_elf_variables(){
	int i;
	printf("\33[1;33m%-*s %-10s\33[0m\n",15,"name","value" );
	for(i=0;i<nr_symtab_entry;i++){
		printf("%-*s 0x%-10x\n" ,15,strtab+symtab[i].st_name ,symtab[i].st_value);
	}
	return ;
}

int get_address_by_name(char * name){
	int i;
	for(i=0;i<nr_symtab_entry;i++){
		if(/* symtab[i].st_info!=17 || // Check if variable */ strncmp(name, strtab + symtab[i].st_name, strlen(name))) continue;
		return symtab[i].st_value;
	}
	return 0;
}

char * get_name_by_address(int value){
	int i;
	for(i=0;i<nr_symtab_entry;i++){
		if(symtab[i].st_value==value) return strtab + symtab[i].st_name;
	}
	return NULL;
}

bool is_variable(char * name){

	int i;
	for(i=0;i<nr_symtab_entry;i++){
//		if(symtab[i].st_info!=17) continue;
		char *temp = strtab + symtab[i].st_name;
		if(temp!=NULL && strncmp(name, temp, strlen(temp))==0) return strlen(temp);
	}
	return false;
}

char * get_func_name_by_address(int value, int * offset, int * address){
	int i;
	int temp = 0xffffff;
	int ret = 0;
	for(i=0;i<nr_symtab_entry;i++){
		int type = symtab[i].st_info & 0xf;
		if((type != 0x2 && type != 0x0) || symtab[i].st_value > value) continue;
		//Log("HIT");
		if(value - symtab[i].st_value < temp){
			ret = i;
			temp = value - symtab[i].st_value;
		}
	}
	* offset = temp;
	* address = symtab[ret].st_value;
	return strtab + symtab[ret].st_name;
}

