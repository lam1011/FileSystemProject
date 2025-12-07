#include "inode.h"
#include "blocks.h"
#include "bitmap.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define TOTAL_INODE_NUM 1024
#define INODE_BLOCK_NUM 1

void print_inode(inode_t *node) {
  assert(node != NULL);
  printf("inode: refs: %d, mode: %d, size: %d, block: %d\n", node->refs, node->mode, node->size, node->block);
}

inode_t *get_inode(int inum) {
   void *block = blocks_get_block(INODE_BLOCK_NUM);  // wherever you put inodes
  inode_t *inodes = (inode_t *)block;
  return &inodes[inum];
}

int alloc_inode() {
  void *inode_bitmap = get_inode_bitmap();
  
  for (int i = 0; i < TOTAL_INODE_NUM; i++) {
    if (!bitmap_get(inode_bitmap, i)) {
      bitmap_put(inode_bitmap, i, 1);
      
      inode_t *node = get_inode(i);
      node->refs = 0;
      node->mode = 0;
      node->size = 0;
      node->block = 0;
      node->next = -1; 
      
      return i;
    }
  }
  return -1; 
}

void free_inode(int inum) {
  inode_t *node = get_inode(inum);
  if (node->block != 0) {
    free_block(node->block);
  }
  void *inode_bitmap = get_inode_bitmap();
  bitmap_put(inode_bitmap, inum, 0);
}

int grow_inode(inode_t *node, int size) {
  assert(node->size + size <= BLOCK_SIZE);
  node->size += size;
  inode_t *inodes = (inode_t *)blocks_get_block(1);
  return (node - inodes);
}

int shrink_inode(inode_t *node, int size) {
  assert(node->size - size >= 0);
  node->size -= size;
  inode_t *inodes = (inode_t *)blocks_get_block(1);
  return (node - inodes);
}
