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
#include "gpio/pin.h"

#include "txtio/inout.h"

#include "config/config.h"

#include <stdlib.h>
#include <string.h>

const struct cfg_gpio *gpio_cfg;

#define printf ets_uart_printf
#define RFOUT_GPIO gpio_cfg->out_rf
#define RFIN_GPIO gpio_cfg->in_rf
#define BUTTON_GPIO gpio_cfg->in_setButton


#ifdef ACCESS_GPIO
enum mcu_pin_mode pin_getPinMode(unsigned gpio_number) {
  if (gpio_number >= sizeof gpio_cfg->gpio)
    return -1;
  return  gpioCfg_getPinMode(gpio_cfg, gpio_number);
}
enum mcu_pin_level pin_getPinLevel(unsigned gpio_number) {
  if (gpio_number >= sizeof gpio_cfg->gpio)
    return -1;
  return  gpioCfg_getPinLevel(gpio_cfg, gpio_number);
}
#endif

#define gpioUsable (BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(12)|BIT(13)|BIT(14)|BIT(15))
static u32 pins_in_use, pins_not_cli;

bool is_gpio_number_usable(int gpio_number, bool cli) {
  if (GET_BIT(gpioUsable, gpio_number)) {
    if (cli) {
      return !GET_BIT((pins_in_use | pins_not_cli), gpio_number);
    } else {
      return !GET_BIT(pins_in_use, gpio_number);
    }
  }
  return false;
}

void IRAM_ATTR mcu_put_txPin(u8 level) {
  GPIO_OUTPUT_SET(RFOUT_GPIO, level);
  //io_putc(level ? '1' : '0');
}

u8 IRAM_ATTR mcu_get_rxPin() {
  return GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_GPIO));
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



static const char* pin_set_mode_int(int gpio_number, mcu_pin_mode mode, mcu_pin_level level) {
  if (gpio_number == -1)
    return 0;
  if (mode != PIN_DEFAULT && GET_BIT(pins_in_use, gpio_number))
    return "Pin is already in use";

  bool pullup = mode == PIN_INPUT && level == PIN_HIGH;

  PUT_BIT(pins_in_use, gpio_number, mode != PIN_DEFAULT);

  if (mode == PIN_DEFAULT || mode == PIN_INPUT) {
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(gpio_number));
  }

  switch (gpio_number) {

  case 0:
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    if (pullup)
      PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);
    else
      PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);
    break;

  case 1:
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
    if (pullup)
      PIN_PULLUP_EN(PERIPHS_IO_MUX_U0TXD_U);
    else
      PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    break;

  case 2:
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    if (pullup)
      PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);
    else
      PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO2_U);
    break;

  case 3:
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
    if (pullup)
      PIN_PULLUP_EN(PERIPHS_IO_MUX_U0RXD_U);
    else
      PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0RXD_U);
    break;

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

  if (mode != PIN_INPUT && (level == PIN_LOW || level == PIN_HIGH))
    GPIO_OUTPUT_SET(gpio_number, (level == PIN_HIGH));

  return NULL;
}

const char* pin_set_mode(int gpio_number, mcu_pin_mode mode, mcu_pin_level level) {
  if (GET_BIT(pins_not_cli, gpio_number))
    return "this gpio cannot be set from CLI";
  return pin_set_mode_int(gpio_number, mode, level);
}

static const char* mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {
  if (!GET_BIT(gpioUsable, gpio_number))
    return "gpio not usable";
  if (!GET_BIT(pins_in_use, gpio_number))
    return "gpio is unused";


  if (state == PIN_SET) {
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
  if (state != PIN_READ && GET_BIT(pins_not_cli, gpio_number))
    return "this gpio cannot be set from CLI";
#if 0
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
#endif
    return mcu_access_pin2(gpio_number, result, state);
}

void
setup_pin(const struct cfg_gpio *c) {
  if (!gpio_cfg) {
    gpio_cfg = calloc(1, sizeof *gpio_cfg);
  } else {
    pin_set_mode_int(RFOUT_GPIO, PIN_DEFAULT, PIN_LOW);
    pin_set_mode_int(RFIN_GPIO, PIN_DEFAULT, PIN_FLOATING);
    pin_set_mode_int(BUTTON_GPIO, PIN_DEFAULT, PIN_HIGH);
    pins_in_use = 0;
  }
  memcpy((void*)gpio_cfg, c, sizeof *gpio_cfg);

  pin_set_mode_int(RFOUT_GPIO, PIN_OUTPUT, PIN_LOW);
  pin_set_mode_int(RFIN_GPIO, PIN_INPUT, PIN_FLOATING);
  pin_set_mode_int(BUTTON_GPIO, PIN_INPUT, PIN_HIGH);
  pins_not_cli = pins_in_use;

#ifdef ACCESS_GPIO
  int i;
  for (i = 0; i < CONFIG_GPIO_SIZE; ++i) {
      if (gpioCfg_getPinMode(gpio_cfg, i) == PIN_DEFAULT)
      continue;
    pin_set_mode(i, gpioCfg_getPinMode(gpio_cfg, i), gpioCfg_getPinLevel(gpio_cfg, i));
    if (gpioCfg_getPinMode(gpio_cfg, i) == PIN_INPUT) {
      //TODO-implement-me: pin_setup_input_handler(i);
    }

  }
#endif
}
