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

#include "../userio/inout.h"
#include "driver/uart.h"

#include "fernotron/fer.h"
#include "config/config.h"


#define printf ets_uart_printf

#define RFOUT_GPIO 4
#define RFIN_GPIO 5
#define BUTTON_GPIO 0


const char*  mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state);

bool  is_gpio_number_usable(int gpio_number, bool cli) {
  if (cli) {
    switch (gpio_number) {
    case 12:
    case 13:
    case 14:
    case 15:
      return true;
      break;

    default:
      break;

    }
  } else {
    if (0 <= gpio_number && 16 <= gpio_number) {
      return true;
    }
  }
  return false;
}

void mcu_put_txPin(bool dat) {
  GPIO_OUTPUT_SET(RFOUT_GPIO, dat);
}

bool mcu_get_rxPin() {
  return GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_GPIO));
}

void 
setup_pin(void) {
  int i;

  for (i = 0; i < 17; ++i) {
    mcu_pin_state state = C.gpio[i];
    if (state == PIN_DEFAULT)
      continue;
    else if (state == PIN_INPUT || state == PIN_INPUT_PULLUP || state == PIN_OUTPUT)
      mcu_access_pin2(i, NULL, state);
    else if (state == PIN_SET || state == PIN_CLEAR) {
      mcu_access_pin2(i, NULL, PIN_OUTPUT);
      mcu_access_pin2(i, NULL, state);
    }
  }

  mcu_access_pin2(RFOUT_GPIO, NULL, PIN_OUTPUT);
  mcu_access_pin2(RFIN_GPIO, NULL, PIN_INPUT);
  mcu_access_pin2(BUTTON_GPIO, NULL, PIN_INPUT_PULLUP);

  mcu_put_txPin(false); // make sure, the RF transmitter is off
}

bool  mcu_get_buttonUpPin(void) {
  return false;
}

bool  mcu_get_buttonDownPin(void) {
  return false;
}

bool  mcu_get_buttonPin(void) {
  bool val = GPIO_INPUT_GET(GPIO_ID_PIN(BUTTON_GPIO));
  return val == 0;  // 0 means pressed
}

const char*  mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {

  if (state == PIN_OUTPUT || state == PIN_INPUT || state == PIN_INPUT_PULLUP) {
    bool pullup = state == PIN_INPUT_PULLUP;

    if (state != PIN_OUTPUT) {
      GPIO_DIS_OUTPUT(GPIO_ID_PIN(gpio_number));
    }

    switch (gpio_number) {

    case 4:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
      if (pullup)
        PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
      else
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);
      break;

    case 5:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
      if (pullup)
        PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
      else
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
      break;

    case 12:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
      if (pullup)
        PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDI_U);
      else
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);
      break;

    case 13:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
      if (pullup)
        PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
      else
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTCK_U);
      break;

    case 14:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
      if (pullup)
        PIN_PULLUP_EN(PERIPHS_IO_MUX_MTMS_U);
      else
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTMS_U);
      break;

    case 15:
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
      if (pullup)
        return "pullup for gpio15 not possible";
      break;

    default:
      return "gpio number can not be used";
    }

  } else if (state == PIN_SET) {
    GPIO_OUTPUT_SET(gpio_number, 1);

  } else if (state == PIN_CLEAR) {
    GPIO_OUTPUT_SET(gpio_number, 0);

  } else if (state == PIN_TOGGLE) {
    bool old = GPIO_INPUT_GET(GPIO_ID_PIN(gpio_number));
    GPIO_OUTPUT_SET(gpio_number, !old);

  } else if (state == PIN_READ) {
    *result = GPIO_INPUT_GET(GPIO_ID_PIN(gpio_number)) ? PIN_SET : PIN_CLEAR;

  } else {
    return "not implemented";
  }

  return NULL;

}

const char*  mcu_access_pin(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {

  if (state != PIN_READ)
    switch (gpio_number) {
   case 12:
   case 13:
   case 14:
   case 15:
     break;

   default:
     return "gpio number not writable (use 12, 13, 14, 15)";
   }

    return mcu_access_pin2(gpio_number, result, state);
}
