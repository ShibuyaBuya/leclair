#include "mem.h"
#include "../lib/stdint.h"
#include "../drivers/uart.h"
static uint64_t ram_start;
static uint64_t ram_end;
static struct mem_region reserved_regions[MAX_RESERVED_REGIONS];
static int reserved_count = 0;

void reserve_region(uint64_t base, uint64_t size) {
    if (reserved_count >= MAX_RESERVED_REGIONS) {
        uart_puts("Cannot reserve more regions: max reached\n");
        return;
    }

    // Simple coalescing check (optional)
    if (reserved_count > 0) {
        struct mem_region *last = &reserved_regions[reserved_count-1];
        if (last->base + last->size == base) {
            last->size += size;
            return;
        }
    }

    reserved_regions[reserved_count++] = (struct mem_region){
        .base = base,
        .size = size
    };
}
void init_memory_manager(uint64_t base, uint64_t size) {
    ram_start = base;
    ram_end = base + size;
    reserved_count = 0;

    // Mark MMIO as reserved (0x80000000-0x80200000)
    reserve_region(0x80000000, 0x200000);
}
void print_reserved_regions(void) {
    uart_puts("Reserved memory regions:\n");
    for (int i = 0; i < reserved_count; i++) {
        uart_puts(" - 0x");
        uart_puthex64(reserved_regions[i].base);
        uart_puts(" - 0x");
        uart_puthex64(reserved_regions[i].base + reserved_regions[i].size);
     //   uart_puts(" (");
      //  uart_putdec64(reserved_regions[i].size / 1024);
//        uart_puts(" KB)\n");
    }
}
