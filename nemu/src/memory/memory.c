#include "common.h"
// #include "cache.h"
#include "cpu/reg.h"
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
void cache_write(int, int, int);
int cache_read(int, int);

extern int nemu_state;
/* Memory accessing interfaces */

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	if(!cr0.protect_enable) return addr;
	//Log("addr: %x\tlen: %x\tsreg: %x" ,addr,(int) len ,sreg);
	//Log("limit: %x" ,cpu.sreg[sreg].invisible.cache.limit);
	Assert(addr+len < cpu.sreg[sreg].invisible.cache.limit, "Segmentation Fault.");
	return cpu.sreg[sreg].invisible.cache.base + addr;
//	return addr + t
}



uint32_t hwaddr_read(hwaddr_t addr, size_t len) {

	return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
	
	//return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	/* 	~0u : ~unsigned int 0 = 0xffff;
	 *	取低地址的数据：如要取全部，应使得len = 4;
		len = 1 : 右移 3 字节 取高 1 位
		len = 2 : 右移 2 字节 取高 2 位
		len = 4 : 右移 0 字节 取 4 位
	*/
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache_write(addr, len, data);
	//dram_write(addr, len, data);
}

hwaddr_t page_translate(lnaddr_t addr){
	PTE dir;
	PDE page;
	dir.val = 0;
	page.val = 0;
	hwaddr_t hwaddr;
	if(!cr0.paging||!cr0.protect_enable) return addr;
	dir.val = hwaddr_read((cr3.page_directory_base<<12)+((addr>>22)<<2), 4);
	Assert(dir.present, "page_value %x, eip: %x" ,dir.val, cpu.eip);
	page.val = hwaddr_read((page.page_frame<<12)+(((addr>>12)&0x3ff)<<2), 4);
	hwaddr = (page.page_frame << 12) + (addr & 0x3ff);
	return hwaddr;
	// uint32_t dir = (addr >> 21) & 0x7ff;
	// uint32_t page = (addr >> 11) & 0x7ff;
	// uint32_t offset = addr & 0xfff;

	// // Assert(dir<NR_PDE, "dir (0x%x) out of range " ,dir);
	// // Assert(page<NR_PTE, "page (0x%x) out of range ",page);

	// uint32_t page_directory_addr = cr3.page_directory_base<<12;
	// PDE * page_directory = (void *) (long) page_directory_addr;
	// Log("page_directory_addr  = %x" ,page_directory_addr);
	// uint32_t page_table_addr = page_directory[dir].page_frame<<12;
	// PTE * page_table = (void *) (long) page_table_addr;

//	Assert(page_table[page].present, "present = %x" page_table[page].present);

	// uint32_t physical_addr = offset + (page_table[page].page_frame<<12);
	// return physical_addr;
}
uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len==1 || len==2 || len==4);
	size_t max_len = ((~addr) & 0xfff) + 1;
	if(len>max_len){
		uint32_t low = lnaddr_read(addr, max_len);
		uint32_t high = lnaddr_read(addr + max_len, len - max_len);
		return (high<<(max_len<<3)) | low;
	}else{
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	size_t max_len = ((~addr) & 0xfff) + 1;
	if(len > max_len){
		lnaddr_write(addr, max_len, data & ((1<<(max_len<<3)) - 1));
		lnaddr_write(addr + max_len, len - max_len, data>>(max_len<<3));
	}else{
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
	return ;
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

