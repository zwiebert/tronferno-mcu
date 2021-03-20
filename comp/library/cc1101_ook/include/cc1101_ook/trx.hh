/*
 * trx.hh
 *
 *  Created on: Feb 25, 2021
 *      Author: bertw
 */

#pragma once
#include <stdint.h>

bool cc1101_ook_setDirection(bool tx);

bool cc1101_ook_gdo_hw(int gdo_num, bool high);
bool cc1101_ook_gdo_isConnected(int gdo_num, int gpio_num);
bool cc1101_ook_get_regfile(uint8_t *dst, size_t *dst_size);
bool cc1101_ook_dump_config(uint8_t *dst, size_t *dst_size);
bool cc1101_ook_dump_status(uint8_t *buf, size_t *length);
bool cc1101_ook_upd_regfile(const uint8_t *src);
