/**
 * \file cc1101_ook/spi.hh
 * \brief CC1101 transceiver hardware SPI setup
 */

#pragma once

#include <stdint.h>

struct cc1101_settings {
  int8_t sclk, ss, miso, mosi;
  bool enable;
};

/**
 * \brief      Setup CC1101 SPI or disable CC1101
 * \param cfg  pointer to configuration object
 */
void cc1101_ook_spi_setup(const struct cc1101_settings *cfg);

/**
 * \brief     disable CC1101 hardware before reboot (useless function?)
 */
void cc1101_ook_spi_disable();
