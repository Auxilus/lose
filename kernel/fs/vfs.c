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
  identify();

  vfs_info = (_vfs_info *)malloc(sizeof(_vfs_info));

  char *boot_sector = (char *)malloc(512);
  ata_pio_read48((uint64_t)0, (uint16_t)1, boot_sector);

  fat12_boot_record_t *boot_record = (fat12_boot_record_t *)boot_sector;
  vfs_boot_record = boot_record;
  int total_size = boot_record->bytes_per_sector * boot_record->total_sectors;

  vfs_info->total_size = total_size;
  vfs_info->drive_number = boot_record->drive_number;
  vfs_info->sectors_per_cluster = boot_record->sectors_per_cluster;
  memcpy(boot_record->volume_label, vfs_info->volume_label, 12);
  memcpy(boot_record->system_id, vfs_info->system_id, 9);
  vfs_info->volume_label[11] = '\0';
  vfs_info->system_id[8] = '\0';

  // FAT table
  int fat_start_sector = boot_record->reserved_sectors;
  int fat_sectors = boot_record->sectors_per_fat * boot_record->fat_count;
  fat_table = (char *)malloc(fat_sectors * boot_record->bytes_per_sector);
  ata_pio_read48(fat_start_sector, fat_sectors, fat_table);

  // Root directory
  int root_start_sector = fat_start_sector + fat_sectors;
  int root_sectors = (32 * boot_record->dir_entry + boot_record->bytes_per_sector - 1) / boot_record->bytes_per_sector;
  int size = sizeof(fat12_dir_entry_t) * boot_record->dir_entry;
  int sectors = (size / boot_record->bytes_per_sector);
  if (size % boot_record->bytes_per_sector > 0)
    sectors++;
  vfs_info->root_directory_end = (boot_record->reserved_sectors + boot_record->sectors_per_fat * boot_record->fat_count) + sectors;

  // read root directory
  char *dir_buf = (char *)malloc(root_sectors * 512);
  int max_dir_entries = (root_sectors * 512) / sizeof(fat12_dir_entry_t);
  ata_pio_read48(root_start_sector, root_sectors, dir_buf);
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
  char **dir_tree = (char **)malloc((node_itr->depth + 1) * 12);
  int entries = 0;

  dir_tree[0] = "/";
  entries++;
  while (node_itr->parent != NULL)
  {

    char *trimmed = (char *)malloc(12);
    trimwhitespace(trimmed, strlen(node_itr->name), node_itr->name);
    dir_tree[node_itr->depth] = trimmed;
    entries++;
    node_itr = node_itr->parent;
  }

  for (int i = 0; i < entries; i++)
  {
    strcat(pwd_string, dir_tree[i]);
    if (i > 0)
    {
      strcat(pwd_string, "/");
    }
  }

  return pwd_string;
}

int vfs_change_dir(char *dirname)
{

  fs_node *node;
  int found = 0;
  for (int i = 0; i < current_top_node->node_count; i++)
  {
    node = (fs_node *)current_top_node->child[i];
    char *trimmed = (char *)malloc(strlen(node->name));
    trimwhitespace(trimmed, strlen(node->name), (char *)&node->name);
    if (strcmp(trimmed, dirname) == 0 && node->type == 0x10)
    {
      node->parent = current_top_node;
      found = 1;
      break;
    }
  }

  if (found == 0)
  {
    return -1;
  }

  if (strcmp(dirname, ".") == 0 && current_top_node != NULL)
  {
    return 1;
  }

  if (strcmp(dirname, "..") == 0 && last_top_node != NULL)
  {
    if (current_top_node->parent != NULL)
    {
      current_top_node = current_top_node->parent;
    }
    return 1;
  }

  // current_top_node = node;

  // process child nodes
  int cluster = node->first_cluster;
  int sector = 33 + cluster - 2;

  char *dir_buf = (char *)malloc(vfs_info->sectors_per_cluster * 512);
  int max_dir_entries = (vfs_info->sectors_per_cluster * 512) / sizeof(fat12_dir_entry_t);
  ata_pio_read48(sector, vfs_info->sectors_per_cluster, dir_buf);
  int offset = 0;

  fs_node **child_nodes;
  node->child = (fs_node **)malloc(sizeof(*child_nodes) * max_dir_entries);
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
    memcpy(dir_entry->name, new_node->name, 11);
    new_node->name[11] = '\0';

    new_node->size = dir_entry->size;
    new_node->first_cluster = dir_entry->first_cluster_low;
    if (strcmp((const char *)&new_node->name, "..         ") == 0)
    {
      new_node->parent = current_top_node;
    }
    else
    {
      new_node->parent = node;
    }
    new_node->type = dir_entry->attributes;

    // if ((dir_entry->attributes != ATTR_DIRECTORY) || (dir_entry->attributes != ATTR_VOLUME_ID))
    // {
    //   new_node->parent = NULL;
    // }

    if (dir_entry->attributes == ATTR_VOLUME_ID)
    {
      current_top_node->volume = new_node;
    }
    else
    {
      new_node->depth = node->depth + 1;
      node->child[current_top_node_cnt] = new_node;
      current_top_node_cnt++;
      node->node_count++;
    }

    offset += sizeof(fat12_dir_entry_t);
  }

  last_top_node = current_top_node;
  current_top_node = node;
  return 1;
}

vfs_file *vfs_cat_dir(char *dirname)
{
  fs_node *node;
  int found = 0;

  for (int i = 0; i < current_top_node->node_count; i++)
  {
    node = (fs_node *)current_top_node->child[i];
    char *trimmed = (char *)malloc(strlen(node->name));
    trimwhitespace(trimmed, strlen(node->name), (char *)&node->name);
    if (strcmp(trimmed, dirname) == 0 && (node->type == 0x20 || node->type == 0x01))
    {
      found = 1;
      break;
    }
    free(strlen(node->name));
  }

  if (!found)
  {
    return NULL;
  }

  vfs_file *ret = (vfs_file*)malloc(sizeof(vfs_file));
  ret->size = node->size;
  ret->read_size = node->size + vfs_boot_record->bytes_per_sector;
  ret->fptr = (char *)malloc(node->size + vfs_boot_record->bytes_per_sector);

  int current_cluster = node->first_cluster;
  char *file_data = ret->fptr;

  do
  {
    uint64_t sector = vfs_info->root_directory_end + (current_cluster - 2) * vfs_boot_record->sectors_per_cluster;

    ata_pio_read48(sector, vfs_boot_record->sectors_per_cluster, file_data);
    file_data += vfs_boot_record->sectors_per_cluster * vfs_boot_record->bytes_per_sector;

    uint32_t fat_index = current_cluster * 3 / 2;
    if (current_cluster % 2 == 0)
    {
      current_cluster = (*(uint16_t *)(fat_table + fat_index)) & 0x0FFF;
    }
    else
    {
      current_cluster = (*(uint16_t *)(fat_table + fat_index)) >> 4;
    }
  } while (current_cluster < 0x0FF8);

  return ret;
}