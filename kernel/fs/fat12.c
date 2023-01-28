#include "fat12.h"
#include "../../utils/mem.h"
#include "../../utils/string.h"
#include "../../utils/console.h"

void fat12_init(char *buffer)
{
  fat12_boot_record_t *fat12_boot_record = (fat12_boot_record_t *)buffer;
  int total_size = fat12_boot_record->bytes_per_sector * fat12_boot_record->total_sectors;

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
  sprintf(fat12_boot_info, "%s\n%s\n%s\n%u bytes\n",
          oem,
          volume_label,
          system_id,
          total_size);
  console_pre_print(fat12_boot_info);
}