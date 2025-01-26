#ifndef _MMU_H
#define _MMU_H

#include <stdint.h>

// Memory attributes
#define MT_DEVICE_nGnRnE 0x0
#define MT_NORMAL_NC      0x1
#define MT_NORMAL         0x2

// Page table entry flags
#define PTE_VALID         (1 << 0)
#define PTE_TABLE         (1 << 1)
#define PTE_BLOCK         (0 << 1)
#define PTE_ACCESS        (1 << 10)
#define PTE_SH_INNER      (3 << 8)
#define PTE_AF            (1 << 10)
#define PTE_NG            (1 << 11)
#define PTE_PXN           (1UL << 53)
#define PTE_UXN           (1UL << 54)

// TCR Register Flags
#define TCR_TG1_4K        (1 << 30)
#define TCR_SH0_INNER     (3 << 12)
#define TCR_ORGN0_WBWA    (1 << 10)
#define TCR_IRGN0_WBWA    (1 << 8)
#define TCR_T0SZ(x)       ((64 - (x)) << 0)
#define TCR_T1SZ(x)       ((64 - (x)) << 16)
#define TCR_A1            (1 << 22)
#define TCR_ASID16        (1 << 36)

// SCTLR Register Flags
#define SCTLR_M           (1 << 0)  // MMU enable
#define SCTLR_C           (1 << 2)  // Data cache
#define SCTLR_I           (1 << 12) // Instruction cache

void mmu_init(void);
void mmu_map_range(uint64_t phys, uint64_t virt, uint64_t size, int mem_type);

#endif
