#ifndef _VFS_H
#define _VFS_H

#include "../lib/stdint.h"

struct file {
    void *data;
    size_t size;
    size_t pos;
};

// Add create function declaration
struct file* vfs_create_file(void);
void vfs_mount(const char *fstype, uint64_t partition);
struct file *vfs_open(const char *path);
size_t vfs_read(struct file *f, void *buf, size_t count);

#endif
