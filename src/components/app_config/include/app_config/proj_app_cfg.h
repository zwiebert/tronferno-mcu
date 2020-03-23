// User configuration
#ifndef PROJ_APP_CONFIG_H_
#define PROJ_APP_CONFIG_H_

#define APP_VERSION "0.9.7"

#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NETWORK
#define USE_NTP
#define USE_WLAN
#define USE_WLAN_AP
#define USE_LAN
#define USE_MQTT
#define USE_HTTP
#define CONFIG_BLOB
#define CONFIG_DICT
#define ACCESS_GPIO
#define POSIX_TIME 1
#define MDR_TIME
#define USE_PAIRINGS
#define USE_JSON
#define USE_MUTEX
#define USE_OTA


#if defined MCU_ESP8266
#include "../../esp8266/esp8266_user_config.h"
#elif defined MCU_ESP32
#include "app_config/esp32/esp32_user_config.h"
#else
#define IRAM_ATTR
#error "no supported MCU"
#endif

#include "misc/int_types.h"

#define ENABLE_SET_ENDPOS 1

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif

#endif /* PROJ_APP_CONFIG_H_ */
