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
#include "fernotron_uout/fer_uo_publish.h"
#include "fernotron/fer_main.h"
#include "utils_misc/int_types.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "app_uout/status_output.h"
#include "fernotron_trx/astro.h"
#include <app_uout/so_config.h>
#include <cc1101_ook/trx.hh>

//#include "utils_misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define D(x)

void soCfg_BAUD(const struct TargetDesc &td) {
#ifndef MCU_ESP32
  td.so().print(settings_get_optKeyStr(CB_BAUD), config_read_baud());
#endif
}

void soCfg_RTC(const struct TargetDesc &td) {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    td.so().print(otok_strings[(int) otok::k_rtc], buf);
  }
}

void soCfg_CU(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_CUID), fer_config.cu, 16);
}

void soCfg_NETWORK(const struct TargetDesc &td) {
#ifdef CONFIG_APP_USE_NETWORK
  td.so().print(settings_get_optKeyStr(CB_NETWORK_CONNECTION), cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ(const struct TargetDesc &td) {
#ifdef CONFIG_APP_USE_POSIX_TIME
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_TZ), config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_LONGITUDE(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_LONGITUDE), config_read_longitude(), 2);
}

void soCfg_LATITUDE(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_LATITUDE), config_read_latitude(), 2);
}

void soCfg_TIMEZONE(const struct TargetDesc &td) {
#ifndef CONFIG_APP_USE_POSIX_TIME
  td.so().print(settings_get_optKeyStr(CB_TIMEZONE), config_read_timezone(), 5);
#endif
}

void soCfg_DST(const struct TargetDesc &td) {
#ifdef MDR_TIME
  {
    enum dst geo_dst = config_read_dst();
    const char *dst = (geo_dst == dstEU ? "eu" : (geo_dst == dstNone ? "0" : "1"));
    td.so().print(settings_get_optKeyStr(CB_DST), dst);
  }
#endif
}

void soCfg_GM_USED(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_USED_MEMBERS), config_read_used_members(), 16);
}

void soCfg_GPIO_RFOUT(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFOUT_GPIO), config_read_rfout_gpio());
}

void soCfg_GPIO_RFIN(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFIN_GPIO), config_read_rfin_gpio());
}

void soCfg_GPIO_SETBUTTON(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_SETBUTTON_GPIO), config_read_setbutton_gpio());
}

void soCfg_RF_TRX(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RF_TRX), cfg_args_rfTrx[config_read_rf_trx()]);
}

void soCfg_RF_REPEATER(const struct TargetDesc &td) {
  char buf[80];
  td.so().print(settings_get_optKeyStr(CB_RF_REPEATER), config_read_rf_repeater(buf, sizeof buf));
}

void soCfg_GPIO_RFSCK(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFSCK_GPIO), config_read_rfsck_gpio());
}

void soCfg_GPIO_RFMISO(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFMISO_GPIO), config_read_rfmiso_gpio());
}

void soCfg_GPIO_RFMOSI(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFMOSI_GPIO), config_read_rfmosi_gpio());
}

void soCfg_GPIO_RFSS(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_RFSS_GPIO), config_read_rfss_gpio());
}


// TODO: should this show the current registers or the saved ones (if none are saved, how to get the default ones?).
void soCfg_CC1101_CONFIG(const struct TargetDesc &td) {
  uint8_t regFile[48];
  size_t regFileSize = sizeof regFile;
  if (cc1101_ook_dump_config(regFile, &regFileSize)) {
    char rs[regFileSize * 2 + 1];
    for (int i = 0; i < regFileSize; ++i) {
      sprintf(&rs[i * 2], "%02x", regFile[i]);
    }
    td.so().print("cc1101-config", rs);
  }
}

void soCfg_CC1101_CONFIG_xxx(const struct TargetDesc &td) { // TODO:
  char cc1101_config[97];
  if (config_read_cc1101_config(cc1101_config, sizeof cc1101_config)) {
      td.so().print("cc1101-config", cc1101_config);
  } else {
    td.so().print("cc1101-config", "");
  }
}


void soCfg_GPIO_PIN(const struct TargetDesc &td, const int gpio_number) {
#ifdef CONFIG_APP_USE_GPIO_PINS
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
#ifdef CONFIG_APP_USE_GPIO_PINS
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
#ifdef CONFIG_APP_USE_GPIO_PINS
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
  td.so().print(settings_get_optKeyStr(CB_ASTRO_CORRECTION), config_read_astro_correction());
}

void soCfg_begin(const struct TargetDesc &td) {
  td.so().x_open("config");
}

void soCfg_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soCfg_all_net(const struct TargetDesc &td) {
  soCfg_NETWORK(td);
#ifdef CONFIG_APP_USE_WLAN
  soCfg_WLAN_SSID(td);
  soCfg_WLAN_PASSWORD(td);
#endif
#ifdef CONFIG_APP_USE_NTP
  soCfg_NTP_SERVER(td);
#endif
#ifdef CONFIG_APP_USE_LAN
  soCfg_LAN_PHY(td);
  soCfg_LAN_PWR_GPIO(td);
#endif
#ifdef CONFIG_APP_USE_MQTT
  soCfg_MQTT_ENABLE(td);
  soCfg_MQTT_URL(td);
  soCfg_MQTT_USER(td);
  soCfg_MQTT_PASSWORD(td);
  soCfg_MQTT_CLIENT_ID(td);
  soCfg_MQTT_ROOT_TOPIC(td);
#endif
#ifdef CONFIG_APP_USE_HTTP
  soCfg_HTTP_ENABLE(td);
  soCfg_HTTP_USER(td);
  soCfg_HTTP_PASSWORD(td);
#endif
}

void soCfg_all_misc(const struct TargetDesc &td) {
  soCfg_BAUD(td);
  soCfg_VERBOSE(td);
  soCfg_RF_TRX(td);
#if 0 // disabled, becuase reading resgisters via SPI is not reentrant. It did crash when accessing config from FHEM and fetch registers periodically from Web-Browser.
  soCfg_CC1101_CONFIG(td);
#endif
}

void soCfg_all_gpio(const struct TargetDesc &td) {
  soCfg_GPIO_RFOUT(td);
  soCfg_GPIO_RFIN(td);
  soCfg_GPIO_SETBUTTON(td);
  soCfg_GPIO_RFMISO(td);
  soCfg_GPIO_RFMOSI(td);
  soCfg_GPIO_RFSCK(td);
  soCfg_GPIO_RFSS(td);
  soCfg_GPIO_MODES(td);
  soCfg_GPIO_MODES_AS_STRING(td);
}

void soCfg_all_fer(const struct TargetDesc &td) {
  soCfg_CU(td);
  soCfg_GM_USED(td);
  soCfg_LONGITUDE(td);
  soCfg_LATITUDE(td);
  soCfg_ASTRO_CORRECTION(td);
}

void soCfg_all_time(const struct TargetDesc &td) {
  soCfg_RTC(td);
#ifdef CONFIG_APP_USE_POSIX_TIME
  soCfg_TZ(td);
#else
  soCfg_TIMEZONE(td);
  soCfg_DST(td);
#endif
}

typedef void (*soCfg_fnT)(const struct TargetDesc &td);

const soCfg_fnT soCfg_fns[] = {
  soCfg_all_time,
  soCfg_all_fer,
  soCfg_all_gpio,
  soCfg_all_misc,
  soCfg_all_net,
};

constexpr size_t soCfg_fnsCt = sizeof soCfg_fns / sizeof soCfg_fns[0];

void soCfg_all(const struct TargetDesc &td) {

  soCfg_BAUD(td);
  soCfg_VERBOSE(td);

  soCfg_all_fer(td);
  soCfg_all_time(td);
  soCfg_all_net(td);

  soCfg_all_gpio(td);
  soCfg_RF_TRX(td);
}

void soCfg_all_part(const struct TargetDesc &td, int part_num, int part_size) {
  const int start_idx = part_num * part_size;
  const int end_idx = MIN(start_idx + part_size, soCfg_fnsCt);

  for (int i = start_idx; i < end_idx; ++i) {
    if (auto fn = soCfg_fns[i]; fn)
      fn(td);
  }
}
