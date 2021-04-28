#ifndef __UART_H__
#define __UART_H__

/*
 * Exported functions
 */
void uart_init(unsigned int baudrate);
int uart_getc(void);
void uart_putc(int c);
int uart_puts(const char *s);
int outbyte(int c);

int uart_printf(const char *format, ...);
int uart_sprintf(char *out, const char *format, ...);
int uart_snprintf( char *buf, unsigned int count, const char *format, ... );
#endif	// __UART_H__
