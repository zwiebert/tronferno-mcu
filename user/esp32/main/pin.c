/*
 * pin.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include "../main/pin.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"


#include "main/inout.h"
#include "fernotron/fer.h"
#include "main/common.h"
#include "config/config.h"

#define printf ets_uart_printf

#define RFOUT_GPIO GPIO_NUM_22
#define RFIN_GPIO GPIO_NUM_17
#define BUTTON_GPIO GPIO_NUM_6


const char*  mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state);

bool is_gpio_number_usable(int gpio_number, bool cli) {
  switch (gpio_number) {
  case GPIO_NUM_3:
  case GPIO_NUM_4:
  case GPIO_NUM_5:
  case GPIO_NUM_6:
  case GPIO_NUM_7:
  case GPIO_NUM_8:
  case GPIO_NUM_9:
  case GPIO_NUM_10:
  case GPIO_NUM_11:
  case GPIO_NUM_12:
  case GPIO_NUM_13:
  case GPIO_NUM_14:
  case GPIO_NUM_15:
  case GPIO_NUM_16:
  case GPIO_NUM_17:
  case GPIO_NUM_18:
  case GPIO_NUM_19:

  case GPIO_NUM_21:
  case GPIO_NUM_22:
  case GPIO_NUM_23:

  case GPIO_NUM_25:
  case GPIO_NUM_26:
  case GPIO_NUM_27:

  case GPIO_NUM_32:
  case GPIO_NUM_33:
    if (cli && (gpio_number == RFOUT_GPIO || gpio_number == RFIN_GPIO)) {
      break;
    }
    return true;
    break;

  default:
    break;

  }
  return false;
}

void IRAM_ATTR mcu_put_txPin(bool dat) {
  GPIO_OUTPUT_SET(RFOUT_GPIO, dat);
}

bool IRAM_ATTR mcu_get_rxPin() {
    return GPIO_INPUT_GET(RFIN_GPIO);
}

void
setup_pin(void) {
  int i;

  for (i = 0; i < CONFIG_GPIO_SIZE; ++i) {
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

bool mcu_get_buttonUpPin(void) {
  return false;
}

bool mcu_get_buttonDownPin(void) {
  return false;
}

bool mcu_get_buttonPin(void) {
  bool val = gpio_get_level(BUTTON_GPIO);
  return val == 0;  // 0 means pressed
}


const char* mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {

  if (state == PIN_OUTPUT || state == PIN_INPUT || state == PIN_INPUT_PULLUP) {
    bool pullup = state == PIN_INPUT_PULLUP;

    if (state != PIN_OUTPUT) {
      GPIO_DIS_OUTPUT(GPIO_ID_PIN(gpio_number));
    }

    switch (gpio_number) {
    case GPIO_NUM_3:
    case GPIO_NUM_4:
    case GPIO_NUM_5:
    case GPIO_NUM_6:
    case GPIO_NUM_7:
    case GPIO_NUM_8:
    case GPIO_NUM_9:
    case GPIO_NUM_10:
    case GPIO_NUM_11:
    case GPIO_NUM_12:
    case GPIO_NUM_13:
    case GPIO_NUM_14:
    case GPIO_NUM_15:
    case GPIO_NUM_16:
    case GPIO_NUM_17:
    case GPIO_NUM_18:
    case GPIO_NUM_19:

    case GPIO_NUM_21:
    case GPIO_NUM_22:
    case GPIO_NUM_23:

    case GPIO_NUM_25:
    case GPIO_NUM_26:
    case GPIO_NUM_27:

    case GPIO_NUM_32:
    case GPIO_NUM_33:
      if (state == PIN_OUTPUT) {
        gpio_set_direction(gpio_number, GPIO_MODE_OUTPUT);
    } else if (state == PIN_INPUT) {
        gpio_set_direction(gpio_number, GPIO_MODE_INPUT);
    }

      break;

    case GPIO_NUM_34:
    case GPIO_NUM_35:
    case GPIO_NUM_36:
    case GPIO_NUM_37:
    case GPIO_NUM_38:
    case GPIO_NUM_39:
        if (state == PIN_OUTPUT) {
          return "GPIOs 34..39 can only used for input";
      } else if (state == PIN_INPUT) {
          gpio_set_direction(gpio_number, GPIO_MODE_INPUT);
      }

      break;

     default:
      return "gpio number can not be used";
    }

  } else if (state == PIN_SET) {
    gpio_set_level(gpio_number, 1);

  } else if (state == PIN_CLEAR) {
    gpio_set_level(gpio_number, 0);

  } else if (state == PIN_TOGGLE) {
    gpio_set_level(gpio_number, !gpio_get_level(gpio_number)); // TODO: works only for GPIO_MODE_INPUT_OUTPUTINPUT_OUTPUT

  } else if (state == PIN_READ) {
    *result = gpio_get_level(GPIO_ID_PIN(gpio_number)) ? PIN_SET : PIN_CLEAR;

  } else {
    return "not implemented";
  }

  return NULL;

}

const char *mcu_access_pin(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {
#ifdef DISTRIBUTION
  if (state != PIN_READ)
    switch (gpio_number) {
   case 12:
   case 13:
   case 14:
   case 15:
     break;

   default:
     return "gpio number not writable (use 12, 13, 14, 15)";  // FIXME:
   }
#endif

    return mcu_access_pin2(gpio_number, result, state);
}
