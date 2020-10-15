/*
 * pin.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#pragma once
#include "app_config/proj_app_cfg.h"
#include <stdbool.h>

//                                             d            i              q                     Q
typedef enum mcu_pin_mode { PIN_MODE_none = -1, PIN_DEFAULT=0, PIN_INPUT,  PIN_INPUT_OUTPUT_OD, PIN_INPUT_OUTPUT,
                            //  o               O
                            PIN_OUTPUT_OD, PIN_OUTPUT } mcu_pin_mode;

typedef enum mcu_pin_level { PIN_LEVEL_none = -1, PIN_FLOATING, PIN_LOW, PIN_HIGH, PIN_HIGH_LOW } mcu_pin_level;

typedef enum mcu_pin_state { PIN_STATE_none = -1, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE } mcu_pin_state;



const char* mcu_access_pin(int gpio_number, mcu_pin_state *result, mcu_pin_state state);
bool  is_gpio_number_usable(int gpio_number, bool cli);
void gpio_get_levels(unsigned long long gpio_mask, char *buf, int buf_size);



struct cfg_gpio {
  int8_t out_rf, in_rf, in_setButton;
#ifdef ACCESS_GPIO
  /* enum mcu_pin_mode */ uint8_t gpio[CONFIG_GPIO_SIZE];
#define gpioCfg_getPinMode(c,gpio_num) (mcu_pin_mode)((c)->gpio[(gpio_num)] & 0x3f)
#define gpioCfg_getPinLevel(c,gpio_num) (mcu_pin_level)((c)->gpio[(gpio_num)] >> 6)
#define gpioCfg_setPin(c,gpio_num, mode, level) (c)->gpio[(gpio_num)] = (mode | (level << 6))
#endif
};


void setup_pin(const struct cfg_gpio *c);

enum mcu_pin_mode pin_getPinMode(unsigned gpio_number);
enum mcu_pin_level pin_getPinLevel(unsigned gpio_number);
const char* pin_set_mode(int gpio_number, mcu_pin_mode mode, mcu_pin_level level);

void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();

bool mcu_get_buttonUpPin(void);
bool mcu_get_buttonDownPin(void);
bool mcu_get_buttonPin(void);

void pin_notify_input_change(void);
extern void (*gpio_INPUT_PIN_CHANGED_ISR_cb)();
