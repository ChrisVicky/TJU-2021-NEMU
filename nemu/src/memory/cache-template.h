#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

typedef struct concat3(_cache_,level,_block_){
    char valid;
#if level == 2:
    char dirty;
#endif
    unsigned int tag;
    unsigned char block[1<<max_block_offset];
} concat3(_cache_, level, _block_);
typedef struct concat3(_cache_,level,_) {
    concat3(_cache_,level,_block_) set [1<<max_set_offset][cache_line];
    void (* write) (int addr, int content);
    char (* read) (int addr);    
} concat3(_cache_,level,_);
concat3(_cache_,level,_) concat(cache_,level);

unsigned int concat(make_addr_,level)(int tag, int set_offset, int block_offset){
    return (tag<<(max_set_offset+max_block_offset))+(set_offset<<max_block_offset)+(block_offset);
}

static void concat(write_, level)(int addr, int content){
    unsigned int block_offset = addr & ((1<<max_block_offset)-1);
    unsigned int set_offset = (addr>>max_block_offset) & ((1<<max_set_offset)-1);
    unsigned int tag = (addr>>(32-max_block_offset-max_set_offset)) & ((1<<(32-max_block_offset-max_set_offset))-1);
    int i;
#if level == 1:
    dram_write(addr, 1, content);
#endif
    for(i=0;i<line_size;i++){
        if(concat(cache_,level).set[set_offset][i].valid && concat(cache_,level).set[set_offset][i].tag==tag){
            concat(cache_,level).set[set_offset][i].block[block_offset] = content;
    #if level==2:
            concat(cache_,level).set[set_offset][i].dirty = 1;
    #endif 
            return;
        }
    }
#if level==1:
    write_2(addr, content);
#elif level==2:
    dram_write(addr, 1, content);
    // write-back
    int target_line = 0;
    for(i=0;i<cache_line;i++){
        if(!cache.set[set_offset][i].valid){
            target_line = i;
            break;
        }
    }
    if(target = 0){
        srand((unsigned)tiem(NULL));
        target_line = rand()%(line_size-1);
    }
    if(concat(cache_,level).set[set_offset][target_line].dirty){
        for(block_offset=0;block_offset<(1<<max_block_offset);block_offset++){
            dram_write(concat(make_addr_,level)(concat(cache_,level).set[set_offset][target_line].tag, set_offset, block_offset), 1, concat(cache_,level).set[set_offset][target_line].block[block_offset]);          
        }
    }
    for(block_offset=0;block_offset<(1<<max_block_offset);block_offset++){
        concat(cache_, level).set[set_offset][target_line].block[block_offset] = dram_read(concat(make_addr_,level)(concat(cache_,level).set[set_offset][target_line].tag, set_offset, block_offset),1);
    }
#endif
}



















#undef concat_temp(x, y)
#undef concat(x, y)
#undef concat3(x, y, z)
#undef concat4(x, y, z, w)
#undef concat5(x, y, z, v, w)