/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "../userio/inout.h"

#if !defined MCU_ESP8266
#if __has_include("../sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#endif

#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION
#include "../sensitive/defaults.h"
#endif

#ifndef MY_FER_CENTRAL_UNIT_ID
#ifdef CONFIG_MY_FER_CENTRAL_UNIT_ID
#define MY_FER_CENTRAL_UNIT_ID CONFIG_MY_FER_CENTRAL_UNIT_ID
#else
#define MY_FER_CENTRAL_UNIT_ID 0x801234
#endif
#endif

#ifndef MY_MCU_UART_BAUD_RATE
#ifdef CONFIG_MY_MCU_UART_BAUD_RATE
#define MY_MCU_UART_BAUD_RATE CONFIG_MY_MCU_UART_BAUD_RATE
#else
#ifdef MCU_ATMEGA328
#define MY_MCU_UART_BAUD_RATE 57600
#else
#define MY_MCU_UART_BAUD_RATE 115200
#endif
#endif
#endif

// Berlin/Germany
#ifndef MY_GEO_LONGITUDE
#ifdef CONFIG_MY_GEO_LONGITUDE
#define MY_GEO_LONGITUDE CONFIG_MY_GEO_LONGITUDE
#else
#define MY_GEO_LONGITUDE +13.38
#endif
#endif

#ifndef MY_GEO_LATITUDE
#ifdef CONFIG_MY_GEO_LATITUDE
#define MY_GEO_LATITUDE CONFIG_MY_GEO_LATITUDE
#else
#define MY_GEO_LATITUDE +52.53
#endif
#endif

#ifndef MY_GEO_TZ
#ifdef CONFIG_MY_GEO_TZ
#define MY_GEO_TZ CONFIG_MY_GEO_TZ
#else
#define MY_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0" // POSIX TZ format
#endif
#endif

#ifndef MY_GEO_TIMEZONE
#ifdef CONFIG_MY_GEO_TIMEZONE
#define MY_GEO_TIMEZONE CONFIG_MY_GEO_TIMEZONE
#else
#define MY_GEO_TIMEZONE +1.0
#endif
#endif

#ifndef MY_GEO_DST
#ifdef CONFIG_MY_GEO_DST
#define MY_GEO_DST CONFIG_MY_GEO_DST
#else
#define MY_GEO_DST dstEU
#endif
#endif

#ifndef MY_WIFI_SSID
#ifdef CONFIG_MY_WIFI_SSID
#define MY_WIFI_SSID CONFIG_MY_WIFI_SSID
#else
#define MY_WIFI_SSID ""
#endif
#endif

#ifndef MY_WIFI_PASSWORD
#ifdef CONFIG_MY_WIFI_PASSWORD
#define MY_WIFI_PASSWORD CONFIG_MY_WIFI_PASSWORD
#else
#define MY_WIFI_PASSWORD ""
#endif
#endif

#ifndef MY_APP_CONFIG_PASSWORD
#ifdef CONFIG_MY_APP_CONFIG_PASSWORD
#define MY_APP_CONFIG_PASSWORD CONFIG_MY_APP_CONFIG_PASSWORD
#else
#define MY_APP_CONFIG_PASSWORD ""
#endif
#endif

#ifndef MY_APP_EXPERT_PASSWORD
#ifdef CONFIG_MY_APP_EXPERT_PASSWORD
#define MY_APP_EXPERT_PASSWORD CONFIG_MY_APP_EXPERT_PASSWORD
#else
#define MY_APP_EXPERT_PASSWORD ""
#endif
#endif

#ifndef MY_MQTT_URL
#ifdef CONFIG_MY_MQTT_URL
#define MY_MQTT_URL CONFIG_MY_MQTT_URL
#else
#define MY_MQTT_URL ""
#endif
#endif

#ifndef MY_MQTT_USER
#ifdef CONFIG_MY_MQTT_USER
#define MY_MQTT_USER CONFIG_MY_MQTT_USER
#else
#define MY_MQTT_USER ""
#endif
#endif

#ifndef MY_MQTT_PASSWORD
#ifdef CONFIG_MY_MQTT_PASSWORD
#define MY_MQTT_PASSWORD CONFIG_MY_MQTT_PASSWORD
#else
#define MY_MQTT_PASSWORD ""
#endif
#endif


/*
 esp-idf: kconfig does not support float numbers...
config MY_GEO_LONGITUDE
  float "geographical longitude"
  default +13.38

config MY_GEO_LATITUDE
  float "geographical latitude"
  default +52.53

 */










#ifndef MY_FER_GM_USE
#define MY_FER_GM_USE 0x77777777
#endif


#include "main/common.h"

enum dst {
	dstNone, dstEU, dstUS, dstAlways,
};

enum receiver {
	recvNone, recvTick
};

enum transmitter {
	transmNone, transmTick
};

enum rtclock {
	rtcNone, rtcAvrTime
};

enum verbosity {
	vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};

typedef struct {
	uint32_t fer_centralUnitID, mcu_serialBaud;
	float geo_longitude, geo_latitude;

	float geo_timezone;
#if !POSIX_TIME
  enum dst geo_dST;
#endif
  int32_t app_rtcAdjust;
	enum receiver app_recv;
	enum transmitter app_transm;
	enum rtclock app_rtc;
	enum verbosity app_verboseOutput;
	uint32_t fer_usedMembers; // each of the nibbles 1-7 stands for a group. nibble 1 == group 1. nibble 0 = number of used Groups (stored for the front-end, not used here on the MCU)

#ifdef USE_WLAN
	char wifi_SSID[32];
	char wifi_password[64];
#endif
	char app_configPassword[16];
	char app_expertPassword[16];
#ifdef CONFIG_GPIO_SIZE
	 enum mcu_pin_state gpio[CONFIG_GPIO_SIZE];
#endif
#if POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_MQTT
  char mqtt_url[64];
  char mqtt_user[16];
  char mqtt_password[16];
#endif
} config;

extern config C;

void read_config(uint32_t mask);  // restore C from persistent storage
void save_config(uint32_t mask);  // save C to persistent storage

#define CONFIG_ALL (~0UL)
#define CONFIG_RECV (1UL << 0)
#define CONFIG_TRANSM (1UL << 1)
#define CONFIG_CUID (1UL << 2)
#define CONFIG_USED_MEMBERS (1UL << 3)
#define CONFIG_BAUD (1UL << 4)
#define CONFIG_GPIO (1UL << 5)
#define CONFIG_VERBOSE (1UL << 6)
#define CONFIG_WIFI_SSID (1UL << 7)
#define CONFIG_WIFI_PASSWD (1UL << 8)
#define CONFIG_CFG_PASSWD (1UL << 9)
#define CONFIG_LONGITUDE (1UL << 10)
#define CONFIG_LATITUDE (1UL << 11)
#define CONFIG_TIZO (1UL << 12)
#define CONFIG_TZ (1UL << 13)
#define CONFIG_DST (1UL << 14)
#define CONFIG_MQTT_URL (1UL << 15)
#define CONFIG_MQTT_USER (1UL << 16)
#define CONFIG_MQTT_PASSWD (1UL << 17)
#endif /* CONFIG_H_ */
