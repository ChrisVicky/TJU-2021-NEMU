#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#include "FLOAT.h"

extern char _vfprintf_internal;
extern char _fpmaxtostr;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);
extern char _ppfs_setargs;

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
  /* TODO: Format a FLOAT argument `f' and write the formating
   * result to `stream'. Keep the precision of the formating
   * result with 6 by truncating. For example:
   *              f          result
   *         0x00010000    "1.000000"
   *         0x00013333    "1.199996"
   */

  char buf[80];
  int temp_f = f;
  int offset = 0;
  int len_int = 0;
  if(f<0){
	  f = ~f + 1;
	  *buf = '-';
	  offset = 1;
	  len_int = 1;
  }
  len_int += sprintf(buf + offset, "%d", F2int(f));
  buf[len_int++] = '.';
  char temp_buf[80];
  int len_float = 4 * 4;
  f = f & 0xffff;
  int i;
  int w = 0, head = 5 * (10000000);
  for (i = 0; i < len_float; i++) {
    w += head * ((f & (1 << len_float - i - 1))!=0);
//	if(f & (1 << len_float - i - 1)) w+=head;
    head /= 2;
  }
  len_float = sprintf(buf + len_int, "%d", w);

  int len = len_int + 6;
  if (len_float < 6) {
    for (i = len_float; i < 6; i++) {
      buf[len_int + i] = '0';
    }
  }

  buf[len] = '\0';
  // len += sprintf(buf + len, " 0x%08x", temp_f);
  return __stdio_fwrite(buf, len, stream);
}

static unsigned int reverse(unsigned int offset) {
  return ((offset & 0xff) << (6 * 4)) + ((offset & 0xff00) << (2 * 4)) +
         ((offset & 0xff0000) >> (2 * 4)) + ((offset & 0xff000000) >> (6 * 4));
}

static void modify_vfprintf() {
  unsigned int addr_vfp = &_vfprintf_internal;
  unsigned int addr_format = &format_FLOAT;
  unsigned int addr_fpmax = &_fpmaxtostr;
  unsigned int call_address = addr_vfp + (0x8049e18 - 0x08049b12) + 1;
  unsigned int push_addr = addr_vfp + (0x8049e0e - 0x08049b12);
  unsigned int sub_addr = addr_vfp + (0x8049e0b - 0x08049b12);
  unsigned int sub_push_addr = addr_vfp + (0x8049e66 - 0x08049b67);
  unsigned int nop_addr = addr_vfp + (0x800e66 - 0x800b7e);
  // 开锁
//	mprotect((void *)((call_address - 100) & 0xfffff000), 4096 * 2,
  //           PROT_READ | PROT_WRITE | PROT_EXEC);

  /* Change the call destination. */
  int *call_pointer = call_address;
  unsigned int offset = addr_format - addr_fpmax;
  unsigned int new_destination = offset + (*call_pointer);
  *(call_pointer) = new_destination;
  //  printf("%x: %x\n", call_pointer, *(call_pointer));
  /* done */

  /* Change push */
  int *push_pointer = (int *)push_addr;
  unsigned int saved_bit = *push_pointer & 0xff000000;
  unsigned int change_bit = *push_pointer & 0x00ffffff;

  change_bit = 0x0072ff;
  *(push_pointer) = saved_bit + change_bit;
  //	change_bit = 0xff90

  /* Change Stack */
  int *sub_pointer = (int *)sub_addr;
  saved_bit = *sub_pointer & 0xff00ffff;
  change_bit = *sub_pointer & 0x00ff0000;
  change_bit -= (0x4 << (4 * 4));
  *(sub_pointer) = change_bit + saved_bit;

	/* Change Nop */
	int * nop_pointer = (int *)nop_addr;
	saved_bit = *(nop_pointer) &0xffff0000;
	change_bit = 0x9090;
	*(nop_pointer) = change_bit + saved_bit;
  /* TODO: Implement this function to hijack the formating of "%f"
   * argument during the execution of `_vfprintf_internal'. Below
   * is the code section in _vfprintf_internal() relative to the
   * hijack.
   */
#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

  /* You should modify the run-time binary to let the code above
   * call `format_FLOAT' defined in this source file, instead of
   * `_fpmaxtostr'. When this function returns, the action of the
   * code above should do the following:
   */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif
}

static void modify_ppfs_setargs() {

	unsigned int ppfs_addr = &_ppfs_setargs;
	unsigned int jmp_addr = ppfs_addr + (0x801157 - 0x8010e3);
	unsigned int destination_addr = ppfs_addr + (0x801101 - 0x8010e3);
	int * jmp_pointer = (int *) jmp_addr;
	unsigned int saved_bit = *(jmp_pointer) & 0xff000000;
	//(0x801186 - 0x801159)
	// *(jmp_pointer) = 0x90a8eb + saved_bit;
	*(jmp_pointer) = 0x902deb + saved_bit;
	
//	*(jmp_pointer) = *(jmp_pointer) & 0xff000000 + (rel1<<8) + 0xeb + (0x90 << 16);
  /* TODO: Implement this function to modify the action of preparing
   * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
   * Below is the code section in _vfprintf_internal() relative to
   * the modification.
   */

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK (0xff00)
#define __PA_FLAG_CHAR (0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT (0x0200)
#define PA_FLAG_LONG (0x0400)
#define PA_FLAG_LONG_LONG (0x0800)
#define PA_FLAG_LONG_DOUBLE PA_FLAG_LONG_LONG
#define PA_FLAG_PTR (0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

  /* You should modify the run-time binary to let the `PA_DOUBLE'
   * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
   * branch. Comparing to the original `PA_DOUBLE' branch, the
   * target branch will also prepare a 64-bit argument, without
   * introducing floating point instructions. When this function
   * returns, the action of the code above should do the following:
   */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif
}

void init_FLOAT_vfprintf() {
  modify_vfprintf();
  modify_ppfs_setargs();
}
