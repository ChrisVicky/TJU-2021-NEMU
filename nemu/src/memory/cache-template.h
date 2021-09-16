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
concat3(_cache_,level,_) cache;

unsigned int concat(make_addr_,level)(int tag, int set_offset, int block_offset){
    return (tag<<(max_set_offset+max_block_offset))+(set_offset<<max_block_offset)+(block_offset);
}

static void concat(write_, level)(int addr, int content){
    unsigned int block_offset = addr & 0x3f;
    unsigned int set_offset = (addr>>6) & 0x7f;
    unsigned int tag = (addr>>13) & 0x7fffff;
}

















#undef concat_temp(x, y)
#undef concat(x, y)
#undef concat3(x, y, z)
#undef concat4(x, y, z, w)
#undef concat5(x, y, z, v, w)