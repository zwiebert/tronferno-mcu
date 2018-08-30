#ifndef _inout_h
#define _inout_h

#include <stdint.h>
#include <c_types.h>
#ifndef true
#include <stdbool.h>
#endif

extern int (*io_putc_fun)(char c);
extern int (*io_getc_fun)(void);
extern int (*io_printf_fun)(const char *fmt, ...);

int io_putc(char c);
int io_getc(void);
int io_putlf(void);
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



bool mcu_get_buttonUpPin(void);
bool mcu_get_buttonDownPin(void);
bool mcu_get_buttonPin(void);

typedef enum mcu_pin_state { PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE } mcu_pin_state;
const char* mcu_access_pin(int gpio_number, mcu_pin_state *result, mcu_pin_state state);
bool  is_gpio_number_usable(int gpio_number, bool cli);
#endif

