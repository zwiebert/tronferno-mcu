#include "../userio/inout.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "main/common.h"
#include "misc/int_macros.h"

int (*io_putc_fun)(char c);
int (*io_getc_fun)(void);
int (*io_printf_fun)(const char *fmt, ...);

extern char *itoa(int val, char *s, int radix);
extern char *ltoa(long val, char *s, int radix);

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

void ICACHE_FLASH_ATTR
io_putx8(u8 n) {
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
io_print_hex_8(u8 n, bool comma) {
  char s[3];
  itoa(n, s, 16);
  io_puts((n & 0xF0) ? "0x" : "0x0");
  io_puts(s);
  if (comma)
    io_puts(", ");
}
 

void ICACHE_FLASH_ATTR
io_print_hex_16(u16 n, bool comma) {
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
io_print_hex_32(u32 n, bool comma) {
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
io_print_hex(u32 n, bool prefix) {
  char s[10];
  ltoa(n, s, 16);
  if (prefix)
    io_puts("0x");
  io_puts(s);
}
      
void ICACHE_FLASH_ATTR
io_print_dec_32(i32 n, bool comma) {
  char s[12];
  ltoa(n, s, 10);
  io_puts(s);

  if (comma)
    io_puts(", ");
}
      
void ICACHE_FLASH_ATTR
io_print_dec_16(i16 n, bool comma) {
  char s[10];
  io_puts(itoa(n, s, 10));
  if (comma)
    io_puts(", ");
}

void ICACHE_FLASH_ATTR io_print_float(float f, int n) {
	int i;
	i32 mult;
	u32 rop;
	i16 lop = (i16) f;
	io_print_dec_16(lop, false);
	io_putc('.');

	f -= lop;

	mult = (lop < 0) ? -1 : 1;

	for (i = 0; i < n; ++i)
		mult *= 10;

	rop = (u32) (f * mult);

	io_print_dec_32(rop, false);
}




void ICACHE_FLASH_ATTR
io_putn(int n, int radix) {
  char s[10];
  io_puts(itoa(n, s, radix));
}


void ICACHE_FLASH_ATTR
io_putl(i32 n, int radix) {
  char s[13];
  io_puts(ltoa(n, s, radix));
}


void ICACHE_FLASH_ATTR
io_putd(int n) {
  io_putn(n, 10);
}


void ICACHE_FLASH_ATTR
io_putld(i32 n) {
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
printBCD(u8 bcd) {
  char s[10];
  io_puts(itoa(GET_HIGH_NIBBLE(bcd), s , 16)); 
  io_puts(itoa(GET_LOW_NIBBLE( bcd), s , 16)); 
}

void ICACHE_FLASH_ATTR
print_array_8(const u8 *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(src[i], true);
  }
  io_putlf();
}

void ICACHE_FLASH_ATTR
print_array_8_inv(const u8 *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(~src[i], true);
  }
  io_putlf();
}

