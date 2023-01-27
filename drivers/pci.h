/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __PCI_H_
#define __PCI_H_
#include "../cpu/types.h"

struct __pci_driver;

typedef struct
{
	uint32_t vendor;
	uint32_t device;
	uint32_t function;
	uint32_t slot;
	uint32_t bus;
	uint32_t headerType;
	uint32_t classId;
	uint32_t subClassId;
	uint32_t progIF;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	uint32_t CardbusCISPointer;
	uint16_t subVendorId;
	uint16_t subId;
	uint32_t ExpROMBase;
	uint8_t capPtr;
	struct __pci_driver *driver;
} pci_device;

typedef struct
{
	uint32_t vendor;
	uint32_t device;
	uint32_t func;
} pci_device_id;

typedef struct __pci_driver
{
	pci_device_id *table;
	char *name;
	uint8_t (*init_one)(pci_device *);
	uint8_t (*init_driver)(void);
	uint8_t (*exit_driver)(void);
} pci_driver;

typedef struct
{
	int count;
	pci_device **devices;
} pci_device_search;


static pci_device **pci_devices;
static uint32_t devs;

static pci_driver **pci_drivers;
static uint32_t drivs;

void pci_init();
void pci_proc_dump();
pci_device_search* pci_find_by_type(uint16_t class_id, uint16_t sub_class_id, uint16_t prog_if);

#endif
