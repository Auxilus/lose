#include "acpi.h"
#include "serial.h"
#include "../utils/mem.h"
#include "../utils/string.h"
#include "../utils/console.h"

int sigcmp(char *ptr1, char *ptr2);

RSDPDescriptor20 *acpi_locate_rsdp(void)
{
	char *mem_start = (char *)0x10000; // kernel start; maybe we should figure out kernel_end at some point for this
	char *mem_end = (char *)0x100000;

	while (mem_start < mem_end)
	{
		char memstr[8];
		memcpy(mem_start, memstr, 8);
		if (sigcmp(mem_start, "RSD PTR "))
		{
			break;
		};
		mem_start += 8;
	}

	RSDPDescriptor20 *d = (RSDPDescriptor20 *)malloc(sizeof(RSDPDescriptor20));
	memcpy(mem_start, (u8 *)d, sizeof(RSDPDescriptor20));
	char *message = (char *)malloc(64);
	sprintf(message, "ACPI: RSDP revision %u found at 0x%x\n", d->firstPart.Revision, (u32)mem_start);
	console_pre_print(message);

	return d;
}

void acpi_init()
{
	RSDPDescriptor20 *d = acpi_locate_rsdp();
	if (d->firstPart.Revision != 0)
	{
		console_pre_print("ACPI: Unsupported RSDP\n");
		return;
	}
	SDTHeader *xsdt = (SDTHeader *)d->firstPart.RsdtAddress;

	int entries = (xsdt->Length - sizeof(SDTHeader)) / 8;

	console_pre_print("ACPI: XSDT tables - ");
	for (int i = 0; i < entries; i++)
	{
		SDTHeader *new = (SDTHeader *)*(uint32_t *)((uint32_t)xsdt + sizeof(SDTHeader) + (i * 8));
		char sig[5];
		memcpy((u8 *)&new->Signature, sig, 4);
		sig[4] = '\0';
		console_pre_print(sig);
		console_pre_print(" ");
	}
	console_pre_print("\n");
}

int sigcmp(char *ptr1, char *ptr2)
{
	int match = 1;
	for (int i = 0; i < strlen(ptr2); i++)
	{
		if (ptr1[i] != ptr2[i])
		{
			return 0;
		}
	}

	if (match)
	{
		int a = 0;
	}

	return match;
}
