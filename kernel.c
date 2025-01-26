#include "drivers/uart.h"
#include "drivers/ahci.h"
#include "fs/vfs.h"
#include "drivers/mmu.h"
#include "mm/mem.h"
#include <stdint.h>
// In kernel.c (before ahci_init())
/*void early_mmu_init() {
    mmu_map_region(0x30000000, 0x30000000, 0x10000, 0x704);
}*/
// Early initialization
/*extern int enable_mmu();
void kernel_early() {
    // Map kernel code/data (adjust addresses to match your linker script)
    mmu_map_range(0x40000000, 0x40000000, 0x100000, MT_NORMAL); // 1MB kernel
    
    // Map AHCI registers
    mmu_map_range(0x30000000, 0x30000000, 0x10000, MT_DEVICE_nGnRnE);
    
    enable_mmu();  // Call assembly wrapper
}*/
//alignas(4096) uint8_t buffer[4096];
// kernel.c

extern void *dtb_ptr; // Defined in boot.S

uint32_t ntohl(uint32_t netlong) {
    return ((netlong & 0xFF000000) >> 24) |
           ((netlong & 0x00FF0000) >> 8)  |
           ((netlong & 0x0000FF00) << 8)  |
           ((netlong & 0x000000FF) << 24);
}
// Minimal DTB parser to find RAM size
uint64_t get_ram_size(void *dtb) {
    // DTB header structure
    struct fdt_header {
        uint32_t magic;
        uint32_t totalsize;
        uint32_t off_dt_struct;
        uint32_t off_dt_strings;
        uint32_t off_mem_rsvmap;
        uint32_t version;
        uint32_t last_comp_version;
        uint32_t boot_cpuid_phys;
        uint32_t size_dt_strings;
        uint32_t size_dt_struct;
    };

    struct fdt_header *header = (struct fdt_header*)dtb;

    // Validate DTB magic
    if (header->magic != 0xd00dfeed) {
        uart_puts("Invalid DTB magic!\n");
        return 0;
    }

    // Traverse nodes to find "memory" node
    char *node = (char*)dtb + ntohl(header->off_dt_struct);
    while (1) {
        uint32_t token = ntohl(*(uint32_t*)node);
        node += 4;

        if (token == 0x00000001) { // FDT_BEGIN_NODE
            char *name = node;
            if (strcmp(name, "memory") == 0) {
                // Found memory node - look for "reg" property
                node += (strlen(name) + 4) & ~3; // Align to 4 bytes
                break;
            }
            node += (strlen(name) + 1 + 3) & ~3; // Skip node name
        }
    }

    // Extract "reg" property (address and size pairs)
    while (1) {
        uint32_t token = ntohl(*(uint32_t*)node);
        node += 4;

        if (token == 0x00000003) { // FDT_PROP
            uint32_t len = ntohl(*(uint32_t*)node);
            node += 4;
            char *name = (char*)dtb + ntohl(header->off_dt_strings) + ntohl(*(uint32_t*)node);
            node += 4;

            if (strcmp(name, "reg") == 0) {
                // Read first address/size pair (QEMU virt uses 2 cells)
                uint64_t addr = ((uint64_t)ntohl(*(uint32_t*)node) << 32) | ntohl(*(uint32_t*)(node + 4));
                uint64_t size = ((uint64_t)ntohl(*(uint32_t*)(node + 8)) << 32) | ntohl(*(uint32_t*)(node + 12));
                return size;
            }
            node += (len + 3) & ~3; // Skip property value
        }
    }

    return 0;
}

void kmain() {
//    kernel_early();
    uart_init();
    uint64_t ram_size = get_ram_size(dtb_ptr);
    uart_puts("Detected RAM size: ");
    uart_puthex64(ram_size);
    uart_puts("\n");
    init_memory_manager(0x40000000, ram_size);
    uart_puts("Stage 1: UART initialized\n");
    // In kernel.c (before ahci_init())
   // early_mmu_init();
    ahci_init();
    
    uart_puts("Stage 2: AHCI initialized\n");
    
    if(vfs_mount("ext2", 0) != 0) {
        uart_puts("ERROR: Filesystem mount failed\n");
        while(1);
    }
    uart_puts("Stage 3: Filesystem mounted\n");
    
    struct file *cli = vfs_open("/bin/lcl");
    if(!cli) {
        uart_puts("ERROR: CLI not found\n");
        while(1);
    }
    uart_puts("Stage 4: CLI loa ded\n");
    
    void (*entry)(void) = (void (*)(void))cli->data;
    entry();
    while(1);
}
/*void main() {
    uart_init();
    uart_puts("\nBooting Custom Kernel...\n");
    
    ahci_init();
    vfs_mount("ext2", 0);
    
    // Load and execute CLI
    struct file *cli = vfs_open("/bin/lcl");
    if(cli) {
        void (*entry)(void) = (void (*)(void))cli->data;
        entry();
    } else {
        uart_puts("Failed to load CLI\n");
    }
    
    while(1);
}*/
