.section .text.boot
// Add before any usage
.global enable_mmu
enable_mmu:
    bl mmu_init
    dsb sy
    isb
    ret

.global _start
// Add before jumping to C code

_start:
    // Set stack pointer (QEMU virt machine RAM starts at 0x40000000)
    ldr x0, =0x40000000
    add x0, x0, #0x100000
    mov sp, x0
    // In boot.S after stack initialization
//    enable_mmu:
    // 1. Create page tables first (if not already done)
    // 2. Map AHCI region
  //  ldr x0, =0x30000000         // Physical address
    //ldr x1, =0x30000000         // Virtual address
//    mov x2, #0x10000            // 64KB size
//    mov x3, #0x704              // Device-nGnRnE attributes
//    bl  mmu_map_region
    
    // 3. Continue with MMU enable  // Load page table base
  //     msr TTBR0_EL1, x25    // Set TTBR0_EL1
//       isb                   // Instruction barrier
//       mrs x0, SCTLR_EL1
//       orr x0, x0, #1 << 0   // Enable MMU
//       msr SCTLR_EL1, x0
//       isb

    // Clear BSS section
    ldr x0, =__bss_start
    ldr x1, =__bss_end
    bss_clear_loop:
        cmp x0, x1
        b.ge bss_clear_done
        str xzr, [x0], #8
        b bss_clear_loop
    bss_clear_done:
    // Jump to main
    bl kmain

    // Halt if main returns
    b .
