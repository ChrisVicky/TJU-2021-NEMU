#ifndef _CACHE_H_
#define _CACHE_H_
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

typedef struct _cache_block_ {
    int valid;
    unsigned int tag;
    unsigned char line[64];
} _cache_block_;

typedef struct _cache_set_ {
    _cache_block_ lines[8];
} _cache_set_;

typedef struct _cache_ {
    _cache_set_ set [128];

    void (* op1) (struct _cache_ *this, int addr, char content);
    char (* op2) (struct _cache_ *this, int addr);    
} _cache_;

void add(_cache_ *this, int addr, char content){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    _cache_set_ temp_set = this->set[set_offset];
    int i;
    int flag = 0;
    for(i=0;i<7;i++){
        _cache_block_ temp_line = temp_set.lines[i];
        if(temp_line.valid && temp_line.tag==tag){
            temp_line.line[block_offset] = content;
            flag = 1;
            break;
        }
    }
    if(flag) return;
    for(i=0;i<7;i++){
        _cache_block_ temp_line = temp_set.lines[i];
        if(!temp_line.valid){
            temp_line.line[block_offset] = content;
            flag = 1;
            break;
        }
    }
    if(flag) return;
    int j=4; /* random */
     _cache_block_ temp_line = temp_set.lines[j];
    for(i=0;i<63;i++){
        unsigned int hd_addr = (tag<<13)+(set_offset<<6)+i;
        dram_write(hd_addr, 1, temp_line.line[i]);
    }
    return ;
}

char read(_cache_ *this, int addr){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
    _cache_set_ temp_set = (*this).set[set_offset];
    int i;
    for(i=0;i<7;i++){
        _cache_block_ temp_line = temp_set.lines[i];
        if(temp_line.valid && temp_line.tag==tag){
            return temp_line.line[block_offset];
        }
    }
    return dram_read(addr, 1);
}

_cache_ cache;

void cache_write(int address, char content){
    return cache.op1(&cache, address, content);
}

int cache_read(int address, int len){
    int i;
    int ret = 0;
    for(i=0;i<len;i++){
        int temp = cache.op2(&cache, address+i);        
        ret = temp<<(i*8);
    }
    printf("ret = %x\n" ,ret);
    return ret;
}
void initialize_cache(){
    cache.op1 = add;
    cache.op2 = read;
    printf("Initialize");
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

#endif
