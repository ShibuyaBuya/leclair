#include <stdint.h>
#define UART0_BASE 0x09000000

volatile uint32_t* uart = (uint32_t*)UART0_BASE;
// Change variable name to match header
volatile uint32_t* uart_reg = (uint32_t*)0x09000000;

// Rest of the UART implementation remains the same
void uart_putc(char c) {
    while(uart[5] & 0x20);
    uart[0] = c;
}

void uart_puts(const char* s) {
    while(*s) uart_putc(*s++);
}
/*void uart_hexdump(const void *addr, size_t len) {
    const uint8_t *buf = addr;
    for (size_t i = 0; i < len; i++) {
        uart_puthex8(buf[i]);
        uart_putc(' ');
        if ((i + 1) % 16 == 0) uart_putc('\n');
    }
    uart_putc('\n');
}*/
void uart_puthex16(uint16_t value) {
    const char hex_chars[] = "0123456789abcdef";
    
    // Print "0x" prefix
    uart_putc('0');
    uart_putc('x');

    // Print each nibble (4 digits for 16-bit)
    for(int i = 12; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0xF;
        uart_putc(hex_chars[nibble]);
    }
}
// Hexadecimal output
void uart_puthex32(uint32_t value) {
    const char hex_chars[] = "0123456789abcdef";
    uart_putc('0');
    uart_putc('x');
    
    for(int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (value >> i) & 0xF;
        uart_putc(hex_chars[nibble]);
    }
}

void uart_puthex64(uint64_t value) {
    uart_puthex32(value >> 32);
    uart_puthex32(value & 0xFFFFFFFF);
}

// Decimal output
void uart_putdec32(uint32_t value) {
    if(value == 0) {
        uart_putc('0');
        return;
    }
    
    char buffer[11]; // Max 10 digits for 32-bit + null
    int pos = 10;
    buffer[10] = '\0';
    
    while(value > 0) {
        buffer[--pos] = '0' + (value % 10);
        value /= 10;
    }
    
    uart_puts(&buffer[pos]);
}

// Add to uart.h
//void uart_puthex32(uint32_t value);
//void uart_puthex64(uint64_t value);
//void uart_putdec32(uint32_t value);
void uart_init() {
    // QEMU already initializes UART
// Disable FIFO (important for bare-metal)
    uart_reg[0x08/4] = 0;  // UARTLCR_H: 8N1 mode
    uart_reg[0x0C/4] = 0;  // UARTCR: Disable UART first
    uart_reg[0x24/4] = 0x10;  // UARTIBRD: Baud rate integer
    uart_reg[0x28/4] = 0xC35; // UARTFBRD: Baud rate fractional
    uart_reg[0x2C/4] = 0x7<<4; // UARTLCR_H: 8 bit word length
    uart_reg[0x30/4] = 0x301; // UARTCR: Enable UART, Tx/Rx
}
