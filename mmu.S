// In mmu.S
.global mmu_map_region
mmu_map_region:
    // x0 = phys, x1 = virt, x2 = size, x3 = attributes
    lsr x2, x2, #12            // Convert size to page count
1:
    orr x4, x0, x3             // Combine address and attributes
    adrp x5, TTBR0_EL1
    add x5, x5, x1, lsr #(12 - 3)  // Calculate entry offset
    str x4, [x5]               // Store entry
    add x0, x0, #0x1000        // Next phys page
    add x1, x1, #0x1000        // Next virt page
    subs x2, x2, #1
    b.ne 1b
    ret
