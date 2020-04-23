/*
 * pin.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#pragma once
#include "app_config/proj_app_cfg.h"

#ifdef ACCESS_GPIO
struct cfg_gpio {
  /* enum mcu_pin_state */ uint8_t gpio[CONFIG_GPIO_SIZE];
};
#endif

void setup_pin(void);

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

