#include "common.h"
#include "memory/tlb.h"
#include "cpu/reg.h"
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
void cache_write(int, int, int);
int cache_read(int, int);
extern int nemu_state;

/* Memory accessing interfaces */

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	if(!cr0.protect_enable) return addr;
	Assert(addr+len < cpu.sreg[sreg].invisible.cache.limit, "Segmentation Fault.");
	return cpu.sreg[sreg].invisible.cache.base + addr;
}



uint32_t hwaddr_read(hwaddr_t addr, size_t len) {

	//Log("addr : 0x%x " ,addr);
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
hwaddr_t page_translate(lnaddr_t addr) {
	if(cr0.protect_enable && cr0.paging) {
		hwaddr_t tmpaddr;
		if ((tmpaddr = readTLB(addr & 0xfffff000)) != -1){
			return (tmpaddr << 12) + (addr & 0xfff);
		}
		PTE dir;
		PDE page;
		uint32_t dir_offset = ((addr>>22) & 0x3ff);
		uint32_t page_offset = ((addr>>12) & 0x3ff);
		uint32_t offset = addr & 0xfff;
		dir.val = hwaddr_read((cr3.page_directory_base << 12) + (dir_offset << 2), 4);
		Assert(dir.present, "Invalid Page!	eip: %x	lnaddr: %x\n" ,cpu.eip ,addr);
		page.val = hwaddr_read((dir.page_frame << 12) + (page_offset << 2), 4);
		Assert(page.present, "Invalid Page!	eip: %x	lnaddr: %x\n" ,cpu.eip ,addr);
		writeTLB(addr & 0xfffff000, page.page_frame);
		addr = (page.page_frame << 12)+offset;
	}
	//Log("addr: %x" ,addr);
	return addr;
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

