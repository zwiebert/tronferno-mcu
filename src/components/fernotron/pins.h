/*
 * pins.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_PINS_H_
#define COMPONENTS_FERNOTRON_PINS_H_

#include <stdint.h>
#include "fer_app_cfg.h"

// hooks
extern   uint8_t (*volatile ferHook_rx_pin)(void);
extern   void (*volatile ferHook_tx_pin)(uint8_t on);

// these functions just call the hooks
uint8_t fer_get_rx_pin();
void fer_put_tx_pin(uint8_t on);


// redefine these macros at build time to use statically linked functions
#ifndef FER_PUT_TX_PIN
#define FER_PUT_TX_PIN fer_put_tx_pin
#endif
#ifndef FER_GET_RX_PIN
#define FER_GET_RX_PIN fer_get_rx_pin
#endif

#endif /* COMPONENTS_FERNOTRON_PINS_H_ */
