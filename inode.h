// Inode manipulation routines.
//
// Feel free to use as inspiration. Provided as-is.

// based on cs3650 starter code
#ifndef INODE_H
#define INODE_H

#include "blocks.h"

#define READ_BIT  0b0001
#define WRITE_BIT 0b0010
#define EXEC_BIT  0b0100
#define DIR_BIT   0b1000

#define ROOT_DIR_INODE_INDEX 0

typedef struct inode {
  // Number of files referenced to this inode.
  int refs;
  // The mode the file is on.
  int mode;
  // The size of the data stored in the block the inode is referencing on in bytes.
  int size;
  // The block index the inode is referencing on.
  int block;
  // The next inode, reserved for big file support.
  int next;
} inode_t;

/**
 * Print the information of an inode.
 *
 * @param node The node to print information from.
 */
void print_inode(inode_t *node);
/**
 * Get the instance of a node at inum.
 *
 * @param inum The index of the node to get instance from.
 * @return The inode instance.
 */
inode_t *get_inode(int inum);
/**
 * Allocate an inode.
 *
 * @return The inum of the allocated inode.
 */
int alloc_inode();
/**
 * Free an inode.
 *
 * @param inum The inum of the inode to be freed.
 */
void free_inode(int inum);
/**
 * Increase the size of the data the inode is referencing on.
 *
 * @param node The node that the inode is referencing on.
 * @param size The size to shink.
 * @return The inum of the shrinked inode.
 */
int grow_inode(inode_t *node, int size);
/**
 * Shrink the size of the data the inode is referencingn on.
 *
 * @param node The inode to shink size.
 * @param size The size to shrink.
 * @return The inum of the shrinked inode.
 */
int shrink_inode(inode_t *node, int size);

#endif
