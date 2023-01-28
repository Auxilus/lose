#ifndef _FS_FAT12
#define _FS_FAT12

#include "../../cpu/types.h"

typedef struct
{
  uint8_t boot_jmp[3];
  uint8_t oem[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t fat_count;
  uint16_t dir_entry;
  uint16_t total_sectors;
  uint8_t media_descriptor_type;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t heads;
  uint32_t hidden_sectors;
  uint32_t large_sector_count;

  // extended boot record
  uint8_t drive_number;
  uint8_t reserved;
  uint8_t signature;
  uint32_t volume_id;
  uint8_t volume_label[11];
  uint8_t system_id[8];
} __attribute__((packed)) fat12_boot_record_t;

void fat12_init(char *buffer);

#endif