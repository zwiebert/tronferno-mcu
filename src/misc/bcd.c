#include <stdint.h>
#include <stdlib.h>

#include "main/common.h"
#include "bcd.h"


uint8_t ICACHE_FLASH_ATTR bcd2dec(uint8_t bcd) {
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

uint8_t ICACHE_FLASH_ATTR dec2bcd(uint8_t dec) {
	div_t result = div(dec, 10);
	return ((0xf0 & (result.quot << 4)) | (0xf & result.rem));
}


uint8_t ICACHE_FLASH_ATTR dec2bcd_special(uint8_t dec) {
	div_t result = div(dec, 10);

	return ((0xf0 & ((result.quot == 9 ? 0xf : result.quot) << 4)) | (0xf & (result.rem == 9 ? 0xf : result.rem)));
}
