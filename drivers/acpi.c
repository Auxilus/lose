#include "acpi.h"
#include "serial.h"
#include "../utils/mem.h"
#include "../utils/string.h"

RSDPDescriptor20 *acpi_locate_rsdp(void)
{
	char *mem_start = 0x1000; //kernel start; maybe we should figure out kernel_end at some point for this
	char *mem_end	 = 0x100000;

	while (mem_start < mem_end) {
		char memstr[8];
		memcpy(mem_start, memstr, 8);
		if (sigcmp(mem_start, "RSD PTR ")) {
			char *message = (char*)malloc(64);
			sprintf(message, "ACPI: RSDP found at 0x%x\n", mem_start);
			serial_print(message);
			break;
		};
		mem_start+=8;
	}

	RSDPDescriptor20 *d;
	return d;
}

int sigcmp(char* ptr1, char* ptr2)
{
	int match = 0;
	for (int i = 0; i < strlen(ptr2); i++) {
		match = ptr1[i] == ptr2[i];
	}

	return match;
}
