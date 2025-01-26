#ifndef _UART_H
#define _UART_H
#include "../lib/stdint.h"
void uart_init();
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex32(uint32_t value);
void uart_puthex64(uint64_t value);
void uart_putdec32(uint32_t value);
void uart_puthex16(uint16_t value);
void uart_hexdump(const void *addr, size_t len);
#endif
