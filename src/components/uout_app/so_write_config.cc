/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include "so_out.h"
#include "so_print.h"
#include "uout_app/callbacks.h"
#include "app/common.h"
#include "app/firmware.h"
#include "app/rtc.h"
#include "config/config.h"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.h"
#include "uout_app/status_output.h"

#include <uout_app/so_config.h>

//#include "misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define D(x)

bool so_output_message2(so_msg_t mt, const void *arg);

void soCfg_BAUD() {
#ifndef MCU_ESP32
    so_out_x_reply_entry_l(SO_CFG_BAUD, config_read_baud());
#endif
}

void soCfg_RTC() {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    so_out_x_reply_entry_s(SO_CFG_RTC, buf);
  }
}

void soCfg_CU() {
  so_out_x_reply_entry_lx(SO_CFG_CU, cfg_getCuId());
}

void soCfg_NETWORK() {
#ifdef USE_NETWORK
  so_out_x_reply_entry_s(SO_CFG_NETWORK, cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ() {
#ifdef POSIX_TIME
  char buf[64];
  so_out_x_reply_entry_s(SO_CFG_TZ, config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_LONGITUDE() {
  so_out_x_reply_entry_f(SO_CFG_LONGITUDE, config_read_longitude(), 5);
}

void soCfg_LATITUDE() {
  so_out_x_reply_entry_f(SO_CFG_LATITUDE, config_read_latitude(), 5);
}

void soCfg_TIMEZONE() {
#ifndef POSIX_TIME
    so_out_x_reply_entry_f(SO_CFG_TIMEZONE, config_read_timezone(), 5);
#endif
}

void soCfg_DST() {
#ifdef MDR_TIME
  {
    enum dst geo_dst = config_read_dst();
    const char *dst = (geo_dst == dstEU ? "eu" : (geo_dst == dstNone ? "0" : "1"));
    so_out_x_reply_entry_s(SO_CFG_DST, dst);
  }
#endif
}

void soCfg_GM_USED() {
  so_out_x_reply_entry_lx(SO_CFG_GM_USED, config_read_used_members());
}

void soCfg_GPIO_RFOUT() {
  so_out_x_reply_entry_d(SO_CFG_GPIO_RFOUT, config_read_rfout_gpio());
}

void soCfg_GPIO_RFIN() {
  so_out_x_reply_entry_d(SO_CFG_GPIO_RFIN, config_read_rfin_gpio());
}

void soCfg_GPIO_SETBUTTON() {
  so_out_x_reply_entry_d(SO_CFG_GPIO_SETBUTTON, config_read_setbutton_gpio());
}

void soCfg_GPIO_PIN(const int gpio_number) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    char key[10] = "gpio";
    strcpy(key + 4, itoa(gpio_number, buf, 10));
    char ps[2] = "x";
    if (is_gpio_number_usable(gpio_number, true)) {
      enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
      ps[0] = pin_state_args[mps];
    }
    so_out_x_reply_entry_ss(key, ps);
  }
#endif
}

void soCfg_GPIO_MODES() {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    int gpio_number;
    char key[10] = "gpio";
    char pin_level_args[] = "mhl";
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      strcpy(key + 4, itoa(gpio_number, buf, 10));
      char ps[3] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        if (mps != PIN_DEFAULT) {
          enum mcu_pin_level mpl = pin_getPinLevel(gpio_number);
          ps[0] = pin_mode_args[mps];
          ps[1] = pin_level_args[3 - mpl];
          so_out_x_reply_entry_ss(key, ps);
        }
      }

    }
  }
#endif
}

void soCfg_GPIO_MODES_AS_STRING() {
#ifdef ACCESS_GPIO
  {
    int gpio_number;
    char val[CONFIG_GPIO_SIZE + 1];
    val[CONFIG_GPIO_SIZE] = '\0';
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        val[gpio_number] = pin_mode_args[mps];
      } else {
        val[gpio_number] = 'x';
      }
    }
    so_out_x_reply_entry_ss("gpio", val);
  }
#endif
}

void soCfg_ASTRO_CORRECTION() {
  so_out_x_reply_entry_l(SO_CFG_ASTRO_CORRECTION, config_read_astro_correction());
}

void soCfg_begin() {
  so_out_x_open("config");
}

void soCfg_end() {
  so_out_x_close();
}

void so_output_message(so_msg_t mt, const void *arg) {

  if (so_output_message2(mt, arg))
    return;

  switch (mt) {

  /////////////////////////////////////////////////////////////////////////////////
  case SO_CFG_all: {
    for (int i = SO_CFG_begin + 1; i < SO_CFG_end; ++i) {
      so_output_message(static_cast<so_msg_t>(i), NULL);
    }
  }
    break;
  case SO_CFG_begin:
    soCfg_begin();
    break;
  case SO_CFG_CU:
    soCfg_CU();
    break;
  case SO_CFG_BAUD:
    soCfg_BAUD();
    break;
  case SO_CFG_RTC:
    soCfg_RTC();
    break;
  case SO_CFG_NETWORK:
    soCfg_NETWORK();
    break;
  case SO_CFG_TZ:
    soCfg_TZ();
    break;
  case SO_CFG_LONGITUDE:
    soCfg_LONGITUDE();
    break;
  case SO_CFG_LATITUDE:
    soCfg_LATITUDE();
    break;
  case SO_CFG_TIMEZONE:
    soCfg_TIMEZONE();
    break;
  case SO_CFG_DST:
    soCfg_DST();
    break;
  case SO_CFG_GM_USED:
    soCfg_GM_USED();
    break;
  case SO_CFG_GPIO_RFOUT:
    soCfg_GPIO_RFOUT();
    break;
  case SO_CFG_GPIO_RFIN:
    soCfg_GPIO_RFIN();
    break;
  case SO_CFG_GPIO_SETBUTTON:
    soCfg_GPIO_SETBUTTON();
    break;
  case SO_CFG_GPIO_PIN:
    soCfg_GPIO_PIN(*(int*) arg);
    break;
  case SO_CFG_GPIO_MODES:
    soCfg_GPIO_MODES();
    break;
  case SO_CFG_GPIO_MODES_AS_STRING:
    soCfg_GPIO_MODES_AS_STRING();
    break;
  case SO_CFG_ASTRO_CORRECTION:
    soCfg_ASTRO_CORRECTION();
    break;
  case SO_CFG_end:
    soCfg_end();
    break;

  default:
#ifndef DISTRIBUTION
    io_puts("internal_error:so_output_message() unhandled message: "), io_putd(mt), io_putlf();
#endif
    break;
  }
}
///////////////////////////////////////////////////////////////

