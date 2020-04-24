/*
 * pin.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#pragma once
#include "app_config/proj_app_cfg.h"


struct cfg_gpio {
  int8_t out_rf, in_rf, in_setButton;
#ifdef ACCESS_GPIO
  /* enum mcu_pin_state */ uint8_t gpio[CONFIG_GPIO_SIZE];
#endif
};


void setup_pin(const struct cfg_gpio *c);

enum mcu_pin_mode pin_getPinMode(unsigned gpio_number);

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

