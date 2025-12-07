#include "storage.h"
#include "blocks.h"
#include "helpers/slist.h"
#include "inode.h"
#include "slist.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME_MAX_SIZE 11
#define DIR_MAX_ENTRIES 128
#define PATH_SEPERATOR '/'

typedef struct dir_entry {
  // The name of the current directory
  char name[FILE_NAME_MAX_SIZE + 1];
  // The inum of the entry refering to
  int inum;
} dir_entry_t;

#define CURR_ENTRY_INDEX 0 // The entry of "."
#define PREV_ENTRY_INDEX 1 // The entry of ".."

typedef struct directory {
  // Number of entries in the directory.
  int entries_count;
  // directory entries
  dir_entry_t entries[DIR_MAX_ENTRIES];
} directory_t;

static int get_dir_entry(directory_t* dir, const char* entry_name, dir_entry_t** result) {
  assert(dir != NULL);
  assert(result != NULL);
  for (int i = 0; i < dir->entries_count; ++i) {
    if (strncmp(entry_name, dir->entries[i].name, FILE_NAME_MAX_SIZE)) {
      *result = dir->entries + i;
      return 0;
    }
  }
  return -ENOENT;
}

static void* get_inode_data(int inum) {
  return blocks_get_block(get_inode(inum)->block);
}

static int get_entry_from_rel_path(directory_t* dir, const char* path, dir_entry_t** result) {
  slist_t* path_list = slist_explode(path, PATH_SEPERATOR);
  slist_t* curr_node = path_list;
  dir_entry_t* curr_entry = NULL;
  while (curr_node->next != NULL) {
    inode_t* curr_inode = get_inode(curr_entry->inum);
    // TODO: handle . and ..
    if (curr_inode->mode & DIR_BIT) {
      // Not a directory
      slist_free(path_list);
      printf("%s is not a directory.\n", path);
      return -ENOTDIR;
    }
    directory_t* curr_dir = get_inode_data(curr_entry->inum);
    get_dir_entry(curr_dir, curr_node->data, &curr_entry);
    if (curr_entry == (dir_entry_t*)(-1)) {
      // Not found
      slist_free(path_list);
      printf("%s not found.\n", path);
      return -ENOENT;
    }
  }
  slist_free(path_list);
  if (curr_node == NULL) {
    // it is the current entry, return dir
    *result = dir->entries + CURR_ENTRY_INDEX;
  }
  else {
    *result = curr_entry;
  }
  return 0;
}

void storage_init(const char *image_path) {
  blocks_init(image_path);
}

int storage_stat(const char *path, struct stat *st) {
  return -1; // TODO
}

int storage_read(const char *path, char *buf, size_t size, off_t offset) {
  // inode[0] should be the root dir:
  directory_t* root_dir = blocks_get_block(ROOT_DIR_INODE_INDEX);
  dir_entry_t* dir_entry = NULL;
  int ret = get_entry_from_rel_path(root_dir, path, &dir_entry);
  if (ret != 0) {
    // Failed.
    return ret;
  }
  inode_t* entry_inode = get_inode(dir_entry->inum);
  char* block_data = blocks_get_block(entry_inode->block);
  size_t copy_size = entry_inode->size > size + offset ? size : entry_inode->size;
  memcpy(buf, block_data + offset, copy_size);
  return copy_size;
}

int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
  // inode[0] should be the root dir:
  directory_t* root_dir = blocks_get_block(ROOT_DIR_INODE_INDEX);
  dir_entry_t* dir_entry = NULL;
  int ret = get_entry_from_rel_path(root_dir, path, &dir_entry);
  if (ret != 0) {
    // Failed.
    return ret;
  }
  inode_t* entry_inode = get_inode(dir_entry->inum);
  char* block_data = blocks_get_block(entry_inode->block);
  size_t copy_size = entry_inode->size > size + offset ? size : entry_inode->size;
  memcpy(block_data + offset, buf, copy_size);
  return copy_size;
}

int storage_truncate(const char *path, off_t size) {
  return -1; // TODO
}

int storage_mknod(const char *path, int mode) {
  return -1; // TODO
}

int storage_unlink(const char *path) {
  return -1; // TODO
}

int storage_link(const char *from, const char *to) {
  return -1; // TODO
}

int storage_rename(const char *from, const char *to) {
  return -1; // TODO
}

int storage_set_time(const char *path, const struct timespec ts[2]) {
  return -1; // TODO
}

slist_t *storage_list(const char *path) {
  return NULL; // TODO
}

