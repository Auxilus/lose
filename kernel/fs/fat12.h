#ifndef _FS_FAT12
#define _FS_FAT12

#include "../../cpu/types.h"

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

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

typedef struct
{
  uint8_t name[11];
  uint8_t attributes;
  uint8_t reserved;
  uint8_t created_time_tenth;
  uint16_t created_time;
  uint16_t created_date;
  uint16_t last_access_date;
  uint16_t first_cluster_high;
  uint16_t last_modified_time;
  uint16_t last_modified_date;
  uint16_t first_cluster_low;
  uint32_t size;
} __attribute__((packed)) fat12_dir_entry_t;

static char *fat12_boot;

void fat12_init();

#endif