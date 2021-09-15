#include "burst.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int dram_read(unsigned int, long unsigned int);
void dram_write(unsigned int, long unsigned int, unsigned int);
extern int nemu_state;

typedef struct _cache_block_ {
    int valid;
    unsigned int tag;
    unsigned char block[64];
   
} _cache_block_;

typedef struct _cache_ {
    _cache_block_ set [128][8];
    int id;
    void (* write) (int addr, int content);
    char (* read) (int addr);    
} _cache_;

_cache_ cache;

unsigned int make_addr(int tag, int set_offset, int block_offset){
    return (tag<<13)+(set_offset<<6)+(block_offset);
}
static void write(int addr, int content){
    dram_write(addr, 1, content);
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    int i;
    /* If found in cache */
    for(i=0;i<8;i++){
        if(cache.set[set_offset][i].valid && cache.set[set_offset][i].tag==tag){
            cache.set[set_offset][i].block[block_offset] = content;
            return;
        }
    }
    /* Else: Update all cache */
    for(i=0;i<8;i++){
        if(!cache.set[set_offset][i].valid){
            cache.set[set_offset][i].valid = 1;
            cache.set[set_offset][i].tag = tag;
            for(block_offset=0;block_offset<64;block_offset++){
               cache.set[set_offset][i].block[block_offset] = dram_read((tag<<13)+(set_offset<<6)+(block_offset), 1);
            }
            return;
        }
    }
    srand((unsigned)time(NULL));
    int old_block_offset = rand()%7;
    int old_tag = cache.set[set_offset][old_block_offset].tag;
    for(i=0;i<64;i++){
        dram_write(make_addr(old_tag, set_offset, old_block_offset), 1, cache.set[set_offset][old_block_offset].block[i]);
        cache.set[set_offset][old_block_offset].block[i] = dram_read(make_addr(tag, set_offset, i), 1);
    }
    cache.set[set_offset][old_block_offset].tag = tag;
    return ;
}

static char read(int addr){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    int i;
    for(i=0;i<8;i++){
        if(cache.set[set_offset][i].valid && cache.set[set_offset][i].tag == tag){
            return cache.set[set_offset][i].block[block_offset];
        }
    }
    int ret = dram_read(addr, 1);
    for(i=0;i<8;i++){
        if(!cache.set[set_offset][i].valid){
            cache.set[set_offset][i].valid = 1;
            cache.set[set_offset][i].tag = tag;
            for(block_offset=0;block_offset<64;block_offset++){
                int address = make_addr(tag, set_offset, block_offset);
                cache.set[set_offset][i].block[block_offset] = dram_read(address, 1);
            }
            break;
        }
    }
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


void cache_write(int address, int content, int len){
    int i;
    for(i=0;i<len;i++){
        int data = (content >> (i * 8)) & 0xff;
        cache.write(address+i, data);        
    }
    int cache_data = cache_read(address, len);
    int dram_data = dram_read(address, len) & (~0u >> ((4 - len) << 3));
    if(cache_data != dram_data){
        nemu_state = 0;
        printf("ERROR\n");
    }   
    return;
}

void initialize_cache(){
    cache.write = write;
    cache.read = read;
    cache.id = 123;
    int i;
    for(i=0;i<128;i++){
        int j;
        for(j=0;j<8;j++){
            int k;
            cache.set[i][j].valid = 0;
            cache.set[i][j].tag = 0;
            for(k=0;k<64;k++){
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
