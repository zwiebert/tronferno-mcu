#pragma once
#include <stdint.h>

typedef enum {
  SO_NONE,
#if 0
  SO_FW_START_MSG_PRINT,

  SO_SEP_ENABLE, SO_SEP_DISABLE, SO_SEP_BUTTON_PRESSED_ERROR,

  SO_CUAS_START, SO_CUAS_TIMEOUT, SO_CUAS_DONE, SO_CUAS_STATE,

  SO_PRAS_START_LISTENING, SO_PRAS_STOP_LISTENING, SO_PRAS_TIMEOUT, SO_PRAS_DONE,
#endif

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

#if 0
  SO_CFGPASSWD_OK, SO_CFGPASSWD_WRONG, SO_CFGPASSWD_MISSING,

  SO_TIMER_EVENT_PRINT, SO_ASTRO_MINUTES_PRINT,
  SO_TIMER_PRINT, SO_TIMER_PRINT_begin, SO_TIMER_PRINT_end,

  SO_POS_begin, SO_POS_PRINT_GMP, SO_POS_PRINT_GMPA, SO_POS_PRINT, SO_POS_PRINT_MMP, SO_POS_end,

  SO_SHPREF_OBJ_begin, SO_SHPREF_OBJ_end, SO_SHPREF_OBJ_GM_begin, SO_SHPREF_OBJ_GM_end,

  SO_PRINT_KVD, SO_PRINT_KVS,

  SO_PAIR_begin, SO_PAIR_ALL_begin, SO_PAIR_PRINT_AMM, SO_PAIR_PRINT_KMM, SO_PAIR_PRINT_KMM_SINGLE, SO_PAIR_ALL_end, SO_PAIR_end,

  SO_INET_PRINT_ADDRESS,

  SO_MCU_begin, SO_MCU_BOOT_COUNT, SO_MCU_RUN_TIME, SO_MCU_VERSION, SO_MCU_OTA, SO_MCU_OTA_STATE, SO_MCU_end,
  SO_STATUS_OK, SO_STATUS_ERROR,

  SO_OTA_RUN, SO_OTA_FAIL, SO_OTA_DONE,

  SO_GPIO_PIN_CHANGED,

#endif
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
  struct GmBitMask *mm;
  uint8_t p;
} so_arg_mmp_t;

typedef struct {
  uint8_t g, m, p;
} so_arg_gmp_t;

typedef struct {
  uint32_t a;
  struct GmBitMask *mm;
} so_arg_amm_t;

typedef struct {
  const char *key;
  struct GmBitMask *mm;
} so_arg_kmm_t;

typedef struct {
  uint8_t gpio_num;
  bool level;
} so_arg_pch_t;
