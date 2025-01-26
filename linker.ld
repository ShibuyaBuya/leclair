ENTRY(_start)
MEMORY {
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 6400M
	MMIO (rw) : ORIGIN = 0x80000000, LENGTH = 2M
}
SECTIONS {
    . = 0x40000000;     /* QEMU 'virt' machine load address */
    
    .text : {
	. = ALIGN(8);
        *(.text.boot)
        *(.text .text.*)
	. = ALIGN(8);
    } > RAM
    
    .rodata : {
	. = ALIGN(8);
        *(.rodata .rodata.*)
	. = ALIGN(8);
    } > RAM
    
    .data : {
	. = ALIGN(8);
        *(.data .data.*)
	. = ALIGN(8);
    } > RAM
    
    .bss (NOLOAD) : {
	    . = ALIGN(8);
	    __bss_start = .;
	    *(.bss*)
	    . = ALIGN(8);
	    __bss_end = .;
	    . = ALIGN(4096);
	    tt_l1 = .;
	    . += 4096;
	    tt_l2 = .;
	    . += 4096;
	} > RAM
    /* linker.ld */
    /*MMIO (rw) : ORIGIN = 0x80000000, LENGTH = 2M*/
    
    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}
