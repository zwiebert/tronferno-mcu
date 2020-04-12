/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"


#include "config/config.h"
#include "config_defaults.h"

#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"

#define D(x) 
#ifdef MCU_ESP32
#define CFG_NAMESPACE "Tronferno"
#else
#define CFG_NAMESPACE "config"
#endif
#define CFG_KEY "global.C"

#ifdef CONFIG_DICT

static void nvsStr(void *handle, const char *key, void *dst, size_t dst_len, bool save) {
  if (save) {
    kvs_rw_str(handle, key, dst, dst_len, true);
  } else {
    dst_len = kvs_rw_str(handle, key, dst, dst_len, false);
  }
}

static void nvsBlob(void *handle, const char *key, void *dst, size_t dst_len, bool save) {
    if (save) {
      kvs_rw_blob(handle, key, dst, dst_len, true);
    } else {
      dst_len = kvs_rw_blob(handle, key, dst, dst_len, false);
    }
}

#define nvs_s(mbit, key,val) if (GET_BIT(mask,mbit)) { nvsStr(handle, key, val, sizeof val, write); }
#define nvs_b(mbit, key,val) if (GET_BIT(mask,mbit)) { nvsBlob(handle, key, &val, sizeof val, write); }

#define nvs_dt(DT, mbit, key, val) if (GET_BIT(mask,mbit)) { if (write) { kvs_set_##DT(handle, key, val); } else { val = kvs_get_##DT(handle, key, val, 0); } }
#define nvs_i8(mbit, key,val) nvs_dt(i8, mbit, key, val)
#define nvs_u32(mbit, key,val) nvs_dt(u32, mbit, key, val)

static void rw_config(void *handle, u32 mask, bool write) {
#ifdef CONFIG_IGORE_MASK
  mask &= ~CONFIG_IGORE_MASK;
#endif

#ifdef USE_WLAN
  nvs_s(CB_WIFI_SSID, "C_WIFI_SSID", C.wifi.SSID);
  nvs_s(CB_WIFI_PASSWD, "C_WIFI_PASSWD", C.wifi.password);
#endif
#ifdef USE_NTP
  nvs_s(CB_NTP_SERVER, "C_NTP_SERVER", C.ntp.server);
#endif
  nvs_s(CB_CFG_PASSWD, "C_CFG_PASSWD", C.app_configPassword);
  nvs_b(CB_LONGITUDE, "C_LONGITUDE", C.astro.geo_longitude);
  nvs_b(CB_LATITUDE, "C_LATITUDE", C.astro.geo_latitude);
#ifndef POSIX_TIME
    nvs_b(CB_TIZO, "C_TIZO", C.astro.geo_timezone);
    nvs_i8(CB_DST, "C_DST", C.geo_dST);
#else
  nvs_s(CB_TZ, "C_TZ", C.geo_tz);
  if (!write)
    cfg_tz2timezone();
#endif
#ifdef ACCESS_GPIO
  nvs_s(CB_GPIO, "C_GPIO", C.gpio);
#endif
#ifdef USE_MQTT
  nvs_s(CB_MQTT_URL, "C_MQTT_URL", C.mqtt.url);
  nvs_s(CB_MQTT_USER, "C_MQTT_USER", C.mqtt.user);
  nvs_s(CB_MQTT_PASSWD, "C_MQTT_PASSWD", C.mqtt.password);
  nvs_i8(CB_MQTT_ENABLE, "C_MQTT_ENABLE", C.mqtt.enable);
#endif
#ifdef USE_HTTP
  nvs_s(CB_HTTP_USER, "C_HTTP_USER", C.http.user);
  nvs_s(CB_HTTP_PASSWD, "C_HTTP_PASSWD", C.http.password);
  nvs_i8(CB_HTTP_ENABLE, "C_HTTP_ENABLE", C.http.enable);
#endif
  nvs_i8(CB_RECV, "C_RECEIVER", C.app_recv);
  nvs_i8(CB_TRANSM, "C_TRANSM", C.app_transm);
  nvs_i8(CB_VERBOSE, "C_VERBOSE", C.app_verboseOutput);
#ifdef USE_NETWORK
  nvs_i8(CB_NETWORK_CONNECTION, "C_NW_CONN", C.network);
#endif
#ifdef USE_LAN
  nvs_i8(CB_LAN_PHY, "C_LAN_PHY", C.lan.phy);
  nvs_i8(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", C.lan.pwr_gpio);
#endif

  nvs_i8(CB_ASTRO_CORRECTION, "C_AST_COR", C.astro.astroCorrection);
  nvs_u32(CB_CUID, "C_CUID", C.fer_centralUnitID);
  nvs_u32(CB_USED_MEMBERS, "C_GMU", C.fer_usedMembers);
  nvs_u32(CB_BAUD, "C_BAUD", C.mcu_serialBaud);

  if (GET_BIT(mask, CB_USED_MEMBERS)) {
    gm_fromNibbleCounters(&C.fer_usedMemberMask, C.fer_usedMembers);
  }
}



void config_read_kvs(u32 mask) {
 kvshT handle = kvs_open(CFG_NAMESPACE, kvs_READ);
 if (handle) {
   rw_config(handle, mask, false);
   kvs_close(handle);
 }
}

void config_save_kvs(u32 mask) {
  kvshT handle = kvs_open(CFG_NAMESPACE, kvs_WRITE);

  if (handle) {
    rw_config(handle, mask, true);
    kvs_commit(handle);
    kvs_close(handle);
  } else {
    D(ets_printf("error: cannot open config in key_value_store\n"));
  }
}

void config_setup() {
  read_config(~0);
#ifdef USE_NETWORK
  // for old users without network configuration, do not start with WLAN AP by default
  if (C.wifi.SSID[0] != '\0') {
    C.network = MY_NETWORK_CONNECTION_OLD_USERS;
    read_config_item(CB_NETWORK_CONNECTION);
  }
#endif
}
#endif
