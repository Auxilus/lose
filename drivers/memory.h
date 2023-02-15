#include "../cpu/types.h"

// SMAP entry structure
typedef struct SMAP_entry
{
	uint32_t base_low; // base address uint64_t
	uint32_t base_high;
	uint32_t length_low; // length uint64_t
	uint32_t length_high;
	uint32_t type; // entry Type
	uint32_t ACPI; // extended
} __attribute__((packed)) SMAP_entry_t;

void pmm_dump(void);
