/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "../sensitive/defaults.h"

#ifndef DEFAULTS_H_
#define MY_CENTRAL_UNIT 0x807060
#define MY_ATMEGA328_BAUD_RATE 9600
#define MY_ESP8266_BAUD_RATE 115200
#define MY_GEO_LONGITUDE -13.41053
#define MY_GEO_LATITUDE +52.52437
#define MY_GEO_TIMEZONE +1.0
#define MY_GEO_DST dstEU
#define MY_WIFI_SSID ""
#define MY_WIFI_PASSWORD ""
#endif

#include "common.h"

enum dst {
	dstNone, dstEU, dstUS,
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

typedef struct {
	uint32_t centralUnitID, serialBaud;
	float longitude, latitude, timezone;
	enum dst dST;
	enum receiver recv;
	enum transmitter transm;
	enum rtclock rtc;
	char wifi_SSID[32];
	char wifi_password[64];

} config;

extern config C;

void read_config(void);  // restore C from persistent storage
void save_config(void);  // save C to persistent storage

#endif /* CONFIG_H_ */
