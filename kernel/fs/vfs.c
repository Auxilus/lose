#include "vfs.h"
#include "fat12.h"
#include "../../utils/mem.h"
#include "../../utils/string.h"
#include "../../utils/console.h"
#include "../../drivers/ata.h"

fs_node *vfs_init()
{
  // implement vfs for fat12 for now
  // read first fat12 sector
  // build root directory node tree

  vfs_info = (_vfs_info *)malloc(sizeof(_vfs_info));

  char *boot_sector = (char *)malloc(512);
  read_sector(0, 1, boot_sector);

  fat12_boot_record_t *boot_record = (fat12_boot_record_t *)boot_sector;
  int total_size = boot_record->bytes_per_sector * boot_record->total_sectors;
  
  vfs_info->total_size = total_size;
  vfs_info->drive_number = boot_record->drive_number;
  memcpy(boot_record->volume_label, vfs_info->volume_label, 12);
  memcpy(boot_record->system_id, vfs_info->system_id, 9);
  vfs_info->volume_label[11] = '\0';
  vfs_info->system_id[8] = '\0';

  // FAT table
  int fat_start_sector = boot_record->reserved_sectors;
  int fat_sectors = boot_record->sectors_per_fat * boot_record->fat_count;

  // Root directory
  int root_start_sector = fat_start_sector + fat_sectors;
  int root_sectors = (32 * boot_record->dir_entry + boot_record->bytes_per_sector - 1) / boot_record->bytes_per_sector;

  // read root directory
  char *dir_buf = (char *)malloc(root_sectors * 512);
  int max_dir_entries = (root_sectors * 512) / sizeof(fat12_dir_entry_t);
  read_sector(root_start_sector, root_sectors, dir_buf);
  int offset = 0;

  fs_node **child_nodes;
  current_top_node = (fs_node *)malloc(sizeof(fs_node));
  current_top_node->child = (fs_node **)malloc(sizeof(*child_nodes) * max_dir_entries);
  current_top_node->parent = ((void *)0);
  current_top_node->volume = (fs_node *)malloc(sizeof(fs_node));
  current_top_node->node_count = 0;
  current_top_node->size = 0;
  current_top_node->depth = 0;
  current_top_node->type = 0;

  // current_top_node->name[0] = boot_record->volume_label[0];
  // current_top_node->name[1] = boot_record->volume_label[1];
  // current_top_node->name[2] = boot_record->volume_label[2];
  // current_top_node->name[3] = boot_record->volume_label[3];
  // current_top_node->name[4] = boot_record->volume_label[4];
  // current_top_node->name[5] = boot_record->volume_label[5];
  // current_top_node->name[6] = boot_record->volume_label[6];
  // current_top_node->name[7] = boot_record->volume_label[7];
  // current_top_node->name[8] = boot_record->volume_label[8];
  // current_top_node->name[9] = boot_record->volume_label[9];
  // current_top_node->name[10] = boot_record->volume_label[10];
  memcpy(boot_record->volume_label, current_top_node->name, 12);
  current_top_node->name[11] = '\0';

  int current_top_node_cnt = 0;

  while (dir_buf[offset] != 0x00)
  {
    if (dir_buf[offset] == 0xe5)
    {
      // unused entry, continue
      offset += sizeof(fat12_dir_entry_t);
      continue;
    }

    fat12_dir_entry_t *dir_entry = (fat12_dir_entry_t *)(dir_buf + offset);

    fs_node *new_node = (fs_node *)malloc(sizeof(fs_node));
    // new_node->name[0] = dir_entry->name[0];
    // new_node->name[1] = dir_entry->name[1];
    // new_node->name[2] = dir_entry->name[2];
    // new_node->name[3] = dir_entry->name[3];
    // new_node->name[4] = dir_entry->name[4];
    // new_node->name[5] = dir_entry->name[5];
    // new_node->name[6] = dir_entry->name[6];
    // new_node->name[7] = (dir_entry->name[7] == 0xe5) ? 0x20 : dir_entry->name[7];
    // new_node->name[8] = (dir_entry->name[8] == 0x23) ? 0x20 : dir_entry->name[8];
    // new_node->name[9] = (dir_entry->name[9] == 0x10) ? 0x20 : dir_entry->name[9];
    // new_node->name[10] = dir_entry->name[10];
    memcpy(dir_entry->name, new_node->name, 11);
    new_node->name[11] = '\0';

    new_node->size = dir_entry->size;
    new_node->first_cluster = dir_entry->first_cluster_low;
    new_node->parent = current_top_node;
    new_node->type = dir_entry->attributes;

    if ((dir_entry->attributes != ATTR_DIRECTORY) || (dir_entry->attributes != ATTR_VOLUME_ID))
    {
      new_node->parent = NULL;
    }

    if (dir_entry->attributes == ATTR_VOLUME_ID)
    {
      current_top_node->volume = new_node;
    }
    else
    {
      new_node->depth = 1;
      current_top_node->child[current_top_node_cnt] = new_node;
      current_top_node_cnt++;
      current_top_node->node_count++;
    }

    offset += sizeof(fat12_dir_entry_t);
  }

  return current_top_node;
}

fs_node *vfs_get_top_node()
{
  return current_top_node;
}

char vfs_get_node_type(fs_node *node)
{
  // #define ATTR_READ_ONLY 0x01
  // #define ATTR_HIDDEN 0x02
  // #define ATTR_SYSTEM 0x04
  // #define ATTR_VOLUME_ID 0x08
  // #define ATTR_DIRECTORY 0x10
  // #define ATTR_ARCHIVE 0x20

  char type = '\0';
  switch (node->type)
  {
  case 0x01:
    type = 'X';
    break;
  case 0x02:
    type = 'H';
    break;
  case 0x04:
    type = 'S';
    break;
  case 0x08:
    type = 'V';
    break;
  case 0x10:
    type = 'D';
    break;
  case 0x20:
    type = 'A';
    break;
  default:
    break;
  }

  return type;
}

char *vfs_get_pwd()
{
  char *pwd_string = (char *)malloc(1024);
  fs_node *node_itr = current_top_node;
  char **dir_tree = (char **)malloc(node_itr->depth * 12);

  dir_tree[0] = "/";
  strcat(pwd_string, dir_tree[0]);

  do
  {
  } while (node_itr->parent != NULL);

  for (int i = node_itr->depth; i > 0; i--)
  {
    strcat(pwd_string, dir_tree[i - 1]);
  }

  return pwd_string;
}

char *vfs_change_dir(char *dirname)
{
}