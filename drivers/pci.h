/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __PCI_H_
#define __PCI_H_
#include "../cpu/types.h"

struct __pci_driver;

typedef struct {
	uint32_t vendor;
	uint32_t device;
	uint32_t function;
	uint32_t slot;
	uint32_t bus;
	uint32_t headerType;
	uint32_t classId;
	uint32_t subClassId;
	struct __pci_driver *driver;
} pci_device;

typedef struct {
	uint32_t vendor;
	uint32_t device;
	uint32_t func;
} pci_device_id;

typedef struct __pci_driver {
	pci_device_id *table;
	char *name;
	uint8_t (*init_one)(pci_device *);
	uint8_t (*init_driver)(void);
	uint8_t (*exit_driver)(void);
} pci_driver;

extern void pci_init();
extern void pci_proc_dump();

#endif
