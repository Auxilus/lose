/** @author Levente Kurusa <levex@linux.com> **/
#include "../utils/mem.h"
#include "../utils/string.h"
#include "serial.h"
#include "pci.h"
#include "ports.h"


pci_device **pci_devices = 0;
uint32_t devs = 0;

pci_driver **pci_drivers = 0;
uint32_t drivs = 0;


void add_pci_device(pci_device *pdev)
{
	pci_devices[devs] = pdev;
	devs ++;
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
	outportl (0xCF8, address);
	tmp = (uint16_t)((inportl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0);
	return r0;
}

uint16_t getHeaderType(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0xd);
	return (r0 & ~0xFF00);
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,2);
	return r0;
}

uint16_t getClassId(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0xA);
	return (r0 & ~0x00FF) >> 8;
}

uint16_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0xA);
	return (r0 & ~0xFF00);
}

void pci_probe()
{
	free_mem_addr = 0x10000;

	for(uint32_t bus = 0; bus < 256; bus++)
	{
		for(uint32_t slot = 0; slot < 32; slot++)
		{
			for(uint32_t function = 0; function < 8; function++)
			{
				uint16_t vendor = getVendorID(bus, slot, function);
				if(vendor == 0xffff) continue;
				uint16_t device = getDeviceID(bus, slot, function);
				uint16_t header = getHeaderType(bus, slot, function);

				pci_device *pdev = (pci_device *)malloc(sizeof(pci_device));
				pdev->vendor = vendor;
				pdev->device = device;
				pdev->function = function;
				pdev->slot = slot;
				pdev->bus = bus;
				pdev->classId = getClassId(bus, slot, function);
				pdev->subClassId = getSubClassId(bus, slot, function);
				pdev->headerType = header;
				pdev->driver = 0;

				add_pci_device(pdev);
			}
		}
	}
}

void pci_init()
{
	devs = drivs = 0;
	serial_print("PCI: probe started\n");
	pci_devices = (pci_device **)malloc(32 * sizeof(pci_device));
	pci_drivers = (pci_driver **)malloc(32 * sizeof(pci_driver));
	pci_probe();
	pci_proc_dump();
}

void pci_register_driver(pci_driver *driv)
{
	pci_drivers[drivs] = driv;
	drivs ++;
	return;
}

void pci_proc_dump()
{
	serial_print("PCI: -----------------------------------\n");
	serial_print("PCI: bb:ss.f  cls scls 0xvvvv 0xdddd 0xh\n");
	serial_print("PCI: ===================================\n");
	for(int i = 0; i < devs; i++)
	{
		pci_device *pci_dev = pci_devices[i];
		char *message = (char*)malloc(64);

		if(pci_dev->driver) {
			sprintf(message, "[%x:%x:%x] => %s\n", pci_dev->vendor, pci_dev->device, pci_dev->function, pci_dev->driver->name);
		}
		else {
			sprintf(message, "PCI: %02u:%02u.%u 0x%02x 0x%02x 0x%x 0x%x 0x%x\n", pci_dev->bus, pci_dev->slot, pci_dev->function,
					pci_dev->classId, pci_dev->subClassId, pci_dev->vendor, pci_dev->device, pci_dev->headerType);
		}

		serial_print(message);
	}

	serial_print("PCI: -----------------------------------\n");
	char *message = (char*)malloc(64);
	sprintf(message, "PCI: found %u devices and %u drivers\n", devs, drivs);
	serial_print(message);
}
