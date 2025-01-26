#include "ext2.h"
#include "ahci.h"
#include <stdint.h>
#include "../drivers/uart.h"
#include "vfs.h"  // Add this
#include "../lib/string.h"


// Rest of ext2.c implementation remains the same
// Add vfs_create_file declaration
struct file* vfs_create_file(void);
struct filesystem ext2_operations = {
    .open = ext2_open
};

static struct ext2_superblock sb;
static void* ahci_alloc_mem(size_t size) {
    static _Alignas(4096) uint8_t mem_pool[2 * 1024 * 1024];
    static uintptr_t pool_ptr = 0;
    
    void* ret = UINT_TO_PTR(PTR_TO_UINT(mem_pool) + pool_ptr);
    pool_ptr += size;
    
    return ret;
}
int ext2_init(uint64_t partition_offset) {
    uint8_t buffer[1024];
    
    // Read superblock (sector 2)
    if(ahci_read(0, 2, 2, buffer) != 0) {
        uart_puts("AHCI read failed\n");
        return -1;
    }
    
    memcpy(&sb, buffer + 1024 % 512, sizeof(sb));
    
    uart_puts("EXT2 magic: 0x");
    uart_puthex16(sb.magic);
    uart_puts("\n");
    
    return (sb.magic == 0xEF53) ? 0 : -1;
}

struct file *ext2_open(const char *path) {
    // Simplified inode read
    uint8_t buffer[512];
    ahci_read(0, 2, 1, buffer); // Read root inode
    
    struct file *f = vfs_create_file();
    f->size = 4096; // Temp value
    f->data = ahci_alloc_mem(4096); // Allocate file buffer
    ahci_read(0, 2, 8, f->data); // Read file content
    
    return f;
}
/*int ext2_init(uint64_t partition_offset) {
    uart_puts("Reading EXT2 superblock...\n");
    read_sectors((partition_offset + 1024) / 512, 2, &sb);
    
    uart_puts("Magic: ");
    uart_puthex16(sb.magic);
    uart_puts("\n");
    
    if(sb.magic != 0xEF53) {
        uart_puts("Invalid EXT2 filesystem!\n");
        return -1;
    }
    return 0;
}
struct file *ext2_open(const char *path) {
    uart_puts("Opening: ");
    uart_puts(path);
    uart_puts("\n");
    uint32_t block_size = 1024 << sb.log_block_size;
    uint32_t inode_table_block = sb.first_data_block + 1;
    struct ext2_inode inode;
    read_sectors(inode_table_block * (block_size/512), block_size/512, &inode);
    struct file *f = vfs_create_file();
    f->size = inode.size;
    f->data = (void*)((uint64_t)inode.block[0] * 512);
    
  //  uart_puts("File created at: ");
    //uart_puthex64((uint64_t)f->data);
    //uart_puts("\n");
    
    return f;
}
/*struct file *ext2_open(const char *path) {
    (void)path;  // Silence unused parameter warning
    
    struct ext2_inode inode;
    read_sectors(2, 1, &inode);
    
    struct file *f = vfs_create_file();
    f->size = inode.size;
    f->data = (void*)(inode.block[0] * 512);  // Fix pointer arithmetic
    
    return f;
}
*/
