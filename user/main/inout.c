#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "inout.h"
#include "misc/int_macros.h"

int (*io_putc_fun)(char c);
int (*io_getc_fun)(void);
int (*io_printf_fun)(const char *fmt, ...);

#ifndef AVR
extern char *itoa(int val, char *s, int radix);
extern char *ltoa(long val, char *s, int radix);
#endif


int ICACHE_FLASH_ATTR io_putc(char c) { return (io_putc_fun == 0) ? -1 : io_putc_fun(c); }
int ICACHE_FLASH_ATTR io_getc(void)   { return (io_getc_fun == 0) ? -1 : io_getc_fun(); }



/* use io_putc()/io_getc(). Don't use the function pointers for
   putc/get directly. */
#define io_putc_fun #error
#define io_getc_fun #error

int ICACHE_FLASH_ATTR io_putlf(void) { return io_putc('\n'); }


int ICACHE_FLASH_ATTR io_puts(const char *s) {
  for (;*s != '\0'; ++s) {
    if (io_putc(*s) == -1)
      return -1;
  }
  return 0;
}

#ifdef AVR
int ICACHE_FLASH_ATTR io_puts_P(const char *s) {
	for(;;) {
		char c = pgm_read_byte(s++);
		if (c == '\0')
		break;
		io_putc(c);
	}
	return 0;
}
#endif

#ifdef AVR
int ICACHE_FLASH_ATTR
io_printf(const char *fmt, ...)
{
#if 1
	  va_list argp;
	  int result = -1;

	  if (io_printf_fun != NULL) {
	  va_start(argp, fmt);
	  result = io_printf_fun(fmt, argp);
	  va_end(argp);
	  }
	  return result;
#elif 0
  int result;
  char *buf;
  size_t buf_size;

  buf_size = MAX(90, MIN(40, strlen(fmt) * 2));
  buf = __builtin_alloca(buf_size);

  postcond(40 <=  buf_size && buf_size <= 90);

  if (!buf)
    return -1;

  va_list argp;
  va_start(argp, fmt);
  result = vsnprintf(buf, buf_size, fmt, argp);
  va_end(argp);
  if (result > 0) {
    io_puts(buf);
  }

  return result;
#else
  return -1;
#endif
}
#endif

void ICACHE_FLASH_ATTR
io_putx8(uint8_t n) {
  char s[3];
  if (n & 0xf0) {
    itoa(n, s, 16);
  } else {
    s[0] = '0';
    itoa(n, s+1, 16);
  }
  io_puts(s);
}



void ICACHE_FLASH_ATTR
io_print_hex_8(uint8_t n, bool comma) {
  char s[3];
  itoa(n, s, 16);
  io_puts((n & 0xF0) ? "0x" : "0x0");
  io_puts(s);
  if (comma)
    io_puts(", ");
}
 

void ICACHE_FLASH_ATTR
io_print_hex_16(uint16_t n, bool comma) {
  char s[5];
  itoa(n, s, 16);
  io_puts((n & 0xFFF0)== 0 ? "0x000" :
          (n & 0xFF00)== 0 ? "0x00" :
          (n & 0xF000)== 0 ? "0x0" : "0x");
    
  io_puts(s);

  if (comma)
    io_puts(", ");
}

void ICACHE_FLASH_ATTR
io_print_hex_32(uint32_t n, bool comma) {
  char s[10];
  ltoa(n, s, 16);
  io_puts((n & 0xFFFFFFF0)== 0 ? "0x0000000" :
          (n & 0xFFFFFF00)== 0 ? "0x000000" :
          (n & 0xFFFFF000)== 0 ? "0x00000" :
          (n & 0xFFFF0000)== 0 ? "0x0000" :
          (n & 0xFFF00000)== 0 ? "0x000" :
          (n & 0xFF000000)== 0 ? "0x00" :
          (n & 0xF0000000)== 0 ? "0x0" :
          "0x");
    
  io_puts(s);

  if (comma)
    io_puts(", ");
}

void ICACHE_FLASH_ATTR
io_print_hex(uint32_t n, bool prefix) {
  char s[10];
  ltoa(n, s, 16);
  if (prefix)
    io_puts("0x");
  io_puts(s);
}
      
void ICACHE_FLASH_ATTR
io_print_dec_32(int32_t n, bool comma) {
  char s[12];
  ltoa(n, s, 10);
  io_puts(s);

  if (comma)
    io_puts(", ");
}
      
void ICACHE_FLASH_ATTR
io_print_dec_16(int16_t n, bool comma) {
  char s[10];
  io_puts(itoa(n, s, 10));
  if (comma)
    io_puts(", ");
}

void ICACHE_FLASH_ATTR io_print_float(float f, int n) {
	int i;
	int32_t mult;
	uint32_t rop;
	int16_t lop = (int16_t) f;
	io_print_dec_16(lop, false);
	io_putc('.');

	f -= lop;

	mult = (lop < 0) ? -1 : 1;

	for (i = 0; i < n; ++i)
		mult *= 10;

	rop = (uint32_t) (f * mult);

	io_print_dec_32(rop, false);
}


void ICACHE_FLASH_ATTR
io_putn(int n, int radix) {
  char s[10];
  io_puts(itoa(n, s, radix));
}


void ICACHE_FLASH_ATTR
io_putl(int32_t n, int radix) {
  char s[13];
  io_puts(ltoa(n, s, radix));
}


void ICACHE_FLASH_ATTR
io_putd(int n) {
  io_putn(n, 10);
}


void ICACHE_FLASH_ATTR
io_putld(int32_t n) {
  io_putl(n, 10);
}

int ICACHE_FLASH_ATTR
io_getline(char *buf, unsigned buf_size) {
  int i, c;

  for (i=0; (i+1) < buf_size; ++i) {
    c = io_getc();

    if (c == -1)
      return -1;

    if (c == ';')
      break;
    
    buf[i] = (char)c;
  }

  buf[i] = '\0';
  return i;
}

void ICACHE_FLASH_ATTR
printBCD(uint8_t bcd) {
  char s[10];
  io_puts(itoa(GET_HIGH_NIBBLE(bcd), s , 16)); 
  io_puts(itoa(GET_LOW_NIBBLE( bcd), s , 16)); 
}

void ICACHE_FLASH_ATTR
print_array_8(const uint8_t *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(src[i], true);
  }
  io_putlf();
}

void ICACHE_FLASH_ATTR
print_array_8_inv(const uint8_t *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(~src[i], true);
  }
  io_putlf();
}

