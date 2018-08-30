/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "main/inout.h"

#ifdef AVR
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
#define MY_GEO_LONGITUDE +12.34567
#define MY_GEO_LATITUDE  +54.32109
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
	float geo_longitude, geo_latitude, geo_timezone;
	enum dst geo_dST;
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
#ifdef ACCESS_GPIO
	 enum mcu_pin_state gpio[17];
#endif
} config;



#if 0
void cfg_fer_centralUnitID(uint32_t *v, bool set);
void cfg_mcu_serialBaud(uint32_t *v, bool set);
void cfg_geo_longitude(float *v, bool set);
void cfg_geo_latitude(float *v, bool set);
void cfg_geo_timezone(float *v, bool set);
void cfg_geo_dST(enum dst *v, bool set);
void cfg_app_rtcAdjust(int32_t *v, bool set);
void cfg_app_recv(enum receiver *v, bool set);
void cfg_app_transm(enum transmitter *v, bool set);
void cfg_app_rtc(enum rtclock *v, bool set);
void cfg_app_verboseOutput(enum verbosity *v, bool set);
void cfg_wifi_SSID(char *v, bool set);
void cfg_wifi_password(char *v, bool set);
void cfg_app_configPassword(char *v, bool set);
void cfg_app_expertPassword(char *v, bool set);
#endif


extern config C;

void read_config(void);  // restore C from persistent storage
void save_config(void);  // save C to persistent storage

#endif /* CONFIG_H_ */
