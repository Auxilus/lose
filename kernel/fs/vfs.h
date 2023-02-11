#ifndef _FS_VFS
#define _FS_VFS

#include "../../cpu/types.h"

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
} fs_node;

typedef struct _vfs_info
{
  int total_size;
  int drive_number;
  char volume_label[12];
  char system_id[9];

} _vfs_info;

static fs_node *current_top_node;
static _vfs_info *vfs_info;

int vfs_initialized;

fs_node *vfs_init(void);
fs_node *vfs_get_top_node(void);
char vfs_get_node_type(fs_node *node);
char *vfs_get_pwd();
char *vfs_change_dir(char *dirname);

#endif