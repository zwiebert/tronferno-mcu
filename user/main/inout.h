#include <stdint.h>
#include <stdbool.h>

extern int (*io_putc_fun)(char c);
extern int (*io_getc_fun)(void);
extern int (*io_printf_fun)(const char *fmt, ...);

int io_putc(char c);
int io_getc(void);

int io_puts(const char *s);
int io_getline(char *buf, unsigned buf_size);
int io_printf(const char *fmt, ...);

void io_print_hex_8(uint8_t n, bool comma);
void io_print_hex_16(uint16_t n, bool comma);
void io_print_hex_32(uint32_t n, bool comma);

void io_print_dec_8(int8_t n, bool comma);
void io_print_dec_16(int16_t n, bool comma);
void io_print_dec_32(int32_t n, bool comma);

void io_print_float(float f, int n);

void io_putn(int n, int radix);
void io_putl(int32_t n, int radix);
void io_putd(int n);
void io_putld(int32_t n);

void printBCD(uint8_t bcd);
void print_array_8(const uint8_t *src, int len);
void print_array_8_inv(const uint8_t *src, int len);

#ifdef AVR
int io_puts_P(const char *s);
#else
#define io_puts_P io_puts

extern int ets_uart_printf(const char *fmt, ...);
#define io_printf ets_uart_printf
#endif
