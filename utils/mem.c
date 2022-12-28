#include "mem.h"
void memcpy(u8 *source, u8 *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void *memmove(void* dest, const void* src, unsigned int n)
{
	char *pDest = (char *)dest;
    const char *pSrc =( const char*)src;
    //allocate memory for tmp array
		void* dummy_ptr;
    char *tmp  = (char *)kmalloc(sizeof(char ) * n, 0, dummy_ptr);
    if(tmp == 0)
    {
        return 0;
    }
    else
    {
        unsigned int i = 0;
        // copy src to tmp array
        for(i =0; i < n ; ++i)
        {
            *(tmp + i) = *(pSrc + i);
        }
        //copy tmp to dest
        for(i =0 ; i < n ; ++i)
        {
            *(pDest + i) = *(tmp + i);
        }
        //free(tmp); //free allocated memory
    }
    return dest;
}

void memset(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8*)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
/* Implementation is just a pointer to some free memory which
 * keeps growing */
u32 kmalloc(int size, int align, u32 *phys_addr) {
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    return ret;
}

void *malloc(int size)
{
	void *ret = free_mem_addr;
	free_mem_addr += size + 1;
	return ret;
}
