#include "memory.h"
#include "serial.h"
#include "../utils/string.h"
#include "../utils/mem.h"

void pmm_dump(void)
{
	uint32_t num = *(uint32_t*)0x8900;
	SMAP_entry_t *smaps = (SMAP_entry_t *)0x8904;

	for (uint32_t i = 0; i < num; i++) {
		char *message = (char*)malloc(256);
		sprintf(message, "MEMORY: region: %u 0x%08x%08x 0x%08x%08x %u\n", i, smaps->BaseH, smaps->BaseL, smaps->LengthH, smaps->LengthL, smaps->Type);
		serial_print(message);
		smaps++;
	}

}

