#include "storage.h"
#include "blocks.h"
#include "directory.h"
#include "inode.h"
#include "slist.h"
#include "bitmap.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#define ROOT_INUM 0

static int path_to_inum(const char *path) {
  if (strcmp(path, "/") == 0) {
    return ROOT_INUM;
  }

  if (path[0] == '/') {
    path++;
  }

  int current_inum = ROOT_INUM;
  slist_t *path_list = slist_explode(path, '/');
  slist_t *curr = path_list;

  while (curr != NULL && strlen(curr->data) > 0) {
    inode_t *current_inode = get_inode(current_inum);

    // Check if current is a directory
    if ((current_inode->mode & 040000) == 0) {
      slist_free(path_list);
      return -ENOTDIR;
    }

    int next_inum = directory_lookup(current_inode, curr->data);
    if (next_inum < 0) {
      slist_free(path_list);
      return -ENOENT;
    }

    current_inum = next_inum;
    curr = curr->next;
  }

  slist_free(path_list);
  return current_inum;
}

static int get_parent_and_name(const char *path, int *parent_inum, char *name) {
  const char *last_slash = strrchr(path, '/');
  if (last_slash == NULL) {
    return -EINVAL;
  }

  strncpy(name, last_slash + 1, DIR_NAME_LENGTH);

  if (last_slash == path) {
    *parent_inum = ROOT_INUM;
  } else {
    char parent_path[256];
    int len = last_slash - path;
    strncpy(parent_path, path, len);
    parent_path[len] = '\0';
    
    *parent_inum = path_to_inum(parent_path);
    if (*parent_inum < 0) {
      return *parent_inum;
    }
  }

  return 0;
}

void storage_init(const char *path) {
  blocks_init(path);
  void *blocks_bm = get_blocks_bitmap();
  bitmap_put(blocks_bm, 1, 1);
  directory_init();
}

int storage_stat(const char *path, struct stat *st) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }

  inode_t *node = get_inode(inum);
  memset(st, 0, sizeof(struct stat));
  st->st_mode = node->mode;
  st->st_size = node->size;
  st->st_nlink = node->refs;
  st->st_uid = getuid();
  st->st_gid = getgid();

  return 0;
}

int storage_read(const char *path, char *buf, size_t size, off_t offset) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }

  inode_t *node = get_inode(inum);

  if (node->mode & 040000) {
    return -EISDIR;
  }

  if (offset >= node->size) {
    return 0;
  }

  size_t available = node->size - offset;
  size_t to_read = (size < available) ? size : available;

  char *data = (char *)blocks_get_block(node->block);
  memcpy(buf, data + offset, to_read);

  return to_read;
}

int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }

  inode_t *node = get_inode(inum);

  if (node->mode & 040000) {
    return -EISDIR;
  }

  size_t new_size = offset + size;
  if (new_size > BLOCK_SIZE) {
    return -EFBIG;
  }

  if (new_size > node->size) {
    node->size = new_size;
  }

  char *data = (char *)blocks_get_block(node->block);
  memcpy(data + offset, buf, size);

  return size;
}

int storage_truncate(const char *path, off_t size) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }

  if (size > BLOCK_SIZE) {
    return -EFBIG;
  }

  inode_t *node = get_inode(inum);
  node->size = size;
  return 0;
}

int storage_mknod(const char *path, int mode) {
  if (path_to_inum(path) >= 0) {
    return -EEXIST;
  }


  int parent_inum;
  char name[DIR_NAME_LENGTH];
  int ret = get_parent_and_name(path, &parent_inum, name);
  if (ret != 0) {
    return ret;
  }

  int new_inum = alloc_inode();
  if (new_inum < 0) {
    return -ENOSPC;
  }

  inode_t *node = get_inode(new_inum);
  node->refs = 1;
  node->mode = mode;
  node->size = 0;
  node->block = alloc_block();
  node->next = -1;

  if (mode & 040000) {
    dirent_t *entries = (dirent_t *)blocks_get_block(node->block);
    int max_entries = BLOCK_SIZE / sizeof(dirent_t);
    for (int i = 0; i < max_entries; i++) {
      entries[i].inum = -1;
      entries[i].name[0] = '\0';
    }
  }

  inode_t *parent = get_inode(parent_inum);
  ret = directory_put(parent, name, new_inum);
  if (ret != 0) {
    free_inode(new_inum);
    return ret;
  }

  return 0;
}

int storage_unlink(const char *path) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }

  int parent_inum;
  char name[DIR_NAME_LENGTH];
  int ret = get_parent_and_name(path, &parent_inum, name);
  if (ret != 0) {
    return ret;
  }

  inode_t *parent = get_inode(parent_inum);
  ret = directory_delete(parent, name);
  if (ret != 0) {
    return ret;
  }

  free_inode(inum);

  return 0;
}

int storage_link(const char *from, const char *to) {
  int from_inum = path_to_inum(from);
  if (from_inum < 0) {
    return from_inum;
  }

  int parent_inum;
  char name[DIR_NAME_LENGTH];
  int ret = get_parent_and_name(to, &parent_inum, name);
  if (ret != 0) {
    return ret;
  }

  inode_t *parent = get_inode(parent_inum);
  ret = directory_put(parent, name, from_inum);
  if (ret != 0) {
    return ret;
  }

  inode_t *node = get_inode(from_inum);
  node->refs++;

  return 0;
}

int storage_rename(const char *from, const char *to) {
  int ret = storage_link(from, to);
  if (ret != 0) {
    return ret;
  }
  return storage_unlink(from);
}

int storage_set_time(const char *path, const struct timespec ts[2]) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return inum;
  }
  return 0;
}

slist_t *storage_list(const char *path) {
  int inum = path_to_inum(path);
  if (inum < 0) {
    return NULL;
  }

  inode_t *node = get_inode(inum);
  dirent_t *entries = (dirent_t *)blocks_get_block(node->block);

  slist_t *list = NULL;
  int max_entries = BLOCK_SIZE / sizeof(dirent_t);
  
  for (int i = 0; i < max_entries; i++) {
    if (entries[i].inum >= 0 && entries[i].name[0] != '\0') {
      list = slist_cons(entries[i].name, list);
    }
  }

  return list;
}
