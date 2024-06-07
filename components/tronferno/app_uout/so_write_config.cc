/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include "app_config/options.hh"
#include "so_out.h"
#include "so_print.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "fernotron/fer_main.h"
#include "stdint.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"
#include "fernotron_trx/astro.h"
#include <app_uout/so_config.h>
#include <cc1101_ook/trx.hh>

#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <txtio/comp_glue.hh>

#include <net_http_server/comp_glue.hh>
#include <net/comp_glue.hh>
#include <net_mqtt_client/comp_glue.hh>

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifdef CONFIG_TF_UOUT_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "tf.uout"


void soCfg_BAUD(class UoutWriter &td) {
#ifndef MCU_ESP32
  td.so().print(comp_sett.get_optKeyStr(CB_BAUD), config_read_baud());
#endif
}

void soCfg_RTC(class UoutWriter &td) {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    td.so().print(otok_strings[(int) otok::k_rtc], buf);
  }
}

void soCfg_CU(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_CUID), fer_config.cu, 16);
}

void soCfg_NETWORK(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_NETWORK
  td.so().print(comp_sett.get_optKeyStr(CB_NETWORK_CONNECTION), cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_POSIX_TIME
  char buf[64];
  td.so().print(comp_sett.get_optKeyStr(CB_TZ), config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_LONGITUDE(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_LONGITUDE), config_read_longitude(), 2);
}

void soCfg_LATITUDE(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_LATITUDE), config_read_latitude(), 2);
}

void soCfg_TIMEZONE(class UoutWriter &td) {
#ifndef CONFIG_APP_USE_POSIX_TIME
  td.so().print(comp_sett.get_optKeyStr(CB_TIMEZONE), config_read_timezone(), 5);
#endif
}

void soCfg_GM_USED(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_USED_MEMBERS), config_read_used_members(), 16);
}

void soCfg_GPIO_RFOUT(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFOUT_GPIO), config_read_rfout_gpio());
}

void soCfg_GPIO_RFIN(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFIN_GPIO), config_read_rfin_gpio());
}

void soCfg_GPIO_SETBUTTON(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_SETBUTTON_GPIO), config_read_setbutton_gpio());
}

void soCfg_RF_TRX(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RF_TRX), cfg_args_rfTrx[config_read_rf_trx()]);
}

#ifdef CONFIG_APP_USE_REPEATER
void soCfg_RF_REPEATER(class UoutWriter &td) {
  char buf[80];
  td.so().print(comp_sett.get_optKeyStr(CB_RF_REPEATER), config_read_rf_repeater(buf, sizeof buf));
}
#endif

#ifdef CONFIG_APP_USE_RTC_AUTO_UPD
void soCfg_RTC_AUTO_UPD(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RTC_AUTO_UPD), config_read_rtc_auto_upd());
}
#endif

void soCfg_GPIO_RFSCK(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFSCK_GPIO), config_read_rfsck_gpio());
}

void soCfg_GPIO_RFMISO(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFMISO_GPIO), config_read_rfmiso_gpio());
}

void soCfg_GPIO_RFMOSI(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFMOSI_GPIO), config_read_rfmosi_gpio());
}

void soCfg_GPIO_RFSS(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_RFSS_GPIO), config_read_rfss_gpio());
}


// TODO: should this show the current registers or the saved ones (if none are saved, how to get the default ones?).
void soCfg_CC1101_CONFIG(class UoutWriter &td) {
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

void soCfg_CC1101_CONFIG_xxx(class UoutWriter &td) { // TODO:
  char cc1101_config[97];
  if (config_read_cc1101_config(cc1101_config, sizeof cc1101_config)) {
      td.so().print("cc1101-config", cc1101_config);
  } else {
    td.so().print("cc1101-config", "");
  }
}


void soCfg_GPIO_PIN(class UoutWriter &td, const int gpio_number) {
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

void soCfg_GPIO_MODES(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_GPIO_PINS
  {
    char buf[64];
    int gpio_number;
    char key[10] = "gpio";
    char pin_level_args[] = "mhl";
    for (gpio_number = 0; gpio_number < CONFIG_APP_GPIO_NUMBER_OF_GPIOS; ++gpio_number) {
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

void soCfg_GPIO_MODES_AS_STRING(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_GPIO_PINS
  {
    int gpio_number;
    char val[CONFIG_APP_GPIO_NUMBER_OF_GPIOS + 1];
    val[CONFIG_APP_GPIO_NUMBER_OF_GPIOS] = '\0';
    for (gpio_number = 0; gpio_number < CONFIG_APP_GPIO_NUMBER_OF_GPIOS; ++gpio_number) {
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

void soCfg_ASTRO_CORRECTION(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_ASTRO_CORRECTION), config_read_astro_correction());
}

void soCfg_begin(class UoutWriter &td) {
  td.so().x_open("config");
}

void soCfg_end(class UoutWriter &td) {
  td.so().x_close();
}

void soCfg_all_net(class UoutWriter &td, bool backup) {
  soCfg_NETWORK(td);
#ifdef CONFIG_APP_USE_WLAN
  soCfg_WLAN_SSID(td);
  if (!backup) soCfg_WLAN_PASSWORD(td);
#endif
#ifdef CONFIG_APP_USE_NTP
  soCfg_NTP_SERVER(td);
#endif
#ifdef CONFIG_APP_USE_LAN
  soCfg_LAN_PHY(td);
  soCfg_LAN_PWR_GPIO(td);
#endif
#ifdef CONFIG_APP_USE_MQTT
  soCfg_MQTT(td, backup);
#endif
#ifdef CONFIG_APP_USE_HTTP
  soCfg_HTTP_ENABLE(td);
  soCfg_HTTP_USER(td);
  if (!backup) soCfg_HTTP_PASSWORD(td);
#endif
}

void soCfg_all_misc(class UoutWriter &td, bool backup) {
  soCfg_BAUD(td);
  soCfg_VERBOSE(td);
  soCfg_RF_TRX(td);
#if 0 // disabled, becuase reading resgisters via SPI is not reentrant. It did crash when accessing config from FHEM and fetch registers periodically from Web-Browser.
  soCfg_CC1101_CONFIG(td);
#endif
#ifdef CONFIG_APP_USE_RTC_AUTO_UPD
  soCfg_RTC_AUTO_UPD(td);
#endif
}

void soCfg_all_gpio(class UoutWriter &td, bool backup) {
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

void soCfg_all_fer(class UoutWriter &td, bool backup) {
  soCfg_CU(td);
  soCfg_GM_USED(td);
  soCfg_LONGITUDE(td);
  soCfg_LATITUDE(td);
  soCfg_ASTRO_CORRECTION(td);
#ifdef CONFIG_APP_USE_REPEATER
if (backup) soCfg_RF_REPEATER(td);
#endif
}

void soCfg_all_time(class UoutWriter &td, bool backup) {
  if (!backup) soCfg_RTC(td);
#ifdef CONFIG_APP_USE_POSIX_TIME
  soCfg_TZ(td);
#else
  soCfg_TIMEZONE(td);
  soCfg_DST(td);
#endif
}

typedef void (*soCfg_fnT)(class UoutWriter &td, bool backup);

const soCfg_fnT soCfg_fns[] = {
  soCfg_all_time,
  soCfg_all_fer,
  soCfg_all_gpio,
  soCfg_all_misc,
  soCfg_all_net,
};

constexpr size_t soCfg_fnsCt = sizeof soCfg_fns / sizeof soCfg_fns[0];

void soCfg_all(class UoutWriter &td, bool backup) {

  soCfg_BAUD(td);
  soCfg_VERBOSE(td);

  soCfg_all_fer(td, backup);
  soCfg_all_time(td, backup);
  soCfg_all_net(td, backup);

  soCfg_all_gpio(td, backup);
  soCfg_RF_TRX(td);
}

void soCfg_all_part(class UoutWriter &td, int part_num, int part_size) {
  const int start_idx = part_num * part_size;
  const int end_idx = MIN(start_idx + part_size, soCfg_fnsCt);

  for (int i = start_idx; i < end_idx; ++i) {
    if (auto fn = soCfg_fns[i]; fn)
      fn(td, false);
  }
}
