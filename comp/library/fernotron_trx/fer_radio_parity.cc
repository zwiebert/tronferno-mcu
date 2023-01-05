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
bool IRAM_ATTR is_bits_even(uint8_t val) {
  val ^= val >> 4;
  val ^= val >> 2;
  val ^= val >> 1;
  val &= 0x01;
  return (val == 0);
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
uint8_t IRAM_ATTR fer_get_word_parity(uint8_t data_byte, uint8_t pos) {
  uint8_t result;
  bool is_even = is_bits_even(data_byte);

  result = ((pos & 1)) ? (is_even ? 3 : 1) : (is_even ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
uint16_t IRAM_ATTR fer_add_word_parity(uint8_t data_byte, int pos) {
  uint16_t result = (data_byte | (((uint16_t) fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

/* "return t if parity is even and position matches parity bit \ 1/3
 on even positions and 0,2 on odd positions)" */
bool IRAM_ATTR fer_word_parity_p(uint16_t word, uint8_t pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}
