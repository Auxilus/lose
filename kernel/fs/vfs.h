#ifndef _FS_VFS
#define _FS_VFS

#include "../../cpu/types.h"
#include "./fat12.h"

typedef struct fs_node
{
  uint8_t type;
  uint8_t depth;
  uint8_t name[12];
  int node_count;
  uint16_t first_cluster;
  uint32_t size;
  struct fs_node **child;
  struct fs_node *parent;
  struct fs_node *volume;
} __attribute__((packed)) fs_node;

typedef struct _vfs_info
{
  int total_size;
  int drive_number;
  uint8_t sectors_per_cluster;
  int root_directory_end;
  char volume_label[12];
  char system_id[9];

} __attribute__((packed)) _vfs_info;

typedef struct vfs_file
{
  char *fptr;
  int size;
  int read_size;
} vfs_file;

static fat12_boot_record_t *vfs_boot_record;
static fs_node *current_top_node;
static fs_node *last_top_node;
static _vfs_info *vfs_info;
static char* fat_table;

int vfs_initialized;

fs_node *vfs_init(void);
fs_node *vfs_get_top_node(void);
char vfs_get_node_type(fs_node *node);
char *vfs_get_pwd();
int vfs_change_dir(char *dirname);
vfs_file *vfs_cat_dir(char *dirname);

#endif