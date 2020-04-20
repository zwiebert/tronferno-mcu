/*
 * pin.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#pragma once


void setup_pin(void);

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

