/*
 * input_pins.c
 *
 *  Created on: 17.02.2020
 *      Author: bertw
 */

#include "pins.h"
#include "fer_app_cfg.h"

   uint8_t (*volatile ferHook_rx_pin)(void);
   void (*volatile ferHook_tx_pin)(uint8_t on);

uint8_t IRAM_ATTR fer_get_rx_pin() {
  return ferHook_rx_pin ? ferHook_rx_pin() : 0;
}

void IRAM_ATTR fer_put_tx_pin(uint8_t on) {
  if (ferHook_tx_pin)
    ferHook_tx_pin(on);
}

