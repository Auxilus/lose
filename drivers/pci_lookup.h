#include "../utils/string.h"
#include "../utils/mem.h"

char *to_hstring(uint16_t val)
{
	char *ret = (char *)malloc(8);
	sprintf(ret, "%02x", val);
	return ret;
}

const char *device_classes[20] = {
		"Unclassified",
		"Mass Storage Controller",
		"Network Controller",
		"Display Controller",
		"Multimedia Controller",
		"Memory Controller",
		"Bridge Device",
		"Simple Communication Controller",
		"Base System Peripheral",
		"Input Device Controller",
		"Docking Station",
		"Processor",
		"Serial Bus Controller",
		"Wireless Controller",
		"Intelligent Controller",
		"Satellite Communication Controller",
		"Encryption Controller",
		"Signal Processing Controller",
		"Processing Accelerator",
		"Non Essential Instrumentation"};

const char *get_vendor_name(uint16_t vendorID)
{
	switch (vendorID)
	{
	case 0x8086:
		return "Intel Corp";
	case 0x1022:
		return "AMD";
	case 0x10DE:
		return "NVIDIA Corporation";
	}
	return to_hstring(vendorID);
}

const char *get_device_name(uint16_t vendorID, uint16_t deviceID)
{
	switch (vendorID)
	{
	case 0x8086: // Intel
		switch (deviceID)
		{
		case 0x29C0:
			return "Express DRAM";
		case 0x2918:
			return "LPC Interface";
		case 0x2922:
			return "6 port SATA [AHCI mode]";
		case 0x2930:
			return "SMBus";
		}
	}
	return to_hstring(deviceID);
}

const char *MassStorageControllerSubclassName(uint8_t subclassCode)
{
	switch (subclassCode)
	{
	case 0x00:
		return "SCSI Bus";
	case 0x01:
		return "IDE";
	case 0x02:
		return "Floppy Disk";
	case 0x03:
		return "IPI Bus";
	case 0x04:
		return "RAID";
	case 0x05:
		return "ATA";
	case 0x06:
		return "Serial ATA";
	case 0x07:
		return "Serial Attached SCSI";
	case 0x08:
		return "Non-Volatile Memory";
	case 0x80:
		return "Other";
	}
	return to_hstring(subclassCode);
}

const char *SerialBusControllerSubclassName(uint8_t subclassCode)
{
	switch (subclassCode)
	{
	case 0x00:
		return "FireWire (IEEE 1394)";
	case 0x01:
		return "ACCESS Bus";
	case 0x02:
		return "SSA";
	case 0x03:
		return "USB";
	case 0x04:
		return "Fibre Channel";
	case 0x05:
		return "SMBus";
	case 0x06:
		return "Infiniband";
	case 0x07:
		return "IPMI Interface";
	case 0x08:
		return "SERCOS Interface (IEC 61491)";
	case 0x09:
		return "CANbus";
	case 0x80:
		return "SerialBusController - Other";
	}
	return to_hstring(subclassCode);
}

const char *BridgeDeviceSubclassName(uint8_t subclassCode)
{
	switch (subclassCode)
	{
	case 0x00:
		return "Host";
	case 0x01:
		return "ISA";
	case 0x02:
		return "EISA";
	case 0x03:
		return "MCA";
	case 0x04:
		return "PCI-to-PCI";
	case 0x05:
		return "PCMCIA";
	case 0x06:
		return "NuBus";
	case 0x07:
		return "CardBus";
	case 0x08:
		return "RACEway";
	case 0x09:
		return "PCI-to-PCI";
	case 0x0a:
		return "InfiniBand-to-PCI Host";
	case 0x80:
		return "Other";
	}
	return to_hstring(subclassCode);
}

const char *NetworkControllerSubclassName(uint8_t subclassCode)
{
	switch (subclassCode)
	{
	case 0x00:
		return "Ethernet";
	case 0x01:
		return "Token ring";
	case 0x02:
		return "FDDI";
	case 0x03:
		return "ATM";
	case 0x04:
		return "ISDN";
	case 0x05:
		return "WorldFip";
	case 0x06:
		return "PICMG";
	case 0x07:
		return "Infiniband";
	case 0x08:
		return "Fabric";
	case 0x80:
		return "Other";
	}
	return to_hstring(subclassCode);
}

const char *get_subclass_name(uint8_t classCode, uint8_t subclassCode)
{
	switch (classCode)
	{
	case 0x01:
		return MassStorageControllerSubclassName(subclassCode);
	case 0x02:
		return NetworkControllerSubclassName(subclassCode);
	case 0x03:
		switch (subclassCode)
		{
		case 0x00:
			return "VGA Compatible";
		}
	case 0x06:
		return BridgeDeviceSubclassName(subclassCode);
	case 0x0C:
		return SerialBusControllerSubclassName(subclassCode);
	}
	return to_hstring(subclassCode);
}

const char *GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF)
{
	switch (classCode)
	{
	case 0x01:
		switch (subclassCode)
		{
		case 0x06:
			switch (progIF)
			{
			case 0x00:
				return "Vendor Specific Interface";
			case 0x01:
				return "AHCI 1.0";
			case 0x02:
				return "Serial Storage Bus";
			}
		}
	case 0x03:
		switch (subclassCode)
		{
		case 0x00:
			switch (progIF)
			{
			case 0x00:
				return "VGA Controller";
			case 0x01:
				return "8514-Compatible Controller";
			}
		}
	case 0x0C:
		switch (subclassCode)
		{
		case 0x03:
			switch (progIF)
			{
			case 0x00:
				return "UHCI Controller";
			case 0x10:
				return "OHCI Controller";
			case 0x20:
				return "EHCI (USB2) Controller";
			case 0x30:
				return "XHCI (USB3) Controller";
			case 0x80:
				return "Unspecified";
			case 0xFE:
				return "USB Device (Not a Host Controller)";
			}
		}
	}
	return to_hstring(progIF);
}
