// Disk storage abstracttion.
//
// Feel free to use as inspiration. Provided as-is.

// based on cs3650 starter code

#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "slist.h"

/**
 * Initialize the storage system.
 *
 * @param image_path The path of the image file.
 */
void storage_init(const char *image_path);
/**
 * Get file or directory statistics.
 *
 * @param path The path to the file or directory.
 * @param st Pointer to a stat structure to be filled.
 * @return 0 on success, negative error code on failure.
 */
int storage_stat(const char *path, struct stat *st);
/**
 * Read data from a file.
 *
 * @param path The path to the file.
 * @param buf Buffer to store the read data.
 * @param size Number of bytes to read.
 * @param offset Offset in the file to start reading from.
 * @return Number of bytes read on success, negative error code on failure.
 */
int storage_read(const char *path, char *buf, size_t size, off_t offset);
/**
 * Write data to a file.
 *
 * @param path The path to the file.
 * @param buf Buffer containing the data to write.
 * @param size Number of bytes to write.
 * @param offset Offset in the file to start writing to.
 * @return Number of bytes written on success, negative error code on failure.
 */
int storage_write(const char *path, const char *buf, size_t size, off_t offset);
/**
 * Truncate a file to a specified size.
 *
 * @param path The path to the file.
 * @param size The size to truncate the file to.
 * @return 0 on success, negative error code on failure.
 */
int storage_truncate(const char *path, off_t size);
/**
 * Create a new file or directory.
 *
 * @param path The path to the new file or directory.
 * @param mode The mode (permissions and type) of the new file or directory.
 * @return 0 on success, negative error code on failure.
 */
int storage_mknod(const char *path, int mode);
/**
 * Remove a file.
 *
 * @param path The path to the file to be removed.
 * @return 0 on success, negative error code on failure.
 */
int storage_unlink(const char *path);
/**
 * Create a hard link from one file to another.
 *
 * @param from The source file path.
 * @param to The destination file path.
 * @return 0 on success, negative error code on failure.
 */
int storage_link(const char *from, const char *to);
/**
 * Rename a file or directory.
 *
 * @param from The current path of the file or directory.
 * @param to The new path of the file or directory.
 * @return 0 on success, negative error code on failure.
 */
int storage_rename(const char *from, const char *to);
/**
 * Set the access and modification times of a file.
 *
 * @param path The path to the file.
 * @param ts An array of two timespec structures: ts[0] for access time, ts[1] for modification time.
 * @return 0 on success, negative error code on failure.
 */
int storage_set_time(const char *path, const struct timespec ts[2]);
/**
 * List the contents of a directory.
 *
 * @param path The path to the directory.
 * @return A singly linked list of directory entries on success, NULL on failure.
 */
slist_t *storage_list(const char *path);

#endif
