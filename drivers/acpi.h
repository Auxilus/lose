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
} __attribute__ ((packed)) RSDPDescriptor;

typedef struct {
	RSDPDescriptor firstPart;

	uint32_t Length;
	uint64_t XsdtAddress;
	uint8_t ExtendedChecksum;
	uint8_t reserved[3];
} __attribute__ ((packed)) RSDPDescriptor20;

typedef struct {
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} __attribute__ ((packed)) SDTHeader;

typedef struct {
	SDTHeader Header;
	uint64_t Reserved;
} __attribute__ ((packed)) MCFGHeader;

RSDPDescriptor20 *acpi_locate_rsdp(void);
void acpi_init();

#endif
