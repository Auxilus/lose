#include "fat12.h"
#include "../../utils/mem.h"
#include "../../utils/string.h"
#include "../../utils/console.h"
#include "../../drivers/ata.h"

void fat12_init()
{
  fat12_boot = (char *)malloc(512);
  read_sector(0, 1, fat12_boot);
  fat12_boot_record_t *fat12_boot_record = (fat12_boot_record_t *)fat12_boot;
  int total_size = fat12_boot_record->bytes_per_sector * fat12_boot_record->total_sectors;

  int fat_start_sector = fat12_boot_record->reserved_sectors;
  int fat_sectors = fat12_boot_record->sectors_per_fat * fat12_boot_record->fat_count;

  int root_dir_start_sector = fat_start_sector + fat_sectors;
  int root_dir_sectors = (32 * fat12_boot_record->dir_entry + fat12_boot_record->bytes_per_sector - 1) / fat12_boot_record->bytes_per_sector;

  int data_start_sector = root_dir_start_sector + root_dir_sectors;
  int data_sectors = fat12_boot_record->total_sectors - data_start_sector;

  int total_clusters = data_sectors / fat12_boot_record->sectors_per_cluster;

  char *oem = (char *)malloc(9);
  char *volume_label = (char *)malloc(12);
  char *system_id = (char *)malloc(9);

  memcpy(fat12_boot_record->volume_label, volume_label, 11);
  memcpy(fat12_boot_record->oem, oem, 8);
  memcpy(fat12_boot_record->system_id, system_id, 8);

  oem[8] = '\0';
  volume_label[11] = '\0';
  system_id[8] = '\0';

  char fat12_boot_info[1024];
  console_pre_print("FAT12 loaded\n---------------------\n");
  sprintf(fat12_boot_info, "%s\n%s\n%s\n%u bytes\n%u fat sectors\n",
          oem,
          volume_label,
          system_id,
          total_size,
          fat_sectors);
  console_pre_print(fat12_boot_info);
}