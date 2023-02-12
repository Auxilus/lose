#ifndef _DRIVER_ATA
#define _DRIVER_ATA
#include "../cpu/types.h"
#define ATA_BASE 0x1f0
void read_sector(int addr, int count, char *buffer);
uint8_t identify();
void ata_pio_read28(uint32_t LBA, uint8_t sectorcount, uint8_t *target);
void ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t *target);
void ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t *target);
#endif