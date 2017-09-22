bool byte_even_p (uint8_t val);

void put_bit_16(uint16_t *dst, uint8_t bitnmb, bool val);

uint8_t bcd2dec(uint8_t bcd);
uint8_t dec2bcd(uint8_t dec);

uint8_t dec2bcd_special(uint8_t dec); // replaces all 9 with 0xf in result