#include "memory.h"
#include "serial.h"
#include "../utils/string.h"
#include "../utils/mem.h"
#include "../utils/console.h"

void pmm_dump(void)
{
	console_pre_print("MEMORY: detecting memory\n");

	// memory map is stored at 0xf900 by the bootloader (pmm.asm) using int 0x15
	uint32_t total_regions = *(uint32_t*)0xf900;
	SMAP_entry_t *smaps = (SMAP_entry_t *)0xf904;

	for (uint32_t region = 0; region < total_regions; region++, smaps++) {
		char *message = (char*)malloc(80); // 80 should be enough, right?
		
		char *type = "Unidentified";
		switch(smaps->type) {
			case 1:
				type = "Free";
				break;
			case 2:
				type = "Reserved";
				break;
			default:
				break;
		}
		
		sprintf(message, "MEMORY: region %u - 0x%08x%08x 0x%08x%08x %s\n", 
			region, 
			smaps->base_high,
			smaps->base_low,
			smaps->length_high,
			smaps->length_low,
			type);
		console_pre_print(message);

		free(80);
	}

}

