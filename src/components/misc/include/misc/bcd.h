#ifndef bcd_h_
#define bcd_h_

#include <stdint.h>

uint8_t bcd2dec(uint8_t bcd);
uint8_t dec2bcd(uint8_t dec);

uint8_t dec2bcd_special(uint8_t dec); // like above, but replaces all 9 with 0xf in result


#endif
