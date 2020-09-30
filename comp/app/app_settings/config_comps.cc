/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "app/settings/config.h"
#include "app/settings/config_defaults.h"
#include "config_kvs.h"
#include <fernotron/trx/fer_trx_api.hh>
#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"

#include <string.h>

#define CI(cb) static_cast<configItem>(cb)


#include "gpio/pin.h"
struct cfg_gpio* config_read_gpio(struct cfg_gpio *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_RFOUT_GPIO, c->out_rf);
    kvsR(i8, CB_RFIN_GPIO, c->in_rf);
    kvsR(i8, CB_SETBUTTON_GPIO, c->in_setButton);
#ifdef ACCESS_GPIO
    kvsRb(CB_GPIO, c->gpio);
#endif
    kvs_close(h);
  }
  return c;
}

void config_setup_gpio() {
  struct cfg_gpio c = { .out_rf = MY_RFOUT_GPIO, .in_rf = MY_RFIN_GPIO, .in_setButton = MY_SETBUTTON_GPIO };
  config_read_gpio(&c);
  setup_pin(&c);
}

int8_t config_read_rfout_gpio() {
  return config_read_item_i8(CI(CB_RFOUT_GPIO), MY_RFOUT_GPIO);
}
int8_t config_read_rfin_gpio() {
  return config_read_item_i8(CI(CB_RFIN_GPIO), MY_RFIN_GPIO);
}
int8_t config_read_setbutton_gpio() {
  return config_read_item_i8(CI(CB_SETBUTTON_GPIO), MY_SETBUTTON_GPIO);
}


#if 1
#include "app/cli/fernotron.h"
void config_setup_global() {
  kvshT h;
  C = config { .fer_centralUnitID = MY_FER_CENTRAL_UNIT_ID,
    .mcu_serialBaud = MY_MCU_UART_BAUD_RATE,
    .app_rtcAdjust = 0,
    .app_recv = recvTick,
    .app_transm = transmTick,
    .app_rtc = rtcAvrTime,
    .fer_usedMembers = MY_FER_GM_USE,
    .app_configPassword = {0},
    .app_expertPassword = {0},
};
  STRLCPY(C.app_configPassword, MY_APP_CONFIG_PASSWORD, sizeof C.app_configPassword);
  STRLCPY(C.app_expertPassword, MY_APP_EXPERT_PASSWORD, sizeof C.app_expertPassword);

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {

  //XXX-ignore kvsR(i8, CB_TRANSM, C.app_transm);
    kvsR(u32, CB_CUID, C.fer_centralUnitID);
    kvsR(u32, CB_USED_MEMBERS, C.fer_usedMembers);
    kvsR(u32, CB_BAUD, C.mcu_serialBaud);
    kvs_close(h);
  }
  C.fer_usedMemberMask.fromNibbleCounters(C.fer_usedMembers);
}
uint32_t config_read_used_members() {
  return  config_read_item_u32(CI(CB_USED_MEMBERS), MY_FER_GM_USE);
}

#endif


#if 1
#include "fernotron/astro.h"
double tz2offset(const char *tz);
struct cfg_astro* config_read_astro(struct cfg_astro *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
//#define kvsR(DT, cb, val)  do { val = kvs_get_##DT(h, cfg_key(cb), val, 0); } while(0)

    kvsRb(CB_LONGITUDE, c->geo_longitude);
    kvsRb(CB_LATITUDE, c->geo_latitude);
    kvsRead(i8, astroCorrection, CB_ASTRO_CORRECTION, c->astroCorrection);

#ifndef USE_POSIX_TIME
      kvsRb(CI(CB_TIZO), c->geo_timezone);
#else
    char tz[64] = MY_GEO_TZ;
    kvsRs(CI(CB_TZ), tz);
    c->geo_timezone = tz2offset(tz);
#endif
    kvs_close(h);
  }
  return c;
}
void config_setup_astro() {
  struct cfg_astro c = { .astroCorrection = acAverage, .geo_longitude = MY_GEO_LONGITUDE, .geo_latitude = MY_GEO_LATITUDE,
#ifndef USE_POSIX_TIME
      .geo_timezone = MY_GEO_TIMEZONE,
#endif
      };
  config_read_astro(&c);
  Fer_Trx_API::setup_astro(&c);
}
float config_read_longitude() {
  return config_read_item_f(CI(CB_LONGITUDE), MY_GEO_LONGITUDE);
}
float config_read_latitude() {
  return config_read_item_f(CI(CB_LATITUDE), MY_GEO_LATITUDE);
}
enum astroCorrection config_read_astro_correction() {
  return static_cast<astroCorrection>(config_read_item_i8(CI(CB_ASTRO_CORRECTION), acAverage));
}
#endif

#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
void config_setup_mqttAppClient() {
  config_setup_mqttClient();
  io_mqttApp_setup("tfmcu/");
}
#endif
