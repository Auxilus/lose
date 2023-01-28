/** @author Levente Kurusa <levex@linux.com> **/
#include "../utils/mem.h"
#include "../utils/string.h"
#include "../utils/console.h"
#include "pci.h"
#include "ports.h"
#include "serial.h"
#include "pci_lookup.h"

void add_pci_device(pci_device *pdev)
{
	pci_devices[devs] = pdev;
	devs++;
	return;
}

uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
	uint64_t address;
	uint64_t lbus = (uint64_t)bus;
	uint64_t lslot = (uint64_t)slot;
	uint64_t lfunc = (uint64_t)func;
	uint16_t tmp = 0;
	address = (uint64_t)((lbus << 16) | (lslot << 11) |
											 (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
	outportl(0xCF8, address);
	tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}

uint16_t get_vendor_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 0);
	return r0;
}

uint16_t get_prog_if(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 0x8);
	return (r0 & ~0x00FF) >> 8;
}

uint16_t get_header_type(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 0xd);
	return (r0 & ~0xFF00);
}

uint16_t get_device_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 2);
	return r0;
}

uint16_t get_class_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 0xA);
	return (r0 & ~0x00FF) >> 8;
}

uint32_t getBar5(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t bar = 0x0;
	uint16_t bar50 = pci_read_word(bus, device, function, 0x24);
	uint16_t bar51 = pci_read_word(bus, device, function, 0x26);
	bar |= bar50;
	bar |= bar51 << 16;
	return bar;
}

uint16_t get_subclass_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus, device, function, 0xA);
	return (r0 & ~0xFF00);
}

void pci_probe()
{
	console_pre_print("PCI: probe started\n");

	for (uint32_t bus = 0; bus < 256; bus++)
	{
		for (uint32_t slot = 0; slot < 32; slot++)
		{
			for (uint32_t function = 0; function < 8; function++)
			{
				uint16_t vendor = get_vendor_id(bus, slot, function);
				if (vendor == 0xffff)
					continue;

				uint16_t device = get_device_id(bus, slot, function);

				pci_device *pdev = (pci_device *)malloc(sizeof(pci_device));
				pdev->vendor = vendor;
				pdev->device = device;
				pdev->function = function;
				pdev->slot = slot;
				pdev->bus = bus;
				pdev->driver = 0;
				pdev->classId = get_class_id(bus, slot, function);
				pdev->subClassId = get_subclass_id(bus, slot, function);
				pdev->headerType = get_header_type(bus, slot, function);
				;
				pdev->progIF = get_prog_if(bus, slot, function);

				// if (pdev->headerType == 0x0)
				// {
				// 	pdev->BAR5 = pci_read_word(bus, device, function, 0x26) << 16 | pci_read_word(bus, device, function, 0x24);
				// }

				add_pci_device(pdev);
			}
		}
	}
}

void pci_init()
{
	devs = drivs = 0;
	pci_devices = (pci_device **)malloc(32 * sizeof(pci_device));
	pci_drivers = (pci_driver **)malloc(32 * sizeof(pci_driver));

	pci_probe();
	pci_proc_dump();
}

void pci_register_driver(pci_driver *driv)
{
	pci_drivers[drivs] = driv;
	drivs++;
	return;
}

pci_device_search *pci_find_by_type(uint16_t class_id, uint16_t sub_class_id, uint16_t prog_if)
{
	pci_device_search *ret = (pci_device_search *)malloc(sizeof(pci_device_search));
	ret->devices = (pci_device **)malloc(32 * sizeof(pci_device));
	ret->count = 0;
	for (int i = 0; i < devs; i++)
	{
		pci_device *device = pci_devices[i];
		if (device->classId == class_id && device->subClassId == sub_class_id && device->progIF == prog_if)
		{
			ret->devices[ret->count] = device;
			ret->count++;
		}
	}

	return ret;
}

void pci_proc_dump()
{
	console_pre_print("PCI: ------------------------------------------------------\n");

	for (int i = 0; i < devs; i++)
	{
		pci_device *pci_dev = pci_devices[i];
		char *message = (char *)malloc(80);

		// const char *vendor = get_vendor_name(pci_dev->vendor);

		// sprintf(message, "PCI: %02u:%02u.%u 0x%04x:0x%04x 0x%04x 0x%04x %s (%s)\n",
		sprintf(message, "PCI: 0x%04x:0x%04x 0x%04x 0x%04x %s (%s)\n",
						pci_dev->classId,
						pci_dev->subClassId,
						pci_dev->vendor,
						pci_dev->device,
						device_classes[pci_dev->classId],
						get_subclass_name(pci_dev->classId, pci_dev->subClassId));
		// sprintf(message, "PCI: %02u:%02u.%u [%-23s] [%-25s] [%-10s] [%s] 0x%x\n",
		// 				pci_dev->bus,
		// 				pci_dev->slot,
		// 				pci_dev->function,
		// 				device_classes[pci_dev->classId],
		// 				get_subclass_name(pci_dev->classId, pci_dev->subClassId),
		// 				get_vendor_name(pci_dev->vendor),
		// 				get_device_name(pci_dev->vendor, pci_dev->device),
		// 				pci_dev->headerType);

		console_pre_print(message);
		free(80);
	}

	console_pre_print("PCI: ------------------------------------------------------\n");

	char *message = (char *)malloc(30);
	sprintf(message, "PCI: found %u devices\n", devs);
	
	console_pre_print(message);
}
