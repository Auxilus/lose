#ifndef _DRIVER_ACPI
#define _DRIVER_ACPI

#include "../cpu/types.h"

extern char *kernel_end;

typedef struct {
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
} RSDPDescriptor __attribute__ ((packed));

typedef struct {
	RSDPDescriptor firstPart;

	uint32_t Length;
	uint64_t XsdtAddress;
	uint8_t ExtendedChecksum;
	uint8_t reserved[3];
} RSDPDescriptor20 __attribute__ ((packed));

RSDPDescriptor20 *acpi_locate_rsdp(void);

#endif
