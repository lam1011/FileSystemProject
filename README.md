# NUFS: A FUSE File System in C

A user-space file system written in C with FUSE, built with a partner for CS 3650 (Computer Systems) at Northeastern University, Fall 2025.

NUFS stores an entire file system, including files and nested directories, inside a single 1 MB disk image. Once mounted, the image behaves like a normal directory: you can create, read, write, move, and link files with standard tools like `ls`, `cat`, `mkdir`, and `mv`.

## Features

- Create files and nested directories
- Read and write at arbitrary offsets
- Truncate files
- Hard links with reference counting
- Rename and move files between directories
- Directory listing

## Design

The disk image is memory-mapped with `mmap` and divided into 256 blocks of 4 KB each.

| Block | Contents |
| --- | --- |
| 0 | Block bitmap and inode bitmap |
| 1 | Inode table |
| 2+ | File and directory data |

- **Allocation.** Free inodes and data blocks are tracked with bitmaps. Allocation finds the first free bit and marks it used.
- **Inodes.** Each inode stores a file's mode, size, link count, and the data block it points to.
- **Directories.** A directory is a data block holding fixed-size entries, each mapping a name to an inode number.
- **Path resolution.** A path like `/foo/bar/file.txt` is split on `/` and resolved one directory at a time, starting from the root inode.
- **FUSE layer.** `nufs.c` translates FUSE callbacks (`getattr`, `read`, `write`, `mknod`, `link`, `rename`, and others) into calls to the storage layer in `storage.c`.

## Building and running

Requires Linux with FUSE 2 (`libfuse-dev`).

```
make            # build
make mount      # mount data.nufs at ./mnt
make unmount    # unmount
make test       # run the test suite
```

## Limitations

- Each file is stored in a single 4 KB block, so files larger than 4 KB are not supported.
- `rmdir` and `chmod` are not implemented.
- Timestamps are not stored.

## Acknowledgments

Built on starter code from CS 3650 at Northeastern University. The block and bitmap helpers in `helpers/` were provided by the course.
