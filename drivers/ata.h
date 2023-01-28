#ifndef _DRIVER_ATA
#define _DRIVER_ATA

#define ATA_BASE 0x1f0
void read_sector(int addr, int count, char *buffer);

#endif