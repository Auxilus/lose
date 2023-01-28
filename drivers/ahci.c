#include "ahci.h"
#include "../utils/console.h"

hba_memory_t *abar;
port_t ports[32];
uint8_t port_count;

static uint8_t check_port_type(hba_port_t *port)
{
  uint32_t sata_status = port->sata_status;

  uint8_t interface_power_management = (sata_status >> 8) & 0x0F;
  uint8_t device_detection = sata_status & 0x0F;

  if (device_detection != HBA_PORT_DEV_PRESENT)
    return NONE;
  if (interface_power_management != HBA_PORT_IPM_ACTIVE)
    return NONE;

  switch (port->signature)
  {
  case SATA_SIGNATURE_ATA:
    return SATA;
    break;
  case SATA_SIGNATURE_ATAPI:
    return SATAPI;
    break;
  case SATA_SIGNATURE_PM:
    return PM;
  case SATA_SIGNATURE_SEMB:
    return SEMB;
  default:
    return NONE;
    break;
  }
}

static void probe_ports()
{
  uint32_t ports_implemented = abar->ports_implemented;

  for (int i = 0; i < 32; i++)
    if (ports_implemented & (1 << i)) // port is active
    {
      int port_type = check_port_type(&abar->ports[i]);

      if (port_type == SATA || port_type == SATAPI)
      {
        ports[port_count].port_number = port_count;
        ports[port_count].port_type = port_type;
        ports[port_count].hba_port = &abar->ports[i];

        port_count++;
      }
    }
}

void ahci_init(pci_device *device)
{
  hba_memory_t *abar = device->BAR5;
  probe_ports();

  for (int i = 0; i < port_count; i++)
  {
    console_pre_print("AHCI: device ports found\n");
    
    // port_t *port = &ports[i];
    // port_configure(port);

    // if (port->port_type == SATA)
    //   console_log("SATA device found. Port number: %d", port->port_number);
    // else if (port->port_type == SATAPI)
    //   console_log("SATAPI device found. Port number: %d", port->port_number);
    // else
    //   console_log("Invalid port type.");
  }
  ahci_initialized = 1;
  // console_log("AHCI driver initialized.");
}