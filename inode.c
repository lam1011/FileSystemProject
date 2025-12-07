#include "inode.h"
#include "inode.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define TOTAL_INODE_NUM 1024

#define BITS_PER_LONG (sizeof(long) / 8)
#define INODE_STATE(i) (inode_status[(i) / BITS_PER_LONG] & (1L << ((i) % BITS_PER_LONG)))
#define TOGGLE_INODE_STATE(i) (inode_status[(i) / BITS_PER_LONG] ^= (1L << ((i) % BITS_PER_LONG)))

// The status of each inode on each bit.
static long inode_status[TOTAL_INODE_NUM / sizeof(long) / 8] = {0};
// The array of inodes.
static inode_t inodes[TOTAL_INODE_NUM];

void print_inode(inode_t *node) {
  assert(node != NULL);
  printf("inode: refs: %d, mode: %d, size: %d, block: %d\n", node->refs, node->mode, node->size, node->block);
}

inode_t *get_inode(int inum) {
  assert(INODE_STATE(inum));
  return inodes + inum;
}

static int available_inode() {
  int i = 0;
  while (INODE_STATE(i)) {
    ++i;
    assert(i < TOTAL_INODE_NUM);
  }
  return i;
}

int alloc_inode() {
  int available = available_inode();
  TOGGLE_INODE_STATE(available);
  inodes[available].refs = 0;
  inodes[available].mode = 0; // Default mode set to no permissions.
  inodes[available].size = 0;
  inodes[available].block = 0;
  inodes[available].next = NULL;
  return available;
}

void free_inode(int inum) {
  assert(!INODE_STATE(inum));
  TOGGLE_INODE_STATE(inum);
}

int grow_inode(inode_t *node, int size) {
  assert(node->size + size <= BLOCK_SIZE); // TODO: allow big file allocations.
  node->size += size;
  return (node - inodes);
}

int shrink_inode(inode_t *node, int size) {
  assert(node->size - size >= 0);
  node->size -= size;
  return (node - inodes);
}
