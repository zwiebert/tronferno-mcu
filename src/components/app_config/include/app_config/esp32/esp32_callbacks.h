/*
 * esp32_callbacks.h
 *
 *  Created on: 28.03.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_
#define COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_

#define frx_MSG_RECEIVED_cb() loop_setBit_rxLoop_fromISR(true);
#define ftx_MSG_TRANSMITTED_cb()  loop_setBit_txLoop_fromISR(true);

#endif /* COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_ */
