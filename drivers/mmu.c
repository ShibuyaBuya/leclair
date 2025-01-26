#include "mmu.h"
#include "uart.h"

// 4KB granule, 48-bit VA
#define TCR_CONFIG (TCR_TG1_4K | TCR_SH0_INNER | TCR_ORGN0_WBWA | TCR_IRGN0_WBWA | \
                   TCR_T0SZ(16) | TCR_T1SZ(16) | TCR_A1 | TCR_ASID16)

// MAIR indices
/*enum {
    MT_DEVICE_nGnRnE = 0,
    MT_NORMAL_NC = 1,
    MT_NORMAL = 2
};*/
// Remove this enum:
/*
enum {
    MT_DEVICE_nGnRnE = 0,
    MT_NORMAL_NC = 1,
    MT_NORMAL = 2
};
*/

// Use the macros directly instead:
static uint64_t attr_index[3] = {
    MT_DEVICE_nGnRnE << 2,
    MT_NORMAL_NC << 2,
    MT_NORMAL << 2
};

// Page table structure (aligned to 4KB)
__attribute__((aligned(4096))) static uint64_t tt_l1[512];
__attribute__((aligned(4096))) static uint64_t tt_l2[512][512];

static void set_pte(uint64_t *pte, uint64_t phys, uint64_t flags) {
    *pte = phys | flags;
}

void mmu_map_range(uint64_t phys, uint64_t virt, uint64_t size, int mem_type) {
    static uint64_t attr_index[3] = {
        MT_DEVICE_nGnRnE << 2,
        MT_NORMAL_NC << 2,
        MT_NORMAL << 2
    };
    
    uint64_t flags = PTE_VALID | PTE_AF | PTE_SH_INNER | 
                   attr_index[mem_type] | PTE_BLOCK;
                   
    if(mem_type == MT_DEVICE_nGnRnE) {
        flags |= PTE_PXN | PTE_UXN;
    }

    while(size > 0) {
        uint64_t block_size = 1UL << 21; // 2MB blocks
        if((phys & (block_size - 1)) || size < block_size)
            block_size = 1UL << 12; // 4KB pages

        uint64_t l1_idx = (virt >> 39) & 0x1FF;
        uint64_t l2_idx = (virt >> 30) & 0x1FF;
        
        if(!(tt_l1[l1_idx] & PTE_VALID)) {
            tt_l1[l1_idx] = (uint64_t)&tt_l2[l1_idx][0] | PTE_TABLE | PTE_VALID;
        }
        
        set_pte(&tt_l2[l1_idx][l2_idx], phys, flags | 
              (block_size == (1 << 21) ? PTE_BLOCK : 0));
        
        phys += block_size;
        virt += block_size;
        size -= block_size;
    }
}

void mmu_init(void) {
	mmu_map_range(0x80000, 0x80000, 0x100000, MT_NORMAL); // Example: 1MB at 0x80000
    // Configure MAIR
	mmu_map_range((uint64_t)&tt_l1, (uint64_t)&tt_l1, 4096, MT_NORMAL);
    for(int i = 0; i < 512; i++) {
        mmu_map_range((uint64_t)&tt_l2[i], (uint64_t)&tt_l2[i], 4096, MT_NORMAL);
    }
    
    // 3. Map UART (if used)
    mmu_map_range(0x09000000, 0x09000000, 4096, MT_DEVICE_nGnRnE);
    uint64_t mair = (0x00 << (MT_DEVICE_nGnRnE * 8)) |  // Device-nGnRnE
                    (0x44 << (MT_NORMAL_NC * 8)) |      // Normal Non-Cacheable
                    (0xFF << (MT_NORMAL * 8));          // Normal Write-Back
    asm volatile("msr mair_el1, %0" : : "r"(mair));
    
    // Configure TCR
    asm volatile("msr tcr_el1, %0" : : "r"(TCR_CONFIG));
    
    // Set TTBR0
    asm volatile("msr ttbr0_el1, %0" : : "r"(tt_l1));
    
    // Enable MMU
    uint64_t sctlr;
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= SCTLR_M | SCTLR_C | SCTLR_I;
    asm volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    asm volatile("isb");
}
