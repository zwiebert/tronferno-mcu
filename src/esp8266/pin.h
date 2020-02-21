/*
 * pin.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef ESP8266_PIN_H_
#define ESP8266_PIN_H_

#include <stdint.h>

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

#endif /* ESP8266_PIN_H_ */
