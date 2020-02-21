/*
 * pin.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#ifndef USER_ESP32_MAIN_PIN_H_
#define USER_ESP32_MAIN_PIN_H_


void setup_pin(void);

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

#endif /* USER_ESP32_MAIN_PIN_H_ */
