/*
 * spi.hh
 *
 *  Created on: Feb 25, 2021
 *      Author: bertw
 */

#pragma once

#include <stdint.h>

struct cc1101_settings {
  int8_t sclk, ss, miso, mosi;
  bool enable;
};

void cc1101_ook_spi_setup(struct cc1101_settings *cfg);
