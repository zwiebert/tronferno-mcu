#pragma once

#include <uout/status_output.h>
#include <stdint.h>

typedef enum {
  SO_NONE,
  SO_CFG_all, SO_CFG_begin,
  SO_CFG_CU, SO_CFG_BAUD, SO_CFG_RTC,
  SO_CFG_NETWORK, SO_CFG_WLAN_SSID, SO_CFG_WLAN_PASSWORD, SO_CFG_NTP_SERVER, SO_CFG_LAN_PHY, SO_CFG_LAN_PWR_GPIO,
  SO_CFG_LONGITUDE, SO_CFG_LATITUDE,  SO_CFG_TIMEZONE, SO_CFG_DST, SO_CFG_TZ, SO_CFG_VERBOSE,
  SO_CFG_MQTT_ENABLE, SO_CFG_MQTT_URL, SO_CFG_MQTT_USER, SO_CFG_MQTT_PASSWORD, SO_CFG_MQTT_CLIENT_ID,
  SO_CFG_HTTP_ENABLE, SO_CFG_HTTP_USER, SO_CFG_HTTP_PASSWORD,
  SO_CFG_GM_USED,
  SO_CFG_ASTRO_CORRECTION,
  SO_CFG_GPIO_RFOUT, SO_CFG_GPIO_RFIN, SO_CFG_GPIO_SETBUTTON, SO_CFG_GPIO_MODES,
  SO_CFG_end, // values between begin/end will be used for 'all=?'
  SO_CFG_GPIO_PIN, SO_CFG_GPIO_MODES_AS_STRING,

} so_msg_t;

typedef struct {
  const char *key, *val;
} so_arg_kvs_t;

typedef struct {
  const char *key;
  int val;
} so_arg_kvd_t;

typedef struct {
  uint8_t g, m;
  struct shutter_timings *st;
} so_arg_gmt_t;


typedef struct {
  uint8_t g, m;
} so_arg_gm_t;

typedef struct {
  struct Fer_GmSet *mm;
  uint8_t p;
} so_arg_mmp_t;

typedef struct {
  uint8_t g, m, p;
} so_arg_gmp_t;

typedef struct {
  uint32_t a;
  struct Fer_GmSet *mm;
} so_arg_amm_t;

typedef struct {
  const char *key;
  struct Fer_GmSet *mm;
} so_arg_kmm_t;

