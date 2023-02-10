#ifndef _FS_VFS
#define _FS_VFS

#include "../../cpu/types.h"

typedef struct fs_node
{
  uint8_t type;
  uint8_t name[12];
  int node_count;
  uint32_t size;
  struct fs_node **child;
  struct fs_node *parent;
  struct fs_node *volume;
} fs_node;

typedef struct _vfs_info
{
  int total_size;
} _vfs_info;

static fs_node *current_top_node;
_vfs_info *vfs_info;

int vfs_initialized;

fs_node *vfs_init(void);
fs_node *vfs_get_top_node(void);

#endif