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

typedef struct _cache_set_ {
    _cache_block_ lines[8];
} _cache_set_;

typedef struct _cache_ {
    _cache_set_ set [128];
    int id;
    void (* add) (struct _cache_ *this, int addr, int content);
    char (* read) (struct _cache_ *this, int addr);    
} _cache_;

_cache_ cache;

static void add(_cache_ *this, int addr, int content){
    dram_write(addr, 1, content);
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    //_cache_set_ temp_set = this->set[set_offset];
    _cache_set_ temp_set = cache.set[set_offset];
    int i;
    int flag = 0;
    for(i=0;i<7;i++){
        _cache_block_ temp_line = temp_set.lines[i];
        if(temp_line.valid && temp_line.tag==tag){
            temp_line.block[block_offset] = content;
            flag = 1;
            break;
        }
    }
    if(flag) return;
        for(i=0;i<7;i++){
        _cache_block_ temp_line = temp_set.lines[i];
        if(!temp_line.valid){
            temp_line.valid = 1;
            temp_line.tag = tag;
            for(block_offset=0;block_offset<64;block_offset++){
               temp_line.block[block_offset] = dram_read((tag<<13)+(set_offset<<6)+(block_offset), 1);
            }
            flag = 1;
            break;
        }
    }
    if(flag) return;
    srand((unsigned)time(NULL));
    int j = rand()%7;
     _cache_block_ temp_line = temp_set.lines[j];
    for(i=0;i<63;i++){
        unsigned int hd_addr = (tag<<13)+(set_offset<<6)+i;
        dram_write(hd_addr, 1, temp_line.block[i]);
    }
    return ;
}
unsigned int make_addr(int tag, int set_offset, int block_offset){
    return (tag<<13)+(set_offset<<6)+(block_offset);
}
static void SEEK_CACHE(_cache_ *this){
    _cache_set_ *set = (*this).set;
    int i=0;
    for(i=0;i<128;i++){
        _cache_block_ *block = set[i].lines;
        int j;
        for(j=0;j<8;j++){
            int tag = block[j].tag;
            if(block[j].valid){
                int k;
                printf("block[%x]\n" ,j);
                for(k=0;k<64;k++){
                    int addr = make_addr(tag, j, k);
                    int dram = dram_read(addr, 1) & 0xff;
                    if(dram!=block[j].block[k]){
                        printf("%x\t%x\n" ,block[j].block[k] ,dram);
                        nemu_state = 0;
                    }
                }
            }
        }
    }
}

static char read(_cache_ *this, int addr){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
   // printf("Looking for tag=%x\n" ,tag);
    _cache_set_ *temp_set = &(*this).set[set_offset];
    int i;
    for(i=0;i<7;i++){
        _cache_block_ *temp_line = &(*temp_set).lines[i];
        if(temp_line->valid && temp_line->tag==tag){
            // printf("FOUND IN BLOCK\n");
            return (*temp_line).block[block_offset];
        }
    }
    int ret = dram_read(addr, 1);
    
    // printf("SEEKING DRAM    0x%x\n" ,ret);
    for(i=0;i<7;i++){
        _cache_block_ *temp_line = &temp_set->lines[i];
        if(!temp_line->valid){
           // printf("Update tag = %x i=%x\n" ,tag,i);
            temp_line->valid = 1;
            //printf("temp_line.valid=%x\n",temp_line->valid);
            temp_line->tag = tag;
            for(block_offset=0;block_offset<64;block_offset++){
               temp_line->block[block_offset] = dram_read(make_addr(tag, set_offset, block_offset), 1);
               
            }
            break;
        }
    }
    SEEK_CACHE(this);
    return ret;
}

void cache_write(int address, char content){
    return cache.add(&cache, address, content);
}

int cache_read(int address, int len){
    int i;
    int ret = 0;
    for(i=0;i<len;i++){
        int temp = cache.read(&cache, address+i) & 0xff;    
        ret += temp<<(i*8);  
       // printf("ret = %08x\n" ,ret);
    }
//    int ret1 = dram_read(address, len);
   // printf("0x08%x 0x%x 0x%x\n" ,address,ret,ret1);
    return ret;
}
void initialize_cache(){
    cache.add = add;
    cache.read = read;
    cache.id = 123;
    int i;
    for(i=0;i<128;i++){
        _cache_set_ set = cache.set[i];
        int j;
        for(j=0;j<8;j++){
            _cache_block_ block = set.lines[j];
            block.valid = 0;
            block.tag = 0;
        }
    }
}