#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "utils.h"



void ICACHE_FLASH_ATTR put_bit_16(uint16_t *dst, uint8_t bitnmb, bool val) {
	if (val)
		*dst |= (1 << bitnmb);
	else
		*dst &= ~(1 << bitnmb);
}

