/*
 * fer_radio_parity.h
 *
 *  Created on: 17.04.2020
 *      Author: bertw
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/*  "t if VAL contains an even number of 1 bits" */
bool is_bits_even(uint8_t val);

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
uint8_t fer_get_word_parity(uint8_t data_byte, uint8_t pos);

/* "extend DATA_BYTE with parity according to POS" */
uint16_t fer_add_word_parity(uint8_t data_byte, int pos);

/* "return t if parity is even and position matches parity bit \ 1/3
 on even positions and 0,2 on odd positions)" */
bool fer_word_parity_p(uint16_t word, uint8_t pos);

