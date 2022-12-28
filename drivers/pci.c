/** @author Levente Kurusa <levex@linux.com> **/
#include "../utils/mem.h"
#include "../utils/string.h"
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
				//serial_printf("PCI: - %u    %u\n", vendor, device);
				pci_device *pdev = (pci_device *)malloc(sizeof(pci_device));
				pdev->vendor = vendor;
				pdev->device = device;
				pdev->func = function;
				pdev->driver = 0;
				add_pci_device(pdev);
			}
		}
	}
}

uint16_t pciCheckVendor(uint16_t bus, uint16_t slot)
{
	uint16_t vendor,device;
	/* check if device is valid */
	if ((vendor = pci_read_word(bus,slot,0,0)) != 0xFFFF) {
		device = pci_read_word(bus,slot,0,2);
		/* valid device */
	} return (vendor);
}


void pci_init()
{
	devs = drivs = 0;
	serial_print("PCI: probe\n");
	pci_devices = (pci_device **)malloc(32 * sizeof(pci_device));
	pci_drivers = (pci_driver **)malloc(32 * sizeof(pci_driver));
	pci_probe();
	serial_print("PCI: driver support loaded\n");
}

void pci_register_driver(pci_driver *driv)
{
	pci_drivers[drivs] = driv;
	drivs ++;
	return;
}

void pci_proc_dump(uint8_t *buffer)
{
	char messages[devs][100];
	for(int i = 0; i < devs; i++)
	{
		pci_device *pci_dev = pci_devices[i];
		if(pci_dev->driver)
			sprintf(messages[i], "[%x:%x:%x] => %s\n", pci_dev->vendor, pci_dev->device, pci_dev->func, pci_dev->driver->name);
		else
			sprintf(messages[i], "[%x:%x:%x]\n", pci_dev->vendor, pci_dev->device, pci_dev->func);

		serial_print(messages[i]);
	}
}
