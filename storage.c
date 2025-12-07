#include "storage.h"

void storage_init(const char *path) {
  // TODO
}

int storage_stat(const char *path, struct stat *st) {
  return -1; // TODO
}

int storage_read(const char *path, char *buf, size_t size, off_t offset) {
  return -1; // TODO
}

int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
  return -1; // TODO
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

