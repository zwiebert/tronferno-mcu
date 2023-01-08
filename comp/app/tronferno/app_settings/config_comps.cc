/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_settings/config_defaults.h"
#include "config_kvs.h"
#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron_trx/raw/fer_radio_trx.h>
#include <fernotron/repeater/repeater.h>
#include <fernotron_uout/fer_uo_publish.h>
#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "cc1101_ook/spi.hh"
#include "cc1101_ook/trx.hh"

#include <string.h>
#include <utils_misc/cstring_utils.hh>

unsigned tfmcu_error_mask;
void tfmcu_put_error(tfmcu_errorT error_code, bool value) {
   PUT_BIT(tfmcu_error_mask, error_code, value);
}

#define CI(cb) static_cast<configItem>(cb)

#include "gpio/pin.h"
struct cfg_gpio* config_read_gpio(struct cfg_gpio *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_i8(h, CB_RFOUT_GPIO, c->out_rf);
    kvsRead_i8(h, CB_RFIN_GPIO, c->in_rf);
    kvsRead_i8(h, CB_SETBUTTON_GPIO, c->in_setButton);
#ifdef CONFIG_APP_USE_GPIO_PINS
    kvsRead_blob(h, CB_GPIO, c->gpio);
#endif
    kvs_close(h);

    const int8_t rfTrx = config_read_rf_trx();
    c->out_rf_inv = rfTrx == rfTrxCc1101 || rfTrx == rfTrxRxItx;
  }
  return c;
}

bool app_safe_mode;

int app_safeMode_increment(bool reset) {
  static bool once;
  if (reset && once)
    return 0;

  constexpr const char *SM_CT = "sm_counter";
  if (kvshT h = kvs_open("misc", kvs_READ_WRITE)) {
    if (reset) {
      kvs_set_i8(h, SM_CT, 1);
      kvs_commit(h);
      app_safe_mode = false;
      once = true;
      return 0;
    }

    auto smCt = kvs_get_i8(h, SM_CT, 0, 0) + 1;
    kvs_set_i8(h, SM_CT, smCt);
    kvs_commit(h);
    kvs_close(h);
    return smCt;
  }

  return -1;
}

void config_setup_gpio() {
  const auto trx = config_read_rf_trx();
  struct cfg_gpio c = { .gpio_in_use = 0, .out_rf = -1, .in_rf = -1, .in_setButton = -1 };

  if (!app_safe_mode)
    config_read_gpio(&c);

  if (trx == rfTrxNone) {
    c.out_rf = c.in_rf = -1;
  }

  if (trx == rfTrxCc1101) {
    struct cc1101_settings cc = { .sclk = -1, .ss = -1, .miso = -1, .mosi = -1 };
    config_read_cc1101(&cc);
    if (cc.sclk >= 0)
      SET_BIT64(c.gpio_in_use, cc.sclk);
    if (cc.ss >= 0)
      SET_BIT64(c.gpio_in_use, cc.ss);
    if (cc.miso >= 0)
      SET_BIT64(c.gpio_in_use, cc.miso);
    if (cc.mosi >= 0)
      SET_BIT64(c.gpio_in_use, cc.mosi);
  }

  setup_pin(&c);
}

int8_t config_read_rfout_gpio() {
  return config_read_item((CB_RFOUT_GPIO), -1);
}
int8_t config_read_rfin_gpio() {
  return config_read_item((CB_RFIN_GPIO), -1);
}
int8_t config_read_setbutton_gpio() {
  return config_read_item((CB_SETBUTTON_GPIO), -1);
}

int8_t config_read_rf_trx() {
  return config_read_item(CB_RF_TRX, rfTrxRxTx);
}

int8_t config_read_rfsck_gpio() {
  return config_read_item((CB_RFSCK_GPIO), -1);
}
int8_t config_read_rfmosi_gpio() {
  return config_read_item((CB_RFMOSI_GPIO), -1);
}
int8_t config_read_rfmiso_gpio() {
  return config_read_item((CB_RFMISO_GPIO), -1);
}
int8_t config_read_rfss_gpio() {
  return config_read_item((CB_RFSS_GPIO), -1);
}

const char* config_read_rf_repeater(char *d, unsigned d_size) {
  const char *s =  config_read_item(CB_RF_REPEATER, d, d_size, "");
  return s;
}

#if 1
void config_setup_global() {
  kvshT h;
  C = config { .mcu_serialBaud = CONFIG_APP_UART_BAUD_RATE, .app_rtcAdjust = 0, .app_recv = recvTick, .app_transm = transmTick, .app_rtc = rtcAvrTime,
      .app_configPassword = { 0 }, };
  STRLCPY(C.app_configPassword, CONFIG_APP_SETTINGS_PASSWORD, sizeof C.app_configPassword);

  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {

    //XXX-ignore kvsRead_i8(h, CB_TRANSM, C.app_transm);
    kvsRead_u32(h, CB_BAUD, C.mcu_serialBaud);
    kvs_close(h);
  }
}
uint32_t config_read_used_members() {
  return config_read_item(CB_USED_MEMBERS, CONFIG_APP_FER_GM_USE);
}

#endif

#if 1
#include "fernotron_trx/astro.h"
double tz2offset(const char *tz);
struct cfg_astro* config_read_astro(struct cfg_astro *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
//#define kvsR(DT, cb, val)  do { val = kvs_get_##DT(h, settings_get_kvsKey(cb), val, 0); } while(0)

    kvsRead_blob(h, CB_LONGITUDE, c->geo_longitude);
    kvsRead_blob(h, CB_LATITUDE, c->geo_latitude);
    kvsRead_i8(h, CB_ASTRO_CORRECTION, c->astroCorrection);

#ifndef CONFIG_APP_USE_POSIX_TIME
      kvsRead_blob(h, CI(CB_TIZO), c->geo_timezone);
#else
    char tz[64] = CONFIG_APP_GEO_TZ;
    kvsRead_charArray(h, CI(CB_TZ), tz);
    c->geo_timezone = tz2offset(tz);
#endif
    kvs_close(h);
  }
  return c;
}
void config_setup_astro() {
  struct cfg_astro c = { .astroCorrection = acAverage, .geo_longitude = MY_GEO_LONGITUDE, .geo_latitude = MY_GEO_LATITUDE,
#ifndef CONFIG_APP_USE_POSIX_TIME
      .geo_timezone = MY_GEO_TIMEZONE,
#endif
      };
  config_read_astro(&c);
  Fer_Trx_API::setup_astro(&c);
}
float config_read_longitude() {
  return config_read_item((CB_LONGITUDE), MY_GEO_LONGITUDE);
}
float config_read_latitude() {
  return config_read_item((CB_LATITUDE), MY_GEO_LATITUDE);
}
enum astroCorrection config_read_astro_correction() {
  return config_read_item((CB_ASTRO_CORRECTION), acAverage);
}
#endif

#ifdef CONFIG_APP_USE_MQTT
#include "app_mqtt/mqtt.h"
void config_setup_mqttAppClient() {
  config_setup_mqttClient();
  char buf[32];
  io_mqttApp_setup(config_read_mqtt_root_topic(buf, sizeof buf));
}
#endif

struct cc1101_settings* config_read_cc1101(struct cc1101_settings *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {

    kvsRead_i8(h, CB_RFSCK_GPIO, c->sclk);
    kvsRead_i8(h, CB_RFSS_GPIO, c->ss);
    kvsRead_i8(h, CB_RFMISO_GPIO, c->miso);
    kvsRead_i8(h, CB_RFMOSI_GPIO, c->mosi);
    kvs_close(h);
    c->enable = config_read_rf_trx() == rfTrxCc1101;
    return c;
  }
  return 0;
}

const char *config_read_cc1101_config(char *buf, size_t len) {
  if (len < 97)
    return NULL;

  if (const char *item = config_read_item(CB_CC1101_CONFIG, buf, len, ""); *item && strlen(item) == 96) {
    return item;
  }

  return NULL;
}



void config_setup_cc1101() {

  struct cc1101_settings c = { .sclk = -1, .ss = -1, .miso = -1, .mosi = -1 };
  config_read_cc1101(&c);
  cc1101_ook_spi_setup(&c);

  auto rfin_gpio = config_read_rfin_gpio();
  tfmcu_put_error(TFMCU_ERR_CC1101_RFIN_NOT_CONNECTED, rfin_gpio >= 0 && !cc1101_ook_gdo_isConnected(2, rfin_gpio));

  if (c.enable) {
    char cc1101_config[97];
    if (config_read_cc1101_config(cc1101_config, sizeof cc1101_config)) {
        cc1101_ook_updConfig_fromSparse(cc1101_config);
    }
  }
}

void config_setup_repeater() {
  char buf[80];
  config_read_rf_repeater(buf, sizeof buf);
  ferRep_setup(buf);
}

void config_ext_setup_txtio() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_pctChange = true;
  flags.evt.uo_evt_flag_rfMsgReceived = true;
  config_setup_txtio(&flags);

}

void config_ext_setup_cliTcpServer() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_pctChange = true;
  flags.evt.uo_evt_flag_rfMsgReceived = true;
#ifndef TEST_HOST
  config_setup_cliTcpServer(&flags);
#endif
}
