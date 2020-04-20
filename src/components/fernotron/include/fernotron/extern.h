#pragma once

#include <stdint.h>

uint8_t mcu_get_rxPin();  //IMPLEMENT_ME (called from interrupt!)
void mcu_put_txPin(uint8_t on); //IMPLEMENT_ME (called from interrupt!)
void mcu_delay_us(uint16_t us); //IMPLEMENT_ME

