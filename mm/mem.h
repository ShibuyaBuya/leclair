#ifndef _MEM_H
#define _MEM_H

#include "../lib/stdint.h"
#define MAX_RESERVED_REGIONS 32

struct mem_region {
    uint64_t base;
    uint64_t size;
};

void init_memory_manager(uint64_t base, uint64_t size);

void reserve_region(uint64_t base, uint64_t size);

#endif
