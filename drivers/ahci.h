#ifndef _AHCI_H
#define _AHCI_H

#include "../lib/stdint.h"

//#define AHCI_BASE 0x30000000 // Correct base for QEMU virt machine
#define AHCI_BASE       0x80000000  // QEMU virt machine AHCI base
#define AHCI_MAX_PORTS  32
// ahci.h
#define AHCI_PHYS_BASE 0x80000000  // QEMU's AHCI physical address
#define AHCI_VIRT_BASE 0x80000000  // Identity-mapped virtual address
extern struct hba_mem *hba;
// HBA Memory Registers
struct hba_mem {
    uint32_t cap;       // 0x00: Host capability
    uint32_t ghc;       // 0x04: Global host control
    uint32_t is;        // 0x08: Interrupt status
    uint32_t pi;        // 0x0C: Ports implemented
    uint32_t vs;        // 0x10: Version
    uint8_t  rsvd[0xA0-0x14];
    uint8_t  ncs;       // 0xA0: Number of command slots
    uint8_t  ccc_ctl;   // 0xA1: Command completion coalescing control
    uint16_t ccc_ports; // 0xA2: CCC ports
    uint8_t  rsvd2[0x100-0xA4];
    struct hba_port {
        uint32_t clb;       // 0x00: Command list base
        uint32_t clbu;      // 0x04: Command list base upper
        uint32_t fb;        // 0x08: FIS base
        uint32_t fbu;       // 0x0C: FIS base upper
        uint32_t is;        // 0x10: Interrupt status
        uint32_t ie;        // 0x14: Interrupt enable
        uint32_t cmd;       // 0x18: Command
        uint32_t rsvd;      // 0x1C: Reserved
        uint32_t tfd;       // 0x20: Task file data
        uint32_t sig;       // 0x24: Signature
        uint32_t ssts;      // 0x28: SATA status
        uint32_t sctl;      // 0x2C: SATA control
        uint32_t serr;      // 0x30: SATA error
        uint32_t sact;      // 0x34: SATA active
        uint32_t ci;        // 0x38: Command issue
        uint32_t sntf;      // 0x3C: SATA notification
        uint32_t fbs;       // 0x40: FIS-based switching
        uint8_t  rsvd2[0x80-0x44];
    } ports[AHCI_MAX_PORTS];
} __attribute__((packed));

// HBA Command Header
struct hba_cmd_header {
    uint16_t opts;      // Command options
    uint16_t prdtl;     // PRDT length
    uint32_t prdbc;     // PRD byte count
    uint32_t ctba;      // Command table base
    uint32_t ctbau;     // Command table base upper
    uint32_t rsvd[4];
} __attribute__((packed));

// HBA Command Table
struct hba_cmd_table {
    uint8_t  cfis[64];  // Command FIS
    uint8_t  acmd[16];  // ATAPI command
    uint8_t  rsvd[48];  // Reserved
    
    // PRDT (Physical Region Descriptor Table)
    struct {
        uint32_t dba;   // Data base address
        uint32_t dbau;  // Data base address upper
        uint32_t rsvd;
        uint32_t dbc;   // Data byte count (0-based, 4M max)
    } prdt[];
} __attribute__((packed));

// Global HBA controller
extern struct hba_mem *hba;

// AHCI Functions
void ahci_init();
int ahci_read(uint8_t port, uint64_t lba, uint32_t count, void *buffer);

#endif
