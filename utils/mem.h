#ifndef _UTILS_MEM
#define _UTILS_MEM

#include "../cpu/types.h"

static volatile u32 free_mem_addr = 0x10000;

void memcpy(u8 *source, u8 *dest, int nbytes);
void memset(u8 *dest, u8 val, u32 len);

/* At this stage there is no 'free' implemented. */
u32 kmalloc(int size, int align, u32 *phys_addr);
void *malloc(int size);

#endif
