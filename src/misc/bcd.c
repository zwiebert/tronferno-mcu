#include <stdint.h>
#include <stdlib.h>

#include "main/common.h"
#include "bcd.h"


u8  bcd2dec(u8 bcd) {
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

u8  dec2bcd(u8 dec) {
	div_t result = div(dec, 10);
	return ((0xf0 & (result.quot << 4)) | (0xf & result.rem));
}


u8  dec2bcd_special(u8 dec) {
	div_t result = div(dec, 10);

	return ((0xf0 & ((result.quot == 9 ? 0xf : result.quot) << 4)) | (0xf & (result.rem == 9 ? 0xf : result.rem)));
}
