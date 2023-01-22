#include "memory.h"
#include "serial.h"
#include "../utils/string.h"
#include "../utils/mem.h"
#include "../utils/console.h"

void pmm_dump(void)
{
	console_pre_print("MEMORY: detecting memory\n");
	uint32_t num = *(uint32_t*)0x8900;
	SMAP_entry_t *smaps = (SMAP_entry_t *)0x8904;

	for (uint32_t i = 0; i < num; i++) {
		char *message = (char*)malloc(256);
		char *type = (char*) malloc(16);
		type = "unidentified";

		switch(smaps->Type) {
			case 1:
				type = "Free";
				break;
			case 2:
				type = "Reserved";
				break;
			default:
				break;
		}
		sprintf(message, "MEMORY: region: %u 0x%08x%08x 0x%08x%08x %s\n", i, smaps->BaseH, smaps->BaseL, smaps->LengthH, smaps->LengthL, type);
		console_pre_print(message);
		smaps++;
	}

}

