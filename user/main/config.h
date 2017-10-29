/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef AVR
#if __has_include("../sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#else /* ESP */  // on linux the xtensa-gcc version has not __has_include  FIXME: check for gcc version

#endif


#ifdef HAVE_USER_DEFAULTS

#include "../sensitive/defaults.h"

#else

#define MY_FER_CENTRAL_UNIT_ID 0x807060
#define MY_MCU_ATMEGA328_BAUD_RATE 19200
#define MY_MCU_ESP8266_BAUD_RATE 115200
#define MY_GEO_LONGITUDE -13.41053
#define MY_GEO_LATITUDE +52.52437
#define MY_GEO_TIMEZONE +1.0
#define MY_GEO_DST dstEU
#define MY_WIFI_SSID ""
#define MY_WIFI_PASSWORD ""

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
	vrbNone, vrb1, vrb2, vrb3, vrb4
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
	char wifi_SSID[32];
	char wifi_password[64];

} config;

extern config C;

void read_config(void);  // restore C from persistent storage
void save_config(void);  // save C to persistent storage

#endif /* CONFIG_H_ */
