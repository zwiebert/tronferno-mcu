/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "main/inout.h"
#include "../user_config.h"

#if defined AVR || defined MCU_ESP32
#if __has_include("../sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#else /* ESP */  // on linux the xtensa-gcc version has not __has_include  FIXME: check for gcc version

#endif


#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION

#include "../sensitive/defaults.h"

#else

#define MY_FER_CENTRAL_UNIT_ID 0x8012345
#define MY_MCU_ATMEGA328_BAUD_RATE 19200
#define MY_MCU_ESP8266_BAUD_RATE 115200
#define MY_MCU_ESP32_BAUD_RATE 115200


// Berlin/Germany
#define MY_GEO_LONGITUDE +13.38
#define MY_GEO_LATITUDE  +52.53

#define MY_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0" // POSIX TZ format

#define MY_GEO_TIMEZONE +1.0
#define MY_GEO_DST dstEU

#define MY_WIFI_SSID ""
#define MY_WIFI_PASSWORD ""
#define MY_APP_CONFIG_PASSWORD ""
#define MY_APP_EXPERT_PASSWORD ""


#endif

#ifndef MY_FER_GM_USE
#define MY_FER_GM_USE 0x77777777
#endif
#include "common.h"

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

#if 1 //ndef USE_WLAN
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
} config;

extern config C;

void read_config(uint32_t mask);  // restore C from persistent storage
void save_config(uint32_t mask);  // save C to persistent storage

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

#endif /* CONFIG_H_ */
