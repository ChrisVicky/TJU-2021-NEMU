#include "burst.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int dram_read(unsigned int, long unsigned int);
void dram_write(unsigned int, long unsigned int, unsigned int);
extern int nemu_state;
/* first cache */
#define max_set_offset 7
#define cache_line 8
#define max_block_offset 6
#define level 1
#include "cache-template.h"
#undef max_set_offset
#undef cache_line
#undef max_block_offset
#undef level
/* second cache */

static void update_cache(int tag, int set_offset, int block_offset){
    int i;
    for(i=0;i<cache_line;i++){
        if(!cache.set[set_offset][i].valid){
            cache.set[set_offset][i].valid = 1;
            cache.set[set_offset][i].tag = tag;
            for(block_offset=0;block_offset<(1<<max_block_offset);block_offset++){
                cache.set[set_offset][i].block[block_offset] = dram_read(make_addr(tag, set_offset, block_offset), 1);
            }
            return;
        }
    }
    srand((unsigned)time(NULL));
    int line = rand()%7;
    for(block_offset=0;block_offset<(1<<max_block_offset);block_offset++){
        dram_write(make_addr(cache.set[set_offset][line].tag, set_offset, block_offset), 1, cache.set[set_offset][line].block[block_offset]);
        cache.set[set_offset][line].block[block_offset] = dram_read(make_addr(tag, set_offset, block_offset), 1);
    }
    cache.set[set_offset][line].tag = tag;
    return;
}
static void write(int addr, int content){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    int i;
    /* If found in cache */
    dram_write(addr, 1, content);
    for(i=0;i<8;i++){
        if(cache.set[set_offset][i].valid && cache.set[set_offset][i].tag==tag){
            cache.set[set_offset][i].block[block_offset] = content;
            return;
        }
    }
    update_cache(tag, set_offset, block_offset);
    return;
}
static char read(int addr){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    int i;
    for(i=0;i<cache_line;i++){
        if(cache.set[set_offset][i].valid && cache.set[set_offset][i].tag == tag){
            return cache.set[set_offset][i].block[block_offset];
        }
    }
    int ret = dram_read(addr, 1);
    update_cache(tag, set_offset, block_offset);
    return ret;
}


int cache_read(int address, int len){
    int i;
    int ret = 0;
    for(i=0;i<len;i++){
        int temp = cache.read(address+i) & 0xff;
        ret += temp<<(i*8);
    }
    return ret;
}


void cache_write(int address, int len, int content){
    int i;
    for(i=0;i<len;i++){
        cache.write(address+i, ((content >> (i * 8)) & 0xff));        
    }
    return;
}

void initialize_cache(){
    cache.write = write;
    cache.read = read;
    cache.id = 123;
    int i;
    for(i=0;i<cache_set;i++){
        int j;
        for(j=0;j<cache_line;j++){
            int k;
            cache.set[i][j].valid = 0;
            cache.set[i][j].tag = 0;
            for(k=0;k<cache_block;k++){
                cache.set[i][j].block[k] = 0;
            }
        }
    }
}

/*
DEBUG
static void SEEK_CACHE(){
    int i=0;
    for(i=0;i<128;i++){
        int j;
        for(j=0;j<8;j++){
            if(cache.set[i][j].valid){
                int k;
                for(k=0;k<64;k++){
                    int dram = dram_read(make_addr(cache.set[i][j].tag, i, k), 1)&0xff;
                    int cache_ram = cache.set[i][j].block[k];
                    if(dram!=cache_ram){
                       // printf("address: %x\tcache: %x\tdram: %x\n",make_addr(cache.set[i][j].tag, i, k),cache_ram, dram);
                    }
                }
            }
        }
    }
}
*/
