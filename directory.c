#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "directory.h"
#include "bitmap.h"

#define DIR_MAX_ENTRIES (BLOCK_SIZE / sizeof(dirent_t)) 

void directory_init() {
   void *inode_bitmap = get_inode_bitmap();
  
  if (!bitmap_get(inode_bitmap, 0)) {
    int root_inum = alloc_inode();
    
    inode_t *root = get_inode(root_inum);
    root->refs = 1;
    root->mode = 040755;
    root->size = 0;
    root->block = alloc_block();
    root->next = -1;

    dirent_t *entries = (dirent_t *)blocks_get_block(root->block);
    for (int i = 0; i < DIR_MAX_ENTRIES; i++) {
      entries[i].inum = -1;
      entries[i].name[0] = '\0';
    }
  }
}

int directory_lookup(inode_t *di, const char *name) {
  dirent_t *entries = (dirent_t *)blocks_get_block(di->block);

  for (int i = 0; i < DIR_MAX_ENTRIES; i++) {
    if (entries[i].inum != -1 && strcmp(entries[i].name, name) == 0) {
      return entries[i].inum;
    }
  }
  return -ENOENT;
}

int directory_put(inode_t *di, const char *name, int inum) {
  dirent_t *entries = (dirent_t *)blocks_get_block(di->block);

  for (int i = 0; i < DIR_MAX_ENTRIES; i++) {
    if (entries[i].inum == -1) {
      strncpy(entries[i].name, name, DIR_NAME_LENGTH);
      entries[i].inum = inum;
      di->size += sizeof(dirent_t);
      return 0;
    }
  }
  return -ENOSPC;
}

int directory_delete(inode_t *di, const char *name) {
  dirent_t *entries = (dirent_t *)blocks_get_block(di->block);

  for (int i = 0; i < DIR_MAX_ENTRIES; i++) {
    if (entries[i].inum != -1 && strcmp(entries[i].name, name) == 0) {
      entries[i].inum = -1;
      entries[i].name[0] = '\0';
      di->size -= sizeof(dirent_t);
      return 0;
    }
  }
  return -ENOENT;
}

slist_t *directory_list(const char *path) {
 return NULL;
}

void print_directory(inode_t *dd) {
  dirent_t *entries = (dirent_t *)blocks_get_block(dd->block);

  printf("Directory contents:\n");
  for (int i = 0; i < DIR_MAX_ENTRIES; i++) {
    if (entries[i].inum != -1) {
      printf("  %s -> inode %d\n", entries[i].name, entries[i].inum);
    }
  }
}