/*
 * pin.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */
#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "main/inout.h"
#include "main/fer.h"
#include "main/config.h"

#include "esp_missing_includes.h"
#define console_printf ets_uart_printf


#define RFOUT_GPIO 4
#define RFOUT_GPIO_MUX PERIPHS_IO_MUX_GPIO4_U
#define RFOUT_GPIO_FUNC FUNC_GPIO4

#define RFIN_GPIO 5
#define RFIN_GPIO_MUX PERIPHS_IO_MUX_GPIO5_U
#define RFIN_GPIO_FUNC FUNC_GPIO5


void mcu_put_txPin(bool dat) {
	GPIO_OUTPUT_SET(RFOUT_GPIO, dat);
}

bool mcu_get_rxPin() {
	return GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_GPIO));
}

void ICACHE_FLASH_ATTR
setup_pin(void) {
	PIN_FUNC_SELECT(RFOUT_GPIO_MUX, RFOUT_GPIO_FUNC);

	PIN_FUNC_SELECT(RFIN_GPIO_MUX, RFIN_GPIO_FUNC);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(RFIN_GPIO));

	mcu_put_txPin(false); // make sure, the RF transmitter is off
}
