/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "user_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <ctype.h>
#include "config/config.h"
#include "misc/int_macros.h"

#define D(x) 

#define CFG_NAMESPACE "Tronferno"
#define CFG_KEY "global.C"

config C = {
  .fer_centralUnitID = MY_FER_CENTRAL_UNIT_ID,
  .mcu_serialBaud = MY_MCU_UART_BAUD_RATE,
  .geo_longitude = MY_GEO_LONGITUDE,
  .geo_latitude = MY_GEO_LATITUDE,
  .geo_timezone = MY_GEO_TIMEZONE,
  0, // app_rtcAdjust
  recvTick, // recv
  transmTick,// transm
  rtcAvrTime,//rtc
  vrbNone,  //verboseOutput
  .fer_usedMembers = MY_FER_GM_USE,
#ifdef USE_WLAN
  .wifi_SSID = MY_WIFI_SSID,
  .wifi_password = MY_WIFI_PASSWORD,
#endif
  .app_configPassword = MY_APP_CONFIG_PASSWORD,
  .app_expertPassword = MY_APP_EXPERT_PASSWORD,
#ifdef ACCESS_GPIO
  .gpio =  { 0, },
#endif
#ifdef POSIX_TIME
  .geo_tz = MY_GEO_TZ,
#endif
#ifdef USE_MQTT
  .mqtt_url = MY_MQTT_URL,
  .mqtt_user = MY_MQTT_USER,
  .mqtt_password = MY_MQTT_PASSWORD,
  MY_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  .http_user = MY_HTTP_USER,
  .http_password = MY_HTTP_PASSWORD,
  .http_enable = MY_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  .ntp_server = MY_NTP_SERVER,
#endif
#ifdef USE_NETWORK
  .network = MY_NETWORK_CONNECTION,
#endif
#ifdef USE_LAN
  .lan_phy = MY_LAN_PHY,
  .lan_pwr_gpio = MY_LAN_PWR_GPIO,
#endif
  .astroCorrection = acAverage,
};

#ifdef POSIX_TIME
static double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p=tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

void cfg_tz2timezone(void) {
  C.geo_timezone = tz2offset(C.geo_tz);
}
#endif

#ifdef CONFIG_DICT

static void nvsStr(nvs_handle handle, const char *key, void *dst, size_t dst_len, bool save) {
    if (save) {
      nvs_set_str(handle, key, dst);
    } else {
      nvs_get_str(handle, key, dst, &dst_len);
    }
}
static void nvsBlob(nvs_handle handle, const char *key, void *dst, size_t dst_len, bool save) {
    if (save) {
      nvs_set_blob(handle, key, dst, dst_len);
    } else {
      nvs_get_blob(handle, key, dst, &dst_len);
    }
}

#define nvs_s(mbit, key,val) if (GET_BIT(mask,mbit)) { nvsStr(handle, key, val, sizeof val, write); }
#define nvs_b(mbit, key,val) if (GET_BIT(mask,mbit)) { nvsBlob(handle, key, &val, sizeof val, write); }
#define nvs_i8(mbit, key,val) if (GET_BIT(mask,mbit)) { if (write) { nvs_set_i8(handle, key, val); } else { i8 temp; if (ESP_OK == nvs_get_i8(handle, key, &temp)) val = temp;}}
#define nvs_u32(mbit, key,val) if (GET_BIT(mask,mbit)) { if (write) { nvs_set_u32(handle, key, val); } else { nvs_get_u32(handle, key, &val); }}

static void rw_config(nvs_handle handle, u32 mask, bool write) {
#ifdef USE_WLAN
  nvs_s(CB_WIFI_SSID, "C_WIFI_SSID", C.wifi_SSID);
  nvs_s(CB_WIFI_PASSWD, "C_WIFI_PASSWD", C.wifi_password);
#endif
#ifdef USE_NTP
  nvs_s(CB_NTP_SERVER, "C_NTP_SERVER", C.ntp_server);
#endif
  nvs_s(CB_CFG_PASSWD, "C_CFG_PASSWD", C.app_configPassword);
  nvs_b(CB_LONGITUDE, "C_LONGITUDE", C.geo_longitude);
  nvs_b(CB_LATITUDE, "C_LATITUDE", C.geo_latitude);
#ifndef POSIX_TIME
    nvs_s(CB_TIZO, "C_TIZO", C.geo_timezone);
#else
  nvs_s(CB_TZ, "C_TZ", C.geo_tz);
  if (!write)
    cfg_tz2timezone();
#endif
  nvs_s(CB_GPIO, "C_GPIO", C.gpio);
#ifdef USE_MQTT
  nvs_s(CB_MQTT_URL, "C_MQTT_URL", C.mqtt_url);
  nvs_s(CB_MQTT_USER, "C_MQTT_USER", C.mqtt_user);
  nvs_s(CB_MQTT_PASSWD, "C_MQTT_PASSWD", C.mqtt_password);
  nvs_i8(CB_MQTT_ENABLE, "C_MQTT_ENABLE", C.mqtt_enable);
#endif
#ifdef USE_HTTP
  nvs_s(CB_HTTP_USER, "C_HTTP_USER", C.http_user);
  nvs_s(CB_HTTP_PASSWD, "C_HTTP_PASSWD", C.http_password);
  nvs_i8(CB_HTTP_ENABLE, "C_HTTP_ENABLE", C.http_enable);
#endif
  nvs_i8(CB_RECV, "C_RECEIVER", C.app_recv);
  nvs_i8(CB_TRANSM, "C_TRANSM", C.app_transm);
  nvs_i8(CB_VERBOSE, "C_VERBOSE", C.app_verboseOutput);
#ifdef USE_NETWORK
  nvs_i8(CB_NETWORK_CONNECTION, "C_NW_CONN", C.network);
#endif
#ifdef USE_LAN
  nvs_i8(CB_LAN_PHY, "C_LAN_PHY", C.lan_phy);
  nvs_i8(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", C.lan_pwr_gpio);
#endif

  nvs_i8(CB_ASTRO_CORRECTION, "C_AST_COR", C.astroCorrection);
  nvs_u32(CB_CUID, "C_CUID", C.fer_centralUnitID);
  nvs_u32(CB_USED_MEMBERS, "C_GMU", C.fer_usedMembers);
  nvs_u32(CB_BAUD, "C_BAUD", C.mcu_serialBaud);
}

void read_config(u32 mask) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &handle)) == ESP_OK) {
    rw_config(handle, mask, false);
    nvs_close(handle);
  }

}

void save_config(u32 mask) {
  esp_err_t err = 0;
  nvs_handle handle;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &handle)) == ESP_OK) {
    rw_config(handle, mask, true);
    nvs_commit(handle);
    nvs_close(handle);
  } else {
    D(ets_printf("error: cannot open config in nvs (err=%x)\n", err));
  }
}

void config_setup() {
  read_config(~0);
#ifdef USE_NETWORK
  // for old users without network configuration, do not start with WLAN AP by default
  if (C.wifi_SSID[0] != '\0') {
    C.network = MY_NETWORK_CONNECTION_OLD_USERS;
    read_config_item(CB_NETWORK_CONNECTION);
  }
#endif
}
#endif
