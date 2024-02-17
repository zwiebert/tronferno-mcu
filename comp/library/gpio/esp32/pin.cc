/*
 * pin.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */


#include "rom/gpio.h"
#include "gpio/pin.h"

#include "txtio/inout.h"
#include "uout/uo_callbacks.h"
#include <utils_misc/int_macros.h>
#include "freertos/FreeRTOS.h"
#include <esp_intr_alloc.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include <hal/gpio_types.h>

#include "string.h"
#include <atomic>

#define printf ets_uart_printf

#define RFOUT_GPIO static_cast<gpio_num_t>(gpio_cfg->out_rf)
#define RFIN_GPIO static_cast<gpio_num_t>(gpio_cfg->in_rf)
#define BUTTON_GPIO static_cast<gpio_num_t>(gpio_cfg->in_setButton)

static struct cfg_gpio *gpio_cfg;

#define RESERVED_GPIO_APP (1ULL<<RFOUT_GPIO|1ULL<<RFIN_GPIO|1ULL<<BUTTON_GPIO|gpio_cfg->gpio_in_use)
#define RESERVED_GPIO_SPI (BIT64(6)|BIT64(7)|BIT64(8)|BIT64(9)|BIT64(10)|BIT64(11))

#define gpioUnUsuable (BIT64(0)|BIT64(1)|BIT64(2)|BIT64(20)|BIT64(24)|BIT64(28)|BIT64(29)|BIT64(30)|BIT64(31)|RESERVED_GPIO_SPI|RESERVED_GPIO_APP)
#define gpioUsable ((BIT64(GPIO_NUM_MAX)-1) & ~gpioUnUsuable)

uint64_t inputGpioUsable;
uint64_t outputGpioUsable;
static u64 pins_in_use, pins_not_cli;
static volatile std::atomic<uint64_t> pin_int_mask;

#define gpioUsableHigh

#ifdef CONFIG_APP_USE_GPIO_PINS
enum mcu_pin_mode pin_getPinMode(unsigned gpio_number) {
  if (gpio_number >= sizeof gpio_cfg->gpio)
    return PIN_MODE_none;
  return  gpioCfg_getPinMode(gpio_cfg, gpio_number);
}
enum mcu_pin_level pin_getPinLevel(unsigned gpio_number) {
  if (gpio_number >= sizeof gpio_cfg->gpio)
    return PIN_LEVEL_none;
  return  gpioCfg_getPinLevel(gpio_cfg, gpio_number);
}
#endif

void gpio_get_levels(uint64_t gpio_mask, char *buf, int buf_size) {
  int i;
  int buf_len = (buf_size > 40 ? 39 : buf_size - 1);
  buf[buf_len] = '\0';

  for (i = 0; i < buf_len; ++i) {
    if (BIT64(i) & gpioUsable) {
      buf[i] = gpio_get_level(static_cast<gpio_num_t>(i)) ? '1' : '0';
    } else {
      buf[i] = 'x';
    }
  }

}

bool gpio_isLevelReadable(int gpio_number) {
  if (gpio_number == BUTTON_GPIO && gpio_number != GPIO_NUM_NC)
    return true;

  return (gpioUsable & (1ULL << gpio_number)) != 0;
}

bool gpio_isLevelWritable(int gpio_number) {
  return (gpioUsable & (1ULL << gpio_number)) != 0;
}

bool is_gpio_number_usable(int gpio_number, bool cli) {
  return (gpioUsable & (1ULL << gpio_number)) != 0;
}

void IRAM_ATTR mcu_put_txPin(uint8_t level) {
  if (const auto pin = RFOUT_GPIO; pin >= 0) {
    level = !!level == !gpio_cfg->out_rf_inv;
    GPIO_OUTPUT_SET(pin, level);
  }
}

uint8_t IRAM_ATTR mcu_get_rxPin() {
  if (const auto pin = RFIN_GPIO; pin >= 0) {
    return GPIO_INPUT_GET(pin);
  }
  return 0;
}

bool mcu_get_buttonUpPin(void) {
  return false;
}

bool mcu_get_buttonDownPin(void) {
  return false;
}

static bool button_was_pressed;
bool mcu_button_was_pressed() {
  if (!button_was_pressed)
    return false;
  button_was_pressed = false;
  return true;
}

bool mcu_get_buttonPin(void) {
  bool val = gpio_get_level(BUTTON_GPIO);
  return val == 0;  // 0 means pressed
}

static const gpio_mode_t pin_mode_table[] = { GPIO_MODE_DISABLE, GPIO_MODE_INPUT, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_MODE_INPUT_OUTPUT, GPIO_MODE_OUTPUT_OD,
    GPIO_MODE_OUTPUT,

};

static bool pin_set_gpio_mode(gpio_num_t gpio_number, mcu_pin_mode mode, mcu_pin_level level) {
  gpio_mode_t gpio_mode = pin_mode_table[mode];

  if (gpio_mode != GPIO_MODE_DISABLE)
    gpio_pad_select_gpio(gpio_number);
  if (ESP_OK == gpio_set_direction(gpio_number, gpio_mode)) {
    if (gpio_mode == GPIO_MODE_INPUT) {
      gpio_pull_mode_t pm = level == PIN_LOW ? GPIO_PULLDOWN_ONLY : level == PIN_HIGH ? GPIO_PULLUP_ONLY :
                            level == PIN_HIGH_LOW ? GPIO_PULLUP_PULLDOWN : GPIO_FLOATING;
      gpio_set_pull_mode(gpio_number, pm);
    } else if (gpio_mode != GPIO_MODE_DISABLE) {
      gpio_set_level(gpio_number, level == PIN_HIGH);
    }
  }
  return true;
}

const char* pin_set_mode_int(int ngpio_number, mcu_pin_mode mode, mcu_pin_level level) {
  auto gpio_number = static_cast<gpio_num_t>(ngpio_number);

  if (gpio_number == GPIO_NUM_NC)
    return 0;
  if (mode != PIN_DEFAULT && GET_BIT64(pins_in_use, gpio_number))
    return "Pin is already in use";

  PUT_BIT64(pins_in_use, gpio_number, mode != PIN_DEFAULT);

  switch (gpio_number) {
  case GPIO_NUM_2:
  case GPIO_NUM_3:
  case GPIO_NUM_4:
  case GPIO_NUM_5:
#if 0 // used by SPI_FLASH
  case GPIO_NUM_6:
  case GPIO_NUM_7:
  case GPIO_NUM_8:
  case GPIO_NUM_9:
  case GPIO_NUM_10:
  case GPIO_NUM_11:
#endif
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
    pin_set_gpio_mode(gpio_number, mode, level);
    break;

  case GPIO_NUM_34:
  case GPIO_NUM_35:
  case GPIO_NUM_36:
  case GPIO_NUM_37:
  case GPIO_NUM_38:
  case GPIO_NUM_39:
    if (mode > PIN_INPUT || (level != PIN_FLOATING && mode != PIN_DEFAULT))
      return "GPIOs 34..39 can only used for input (w/o pull)";

    pin_set_gpio_mode(gpio_number, mode, level);
    break;

  default:
    return "gpio number can not be used";
  }
  return NULL;
}

const char* pin_set_mode(int ngpio_number, mcu_pin_mode mode, mcu_pin_level level) {
  auto gpio_number = static_cast<gpio_num_t>(ngpio_number);
  if (GET_BIT64(pins_not_cli, gpio_number))
    return "this gpio cannot be set from CLI";
  return pin_set_mode_int(gpio_number, mode, level);
}

static const char* mcu_access_pin2(gpio_num_t gpio_number, mcu_pin_state *result, mcu_pin_state state) {
  if (gpio_number == -1)
    return 0;

  if (state == PIN_SET) {
    gpio_set_level(gpio_number, 1);

  } else if (state == PIN_CLEAR) {
    gpio_set_level(gpio_number, 0);

  } else if (state == PIN_TOGGLE) {
    gpio_set_level(gpio_number, !gpio_get_level(gpio_number));

  } else if (state == PIN_READ) {
    *result = gpio_get_level(gpio_number) ? PIN_SET : PIN_CLEAR;

  } else {
    return "not implemented";
  }

  return NULL;

}

const char* mcu_access_pin(int ngpio_number, mcu_pin_state *result, mcu_pin_state state) {
  auto gpio_number = static_cast<gpio_num_t>(ngpio_number);
  if (state != PIN_READ && GET_BIT64(pins_not_cli, gpio_number))
    return "this gpio cannot be set from CLI";

  return mcu_access_pin2(gpio_number, result, state);
}

void (*gpio_INPUT_PIN_CHANGED_ISR_cb)();

void pin_notify_input_change() {
  uint64_t mask = pin_int_mask;
  pin_int_mask = 0;
  for (uint8_t i = 0; mask; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;
    bool level = gpio_get_level(static_cast<gpio_num_t>(i));
    so_arg_pch_t a { i, level };
    uoCb_publish_pinChange(a);
  }
}

void pin_input_isr_handler(void *args) {
  gpio_num_t gpio_num = static_cast<gpio_num_t>(reinterpret_cast<size_t>(args));
  SET_BIT64(pin_int_mask, gpio_num);
  if (gpio_num == BUTTON_GPIO) {
    if (gpio_get_level(BUTTON_GPIO) == 0)
      button_was_pressed = true;
  }
  if (gpio_INPUT_PIN_CHANGED_ISR_cb)
    gpio_INPUT_PIN_CHANGED_ISR_cb();
}

void pin_setup_input_handler(gpio_num_t gpio_num) {
  static bool isr_init;
  if (gpio_num == -1) {
    if (isr_init) {
      gpio_uninstall_isr_service();
      isr_init = 0;
    }
    return;
  }
  if (isr_init || (isr_init = (ESP_OK == gpio_install_isr_service(ESP_INTR_FLAG_EDGE)))) {
    gpio_set_intr_type(gpio_num, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(gpio_num, pin_input_isr_handler, (void*) gpio_num);
  }
}

void setup_pin(const struct cfg_gpio *c) {
  if (!gpio_cfg) {
    gpio_cfg = static_cast<cfg_gpio*>(calloc(1, sizeof *gpio_cfg));
  } else {
    // clear all gpio
    pin_set_mode_int(RFOUT_GPIO, PIN_DEFAULT, gpio_cfg->out_rf_inv ? PIN_HIGH : PIN_LOW);
    pin_set_mode_int(RFIN_GPIO, PIN_DEFAULT, PIN_FLOATING);
    pin_set_mode_int(BUTTON_GPIO, PIN_DEFAULT,  BUTTON_GPIO < 34 ? PIN_HIGH : PIN_FLOATING);
    pin_setup_input_handler(GPIO_NUM_NC);
    pins_in_use = 0;
  }
  memcpy(gpio_cfg, c, sizeof *gpio_cfg);

  inputGpioUsable = gpioUsable;
  outputGpioUsable = gpioUsable;
  pins_in_use = c->gpio_in_use;

  pin_set_mode_int(RFOUT_GPIO, PIN_OUTPUT, gpio_cfg->out_rf_inv ? PIN_HIGH : PIN_LOW);
  pin_set_mode_int(RFIN_GPIO, PIN_INPUT, PIN_FLOATING);
  pin_set_mode_int(BUTTON_GPIO, PIN_INPUT, BUTTON_GPIO < 34 ? PIN_HIGH : PIN_FLOATING);
  pins_not_cli = pins_in_use;

  if (BUTTON_GPIO != GPIO_NUM_NC) {
    pin_setup_input_handler(static_cast<gpio_num_t>(BUTTON_GPIO));
  }

#ifdef CONFIG_APP_USE_GPIO_PINS
  for (int i = 0; i < CONFIG_APP_GPIO_NUMBER_OF_GPIOS; ++i) {
    if (gpioCfg_getPinMode(gpio_cfg, i) == PIN_DEFAULT)
      continue;
    pin_set_mode(i, gpioCfg_getPinMode(gpio_cfg, i), gpioCfg_getPinLevel(gpio_cfg, i));
    if (auto pm = gpioCfg_getPinMode(gpio_cfg, i); pm == PIN_INPUT || pm == PIN_INPUT_OUTPUT || pm == PIN_INPUT_OUTPUT_OD) {
      pin_setup_input_handler(static_cast<gpio_num_t>(i));
    }

  }
#endif

      }

