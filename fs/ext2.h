#ifndef _EXT2_H
#define _EXT2_H

#include "../lib/stdint.h"  // Add this
#include "vfs.h"      // Add this
extern struct filesystem ext2_operations;
struct ext2_superblock {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t reserved_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t first_data_block;
    uint32_t log_block_size;
    // ... add other necessary fields ...
    uint16_t magic;
} __attribute__((packed));
/*struct ext2_superblock {
    uint32_t inodes_count;7
    uint32_t blocks_count;
    uint16_t magic;
    // Add other superblock fields
} __attribute__((packed));*/
struct ext2_inode {
    uint16_t mode;
    uint32_t size;
    uint32_t block[15];
    // Add other inode fields
};
int ext2_init(uint64_t partition_offset);
struct file *ext2_open(const char *path);

#endif
