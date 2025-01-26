#include "vfs.h"
#include "ext2.h"
#include "../drivers/uart.h"
static struct filesystem *current_fs;

struct file* vfs_create_file(void) {
    static struct file f;
    f.pos = 0;
    f.size = 0;
    f.data = 0;
    return &f;
}

int vfs_mount(const char *fstype, uint64_t partition) {
    if(strcmp(fstype, "ext2") == 0) {
        if(ext2_init(partition) == 0) {
            current_fs = &ext2_operations;
            uart_puts("EXT2 mounted successfully\n");
            return 0;
        }
    }
    uart_puts("Failed to mount filesystem\n");
    return 1;
}

struct file* vfs_open(const char *path) {
    if(!current_fs) {
        uart_puts("No filesystem mounted!\n");
        return 0;
    }
    return current_fs->open(path);
}

size_t vfs_read(struct file *f, void *buf, size_t count) {
    size_t remaining = f->size - f->pos;
    if(count > remaining) count = remaining;

    memcpy(buf, f->data + f->pos, count);
    f->pos += count;
    return count;
}


/*#include "vfs.h"
#include "ext2.h"

// Implement create function
struct file* vfs_create_file(void) {
    static struct file f;
    f.pos = 0;
    return &f;
}

//struct file* vfs_open(const char* path) {
    // Implementation}

//void vfs_mount(const char* fstype, int partition) {
    // Implementation
//}
static struct filesystem *current_fs;

/*struct file {
    void *data;
    size_t size;
    size_t pos;
};

int vfs_mount(const char *fstype, uint64_t partition) {
    if(strcmp(fstype, "ext2") == 0) {
        ext2_init(partition);
        current_fs = &ext2_operations;
	return 0;
    }
    return 1;
}

struct file *vfs_open(const char *path) {
    return current_fs->open(path);
}

size_t vfs_read(struct file *f, void *buf, size_t count) {
    size_t remaining = f->size - f->pos;
    if(count > remaining) count = remaining;
    
    memcpy(buf, f->data + f->pos, count);
    f->pos += count;
    return count;
}
*/
