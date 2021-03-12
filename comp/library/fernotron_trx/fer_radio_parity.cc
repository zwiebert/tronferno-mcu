/*
 * fer_radio_parity.c
 *
 *  Created on: 17.04.2020
 *      Author: bertw
 */

#include "fer_app_cfg.h"

#include "fer_radio_parity.h"
#include "utils_misc/int_types.h"

/*  "t if VAL contains an even number of 1 bits" */
bool IRAM_ATTR is_bits_even(u8 val) {
  val ^= val >> 4;
  val ^= val >> 2;
  val ^= val >> 1;
  val &= 0x01;
  return (val == 0);
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
u8 IRAM_ATTR fer_get_word_parity(u8 data_byte, u8 pos) {
  u8 result;
  bool is_even = is_bits_even(data_byte);

  result = ((pos & 1)) ? (is_even ? 3 : 1) : (is_even ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
u16 IRAM_ATTR fer_add_word_parity(u8 data_byte, int pos) {
  u16 result = (data_byte | (((u16) fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

/* "return t if parity is even and position matches parity bit \ 1/3
 on even positions and 0,2 on odd positions)" */
bool IRAM_ATTR fer_word_parity_p(u16 word, u8 pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}
