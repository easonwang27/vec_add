/*
 * Copyright (c) 2012-2019 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "./inc/platform.h"

#define BAUD_RATE(n)            (UCLKFREQ / (n) / 16)

#define putchar(c) outbyte(c)

#include <stdarg.h>

void uart_init(unsigned int baudrate)
{
	/* Set DLAB to 1 */
	DEV_UART2->LCR |= 0x80;

	/* Set DLL for baudrate */
	DEV_UART2->DLL = (BAUD_RATE(baudrate) >> 0) & 0xff;
	DEV_UART2->DLM = (BAUD_RATE(baudrate) >> 8) & 0xff;

	/* LCR: Length 8, no parity, 1 stop bit. */
	DEV_UART2->LCR = 0x03;

	/* FCR: Enable FIFO, reset TX and RX. */
	DEV_UART2->FCR = 0x07;
}

int uart_getc(void)
{
#define SERIAL_LSR_RDR          0x01
	while ((DEV_UART2->LSR & SERIAL_LSR_RDR) == 0) ;

	return DEV_UART2->RBR;
}

void uart_putc(int c)
{
#define SERIAL_LSR_THRE         0x20
	while ((DEV_UART2->LSR & SERIAL_LSR_THRE) == 0) ;

	DEV_UART2->THR = c;
}

int uart_puts(const char *s)
{
	int len = 0;

	while (*s) {
		uart_putc(*s);

		if (*s == '\n')
			uart_putc('\r');
		s++;
		len++;
	}
	return len;
}

int outbyte(int c)
{
	uart_putc(c);
	if (c =='\n')
		uart_putc('\r');
	return c;
}

static void uart_printchar(char **str, int c)
{
	extern int putchar(int c);

	if (str) {
		**str = c;
		++(*str);
	}
	else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int uart_prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			uart_printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		uart_printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		uart_printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int uart_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return uart_prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			uart_printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + uart_prints (out, s, width, pad);
}

static int uart_print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)((long)va_arg( args, int ));
				pc += uart_prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += uart_printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += uart_printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += uart_printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += uart_printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += uart_prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			uart_printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int uart_printf(const char *format, ...)
{
        va_list args;

        va_start( args, format );
        return uart_print( 0, format, args );
}

int uart_sprintf(char *out, const char *format, ...)
{
        va_list args;

        va_start( args, format );
        return uart_print( &out, format, args );
}


int uart_snprintf( char *buf, unsigned int count, const char *format, ... )
{
        va_list args;

        ( void ) count;

        va_start( args, format );
        return uart_print( &buf, format, args );
}

