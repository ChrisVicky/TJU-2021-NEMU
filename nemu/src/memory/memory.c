#include "common.h"
#include "cache.h"
//uint32_t dram_read(hwaddr_t, size_t);
//void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
//	printf("WER\n");
	return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	/* 	~0u : ~unsigned int 0 = 0xffff;
	 *	取低地址的数据：如要取全部，应使得len = 4;
		len = 1 : 右移 3 字节 取高 1 位
		len = 2 : 右移 2 字节 取高 2 位
		len = 4 : 右移 0 字节 取 4 位
	 */
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	//printf("hwaddr_write\n");
	int i;
	for(i=0;i<len;i++){
		int temp_data = (data>>i) & 0xff;
		cache_write(addr+i, temp_data);
	}
	// dram_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

