/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "config/config.h"
#include "config_defaults.h"
#include "config_kvs.h"

#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"


#if 1
#include "gpio/pin.h"
void config_setup_gpio() {
  kvshT h;
  struct cfg_gpio c = { .out_rf = MY_RFOUT_GPIO, .in_rf = MY_RFIN_GPIO, .in_setButton = MY_SETBUTTON_GPIO };

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_RFOUT_GPIO, c.out_rf);
    kvsR(i8, CB_RFIN_GPIO, c.in_rf);
    kvsR(i8, CB_SETBUTTON_GPIO, c.in_setButton);
#ifdef ACCESS_GPIO
    kvsRb(CB_GPIO, c.gpio);
#endif
    kvs_close(h);
  }
  setup_pin(&c);
}
#endif

#if 1
#include "app/fernotron.h"
void config_setup_global() {
  kvshT h;
  C = (config){   .fer_centralUnitID = MY_FER_CENTRAL_UNIT_ID,
    .mcu_serialBaud = MY_MCU_UART_BAUD_RATE,
    .app_rtcAdjust = 0,
    .app_recv = recvTick,
    .app_transm = transmTick,
    .app_rtc = rtcAvrTime,
    .fer_usedMembers = MY_FER_GM_USE,
    .app_configPassword = MY_APP_CONFIG_PASSWORD,
    .app_expertPassword = MY_APP_EXPERT_PASSWORD,
  #ifdef USE_NETWORK
    .network = MY_NETWORK_CONNECTION,
  #endif
};

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {

  //XXX-ignore kvsR(i8, CB_TRANSM, C.app_transm);
#ifdef USE_NETWORK
    kvsR(i8, CB_NETWORK_CONNECTION, C.network);
#endif

    kvsR(u32, CB_CUID, C.fer_centralUnitID);
    kvsR(u32, CB_USED_MEMBERS, C.fer_usedMembers);
    kvsR(u32, CB_BAUD, C.mcu_serialBaud);
    kvs_close(h);
  }
  gm_fromNibbleCounters(&C.fer_usedMemberMask, C.fer_usedMembers);
  FSB_PUT_DEVID(&default_sender, C.fer_centralUnitID);
}
#endif

#if defined USE_TCPS_TASK || defined USE_TCPS
#include "net/tcp_cli_server.h"
void config_setup_cliTcpServer() {
  struct cfg_tcps c = { .enable = true };
#if 0
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_VERBOSE, c.verbose);
    kvs_close(h);
  }
#endif
#ifdef USE_TCPS
 tcpCli_setup(&c);
#elif defined USE_TCPS_TASK
 tcpCli_setup_task(&c);
#endif
}
#endif

#if 1
#include "txtio/inout.h"
void config_setup_txtio() {
  kvshT h;
  struct cfg_txtio c = { .verbose = MY_VERBOSE };

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_VERBOSE, c.verbose);
    kvs_close(h);
  }
 txtio_setup(&c);
}
#endif

#ifdef USE_LAN
#include "net/ethernet.h"
void config_setup_ethernet() {
  kvshT h;
  struct cfg_lan c = { .phy = MY_LAN_PHY, .pwr_gpio = MY_LAN_PWR_GPIO, };

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_LAN_PHY, c.phy);
    kvsR(i8, CB_LAN_PWR_GPIO, c.pwr_gpio);
    kvs_close(h);
  }
  ethernet_setup(&c);
}
#endif

#ifdef USE_NTP
#include "net/ntp.h"
void config_setup_ntpClient() {
  kvshT h;
  struct cfg_ntp c = { .server = MY_NTP_SERVER };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_NTP_SERVER, c.server);
    kvs_close(h);
  }
  ntp_setup(&c);
}
#endif

#ifdef USE_WLAN
#include "net/wifistation.h"
void config_setup_wifiStation() {
  kvshT h;
  struct cfg_wlan c = { .SSID = MY_WIFI_SSID, .password = MY_WIFI_PASSWORD, };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_WIFI_SSID, c.SSID);
    kvsRs(CB_WIFI_PASSWD, c.password);
    kvs_close(h);
  }
  wifistation_setup(&c);
}
#endif

#if 1
#include "fernotron/astro.h"
double tz2offset(const char *tz);
void config_setup_astro() {
  kvshT h;
  struct cfg_astro c = { .geo_longitude = MY_GEO_LONGITUDE, .geo_latitude = MY_GEO_LATITUDE, .astroCorrection = acAverage,
#ifndef POSIX_TIME
      .geo_timezone = MY_GEO_TIMEZONE,
#endif
      };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRb(CB_LONGITUDE, c.geo_longitude);
    kvsRb(CB_LATITUDE, c.geo_latitude);
    kvsR(i8, CB_ASTRO_CORRECTION, c.astroCorrection);
#ifndef POSIX_TIME
      kvsRb(CB_TIZO, c.geo_timezone);
#else
    char tz[64] = MY_GEO_TZ;
    kvsRs(CB_TZ, tz);
    c.geo_timezone = tz2offset(tz);
#endif
    kvs_close(h);
  }
  astro_init_and_reinit(&c);
}
#endif

#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
void config_setup_mqttClient() {
  kvshT h;
  struct cfg_mqtt c = { .url = MY_MQTT_URL, .user = MY_MQTT_USER, .password = MY_MQTT_PASSWORD, .client_id = MY_MQTT_CLIENT_ID, .enable =
  MY_MQTT_ENABLE };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_MQTT_URL, c.url);
    kvsRs(CB_MQTT_USER, c.user);
    kvsRs(CB_MQTT_PASSWD, c.password);
    kvsRs(CB_MQTT_CLIENT_ID, c.client_id);
    kvsR(i8, CB_MQTT_ENABLE, c.enable);
    kvs_close(h);
  }
  io_mqttApp_setup(&c, "tfmcu/");
}
#endif

#ifdef USE_HTTP
#include "net/http/server/http_server.h"
void config_setup_httpServer() {
  kvshT h;
  struct cfg_http c = { .user = MY_HTTP_USER, .password = MY_HTTP_PASSWORD, .enable = MY_HTTP_ENABLE };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_HTTP_USER, c.user);
    kvsRs(CB_HTTP_PASSWD, c.password);
    kvsR(i8, CB_HTTP_ENABLE, c.enable);

    kvs_close(h);
  }
  hts_setup(&c);
}
#endif
