#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "utils.h"

/*  "t if VAL contains an even number of 1 bits" */
bool is_bits_even(uint8_t val) {
	val ^= val >> 4;
	val ^= val >> 2;
	val ^= val >> 1;
	val &= 0x01;
	return (val == 0);
}

void put_bit_16(uint16_t *dst, uint8_t bitnmb, bool val) {
	if (val)
		*dst |= (1 << bitnmb);
	else
		*dst &= ~(1 << bitnmb);
}

uint8_t bcd2dec(uint8_t bcd) {
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

uint8_t dec2bcd(uint8_t dec) {
	div_t result = div(dec, 10);
	return ((0xf0 & (result.quot << 4)) | (0xf & result.rem));
}

uint8_t dec2bcd_special(uint8_t dec) {
	div_t result = div(dec, 10);

	return ((0xf0 & ((result.quot == 9 ? 0xf : result.quot) << 4)) | (0xf & (result.rem == 9 ? 0xf : result.rem)));
}
