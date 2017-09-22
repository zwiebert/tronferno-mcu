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
#include <gpio16.h>
#include "driver/uart.h"

#include "../main/inout.h"
#include "../main/fer.h"
#include "../main/config.h"
#include "../main/time.h"

#include "esp_missing_includes.h"
#define console_printf ets_uart_printf

#define GPIO_RFIN_PIN 5
#define GPIO_RFOUT_PIN 4

#define RFOUT_GPIO 4
#define RFOUT_GPIO_MUX PERIPHS_IO_MUX_GPIO4_U
#define RFOUT_GPIO_FUNC FUNC_GPIO4

#define RFIN_GPIO 5
#define RFIN_GPIO_MUX PERIPHS_IO_MUX_GPIO5_U
#define RFIN_GPIO_FUNC FUNC_GPIO5

extern uint8_t pin_num[GPIO_PIN_NUM];

#define GPIO_SDK 1

// GPIO_PIN_INTR_NEGEDGE - down
// GPIO_PIN_INTR_POSEDGE - up
// GPIO_PIN_INTR_ANYEDGE - both
// GPIO_PIN_INTR_LOLEVEL - low level
// GPIO_PIN_INTR_HILEVEL - high level
// GPIO_PIN_INTR_DISABLE - disable interrupt
const char *gpio_type_desc[] =
	{ "GPIO_PIN_INTR_DISABLE (DISABLE INTERRUPT)", "GPIO_PIN_INTR_POSEDGE (UP)", "GPIO_PIN_INTR_NEGEDGE (DOWN)", "GPIO_PIN_INTR_ANYEDGE (BOTH)",
			"GPIO_PIN_INTR_LOLEVEL (LOW LEVEL)", "GPIO_PIN_INTR_HILEVEL (HIGH LEVEL)" };

void fer_put_sendPin(bool dat) {
#if GPIO_SDK
	GPIO_OUTPUT_SET(RFOUT_GPIO, dat);
#else
	gpio_write(GPIO_RFOUT_PIN, dat);
#endif
}

bool fer_get_recvPin() {
#if GPIO_SDK
	return GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_GPIO));
#else
	return gpio_read(GPIO_RFIN_PIN) == 1;
#endif
}

void ICACHE_FLASH_ATTR
setup_pin(void) {
#if GPIO_SDK
	extern void ets_wdt_enable(void);
	extern void ets_wdt_disable(void);

	ets_wdt_enable();
	ets_wdt_disable();
	PIN_FUNC_SELECT(RFOUT_GPIO_MUX, RFOUT_GPIO_FUNC);

	PIN_FUNC_SELECT(RFIN_GPIO_MUX, RFIN_GPIO_FUNC);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(RFIN_GPIO));


#else

	{
		GPIO_INT_TYPE gpio_type;

		gpio_type = GPIO_PIN_INTR_DISABLE;
		if (set_gpio_mode(GPIO_RFIN_PIN, GPIO_FLOAT, GPIO_INPUT)) {
#if 0
			console_printf("GPIO%d set interrupt mode\r\n", pin_num[GPIO_RFIN_PIN]);

			if (gpio_intr_init(GPIO_RFIN_PIN, gpio_type)) {
				console_printf("GPIO%d enable %s mode\r\n", pin_num[GPIO_RFIN_PIN], gpio_type_desc[gpio_type]);
				gpio_intr_attach(intr_callback);

			} else {
				console_printf("Error: GPIO%d not enable %s mode\r\n", pin_num[GPIO_RFIN_PIN], gpio_type_desc[gpio_type]);
			}
#endif
		} else {
			console_printf("Error: GPIO%d not set interrupt mode\r\n", pin_num[GPIO_RFIN_PIN]);
		}

		if (set_gpio_mode(GPIO_RFOUT_PIN, GPIO_PULLUP, GPIO_OUTPUT)) {
			console_printf("GPIO%d set GPIO_OUTPUT mode\r\n", pin_num[GPIO_RFOUT_PIN]);
		} else {
			console_printf("Error: GPIO%d not set GPIO_OUTPUT mode\r\n", pin_num[GPIO_RFOUT_PIN]);
		}

	}

#endif

	fer_put_sendPin(false); // make sure, the RX transmitter is off
}
