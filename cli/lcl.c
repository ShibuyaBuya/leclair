/*#include "../drivers/uart.h"

void lcl_main() {
    uart_puts("\nLCL Shell v1.0\n> ");
    
    while(1) {
        char cmd[128];
        int i = 0;
        
        // Read input
        while(1) {
            cmd[i] = uart[0];
            if(cmd[i] == '\r') break;
            uart_putc(cmd[i]);
            i++;
        }
        cmd[i] = 0;
        
        // Execute command
        uart_puts("\n");
        if(strcmp(cmd, "help") == 0) {
            uart_puts("Available commands: help, about\n");
        } else if(strcmp(cmd, "about") == 0) {
            uart_puts("Custom Kernel v0.1\n");
        }
        
        uart_puts("> ");
    }
}
*/
#include "../drivers/uart.h"
#include "../lib/string.h"  // Add this

// Add UART register definition
extern volatile uint32_t* uart_reg;

void lcl_main() {
    uart_puts("\nLCL Shell v1.0\n> ");
    
    while(1) {
        char cmd[128];
        int i = 0;
        
        // Read input
        while(1) {
            cmd[i] = uart_reg[0];  // Use uart_reg instead of uart
            if(cmd[i] == '\r') break;
            uart_putc(cmd[i]);
            i++;
        }
        cmd[i] = 0;
        
        // Execute command
        uart_puts("\n");
        if(strcmp(cmd, "help") == 0) {
            uart_puts("Available commands: help, about\n");
        } else if(strcmp(cmd, "about") == 0) {
            uart_puts("Custom Kernel v0.1\n");
        }
        
        uart_puts("> ");
    }
}
