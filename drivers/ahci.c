#include "ahci.h"
#include "uart.h"
#include "../lib/string.h"
#include "../lib/stddef.h"
#include <stdalign.h>
#include "pci.h"
struct hba_mem *hba = (struct hba_mem*)AHCI_BASE;

// Allocate 4K-aligned memory
static void* ahci_alloc_mem(size_t size) {
    uart_puts("AHCI_ALLOC_MEM");
    static alignas(4096) uint8_t mem_pool[2 * 1024 * 1024]; // 2MB pool
    uart_puts("alignas(4096) uint8_t mem_pool");
    static size_t pool_ptr = 0;

    if (pool_ptr + size > sizeof(mem_pool)) {
        uart_puts("Memory pool exhausted!\n");
        return NULL;
    }

    void *ret = &mem_pool[pool_ptr];
    pool_ptr += size;
    return ret;
}

// Stop a port's command engine
static void port_stop(struct hba_port *port) {
    // Stop command engine
    port->cmd &= ~(1 << 0); // ST = 0
    port->cmd &= ~(1 << 4); // FRE = 0

    // Wait until engine stops (with timeout)
    int timeout = 1000000;
    while ((port->cmd & (1 << 15) || port->cmd & (1 << 14)) && timeout--);

    if (timeout <= 0) {
        uart_puts("Port stop timed out\n");
    }
}

// Initialize a port
static int port_init(uint8_t port_num) {
    struct hba_port *port = &hba->ports[port_num];

    // Check SATA status (PxSSTS.DET must be 0x3)
    uint32_t ssts = port->ssts;
    uint32_t det = ssts & 0xF; // PxSSTS.DET field
    if (det != 0x3) {
        uart_puts("\n[ERROR] No device detected on port ");
        uart_putdec32(port_num);
        uart_puts("\n");
        return -1;
    }

    // Stop command engine
    port_stop(port);

    // Allocate command list (1K) + FIS (256B)
    void *clb = ahci_alloc_mem(1024);
    void *fb = ahci_alloc_mem(256);
    if (!clb || !fb) {
        uart_puts("Memory allocation failed\n");
        return -1;
    }

    port->clb = (uint32_t)(uintptr_t)clb;
    port->clbu = (uint32_t)((uintptr_t)clb >> 32);
    port->fb = (uint32_t)(uintptr_t)fb;
    port->fbu = (uint32_t)((uintptr_t)fb >> 32);

    // Enable FIS reception
    port->cmd |= (1 << 4); // FRE = 1

    // Start command engine
    port->cmd |= (1 << 0); // ST = 1

    return 0;
}

// Dump port registers for debugging
void ahci_dump_port(struct hba_port *port) {
    uart_puts("PxCMD: 0x"); uart_puthex32(port->cmd); uart_puts("\n");
    uart_puts("PxIS: 0x"); uart_puthex32(port->is); uart_puts("\n");
    uart_puts("PxCI: 0x"); uart_puthex32(port->ci); uart_puts("\n");
    uart_puts("PxTFD: 0x"); uart_puthex32(port->tfd); uart_puts("\n");
}

// Initialize AHCI controller
void ahci_init() {
    hba = (struct hba_mem*)AHCI_BASE;
//    find_ahci();
    // Enable AHCI mode

	hba->ghc |= (1 << 31); // Enable AHCI
	int timeout = 10000;
	while (!(hba->ghc & (1 << 31)) && timeout-- > 0);

	if (timeout <= 0) {
	    uart_puts("AHCI mode enable failed!\n");
	    return;
	}

    uart_puts("AHCI initialized\n");

    // Find implemented ports
    uint32_t ports = hba->pi;
    int num_ports = (hba->cap & 0x1F) + 1;

    for (int i = 0; i < num_ports; i++) {
        if (ports & (1 << i)) {
            uart_puts("\n[INFO] Initializing port ");
            uart_putdec32(i);
            if (port_init(i) == 0) {
                uart_puts(" OK\n");
            } else {
                uart_puts(" Failed\n");
            }
        }
    }
}

// Read from disk using AHCI
int ahci_read(uint8_t port_num, uint64_t lba, uint32_t count, void *buffer) {
    struct hba_port *port = &hba->ports[port_num];
    struct hba_cmd_header *cmd_header = (struct hba_cmd_header*)(uintptr_t)port->clb;
    struct hba_cmd_table *cmd_table = (struct hba_cmd_table*)(uintptr_t)cmd_header->ctba;

    // Setup command header
    cmd_header->opts = (5 << 16) | (1 << 6) | (1 << 5); // FIS length, read
    cmd_header->prdtl = 1;

    // Setup PRDT
    cmd_table->prdt[0].dba = (uint32_t)(uintptr_t)buffer;
    cmd_table->prdt[0].dbau = 0;
    cmd_table->prdt[0].dbc = (count * 512) - 1; // 0-based

    // Setup FIS
    memset(cmd_table->cfis, 0, 64);
    uint8_t *fis = cmd_table->cfis;
    fis[0] = 0x27; // Host to device FIS
    fis[1] = 0x80; // Command
    fis[2] = 0x25; // READ DMA EXT
    fis[4] = (lba >> 0) & 0xFF;
    fis[5] = (lba >> 8) & 0xFF;
    fis[6] = (lba >> 16) & 0xFF;
    fis[7] = 0x40 | ((lba >> 24) & 0x0F);
    fis[8] = (lba >> 32) & 0xFF;
    fis[9] = (lba >> 40) & 0xFF;
    fis[10] = count & 0xFF;
    fis[12] = (count >> 8) & 0xFF;

    // Issue command
    port->ci = 1;

    // Wait for completion (with timeout)
    int timeout = 1000000;
    while ((port->ci & 1) && timeout--);
    if (timeout <= 0) {
        uart_puts("AHCI read command timed out\n");
        return -1;
    }

    // Check for errors
    if (port->is & (1 << 30)) { // Task File Error
        uart_puts("AHCI read error: Task File Error\n");
        port->is = (1 << 30); // Clear error
        return -1;
    }

    return 0;
}
