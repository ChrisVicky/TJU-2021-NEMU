#ifndef _CACHE_H_
#define _CACHE_H_
void cache_write(int address, char content);
int cache_read(int address, int len);
void initialize_cache();
#endif
