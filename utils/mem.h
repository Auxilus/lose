#ifndef _UTILS_MEM
#define _UTILS_MEM

#include <stdint.h>
#include <stddef.h>

void memcpy(uint8_t *source, uint8_t *dest, int nbytes);
void memset(uint8_t *dest, uint8_t val, uint32_t len);

/* At this stage there is no 'free' implemented. */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

#endif
