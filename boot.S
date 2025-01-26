.section .text.boot
.global _start

// Page table constants
.equ PTE_VALID,          (1 << 0)
.equ PTE_TABLE,          (1 << 1)
.equ PTE_BLOCK,          (0 << 1)
.equ PTE_AF,             (1 << 10)
.equ PTE_SH_INNER,       (3 << 8)
.equ PTE_UXN,            (1 << 54)
.equ PTE_PXN,            (1 << 53)

// Memory attributes
.equ MT_DEVICE_nGnRnE,   0x0
.equ MT_NORMAL_NC,       0x1
.equ MT_NORMAL,          0x2

// 4KB-aligned page tables
.section .bss
.align 12
tt_l1:
    .space 4096, 0
.align 12
tt_l2:
    .space 4096, 0

.section .text.boot
.global _start
_start:

    // Set stack pointer
    // In your kernel's early boot code (e.g., boot.S)
    ldr x1, =dtb_ptr  // Assume you detected RAM size earlier
    str x0, [x1]
    ldr x0, =0x40000000
    add x0, x0, #0x100000
    and x0, x0, #~0xF
    mov sp, x0

    // Initialize MMU first
    bl mmu_init
    // Clear BSS
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

mmu_init:
    // Create identity mapping for kernel code/data
    ldr x0, =tt_l1            // L1 table address
    ldr x1, =tt_l2            // L2 table address

    // Map first 2MB as Normal Memory
    mov x2, #(PTE_VALID | PTE_AF | PTE_SH_INNER | (MT_NORMAL << 2) | PTE_BLOCK)
//    mov x2, #0x40000000
    orr x2, x2, #0x40000000  // Add execute never bits
//    orr x2, x2, #(PTE_BLOCK | PTE_AF | PTE_SH_INNER | PTE_VALID | (MT_NORMAL << 2))
    str x2, [x1]                     // Map 0x0-0x200000 in L2
    // In boot.S's mmu_init section

    // Link L2 to L1
    ldr x3, =tt_l2
    orr x3, x3, #PTE_TABLE | PTE_VALID
    str x3, [x0]
    // In boot.S's mmu_init section
    // In boot.S's mmu_init section

	// Map AHCI controller (2MB block)
	movz x0, #0x8000, lsl #16        // Physical address
	movz x1, #0x8000, lsl #16        // Virtual address
	mov x2, #0x200000              // 2MB size (standard block size)
	mov x3, #MT_DEVICE_nGnRnE      // Memory type
	bl mmu_map_region
    // Map UART0 (0x09000000) as Device memory
	// Map UART0 (0x09000000) as Device memory
	ldr x1, =tt_l2
	add x1, x1, #(0x09000000 >> 18) & 0x1FF8  // L2 offset
	movz x8, #0x0900, lsl #16    // x8 = 0x09000000
	and x8, x8, #0xFFF00000      // Mask to 0x09000000
	mov x2, #(PTE_VALID | PTE_AF | (MT_DEVICE_nGnRnE << 2) | PTE_BLOCK)
	orr x2, x2, x8               // Combine address and flags
	str x2, [x1]

	// Configure MAIR
	mov x0, #0x00                // Attr0 = Device-nGnRnE
	movz x9, #0x4400             // Attr1 = Normal-NC (0x44 << 8)
	orr x0, x0, x9
	movk x9, #0xFF, lsl #16      // Attr2 = Normal-WB (0xFF << 16)
	orr x0, x0, x9
	msr mair_el1, x0
//    ldr x1, =tt_l2
  //  add x1, x1, #(0x09000000 >> 18) & 0x1FF8  // L2 offset for 0x09000000
//    mov x2, #(PTE_VALID | PTE_AF | (MT_DEVICE_nGnRnE << 2) | PTE_BLOCK)
//    orr x2, x2, #(0x09000000 & 0xFFF00000)  // Invalid immediate orr x2, x2, #(0x09000000 & 0xFFF00000)
//    str x2, [x1]

    // Configure MAIR
  //  mov x0, #0x00                       // Attr0 = Device-nGnRnE
//    orr x0, x0, #0x44 << 8              // Attr1 = Normal-NC
//    orr x0, x0, #0xFF << 16             // Attr2 = Normal-WB
//    msr mair_el1, x0

    // Configure TCR
    mov x0, #(1 << 30)                  // TG1 = 4KB
    orr x0, x0, #(3 << 12)              // SH0 = Inner
    orr x0, x0, #(1 << 10)              // ORGN0 = WBWA
    orr x0, x0, #(1 << 8)               // IRGN0 = WBWA
    orr x0, x0, #((64 - 48) << 0)       // T0SZ = 16
    orr x0, x0, #((64 - 48) << 16)      // T1SZ = 16
    orr x0, x0, #(1 << 22)              // A1 = TTBR0.ASID
    orr x0, x0, #(1 << 36)              // AS = 16-bit ASID
    msr tcr_el1, x0

    // Set TTBR0
    ldr x0, =tt_l1
    msr ttbr0_el1, x0

    // Enable MMU
    mrs x0, sctlr_el1
    orr x0, x0, #(1 << 0)               // M = Enable MMU
    orr x0, x0, #(1 << 2)               // C = Data cache
    orr x0, x0, #(1 << 12)              // I = Instruction cache
    msr sctlr_el1, x0

    // Synchronization
    dsb sy
    isb

    ret

// Helper function to map physical to virtual address
// x0 = physical address, x1 = virtual address, x2 = size, x3 = memory type
    // Implementation depends on your mapping strategy
    // (Can be extended based on your needs)
	// Add to boot.S
// Parameters:
// x0: Physical address (must be 2MB aligned)
// x1: Virtual address (must be 2MB aligned)
// x2: Size (must be multiple of 2MB)
// x3: Memory type (0 = Device, 1 = Normal-NC, 2 = Normal-WB)
mmu_map_region:
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!

1:  cmp x2, #0
    b.le 3f                      // Exit if size <= 0

    // Calculate L1 index (bits 39:31 of VA)
    lsr x4, x1, #39              // x4 = L1 index
    and x4, x4, #0x1FF

    // Calculate L2 index (bits 30:21 of VA)
    lsr x5, x1, #30
    and x5, x5, #0x1FF

    // Get L1 entry
    adrp x6, tt_l1
    add x6, x6, :lo12:tt_l1
    add x6, x6, x4, lsl #3       // x6 = &tt_l1[L1 index]

    ldr x7, [x6]
    tst x7, #PTE_VALID
    b.ne 2f                      // Skip if L2 table exists

    // Create new L2 table entry
    adrp x8, tt_l2               // Use predefined L2 table
    add x8, x8, :lo12:tt_l2
    orr x8, x8, #PTE_TABLE | PTE_VALID
    str x8, [x6]

2:  // Update L2 entry
    and x7, x7, #0xFFFFFFFFF000  // Get L2 table base
    add x7, x7, x5, lsl #3       // x7 = &L2_table[L2 index]
// In mmu_map_region function

	// Build block descriptor
	mov x8, x0                    // Physical address
	and x8, x8, #0xFFFFFFFFE00000 // 2MB aligned

	// Set flags (PTE_BLOCK | PTE_AF | PTE_SH_INNER | PTE_VALID)
	movz x9, #(PTE_BLOCK | PTE_AF | PTE_SH_INNER)  // 0x644 (LSB)
	movk x9, #(PTE_VALID), lsl #16                 // 0x10000 (MSB)
	orr x8, x8, x9

// Set memory attributes
	movz x9, #(MT_DEVICE_nGnRnE << 2)             // 0x0 << 2 = 0x0
	movz x10, #(MT_NORMAL_NC << 2)                // 0x1 << 2 = 0x4
	movz x11, #(MT_NORMAL << 2)                   // 0x2 << 2 = 0x8
    // Build block descriptor
//    mov x8, x0                   // Physical address
 //   and x8, x8, #0xFFFFFFFFE00000 // 2MB aligned
  //  orr x8, x8, #PTE_BLOCK | PTE_AF | PTE_SH_INNER | PTE_VALID

    // Set memory attributes
   // cmp x3, #MT_DEVICE_nGnRnE
   // orr x9, xzr, #(MT_DEVICE_nGnRnE << 2)
   // orr x10, xzr, #(MT_NORMAL_NC << 2)
   // orr x11, xzr, #(MT_NORMAL << 2)
   	 cmp x3, #0
	 csel x12, x9, x10, eq
   	 cmp x3, #2
    	 csel x12, x11, x12, eq
    	 orr x8, x8, x12

    // Set execute-never for device memory
    cmp x3, #MT_DEVICE_nGnRnE
    b.ne 4f
    orr x8, x8, #PTE_UXN | PTE_PXN

4:  str x8, [x7]                 // Write L2 entry

    // Advance addresses
    add x0, x0, #0x200000        // += 2MB
    add x1, x1, #0x200000        // += 2MB
    sub x2, x2, #0x200000        // -= 2MB
    b 1b

3:  ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ret
.global dtb_ptr
dtb_ptr: .quad 0
