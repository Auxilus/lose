#ifndef _DRIVER_AHCI
#define _DRIVER_AHCI

#include "../cpu/types.h"
#include "pci.h"

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1

#define SATA_SIGNATURE_ATA 0x00000101
#define SATA_SIGNATURE_SEMB 0xC33C0101
#define SATA_SIGNATURE_PM 0x96690101
#define SATA_SIGNATURE_ATAPI 0xEB140101

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_READ_DMA_EX 0x25

#define HBA_PxIS_TFES (1 << 30)

// 3.3 in docs
typedef struct hba_port
{
  uint32_t cmd_list_base;
  uint32_t cmd_list_base_uppper;
  uint32_t fis_base;
  uint32_t fis_base_upper;
  uint32_t interrupt_status;
  uint32_t interrupt_enable;
  uint32_t cmd_and_status;
  uint32_t rsv0;
  uint32_t task_file_data;
  uint32_t signature;
  uint32_t sata_status;
  uint32_t sata_control;
  uint32_t sata_error;
  uint32_t sata_active;
  uint32_t cmd_issue;
  uint32_t sata_notification;
  uint32_t fis_switch_control;
  uint32_t rsv1[11];
  uint32_t vendor[4];
} hba_port_t;

// 3 in docs
typedef struct hba_memory
{
  uint32_t host_capabilities;
  uint32_t global_host_control;
  uint32_t interrupt_status;
  uint32_t ports_implemented;
  uint32_t version;
  uint32_t ccc_control;
  uint32_t ccc_ports;
  uint32_t enclosure_managemen_location;
  uint32_t enclosure_managemen_control;
  uint32_t host_capabilities_extended;
  uint32_t bios_handoff_control_status;

  uint8_t rsv0[116];
  uint8_t vendor[96];
  hba_port_t ports[1];
} hba_memory_t;

// 1.3.1 in docs - page 40
typedef struct hba_cmd_header
{
  uint8_t cmd_fis_length : 5;
  uint8_t atapi : 1;
  uint8_t write : 1;
  uint8_t prefetchable : 1;

  uint8_t reset : 1;
  uint8_t bist : 1;
  uint8_t clear_busy : 1;
  uint8_t rsv0 : 1;
  uint8_t port_multiplier : 4;

  uint16_t prdt_length;
  uint32_t prdb_count;
  uint32_t cmd_table_base;
  uint32_t cmd_table_base_upper;
  uint32_t rsv1[4];
} hba_cmd_header_t;

typedef struct hba_prdt_entry
{
  uint32_t data_base_addr;
  uint32_t data_base_addr_upper;
  uint32_t rsv0;

  uint32_t byte_count : 22;
  uint32_t rsv1 : 9;
  uint32_t interrupt_on_completion : 1;
} hba_prdt_entry_t;

typedef struct hba_cmd_table
{
  uint8_t cmd_fis[64];
  uint8_t atapi_cmd[16];
  uint8_t rsv[48];

  hba_prdt_entry_t prdt_entry[1];
} hba_cmd_table_t;

typedef enum
{
  FIS_TYPE_REG_H2D = 0x27,   // Register FIS - host to device
  FIS_TYPE_REG_D2H = 0x34,   // Register FIS - device to host
  FIS_TYPE_DMA_ACT = 0x39,   // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
  FIS_TYPE_DATA = 0x46,      // Data FIS - bidirectional
  FIS_TYPE_BIST = 0x58,      // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS = 0xA1,  // Set device bits FIS - device to host
} FIS_TYPE;

typedef struct FIS_REG_H2D
{
  uint8_t fis_type;

  uint8_t port_multiplier : 4;
  uint8_t rsv0 : 3;
  uint8_t cmd_control : 1;

  uint8_t cmd;
  uint8_t feature_low;

  uint8_t lba0;
  uint8_t lba1;
  uint8_t lba2;
  uint8_t device_register;

  uint8_t lba3;
  uint8_t lba4;
  uint8_t lba5;
  uint8_t feature_high;

  uint8_t count_low;
  uint8_t count_high;
  uint8_t iso_cmd_completion;
  uint8_t control;

  uint8_t rsv1[4];
} FIS_REG_H2D_t;

typedef struct port
{
  uint8_t port_number;
  uint8_t port_type;
  hba_port_t *hba_port;
  uint8_t *buffer; // DMA
} port_t;

enum PORT_TYPE
{
  NONE = 0,
  SATA = 1, // stuff like hard drives
  SEMB = 2,
  PM = 3,
  SATAPI = 4, // optical drives and such
};

static int ahci_initialized;

int ahci_read_port(port_t *port, uint64_t sector, uint32_t sector_count, void *buffer);
int ahci_write_port(port_t *port, uint64_t sector, uint32_t sector_count, void *buffer);
void ahci_init(pci_device *device);

#endif