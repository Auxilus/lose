#include "mem.h"
#include "string.h"
#include "../drivers/serial.h"

void memcpy(u8 *source, u8 *dest, int nbytes)
{
	int i;
	for (i = 0; i < nbytes; i++)
	{
		*(dest + i) = *(source + i);
	}
}

int memcmp(const void *ptr1, const void *ptr2, int num)
{
	const unsigned char *vptr1 = (const unsigned char *)ptr1;
	const unsigned char *vptr2 = (const unsigned char *)ptr2;
	while (num)
	{
		if (*vptr1 > *vptr2)
			return 1;
		else if (*vptr1 < *vptr2)
			return -1;
		vptr1++;
		vptr2++;
		num--;
	}
	return 0;
}

void *memmove(void *dest, const void *src, unsigned int n)
{
	const unsigned char *sp;
	unsigned char *dp;
	if (dest < src)
	{
		sp = (const unsigned char *)src;
		dp = (unsigned char *)dest;
		while (n)
		{
			*dp = *sp;
			dp++;
			sp++;
			n--;
		}
	}
	else
	{
		while (n)
		{
			sp = (unsigned char *)src + n - 1;
			dp = (unsigned char *)dest + n - 1;
			*dp = *sp;
			n--;
		}
	}
	return dest;
}

void memset(u8 *dest, u8 val, u32 len)
{
	u8 *temp = (u8 *)dest;
	while(len-- != 0)
	{
		*temp++ = val;
	}
}

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
/* Implementation is just a pointer to some free memory which
 * keeps growing */
u32 kmalloc(int size, int align, u32 *phys_addr)
{
	/* Pages are aligned to 4K, or 0x1000 */
	if (align == 1 && (free_mem_addr & 0xFFFFF000))
	{
		free_mem_addr &= 0xFFFFF000;
		free_mem_addr += 0x1000;
	}
	/* Save also the physical address */
	if (phys_addr)
		*phys_addr = free_mem_addr;

	u32 ret = free_mem_addr;
	free_mem_addr += size; /* Remember to increment the pointer */
	return ret;
}

void *malloc(int size)
{
	void *ret = (void *)free_mem_addr;
	free_mem_addr += size + 1;
	return ret;
}

void free(int size)
{
	free_mem_addr -= size;
}