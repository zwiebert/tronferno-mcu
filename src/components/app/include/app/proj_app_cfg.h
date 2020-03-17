// User configuration
#ifndef PROJ_APP_CONFIG_H_
#define PROJ_APP_CONFIG_H_

#define APP_VERSION "0.9.6"

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
#include "app/esp32/esp32_user_config.h"
#else
#define IRAM_ATTR
#error "no supported MCU"
#endif


#define ENABLE_SET_ENDPOS 1

#include "main/common.h"

#ifndef FER_GET_RX_PIN
#define FER_GET_RX_PIN() mcu_get_rxPin()
bool mcu_get_rxPin(void);
#endif

#ifndef FER_PUT_TX_PIN
#define FER_PUT_TX_PIN(on) mcu_put_txPin(on)
void mcu_put_txPin(bool);
#endif

#endif /* PROJ_APP_CONFIG_H_ */
