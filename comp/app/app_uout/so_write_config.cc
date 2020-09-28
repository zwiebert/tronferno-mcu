/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#include "app/config/options.hh"
#include "so_out.h"
#include "so_print.h"
#include "app/uout/callbacks.h"
#include "app/common.h"
#include "app/firmware.h"
#include "app/rtc.h"
#include "app/settings/config.h"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "app/uout/status_output.h"
#include "fernotron/astro.h"
#include <app/uout/so_config.h>

//#include "misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define D(x)

bool so_output_message2(const struct TargetDesc &td, so_msg_t mt, const void *arg);

void soCfg_BAUD(const struct TargetDesc &td) {
#ifndef MCU_ESP32
  td.so().print(gk(SO_CFG_BAUD), config_read_baud());
#endif
}

void soCfg_RTC(const struct TargetDesc &td) {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    td.so().print(gk(SO_CFG_RTC), buf);
  }
}

void soCfg_CU(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_CU), cfg_getCuId(), 16);
}

void soCfg_NETWORK(const struct TargetDesc &td) {
#ifdef USE_NETWORK
  td.so().print(gk(SO_CFG_NETWORK),cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ(const struct TargetDesc &td) {
#ifdef POSIX_TIME
  char buf[64];
  td.so().print(gk(SO_CFG_TZ),config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_LONGITUDE(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_LONGITUDE), config_read_longitude(), 5);
}

void soCfg_LATITUDE(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_LATITUDE), config_read_latitude(), 5);
}

void soCfg_TIMEZONE(const struct TargetDesc &td) {
#ifndef POSIX_TIME
  td.so().print(gk(SO_CFG_TIMEZONE), config_read_timezone(), 5);
#endif
}

void soCfg_DST(const struct TargetDesc &td) {
#ifdef MDR_TIME
  {
    enum dst geo_dst = config_read_dst();
    const char *dst = (geo_dst == dstEU ? "eu" : (geo_dst == dstNone ? "0" : "1"));
    td.so().print(gk(SO_CFG_DST), dst);
  }
#endif
}

void soCfg_GM_USED(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_GM_USED),config_read_used_members(), 16);
}

void soCfg_GPIO_RFOUT(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_GPIO_RFOUT),config_read_rfout_gpio());
}

void soCfg_GPIO_RFIN(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_GPIO_RFIN),config_read_rfin_gpio());
}

void soCfg_GPIO_SETBUTTON(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_GPIO_SETBUTTON),config_read_setbutton_gpio());
}

void soCfg_GPIO_PIN(const struct TargetDesc &td, const int gpio_number) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    char key[10] = "gpio";
    STRCPY(key + 4, itoa(gpio_number, buf, 10));
    char ps[2] = "x";
    if (is_gpio_number_usable(gpio_number, true)) {
      enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
      ps[0] = pin_state_args[mps];
    }
    td.so().print(key, ps);
  }
#endif
}

void soCfg_GPIO_MODES(const struct TargetDesc &td) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    int gpio_number;
    char key[10] = "gpio";
    char pin_level_args[] = "mhl";
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      STRCPY(key + 4, itoa(gpio_number, buf, 10));
      char ps[3] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        if (mps != PIN_DEFAULT) {
          enum mcu_pin_level mpl = pin_getPinLevel(gpio_number);
          ps[0] = pin_mode_args[mps];
          ps[1] = pin_level_args[3 - mpl];
          td.so().print(key, ps);
        }
      }

    }
  }
#endif
}

void soCfg_GPIO_MODES_AS_STRING(const struct TargetDesc &td) {
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
    td.so().print("gpio", val);
  }
#endif
}

void soCfg_ASTRO_CORRECTION(const struct TargetDesc &td) {
  td.so().print(gk(SO_CFG_ASTRO_CORRECTION),config_read_astro_correction());
}

void soCfg_begin(const struct TargetDesc &td) {
  td.so().x_open("config");
}

void soCfg_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void so_output_message(const struct TargetDesc &td, so_msg_t mt, const void *arg) {

  if (so_output_message2(td, mt, arg))
    return;

  switch (mt) {

  /////////////////////////////////////////////////////////////////////////////////
  case SO_CFG_all: {
    for (int i = SO_CFG_begin + 1; i < SO_CFG_end; ++i) {
      so_output_message(td, static_cast<so_msg_t>(i), NULL);
    }
  }
    break;
  case SO_CFG_begin:
    soCfg_begin(td);
    break;
  case SO_CFG_CU:
    soCfg_CU(td);
    break;
  case SO_CFG_BAUD:
    soCfg_BAUD(td);
    break;
  case SO_CFG_RTC:
    soCfg_RTC(td);
    break;
  case SO_CFG_NETWORK:
    soCfg_NETWORK(td);
    break;
  case SO_CFG_TZ:
    soCfg_TZ(td);
    break;
  case SO_CFG_LONGITUDE:
    soCfg_LONGITUDE(td);
    break;
  case SO_CFG_LATITUDE:
    soCfg_LATITUDE(td);
    break;
  case SO_CFG_TIMEZONE:
    soCfg_TIMEZONE(td);
    break;
  case SO_CFG_DST:
    soCfg_DST(td);
    break;
  case SO_CFG_GM_USED:
    soCfg_GM_USED(td);
    break;
  case SO_CFG_GPIO_RFOUT:
    soCfg_GPIO_RFOUT(td);
    break;
  case SO_CFG_GPIO_RFIN:
    soCfg_GPIO_RFIN(td);
    break;
  case SO_CFG_GPIO_SETBUTTON:
    soCfg_GPIO_SETBUTTON(td);
    break;
  case SO_CFG_GPIO_PIN:
    soCfg_GPIO_PIN(td, *(int*) arg);
    break;
  case SO_CFG_GPIO_MODES:
    soCfg_GPIO_MODES(td);
    break;
  case SO_CFG_GPIO_MODES_AS_STRING:
    soCfg_GPIO_MODES_AS_STRING(td);
    break;
  case SO_CFG_ASTRO_CORRECTION:
    soCfg_ASTRO_CORRECTION(td);
    break;
  case SO_CFG_end:
    soCfg_end(td);
    break;

  default:
#ifndef DISTRIBUTION
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "internal_error:so_output_message() unhandled message: %d;\n", mt); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
#endif
    break;
  }
}
///////////////////////////////////////////////////////////////

