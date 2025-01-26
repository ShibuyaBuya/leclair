#include <stdint.h>
#include "ahci.h"

// x86 I/O functions
/*void outl(uint32_t port, uint32_t value);
uint32_t inl(uint32_t port);

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    uint32_t addr = (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | (offset & 0xFC);
    outl(0xCF8, addr);
    return inl(0xCFC);
}

void find_ahci() {
    for (int bus = 0; bus < 256; bus++) {
        for (int device = 0; device < 32; device++) {
            for (int func = 0; func < 8; func++) {
                uint32_t vendor_id = pci_read(bus, device, func, 0x00);
                if ((vendor_id & 0xFFFF) == 0xFFFF) continue;

                uint32_t class_rev = pci_read(bus, device, func, 0x08);
                uint8_t class_code = (class_rev >> 24) & 0xFF;
                uint8_t subclass = (class_rev >> 16) & 0xFF;
                uint8_t prog_if = (class_rev >> 8) & 0xFF;

                if (class_code == 0x01 && subclass == 0x06 && prog_if == 0x01) {
                    uint32_t bar5_lo = pci_read(bus, device, func, 0x24);
                    uint32_t bar5_hi = pci_read(bus, device, func, 0x28);
                    hba = (struct hba_mem*)((bar5_lo & ~0xF) | ((uint64_t)bar5_hi << 32));
                    return;
                }
            }
        }
    }
    // Fallback to QEMU address if not found
    hba = (struct hba_mem*)AHCI_BASE;
}
*/
